
#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#else
#error "-D__linux__"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>


typedef unsigned char             u8_t;
typedef unsigned short           u16_t;
typedef unsigned int             u32_t;
typedef char                      s8_t;
typedef short                    s16_t;
typedef int                      s32_t;
typedef u8_t                    byte_t;
typedef u16_t                   word_t;
typedef byte_t                   buf_t;


#define I_REGULAR               0100000 /* regular file, not dir or special */
#define I_BLOCK_SPECIAL         0060000 /* block special file */
#define I_DIRECTORY             0040000 /* file is a directory */
#define I_CHAR_SPECIAL          0020000 /* character special file */
#define I_SET_UID_BIT           0004000 /* set effective uid on exec */
#define I_SET_GID_BIT           0002000 /* set effective gid on exec */


#define BLOCK_SIZE                 1024
#define MAX_TOKENS                   10	
#define LINE_LEN                    200
#define N_BLOCKS                  32000
#define BIT_MAP_SHIFT                13
#define INODE_MAP                     2
#define NR_ZONE_NUMS                  9 /* # zone numbers in an inode */


#define SUPER_MAGIC                            (0x137F)
#define INODE_SIZE             (sizeof(struct d_inode)) /* bytes in disk inode*/
#define INODES_PER_BLOCK        (BLOCK_SIZE/INODE_SIZE) /* # inodes/disk blk */
#define ZONE_NUM_SIZE                     sizeof(u16_t) /* # bytes in zone nr*/
#define NR_INDIRECTS         (BLOCK_SIZE/ZONE_NUM_SIZE)	/* # zones/indir blk */
#define NR_DZONE_NUM                   (NR_ZONE_NUMS-2) /* # zones in inode */
#define NR_DIR_ENTRIES                  (BLOCK_SIZE/16)	/* # dir entries/blk*/


typedef byte_t    (*img_t)[BLOCK_SIZE];


struct super_block {
        u16_t                s_ninodes;
        u16_t                 s_nzones;
        u16_t            s_imap_blocks;
        u16_t            s_zmap_blocks;
        u16_t          s_firstdatazone;
        u16_t          s_log_zone_size;
        u32_t               s_max_size;
        u16_t                  s_magic;
};


struct d_inode {
        u16_t                   i_mode;
        u16_t                    i_uid;
        u32_t                   i_size;
        u32_t                i_modtime;
        u8_t                     i_gid;
        u8_t                  i_nlinks;
        u16_t                i_zone[9];
};


struct partition {
        u8_t                  boot_ind;
        u8_t                      head;
        u16_t                 sect_cyl; /* u8_t sector; u8_t cyl; */
        u8_t                   sys_ind;
        u8_t                  end_head;
        u16_t             end_sect_cyl; /* u8_t end_sector; u8_t end_cyl; */
        u32_t               start_sect;
        u32_t                 nr_sects;
};


static void get_part(char *img_mmap, int id, struct partition *part);
static int do_mkfs2(void);
static void super(u32_t zones, u32_t inodes);
static void eat_file(int inode, FILE *f);
static void eat_dir(int parent);
static void enter_dir(int parent, char *name, int child);
static void rootdir(u16_t inode);
static int alloc_zone(void);
static u16_t alloc_inode(u16_t mode, u16_t usrid, u16_t grpid);
static void insert_bit(u16_t block, u16_t bit, u16_t count);
static u16_t mode_con(char *pmode);
static void get_token(char *line, char *parse[]);
static int get_block(s32_t blkno, buf_t *buf);
static int put_block(s32_t blkno, buf_t *buf);
static void pdebug(const char *msg, ...);
static void pexit(const char *msg, ...);


int lct = 1, zone_map = 3, zone_shift, zoff, zone_size, next_zone, next_inode;
s32_t current_time = 0, bin_time = 0;
char gwarning[] = {65,46,83,46,84,97,110,101,110,98,97,117,109,10};
char *progname;
char *img_file;
char *proto_file;
FILE *proto_fd;
img_t img;
u32_t nrblocks, nrinodes, inode_offset;
buf_t zero[BLOCK_SIZE];
#ifdef __linux__
size_t img_size;
struct stat img_sbuf;
int img_fd;
char *img_mmap;
#else
#error "-D__linux__"
#endif


int main(int argc, char *argv[])
{
#ifdef HD
        struct partition part;
#endif
        progname = argv[0];
        
        if (argc != 3) {
                pexit("Usage: %s img_file proto_file.\n", progname);
        }
        
        img_file = argv[1];
        proto_file = argv[2];
        current_time = time(0);
        
#ifdef __linux__
        img_fd = open(img_file, O_RDWR);
        if(img_fd < 0) {
                perror(img_file);
                return EXIT_FAILURE;
        }
        
        if(fstat(img_fd, &img_sbuf) < 0) {
                perror(img_file);
                close(img_fd);
                return EXIT_FAILURE;
        }
        
        img_size = (size_t)img_sbuf.st_size;
        img_mmap = mmap(NULL, img_size, PROT_READ | PROT_WRITE, MAP_SHARED, img_fd, 0);
        if(img_mmap == MAP_FAILED) {
                perror(img_file);
                close(img_fd);
                return EXIT_FAILURE;
        }
#else
#error "-D__linux__"
#endif

#ifdef FD
        img = (img_t)(img_mmap + 0);
#elif HD
        get_part(img_mmap, 0, &part);
        img = (img_t)(img_mmap + (part.start_sect * 512));
#else
#error "-DFD, -DHD"
#endif
        do_mkfs2();
        
#ifdef __linux__
        munmap(img_mmap, img_size);
        close(img_fd);
#else
#error "-D__linux__"
#endif

        return 0;
}


static void get_part(char *img_mmap, int id, struct partition *part) 
{
  struct partition *p;
  
  p = (struct partition *)&img_mmap[0x1be];
  *part = *(p + id);
}


static int do_mkfs2(void)
{
        char *token[MAX_TOKENS];
        buf_t buf[BLOCK_SIZE];
        u32_t blocks, inodes, zones;
        u16_t mode, usrid, grpid, i;
        
        proto_fd = fopen(proto_file, "r");
        if (NULL == proto_fd) {
                perror(proto_file);
                exit(EXIT_FAILURE);
        }
        
        get_token(buf, token);
        //pdebug("%s ", token[0]);
        
        get_token(buf, token);
        blocks = atoi(token[0]);
        //pdebug("%s ", token[0]);
        if (blocks > N_BLOCKS) {
                pexit("Block count too large\n");
        }
        inodes = atoi(token[1]);
        //pdebug("%s ", token[1]);
        
        get_token(buf, token);
        mode = mode_con(token[0]);
        //pdebug("0%o ", mode);
        usrid = atoi(token[1]);
        //pdebug("0%o ", usrid);
        grpid = atoi(token[2]);
        //pdebug("0%o ", grpid);

        nrblocks = blocks;
        nrinodes = inodes;

        put_block(0, zero);
        zone_shift = 0;
        zones = blocks >> zone_shift;
        super(zones, inodes);

        i = alloc_inode(mode, usrid, grpid);
        rootdir(i);
        eat_dir(i);
        
        fclose(proto_fd);
        return 0;
}


static void super(u32_t zones, u32_t inodes)
{
        u16_t i, inodeblks, initblks, initzones, nrzones, bs;
        u16_t map_size, bit_map_len, b_needed, b_allocated, residual;
        u32_t zo;
        struct super_block *sup;
        buf_t buf[BLOCK_SIZE], *cp;

        sup = (struct super_block *)buf;

        bs                      = 1 << BIT_MAP_SHIFT;
        sup->s_ninodes          = inodes;
        sup->s_nzones           = zones;
        sup->s_imap_blocks      = (inodes + bs) / bs;
        sup->s_zmap_blocks      = (zones + bs - 1) / bs;
        inode_offset            = sup->s_imap_blocks + sup->s_zmap_blocks + 2;
        inodeblks               = (inodes + INODES_PER_BLOCK - 1)/INODES_PER_BLOCK;
        initblks                = inode_offset + inodeblks;
        initzones               = (initblks + (1<<zone_shift) - 1) >> zone_shift;
        nrzones                 = nrblocks >> zone_shift;
        sup->s_firstdatazone    = (initblks + (1<<zone_shift)-1) >> zone_shift;
        zoff 			= sup->s_firstdatazone - 1;
        sup->s_log_zone_size 	= zone_shift;
        sup->s_magic 		= SUPER_MAGIC;		/* identify super blocks */
        zo 			= 7L + (u32_t) NR_INDIRECTS
        			  + (u32_t) NR_INDIRECTS * NR_INDIRECTS;
        sup->s_max_size 	= zo * BLOCK_SIZE;
        zone_size		= 1 << zone_shift;	/* nr of blocks per zone */

        for (cp = buf + sizeof(*sup); cp < &buf[BLOCK_SIZE]; cp++) {
                *cp=0;
        }
        put_block(1, buf);

        /* Clear maps and inodes. */
        for (i = 2; i < initblks; i++) {
                put_block(i, zero);
        }

        next_zone = sup->s_firstdatazone;
        next_inode = 1;

        /* Mark bits beyond end of inodes as allocated.  (Fails if >8192 inodes). */
        map_size = 1 << BIT_MAP_SHIFT;
        bit_map_len = nrinodes + 1;	/* # bits needed in map */
        residual = bit_map_len % (8 * BLOCK_SIZE);
        if (residual == 0) {
                residual = 8 * BLOCK_SIZE;
        }
        b_needed = (bit_map_len + map_size - 1 ) >> BIT_MAP_SHIFT;
        zone_map += b_needed - 1;	/* if imap > 1, adjust start of zone map */
        insert_bit(INODE_MAP + b_needed - 1, residual, 8 * BLOCK_SIZE - residual);

        bit_map_len = nrzones - initzones + 1;	/* # bits needed in map */
        residual = bit_map_len % (8 * BLOCK_SIZE);
        if (residual == 0) {
                residual = 8 * BLOCK_SIZE;
        }
        b_needed = (bit_map_len + map_size - 1 ) >> BIT_MAP_SHIFT;
        b_allocated = (nrzones + map_size - 1 ) >> BIT_MAP_SHIFT;
        insert_bit(zone_map + b_needed - 1, residual, 8 * BLOCK_SIZE - residual);
        if (b_needed != b_allocated) {
                insert_bit(zone_map + b_allocated - 1, 0, map_size);
        }

        insert_bit(zone_map, 0, 1);     /* bit zero must always be allocated */
        insert_bit(INODE_MAP, 0, 1);    /* inode zero not used but must be allocated */
}


static void incr_link(int n)
{
        /* increment the link count to inode n */
        int b, off;
        struct d_inode inode[INODES_PER_BLOCK];

        b = ((n-1)/INODES_PER_BLOCK) + inode_offset;
        off = (n-1) % INODES_PER_BLOCK;
        get_block(b, (buf_t *)inode);
        inode[off].i_nlinks++;
        put_block(b, (buf_t *)inode);
}


static void incr_size(int n, long count)
{
        /* increment the file-size in inode n */
        int b, off;
        struct d_inode inode[INODES_PER_BLOCK];

        b = ((n-1)/INODES_PER_BLOCK) + inode_offset;
        off = (n-1) % INODES_PER_BLOCK;
        get_block(b, (buf_t *)inode);
        inode[off].i_size += count;
        put_block(b, (buf_t *)inode);
}


static void add_zone(int n, int z, long bytes, long cur_time)
{
        /* add zone z to inode n. The file has grown by 'bytes' bytes. */

        int b, off, indir, i;
        u16_t blk[NR_INDIRECTS];
        struct d_inode *p;
        struct d_inode inode[INODES_PER_BLOCK];

        b = ((n-1)/INODES_PER_BLOCK) + inode_offset;
        off = (n-1) % INODES_PER_BLOCK;
        get_block(b, (buf_t *)inode);
        p = &inode[off];
        p->i_size += bytes;
        p->i_modtime = cur_time;
        for (i=0; i < NR_DZONE_NUM; i++)
        	if (p->i_zone[i] == 0) {
        		p->i_zone[i] = z;
        		put_block(b, (buf_t *)inode);
        		return;
        	}
        put_block(b, (buf_t *)inode);

        /* File has grown beyond a small file. */
        if (p->i_zone[NR_DZONE_NUM] == 0) {
                p->i_zone[NR_DZONE_NUM] = alloc_zone();
        }
        indir = p->i_zone[NR_DZONE_NUM];
        put_block(b, (buf_t *)inode);
        b = indir << zone_shift;
        get_block(b, (buf_t *)blk);
        for (i = 0; i < NR_INDIRECTS; i++)
        	if (blk[i] == 0) {
        		blk[i] = (u16_t)z;
        		put_block(b, (buf_t *)blk);
        		return;
        	}
        pexit("File has grown beyond single indirect");
}


static void rootdir(u16_t inode)
{
        u16_t z;

        z = alloc_zone();
        add_zone(inode, z, 32L, current_time);
        enter_dir(inode, ".", inode);
        enter_dir(inode, "..", inode);
        incr_link(inode);
        incr_link(inode);
}


static void eat_dir(int parent)
{
        /*Read prototype lines and set up directory. Recurse if need be. */
        char *token[MAX_TOKENS], *p;
        char line[LINE_LEN];
        int mode, n, usrid, grpid, z, major, minor;
        FILE *f;
        long size;

        while (1) {
	        get_token(line, token);
	        p = token[0];
                //pdebug("%s ", token[0]);
	        if (*p == '$') return;
	        p = token[1];
                //pdebug("%s ", token[1]);
	        mode = mode_con(p);
	        usrid = atoi(token[2]);
                //pdebug("%s ", token[2]);
	        grpid = atoi(token[3]);
                //pdebug("%s ", token[3]);
	        if (grpid & 0200) write(2, gwarning, 14);
	        n = alloc_inode(mode, usrid, grpid);

	        /* Enter name in directory and update directory's size. */
	        enter_dir(parent, token[0], n);
	        incr_size(parent, 16L);

	        /* Check to see if file is directory or special. */
	        incr_link(n);
	        if (*p == 'd') {
	        	/* This is a directory. */
	        	z = alloc_zone();	/* zone for new directory */
	        	add_zone(n, z, 32L, current_time);
	        	enter_dir(n, ".", n);
	        	enter_dir(n, "..", parent);
	        	incr_link(parent);
	        	incr_link(n);
	        	eat_dir(n);
	        } else if (*p == 'b' || *p == 'c') {
	        	/* Special file. */
	        	major = atoi(token[4]);
                        //pdebug("%s ", token[4]);
	        	minor = atoi(token[5]);
                        //pdebug("%s ", token[5]);
                        if (*p == 'b') size = atoi(token[6]);
                        else size = 0;
                        //pdebug("%s ", token[6]);
	        	size = BLOCK_SIZE * size;
	        	add_zone(n, (major<<8)|minor, size, current_time);
	        } else {
	        	/* Regular file. Go read it. */
                        if ((f = fopen(token[4], "r")) == NULL) {
	        		write(2, "Can't open file ", 16);
	        		write(2, token[4], strlen(token[4]) );
	        		write(2, "\n", 1);
	        	} else
	        	   eat_file(n, f);
	        }
        }

}


static void eat_file(int inode, FILE *f)
{
        int z, ct, i, j, k;
        buf_t buf[BLOCK_SIZE];
        long timeval;

        do {
           for (i=0, j=0; i < zone_size; i++, j+=ct ) {
        	for (k = 0; k < BLOCK_SIZE; k++) buf[k] = 0;
                if ((ct = fread(buf, 1, BLOCK_SIZE, f))) {
                        if (i==0) z = alloc_zone();
                        put_block( (z << zone_shift) + i, buf);
        	}
           }
           timeval = current_time;
           if (ct) add_zone(inode, z, (long) j, timeval );
        } while(ct == BLOCK_SIZE);
        fclose(f);
}


static void enter_dir(int parent, char *name, int child)
{
        /* enter child in parent directory */
        /* works for dir > 1 block and zone > block */
        int i, j, k, l, b, z, off;
        char *p1, *p2;
        struct {
        	short inumb;
        	char name[14];
        } dir_entry[NR_DIR_ENTRIES];

        struct d_inode ino[INODES_PER_BLOCK];


        b   = ((parent-1) / INODES_PER_BLOCK) + inode_offset;
        off =  (parent-1) % INODES_PER_BLOCK ;
        get_block(b, (buf_t *)ino);

        for ( k=0; k<NR_DZONE_NUM; k++ ) {
            z = ino[off].i_zone[k];
            if (z == 0) {
        	  z = alloc_zone();
        	  ino[off].i_zone[k] = z;
            }
            for ( l=0; l<zone_size; l++) {
        	  get_block( (z << zone_shift) + l, (buf_t *)dir_entry);
        	  for ( i=0; i < NR_DIR_ENTRIES; i++) {
        	      if (dir_entry[i].inumb == 0) {
        		 dir_entry[i].inumb = child;
        		 p1 = name;
        		 p2 = dir_entry[i].name;
        		 j  = 14;
        		 while (j--) {
        			*p2++ = *p1;
        			if (*p1 != 0) p1++;
        		 }
        		 put_block( (z << zone_shift) + l, (buf_t *)dir_entry);
        		 put_block(b, (buf_t *)ino);
        		 return;
        	      }
        	  }
             }
        }

        printf("Directory-inode %d beyond direct blocks.  Could not enter %s\n",
        	  parent,name);
        pexit("Halt");
}


static u16_t alloc_inode(u16_t mode, u16_t usrid, u16_t grpid)
{
        u16_t num, b, off;
        struct d_inode inode[INODES_PER_BLOCK];

        num = next_inode++;
        if (num >= nrinodes) {
                pexit("File system does not have enough inodes");
        }
        b  = ((num-1) / INODES_PER_BLOCK) + inode_offset;
        off = (num-1) % INODES_PER_BLOCK;
        get_block(b, (buf_t *)inode);
        inode[off].i_mode = mode;
        inode[off].i_uid = usrid;
        inode[off].i_gid = grpid;
        put_block(b, (buf_t *)inode);

        /* Set the bit in the bit map. */
        insert_bit(INODE_MAP, num, 1);
        return num;
}


static int alloc_zone(void)
{
        /* allocate a new zone */
        /* works for zone > block */
        int b,z,i;

        z = next_zone++;
        b = z << zone_shift;

        if ( (b+zone_size) > nrblocks) {
                pexit("File system not big enough for all the files");
        }
        for ( i=0; i < zone_size; i++) {
                put_block ( b+i, zero );        /* give an empty zone */
        }
        insert_bit(zone_map, z - zoff, 1);
        return(z);
}


static void insert_bit(u16_t block, u16_t bit, u16_t count)
{
        /* insert 'count' bits in the bitmap */
        u16_t w, s, i;
        u16_t buf[BLOCK_SIZE/sizeof(u16_t)];

        get_block(block, (buf_t *)buf);
        for (i = bit; i < bit + count; i++) {
	        w = i / (8*sizeof(u16_t));
	        s = i % (8*sizeof(u16_t));
	        buf[w] |= (1 << s);
        }
        put_block(block, (buf_t *)buf);
}


static u16_t mode_con(char *pmode)
{
        u16_t o1, o2, o3, mode;
        char c1, c2, c3;

        c1 = *pmode++;
        c2 = *pmode++;
        c3 = *pmode++;
        o1 = *pmode++ - '0';
        o2 = *pmode++ - '0';
        o3 = *pmode++ - '0';
        mode = (o1 << 6) | (o2 << 3) | o3;
        if (c1 == 'd') {
                mode += I_DIRECTORY;
        }
        if (c1 == 'b') {
                mode += I_BLOCK_SPECIAL;
        }
        if (c1 == 'c') {
                mode += I_CHAR_SPECIAL;
        }
        if (c1 == '-') {
                mode += I_REGULAR;
        }
        if (c2 == 'u') {
                mode += I_SET_UID_BIT;
        }
        if (c3 == 'g') {
                mode += I_SET_GID_BIT;
        }

        return mode;
}


static void get_token(char *line, char *parse[])
{
        int i;
        int c;
        char *p;
        
        for (i = 0; i < LINE_LEN; i++) {
                line[i] = '\0';
        }
        for (i = 0; i < MAX_TOKENS; i++) {
                parse[i] = NULL;
        }
        
        i = 0;
        p = line;
        while (1) {
                *p = fgetc(proto_fd);
                if (*p == '\n') {
                        lct++;
                }
                if (*p <= 0) {
                        pexit("Unexpected end-of-file\n");
                }
                if (*p == ' ' || *p == '\t') {
                        *p = '\0';
                }
                if (*p == '\n') {
                        *p++ = '\0';
                        *p = '\n';
                        break;
                }
                p++;
        }
        
        p = line;
        while (1) {
                c = *p++;
                if (c == '\n') {
                        return;
                }
                if (c == '\0') {
                        continue;
                }
                parse[i++] = p - 1;
                do {
                        c = *p++;
                } while (c != 0 && c != '\n');
        }
}


static int get_block(s32_t blkno, buf_t *buf)
{
        memmove(buf, img + blkno, BLOCK_SIZE);
        return 0;
}


static int put_block(s32_t blkno, buf_t *buf)
{
        memmove(img + blkno, buf, BLOCK_SIZE);
        return 0;
}


static void pdebug(const char *msg, ...)
{
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
}


static void pexit(const char *msg, ...)
{
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        
        exit(EXIT_FAILURE);
}
