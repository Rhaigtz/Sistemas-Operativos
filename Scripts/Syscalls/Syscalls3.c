#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[])
{

    pid_t childpid;
    childpid = fork();

    if (childpid == 0)
    {
        int i = 0;
        while (i++ < 15)
        {
            printf("Proceso hijo...\n");
            sleep(1);
        }
    }
    else if (childpid > 0)
    {
        int i = 0;
        while (i++ < 5)
        {
            printf("Proceso padre...\n");
            sleep(1);
        }
        kill(childpid, SIGKILL);
    }
    else
    {
        printf("Algo fallo...\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
