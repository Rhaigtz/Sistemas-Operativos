#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

volatile int counter = 1;
void par_hijos(count,n){
    if(n == counter){
        return;
    }
    pid_t process = fork();
    n++;
    if(process == 0)
    {   
        printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
        exit(0); 
    }
    for(int i=0;i<2;i++)
        wait(NULL);     
    par_hijos(count,n);
}

int main(int argc, char *argv[]){

    printf("SOY EL PROCESO MAIN (PID %d) \n", getpid());
    int n;
    printf("Ingrese la cantidad de procesos a crear\n");
    scanf("%d", &n);
    pid_t pid = fork();
    n++;
    if(pid > 0){
        pid = fork();
        n++;
        if(pid == 0)
            par_hijos(counter, n);
        else
            wait(NULL);
    }

    return 0;

}