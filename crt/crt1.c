#include <stdlib.h>

void _start(int argc, char *argv[], char *envp[])
{
    int res;

    res = main(argc, argv, envp);
    sys_exit(res);
}
