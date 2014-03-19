#include <stdio.h>

int main(int argc, char* argv[])
{
    char a[100] = { 0 };
    getpwd(a);
    int c = opendir(a);
    char buf[50] = { 0 };
    int e = 0;
    while ((e = readdir(c, buf)) != 0)
    {
        printf("\n %s", buf);
    }
    return 0;
}
