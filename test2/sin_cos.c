

#include<stdio.h>
#include<math.h>

#define PI 3.1415926

int main(int argc, char *argv[])
{
    int n;
    
    scanf("%d", &n);
    printf("%.4f %.4f\n", sin(n*PI/180), cos(n*PI/180));	
    return 0;
}
