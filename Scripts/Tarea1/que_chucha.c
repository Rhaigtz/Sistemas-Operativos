/* JEROGLIFICO 4 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 2

void child(int i)
{
    if (i == MAX)
        return;
    printf("HIJO %d (pid %d) DEL PADRE (pid %d) \n", i, getpid(), getppid());
    pid_t pid = fork();
    if (pid > 0)
    {
        pid = fork();
        if (pid < 0)
            printf("ERROR\n");
        else if (pid == 0)
            child(i++);
        waitpid(pid, NULL, 0);
        exit(0);
    }
    else
    {
        child(i++);
    }
}
int main()
{
    printf("SOY EL PROCESO MAIN (PID %d) \n", getpid());

    int i = 1;
    scanf("%d", &n);
    int pid = fork();
    if (pid < 0)
    {
        printf("ERROR\n");
        return 1;
    }
    if (pid > 0)
    {
        pid = fork();
        if (pid == 0)
        {
            i++;
            child(i);
        }
    }
    else if (pid == 0)
        i++;
    child(i);
    waitpid(pid, NULL, 0);
    return 0;
}