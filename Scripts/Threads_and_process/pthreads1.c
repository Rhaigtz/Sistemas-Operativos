/*
  Compilar:
  	gcc -o pthreads1 pthreads1.c -lpthread
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_THREADS 10

void *print_hello_world(void *tid)
{
    /* Esta funcion imprime el identificador del thread y sale. */
    printf("Hello Multithreaded World. Saludos del thread %ld\n", (long)tid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    /* El programa principal crea 10 threads y sale. */
    pthread_t threads[NUMBER_OF_THREADS];
    int status;
    long i;
    for (i = 0; i < NUMBER_OF_THREADS; i++) {   
        printf("[main] Creando thread %ld\n", i);
        status = pthread_create(&threads[i], NULL, print_hello_world, (void *)i);
        if (status != 0) {
            printf("[main] Oops. pthread_create retorno el codigo de error %d\n", status);
            exit(-1);
        }
    }
    exit(0);
}