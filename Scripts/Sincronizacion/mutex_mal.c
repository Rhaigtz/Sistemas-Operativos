/*
  Compilar:
    gcc -o mutex_mal mutex_mal.c -lpthread
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t tid[2];
int counter;

void *funcionThread(void *arg)
{
    unsigned long i = 0;
    counter += 1;
    printf("\n Inicia job %d \n", counter);
    for (i = 0; i < (0xFFFFFFFF); i++)
        ;
    printf("\n Termina job %d \n", counter);
    return NULL;
}

int main(void)
{
    int i = 0;
    int err;
    while (i < 2)
    {
        err = pthread_create(&(tid[i]), NULL, &funcionThread, NULL);
        if (err != 0)
            printf("\nNo se puede crear el thread :[%s]", strerror(err));
        i++;
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}