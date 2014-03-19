#include <stdio.h>

int main(int argc, char* argv[])
{
    char a[100] = { 0 };
    getpwd(a);
    printf("\n Pwd : %s", a);
    return 0;
}
