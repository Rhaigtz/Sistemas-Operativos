/* JEROGLIFICO 4 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 4
void par_hijos(i){
 for(int i=0;i<2;i++) // loop will run n times (n=5) 
    { 
        if(fork() == 0) 
        { 
            printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); 
            child(i++);
            return;
        } 
    } 
    for(int i=0;i<2;i++) // loop will run n times (n=5) 
    wait(NULL); 
}
void child(int i)
{
    printf("HIJO %d (pid %d) DEL PADRE (pid %d) \n", i, getpid(), getppid());
    if (i == MAX)
        return;
    par_hijos(i);
    wait(NULL);
    return;
}
int main()
{   
    printf("SOY EL PROCESO MAIN (PID %d) \n", getpid());
    int i = 0,count=1;
    child(i++);
    wait(NULL);
    return 0;
}

