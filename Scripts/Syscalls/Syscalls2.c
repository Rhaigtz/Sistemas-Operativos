#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t me ,childpid1, childpid2;
    me = getpid();
    printf("%d\n", me);

    for(int i=1 ;i < 3; i++){
        childpid1 = fork();

        if (childpid1 == 0)
        {
            printf("child pid %d   parent pid %d\n", getpid(), getppid());
        }
        else if (childpid1 > 0)
        {
            printf("Proceso padre iniciado y en espera\n");
            childpid2 = fork();
        }
    }
    return 0;
}
