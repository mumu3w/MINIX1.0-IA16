
#define _JBLEN 3
typedef int jmp_buf[_JBLEN];

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int value);
extern int write(int, void*, int);
extern int strlen(char *s);

jmp_buf buf;
char *s1 = "call test()\n";
char *s2 = "return test()\n";
char *s3 = "main()\n";

void test()
{
    write(1, s1, strlen(s1));
    longjmp(buf, 1);
    write(1, s2, strlen(s2));
}

int main(int argc, char **argv, char **envp)
{
    if (!setjmp(buf)) {
        test();
    } else {
        write(1, s3, strlen(s3));
    }

    return 0;
}
