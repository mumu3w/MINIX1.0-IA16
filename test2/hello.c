
#define NULL                                                          (void*)0
typedef __builtin_va_list                                      __gnuc_va_list;
typedef __gnuc_va_list                                                va_list;
#define va_start(v, l)                                __builtin_va_start(v, l)
#define va_end(v)                                          __builtin_va_end(v)
#define va_arg(v, l)                                    __builtin_va_arg(v, l)
#define MAXWIDTH                                                            32

// char array[32767];
// char array1[32458];

extern int write(int, void*, int);
static void myprintk(char *format, ...);

int main(int argc, char **argv, char **envp)
{
    myprintk("argc = %d\n", argc);
    for (; *argv != NULL; argv++) {
        myprintk("%s\n", *argv);
    }
    for (; *envp != NULL; envp++) {
        myprintk("%s\n", *envp);
    }
    
    return 0;
}

static void kputc(int ch)
{
    write(1, &ch, 1);
}

static void printnum(long n, int base, int sign)
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

static void do_printk(char *format, va_list arg)
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

static void myprintk(char *format, ...)
{
    va_list p;

    va_start(p, format);
    do_printk(format, p);
    va_end(p);
}
