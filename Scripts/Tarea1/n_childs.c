/* JEROGLIFICO 3 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 2
void child(int i){
    printf("HIJO %d (pid %d) DEL PADRE (pid %d) \n", i, getpid(), getppid());
    if (i == MAX)
        return;
    wait(NULL);
    return;
}
int main()
{
    int N,COUNT = 1, i =0;;
    N = 2;
    
    for (int j = 0; j < N; j++)
    {
        pid_t pid = fork();
        if (pid == 0){
            printf(" HIJO (pid %d) DEL PADRE (pid %d)\n, CONTADOR(%d)\n)", getpid(), getppid(),COUNT);
            child(i++); 
            exit(0);
        }
    }
    for (int k = 0; k < N; k++){
        wait(NULL);
    }
}