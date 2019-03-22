#include<cstdio>
void write(int a)
{
    printf("%d\n", a);
}

void write(const char* a)
{
    puts(a);
}

void read(int& a)
{
    scanf("%d", &a);
}
