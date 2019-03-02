

extern void printf(char *format, ...);

int main(int argc, char **argv, char **envp)
{
    double d1 = 3.14;
    double d2 = 3.86;
    int i1 = (int)(d1 + d2);

    printf("d1 = %f, d2 = %f \n", d1, d2);
    printf("i1 = %d\n", i1);

    d1 = 3.14;
    d2 = 2.14;
    i1 = (int)(d1 - d2);
    printf("d1 = %f, d2 = %f \n", d1, d2);
    printf("i1 = %d\n", i1);

    d1 = 3.50;
    d2 = 2.00;
    i1 = (int)(d1 * d2);
    printf("d1 = %f, d2 = %f \n", d1, d2);
    printf("i1 = %d\n", i1);

    d1 = 7.0;
    d2 = 2.0;
    d2 = d1 / d2;
    printf("d1 = %f, d2 = %f \n", d1, d2);
    printf("i1 = %d\n", (int)(d2 + 0.5));

    d1 = 7.5;
    d2 = 6.5;
    printf("d1 = %f, d2 = %f \n", d1, d2);
    if (d1 > d2) printf("d1 > d2 \n");

    d1 = 1.111;
    d2 = 1.112;
    printf("d1 = %f, d2 = %f \n", d1, d2);
    if (d1 < d2) printf("d1 < d2 \n");

    d1 = 1.1111;
    d2 = 1.1111;
    printf("d1 = %f, d2 = %f \n", d1, d2);
    if (d1 == d2) printf("d1 == d2 \n");

    d1 = 1.1112;
    d2 = 1.1111;
    printf("d1 = %f, d2 = %f \n", d1, d2);
    if (d1 != d2) printf("d1 != d2 \n");

    return 0;
}
