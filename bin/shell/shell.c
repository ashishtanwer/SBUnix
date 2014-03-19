#include <stdio.h>
int strlen(const char *);
//#define NULL 0
char *trim(char *);
int isspace(char);
char * strtok(char *, char *);
int matchString(char *, char *);
int atoi1(char *);
void commandlist();
int main()
{
    char entry_user[30] = { 0 };
    printf("\n please use command 'help' to know the list of commands");
    for (;;)
    {
        char prompt[30] = { 0 };
        getpwd(prompt);
        int le = strlen(prompt);
        //printf("\n length calcilate %d",le);
        if (le == 0)
        {
            printf("\n %s@root $", entry_user);
        }
        else
        {
            //printf("\n CM PUNK");
            printf("\n %s@%s $", entry_user, prompt);
        }
        char buff[100] = { 0 };
        scanf("%s", buff);
        if (buff[0] == '\0')
        {
            continue;
        }
        int background_process = 0;
        int s = strlen(buff);
        if (buff[s - 1] == '&')
        {
            background_process = 1;
            buff[s - 1] = '\0';
        }
        int argc;
        char *argv[64];
        argc = 0;
        argv[argc] = strtok(buff, " \t\n");
        while (argc++ < 64)
            if (!(argv[argc] = strtok(0, " \t\n")))
                break;
        if (matchString(argv[0], "cd") == 0)
        {
            if (argc == 1)
            {
                continue;
            }
            setpwd(argv[1]);
            continue;
        }
        if (matchString(argv[0], "sleep") == 0)
        {
            if (argc == 1)
            {
                continue;
            }
            sleep(atoi1(argv[1]));
            continue;
        }
        if (matchString(argv[0], "help") == 0)
        {
            commandlist();
            continue;
        }
        int pid = fork();
        if (pid == 0)
        {
            execve(buff);
        }
        else
        {
            if (background_process == 1)
            {
                continue;
            }
            waitpid(pid);
        }
    }
    return 0;
}
void commandlist()
{
    printf("\nSupported Commands: \n");
    ;
    printf("cd <folder-name>/  bin/ls  bin/ps  bin/pwd  sleep <N> \n");
    printf("Executables (placed in rootfs)can be called with full path\n");
}
int matchString(char *s, char *t)
{
    int ret = 0;
    while (!(ret = *(unsigned char *) s - *(unsigned char *) t) && *t)
    {
        ++s;
        ++t;
    }
    if (ret < 0)
    {
        ret = -1;
    }
    else if (ret > 0)
    {
        ret = 1;
    }
    return ret;
}
int strlen(const char *str)
{
    int len = 0;
    for (len = 0; str[len]; len++)
        ;
    return len;
}
int isspace(char a)
{
    return a == ' ' || a == '\t' || a == '\n';
}
char *trim(char *str)
{
    while (isspace(*str))
        str++;
    if (*str == 0)
        return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end))
        end--;
    *(end + 1) = 0;
    return str;
}
int atoi1(char *str)
{
    int res = 0; // Initialize result
    // Iterate through all characters of input string and update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
    // return result.
    return res;
}
char * strtok(char * str, char *comp)
{
    static int pos;
    static char *s;
    int start = pos;
    // Copying the string for further calls of strtok
    if (str != 0)
    {
        s = str;
        pos = 0;
        //    if (DEBUG) printf("\nNew command entered %s",str);
        start = pos;
        if (*str == '\r')
            return 0;
    }
    //i = 0;
    int j = 0;
    //While not end of string
    while (s[pos] != '\0')
    {
        j = 0;
        //Comparing of one of the delimiter matches the character in the string
        while (comp[j] != '\0')
        {
            //Pos point to the next location in the string that we have to read
            if (s[pos] == comp[j])
            {
                //Replace the delimter by \0 to break the string
                s[pos] = '\0';
                pos = pos + 1;
                if (s[start] != '\0')
                    return (&s[start]);
                else
                {
                    // Move to the next string after the delimiter
                    start = pos;
                    // Decrementing as it will be incremented at the end of the while loop
                    pos--;
                    break;
                }
            }
            j++;
        }
        pos++;
    } //End of Outer while
    s[pos] = '\0';
    if (s[start] == '\0')
        return 0;
    else
        return &s[start];
}
