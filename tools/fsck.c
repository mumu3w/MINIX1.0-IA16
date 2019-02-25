/* 类型定义 */
typedef unsigned char                                                    u8_t;
typedef unsigned short                                                  u16_t;
typedef unsigned long                                                   u32_t;
typedef char                                                             s8_t;
typedef short                                                           s16_t;
typedef long                                                            s32_t;
       
typedef u8_t                                                           byte_t;
typedef u16_t                                                          word_t;
typedef u16_t                                                           seg_t;
typedef u16_t                                                           off_t;
typedef u32_t                                                          size_t;

/* 可变参数 */
#if 1
typedef __builtin_va_list                                      __gnuc_va_list;
typedef __gnuc_va_list                                                va_list;
#define va_start(v, l)                                __builtin_va_start(v, l)
#define va_end(v)                                          __builtin_va_end(v)
#define va_arg(v, l)                                    __builtin_va_arg(v, l)
#else
#define _AUPBND	                                                            1U
#define _ADNBND	                                                            1U
typedef void *                                                        va_list;
#define va_arg(ap, T)  (*(T *)(((ap) += _Bnd(T, _AUPBND)) - _Bnd(T, _ADNBND)))
#define va_end(ap)                                                     (void)0
#define va_start(ap, A)         (void)((ap) = (char *)&(A) + _Bnd(A, _AUPBND))
#define _Bnd(X, bnd)                             (sizeof (X) + (bnd) & ~(bnd))
#endif

/* 外部函数声明 */
extern void fmemcpyb(off_t dst_off, seg_t dst_seg, off_t src_off, 
                                    seg_t src_seg, word_t count);
extern void fmemsetb(off_t off, seg_t seg, byte_t val, word_t count);
extern int fmemcmpb(off_t dst_off, seg_t dst_seg, off_t src_off, 
                                    seg_t src_seg, word_t count);
extern void bios_putc(int c);
extern int bios_getc(void);

/* 内部函数声明 */
static void kputc(int c);
static void kputs(const char *s);
static int kgetc(void);
static void printnum(long n, int base, int sign);
static void do_printk(char *format, va_list ap);
void printk(char *format, ...);

/* 输出字符 */
static void kputc(int c)
{
    if (c == '\n') bios_putc('\r');

    bios_putc(c);
}

/* 输出字符串 */
static void kputs(const char *s)
{
    int i;
    for (i = 0; s[i]; i++) kputc(s[i]);
}

/* 获取字符 */
static int kgetc(void)
{
    int c;
    if ((c = bios_getc()) == '\r') c = '\n';

    kputc(c);
    return c;
}

#define MAXWIDTH                                                            32
/* 数字转字符串 */
static void printnum(long n, int base, int sign)
{
    char *hex_string = "0123456789ABCDEF";
    int i, mod;
    char ascii[MAXWIDTH];
    char *p = ascii;

    if (sign) {
        if (n < 0) n = -n;
        else sign = 0;
    }

    do {
        mod = 0;
        for (i = 0; i < MAXWIDTH; i++) {
            mod <<= 1;
            if (n < 0) mod++;
            n <<= 1;
            if (mod >= base) {
                mod -= base;
                n++;
            }
        }
        *p++ = hex_string[mod];
    } while (n);
    if (sign) *p++ = '-';
    while (p > ascii) kputc(*--p);
}

/* 格式化串 */
static void do_printk(char *format, va_list arg)
{
    char *fmt, *s;

    for (fmt = format; *fmt != 0; fmt++) {
        switch (*fmt) {
            case '%':
                fmt++;
                switch (*fmt) {
                    case 'c': kputc(va_arg(arg, int)); break;
                    case 'b': printnum(va_arg(arg, unsigned), 2, 0); break;
                    case 'B': printnum(va_arg(arg, long), 2, 0); break;
                    case 'o': printnum(va_arg(arg, unsigned), 8, 0); break;
                    case 'O': printnum(va_arg(arg, long), 8, 0); break;
                    case 'd': printnum(va_arg(arg, unsigned), 10, 1); break;
                    case 'D': printnum(va_arg(arg, long), 10, 1); break;
                    case 'u': printnum(va_arg(arg, unsigned), 10, 0); break;
                    case 'U': printnum(va_arg(arg, long), 10, 0); break;
                    case 'x': printnum(va_arg(arg, unsigned), 16, 0); break;
                    case 'X': printnum(va_arg(arg, long), 16, 0); break;
                    case 's':
                        s = va_arg(arg, char *);
                        while (*s) {
                            kputc(*s);
                            s++;
                        } break;
                    case '\0': break;
                    default: kputc(*fmt);
                } break;
            default: kputc(*fmt);
        }
    }
}

/* 输出函数 */
void printk(char *format, ...)
{
    va_list p;

    va_start(p, format);
    do_printk(format, p);
    va_end(p);
}

int main()
{
    int c;
    int key;

    for (;;) {
        printk("\n\n\n\n");
        printk("\nHit key as follows:\n\n");
        printk("    =  start MINIX (root file system in drive 0)\n");
        printk("[1-9]  start MINIX (root file system on /dev/hd[1-9]\n");
        printk("\n# ");
    
        c = kgetc();
        key = c & 0xff;
        switch (key) {
            case '=' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
                return key;
            default:
                printk(" Illegal command\n");
                continue;
        }
    }
}
