
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

extern void fmemcpyb(off_t dst_off, seg_t dst_seg, off_t src_off, 
                    seg_t src_seg, word_t count);
extern void fmemsetb(off_t off, seg_t seg, byte_t val, word_t count);
extern int fmemcmpb(off_t dst_off, seg_t dst_seg, off_t src_off, 
                    seg_t src_seg, word_t count);
extern void bios_putc(int c);
extern int bios_getc(void);

void kputc(char c);
void kputs(const char *s);
int kgetc(void);
void printnum(long n, int base, int sign);
void do_printk(char *format, va_list ap);
void printk(char *format, ...);

void kputc(char c)
{
    if (c == '\n') {
        bios_putc('\r');
    }

    bios_putc(c);
}

void kputs(const char *s)
{
    int i;
    for (i = 0; s[i]; i++) {
        kputc(s[i]);
    }
}

int kgetc(void)
{
    char c;
    if ((c = bios_getc()) == '\r') {
        c = '\n';
    }
    kputc(c);
    return c;
}

#define MAXWIDTH                                                            32
void printnum(long n, int base, int sign)
{
    char *hex_string = "0123456789ABCDEF";
    int i, mod;
    char ascii[MAXWIDTH];
    char *p = ascii;

    if (sign) {
        if (n < 0) {
            n = -n;
        } else {
            sign = 0;
        }
    }

    do {
        mod = 0;
        for (i = 0; i < MAXWIDTH; i++) {
            mod <<= 1;
            if (n < 0) {
                mod++;
            }
            n <<= 1;
            if (mod >= base) {
                mod -= base;
                n++;
            }
        }
        *p++ = hex_string[mod];
    } while (n);
    if (sign) {
        *p++ = '-';
    }
    while (p > ascii) {
        kputc(*--p);
    }
}

void do_printk(char *format, va_list arg)
{
    char *fmt, *s;

    for (fmt = format; *fmt != 0; fmt++) {
        switch (*fmt) {
            case '%':
                fmt++;
                switch (*fmt) {
                    case 'c':
                        kputc(va_arg(arg, int)); break;
                    case 'b':
                        printnum(va_arg(arg, unsigned), 2, 0); break;
                    case 'B':
                        printnum(va_arg(arg, long), 2, 0); break;
                    case 'o':
                        printnum(va_arg(arg, unsigned), 8, 0); break;
                    case 'O':
                        printnum(va_arg(arg, long), 8, 0); break;
                    case 'd':
                        printnum(va_arg(arg, unsigned), 10, 1); break;
                    case 'D':
                        printnum(va_arg(arg, long), 10, 1); break;
                    case 'u':
                        printnum(va_arg(arg, unsigned), 10, 0); break;
                    case 'U':
                        printnum(va_arg(arg, long), 10, 0); break;
                    case 'x':
                        printnum(va_arg(arg, unsigned), 16, 0); break;
                    case 'X':
                        printnum(va_arg(arg, long), 16, 0); break;
                    case 's':
                        s = va_arg(arg, char *);
                        while (*s) {
                            kputc(*s);
                            s++;
                        }
                        break;
                    case '\0':
                        break;
                    default:
                        kputc(*fmt);
                }
                break;
            default:
                kputc(*fmt);
        }
    }
}

void printk(char *format, ...)
{
    va_list p;

    va_start(p, format);
    do_printk(format, p);
    va_end(p);
}

#ifdef DEBUG
void test01(void)
{
    long l1, l2, l3;

    l1 = 12345678;
    l2 = 87654321;
    l3 = l1 + l2;
    printk("\n\nl3 = %D\n", l3);
    l3 = l2 - l1;
    printk("l3 = %D\n", l3);
    l3 = l1 - l2;
    printk("l3 = %D\n", l3);

    l1 = 12345;
    l2 = 5678;
    l3 = l1 * l2;
    printk("l3 = %D\n", l3);
    printk("12345 = %D\n", l3 / l2);
    printk("%D\n", 53450013 % 10);
    printk("%b\n", 0xf);
    printk("Copyright (c) %s Mumu3w@outlook.com\n", __DATE__);
    printk("File %s Line: %d Time: %s\n", __FILE__, __LINE__, __TIME__);
}
#endif

int main()
{
    char c;
#ifdef DEBUG
    test01();
#endif
    for (;;) {
        printk("\n\n\n\n");
        printk("\nHit key as follows:\n\n");
        printk("    =  start MINIX (root file system in drive 0)\n");
        printk("\n# ");
    
        c = kgetc();
        switch (c) {
            case '=' :
                return c;
            default:
                printk("Illegal command\n");
                continue;
        }
    }
}
