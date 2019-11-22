/*
  Compilar:
    gcc -o semaforo semaforo.c -lpthread
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;

void *thread(void *arg)
{
    //wait
    sem_wait(&mutex);
    printf("\nEntro a la SC\n");

    //critical section
    printf("\n Como soy tan bacan... me voy a dormir durante la SC... \n");
    sleep(4);
    printf("\n Buenos dias!!! \n");

    //signal
    printf("\nSalgo de la SC\n");
    sem_post(&mutex);
}

int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread, NULL);
    sleep(2);
    pthread_create(&t2, NULL, thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&mutex);
    return 0;
}