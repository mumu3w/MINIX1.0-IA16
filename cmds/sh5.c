#define Extern extern
#include "signal.h"
#include "errno.h"
#include "setjmp.h"
#include "sh.h"

/* -------- io.c -------- */
/* #include "sh.h" */

/*
 * shell IO
 */
 
static savec(int c, register struct block *bp, int nolit);


int
getc(ec)
register int ec;
{
	register int c;

	if(e.linep > elinep) {
		while((c=readc()) != '\n' && c)
			;
		err("input line too long");
		gflg++;
		return(c);
	}
	c = readc();
	if ((ec != '"') && (ec != '\'')) {
		if(c == '\\') {
			c = readc();
			if (c == '\n' && ec != '\"')
				return(getc(ec));
			c |= QUOTE;
		}
	}
	return(c);
}

void
unget(c)
{
	if (e.iop >= e.iobase)
		e.iop->peekc = c;
}

int
readc()
{
	register c;
	static int eofc;

	for (; e.iop >= e.iobase; e.iop--)
		if ((c = e.iop->peekc) != '\0') {
			e.iop->peekc = 0;
			return(c);
		} else if ((c = (*e.iop->iofn)(&e.iop->arg, e.iop)) != '\0') {
			if (c == -1) {
				e.iop++;
				continue;
			}
			if (e.iop == iostack)
				ioecho(c);
			return(c);
		}
	if (e.iop >= iostack ||
	    multiline && eofc++ < 3)
		return(0);
	leave();
	/* NOTREACHED */
}

void
ioecho(c)
char c;
{
	if (flag['v'])
		write(2, &c, sizeof c);
}

void
pushio(arg, fn)
struct ioarg arg;
int (*fn)();
{
	if (++e.iop >= &iostack[NPUSH]) {
		e.iop--;
		err("Shell input nested too deeply");
		gflg++;
		return;
	}
	e.iop->iofn = fn;
	e.iop->arg = arg;
	e.iop->peekc = 0;
	e.iop->xchar = 0;
	e.iop->nlcount = 0;
	if (fn == filechar || fn == linechar || fn == nextchar)
		e.iop->task = XIO;
	else if (fn == gravechar || fn == qgravechar)
		e.iop->task = XGRAVE;
	else
		e.iop->task = XOTHER;
}

struct io *
setbase(ip)
struct io *ip;
{
	register struct io *xp;

	xp = e.iobase;
	e.iobase = ip;
	return(xp);
}

/*
 * Input generating functions
 */

/*
 * Produce the characters of a string, then a newline, then EOF.
 */
int
nlchar(ap)
register struct ioarg *ap;
{
	register int c;

	if (ap->aword == NULL)
		return(0);
	if ((c = *ap->aword++) == 0) {
		ap->aword = NULL;
		return('\n');
	}
	return(c);
}

/*
 * Given a list of words, produce the characters
 * in them, with a space after each word.
 */
int
wdchar(ap)
register struct ioarg *ap;
{
	register char c;
	register char **wl;

	if ((wl = ap->awordlist) == NULL)
		return(0);
	if (*wl != NULL) {
		if ((c = *(*wl)++) != 0)
			return(c & 0177);
		ap->awordlist++;
		return(' ');
	}
	ap->awordlist = NULL;
	return('\n');
}

/*
 * Return the characters of a list of words,
 * producing a space between them.
 */
static	int	xxchar(), qqchar();

int
dolchar(ap)
register struct ioarg *ap;
{
	register char *wp;

	if ((wp = *ap->awordlist++) != NULL) {
		PUSHIO(aword, wp, *ap->awordlist == NULL? qqchar: xxchar);
		return(-1);
	}
	return(0);
}

static int
xxchar(ap)
register struct ioarg *ap;
{
	register int c;

	if (ap->aword == NULL)
		return(0);
	if ((c = *ap->aword++) == '\0') {
		ap->aword = NULL;
		return(' ');
	}
	return(c);
}

static int
qqchar(ap)
register struct ioarg *ap;
{
	register int c;

	if (ap->aword == NULL || (c = *ap->aword++) == '\0')
		return(0);
	return(c);
}

/*
 * Produce the characters from a single word (string).
 */
int
strchar(ap)
register struct ioarg *ap;
{
	register int c;

	if (ap->aword == 0 || (c = *ap->aword++) == 0)
		return(0);
	return(c);
}

/*
 * Return the characters from a file.
 */
int
filechar(ap)
register struct ioarg *ap;
{
	register int i;
	char c;
	extern int errno;

	do {
		i = read(ap->afile, &c, sizeof(c));
	} while (i < 0 && errno == EINTR);
	return(i == sizeof(c)? c&0177: (closef(ap->afile), 0));
}

/*
 * Return the characters produced by a process (`...`).
 * Quote them if required, and remove any trailing newline characters.
 */
int
gravechar(ap, iop)
struct ioarg *ap;
struct io *iop;
{
	register int c;

	if ((c = qgravechar(ap, iop)&~QUOTE) == '\n')
		c = ' ';
	return(c);
}

int
qgravechar(ap, iop)
register struct ioarg *ap;
struct io *iop;
{
	register int c;

	if (iop->xchar) {
		if (iop->nlcount) {
			iop->nlcount--;
			return('\n'|QUOTE);
		}
		c = iop->xchar;
		iop->xchar = 0;
	} else if ((c = filechar(ap)) == '\n') {
		iop->nlcount = 1;
		while ((c = filechar(ap)) == '\n')
			iop->nlcount++;
		iop->xchar = c;
		if (c == 0)
			return(c);
		iop->nlcount--;
		c = '\n';
	}
	return(c!=0? c|QUOTE: 0);
}

/*
 * Return a single command (usually the first line) from a file.
 */
int
linechar(ap)
register struct ioarg *ap;
{
	register int c;

	if ((c = filechar(ap)) == '\n') {
		if (!multiline) {
			closef(ap->afile);
			ap->afile = -1;	/* illegal value */
		}
	}
	return(c);
}

/*
 * Return the next character from the command source,
 * prompting when required.
 */
int
nextchar(ap)
register struct ioarg *ap;
{
	register int c;

	if ((c = filechar(ap)) != 0)
		return(c);
	if (talking && e.iop <= iostack+1)
		prs(prompt->value);
	return(0);
}

void
prs(s)
register char *s;
{
	if (*s)
		write(2, s, strlen(s));
}

void
putc(c)
char c;
{
	write(2, &c, sizeof c);
}

void
prn(u)
unsigned u;
{
	prs(itoa(u, 0));
}

void
closef(i)
register i;
{
	if (i > 2)
		close(i);
}

void
closeall()
{
	register u;

	for (u=NUFILE; u<NOFILE;)
		close(u++);
}

/*
 * remap fd into Shell's fd space
 */
int
remap(fd)
register int fd;
{
	register int i;
	int map[NOFILE];

	if (fd < e.iofd) {
		for (i=0; i<NOFILE; i++)
			map[i] = 0;
		do {
			map[fd] = 1;
			fd = dup(fd);
		} while (fd >= 0 && fd < e.iofd);
		for (i=0; i<NOFILE; i++)
			if (map[i])
				close(i);
		if (fd < 0)
			err("too many files open in shell");
	}
	return(fd);
}

int
openpipe(pv)
register int *pv;
{
	register int i;

	if ((i = pipe(pv)) < 0)
		err("can't create pipe - try again");
	return(i);
}

void
closepipe(pv)
register int *pv;
{
	if (pv != NULL) {
		close(*pv++);
		close(*pv);
	}
}

/* -------- here.c -------- */
/* #include "sh.h" */

/*
 * here documents
 */

struct	here {
	char	*h_tag;
	int	h_dosub;
	struct	ioword *h_iop;
	struct	here	*h_next;
} *herelist;

struct	block {
	char	*b_linebuf;
	char	*b_next;
	char	b_tmpfile[50];
	int	b_fd;
};

static	struct block *readhere();

#define	NCPB	2048		/* here text block allocation unit */

markhere(s, iop)
register char *s;
struct ioword *iop;
{
	register struct here *h, *lh;

	h = (struct here *) space(sizeof(struct here));
	if (h == 0)
		return;
	h->h_tag = evalstr(s, DOSUB);
	if (h->h_tag == 0)
		return;
	h->h_iop = iop;
	h->h_iop->io_un.io_here = NULL;
	h->h_next = NULL;
	if (herelist == 0)
		herelist = h;
	else
		for (lh = herelist; lh!=NULL; lh = lh->h_next)
			if (lh->h_next == 0) {
				lh->h_next = h;
				break;
			}
	iop->io_flag |= IOHERE|IOXHERE;
	for (s = h->h_tag; *s; s++)
		if (*s & QUOTE) {
			iop->io_flag &= ~ IOXHERE;
			*s &= ~ QUOTE;
		}
	h->h_dosub = iop->io_flag & IOXHERE;
}

gethere()
{
	register struct here *h;

	for (h = herelist; h != NULL; h = h->h_next) {
		h->h_iop->io_un.io_here = 
			readhere(h->h_tag, h->h_dosub? 0: '\'',
				h->h_iop->io_flag & IOXHERE);
	}
	herelist = NULL;
}

static struct block *
readhere(s, ec, nolit)
register char *s;
{
	register struct block *bp;
	register c;
	jmp_buf ev;

	bp = (struct block *) space(sizeof(*bp));
	if (bp == 0)
		return(0);
	bp->b_linebuf = (char *)space(NCPB);
	if (bp->b_linebuf == 0) {
		/* jrp - should release bp here... */
		return(0);
	}
	if (newenv(setjmp(errpt = ev)) == 0) {
		if (e.iop == iostack && e.iop->iofn == filechar) {
			pushio(e.iop->arg, filechar);
			e.iobase = e.iop;
		}

		/* jrp changes */
		bp->b_linebuf[0] = 0;
		bp->b_next = bp->b_linebuf;
		bp->b_tmpfile[0] = 0;
		bp->b_fd = -1;
		for (;;) {
			while ((c = getc(ec)) != '\n' && c) {
				if (ec == '\'')
					c &= ~ QUOTE;
				if (savec(c, bp, nolit) == 0) {
					c = 0;
					break;
				}
			}
			savec(0, bp, nolit);
			if (strcmp(s, bp->b_linebuf) == 0 || c == 0)
				break;
			savec('\n', bp, nolit);
		}
		*bp->b_linebuf = 0;
		if (c == 0) {
			prs("here document `"); prs(s); err("' unclosed");
		}
		quitenv();
	}
	return(bp);
}

static
savec(c, bp, nolit)
register struct block *bp;
{
	/* jrp - gutted routine completely, modified to use temp file. */
	
	/* If the file is not open, see if a filename needs to be
	 * created.  If so, create one.  Then create the file.
	 */
	char *	lp;
	char *	cp;
	static int inc;
	int	len;

	if(bp->b_fd < 0) {
	    if(bp->b_tmpfile[0] == 0) {
		/* Key this by the PID plus a tag... */
		for (cp = bp->b_tmpfile, lp = "/tmp/shtm"; 
		     (*cp = *lp++) != '\0'; cp++)
			;

		inc = (inc + 1) % 100;
		lp = putn(getpid()*100 + inc);
		for (; (*cp = *lp++) != '\0'; cp++)
			;
	    }

	    /* Create the file, then open it for
	     * read/write access.  After opening the
	     * file, unlink it to it'll go away when
	     * we're through using it.
	     */
	    bp->b_fd = creat(bp->b_tmpfile, 0600);
	    close(bp->b_fd);
	    bp->b_fd = open(bp->b_tmpfile, 2);
	    unlink(bp->b_tmpfile);
	    if(bp->b_fd < 0) {
	        return(0);
	    }
	}

	/* Stuff the character into the line buffer.  If it's a
	 * newline, then insert it before the trailing null, write
	 * out the line, and reset the line buffer.
	 */
	if(c == '\n') {
	    bp->b_next[-1] = '\n';
	    bp->b_next[0] = '\0';
	    len = strlen(bp->b_linebuf);

	    /* Write this out, unless the line ended
	     * with a backslash...
	     */
	    if((len > 1) && (bp->b_next[-2] != '\\')) {
		write_linebuf(bp, nolit);
	    }

	    return(1);
	}
	else {
	    if(bp->b_next == &(bp->b_linebuf[NCPB - 1])) {
		prs("here: line buffer full\n");
		return(0);
	    }
	    *(bp->b_next++) = c;
	    return(1);
	}
}

write_linebuf(bp, nolit)
struct block * bp;
{

	char c;
	jmp_buf ev;

	if(nolit) {
		if (newenv(setjmp(errpt = ev)) == 0) {
			PUSHIO(aword, bp->b_linebuf, strchar);
			setbase(e.iop);
			e.iop->task |= XHERE;
			while ((c = subgetc(0, 0)) != 0) {
				c &= ~ QUOTE;
				write(bp->b_fd, &c, sizeof c);
			}
			quitenv();
		
		}
	}
	else {
		write(bp->b_fd, bp->b_linebuf, strlen(bp->b_linebuf));
	}

	/* Zap the line buffer for next time... */
	bp->b_next = bp->b_linebuf;
	bp->b_linebuf[0] = 0;
}

herein(bp, xdoll)
struct block *bp;
{
	int	ret_fd;

	if (bp == 0)
		return(-1);

	/* If we have a temp file, then rewind it to the beginning */
	if(bp->b_fd < 0) {
		return(-1);
	}

	lseek(bp->b_fd, 0L, 0);

	/* Free up this block pointer, as we're
	 * not going to need it anymore.
	 */
	xfree(bp->b_linebuf);
	xfree(bp);

	return(bp->b_fd);
}

scraphere()
{
	struct here * h;
	struct here * nexth;
	struct block * bp;


	/* Close and unlink any files associated with
	 * heres in progress, and free up all the
	 * associated structures. 
	 */
	h = herelist;
	while(h != NULL) {
		nexth = h->h_next;
		bp = (struct block *)h->h_iop->io_un.io_here;
		if(bp != NULL) {
			if(bp->b_fd >= 0) { close(bp->b_fd); }
			if(*bp->b_tmpfile) { unlink(bp->b_tmpfile); }
			xfree(bp->b_linebuf);
			xfree(bp);
		}
		xfree(h);
		h = nexth;
	}

	herelist = NULL;
}

char *
memcpy(ato, from, nb)
register char *ato, *from;
register int nb;
{
	register char *to;

	to = ato;
	while (--nb >= 0)
		*to++ = *from++;
	return(ato);
}
