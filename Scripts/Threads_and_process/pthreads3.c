/*  
  Compilar:
	gcc -o pthreads3 pthreads3.c -lpthread  
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> /* POSIX threads */
#include <sched.h>

void *mythread(void *);        /* prototipo de la rutina de threads */
volatile int sharedVar = 1234; /* variable global */

int main()
{
    pthread_t tid;
    int pid;
    printf("\n Main: sharedVar = %d.\n", sharedVar);
    pid = fork(); /* se crea proceso hijo */
    if (pid == 0) /* codigo del proceso hijo */
    {
        pthread_create(&tid, NULL, mythread, NULL);
        printf("\n Hijo: creo thread %u. sharedVar = %d \n", tid, sharedVar);
        sched_yield(); /* cede paso a otro thread */
        printf("\n Hijo: sharedVar = %d.\n", sharedVar);
        sched_yield(); /* cede paso a otro thread */
        printf("\n Hijo: sharedVar = %d.\n", sharedVar);
        pthread_join(tid, NULL); /* espera al termino del thread tid */
        printf("Hijo: Terminado\n");
    }
    if (pid > 0) /* codigo del proceso padre */
    {
        pthread_create(&tid, NULL, mythread, NULL);
        printf("\n Padre: creo thread %d. sharedVar = %d \n", tid, sharedVar);
        printf("\n Padre: sharedVar = %d.\n", sharedVar);
        sharedVar = 44444444;
        printf("\n Padre: sharedVar = %d.\n", sharedVar);
        sched_yield(); /* cede paso a otro thread */
        printf("\n Padre: sharedVar = %d.\n", sharedVar);
        sched_yield(); /* cede paso a otro thread */
        printf("\n Padre: sharedVar = %d.\n", sharedVar);
    }
    exit(0);
}

/*********************/
/* Rutina del thread */
/*********************/

void *mythread(void *arg)
{
    /* mostrar valor de la variable global */
    printf("\n thread %lu: sharedVar ahora vale %d.\n", pthread_self(), sharedVar);
    sharedVar = 111;
    printf("\n thread %lu: sharedVar ahora vale %d.\n", pthread_self(), sharedVar);
    sched_yield(); /* cede paso a otro thread */
    printf("\n thread %lu: sharedVar ahora vale %d.\n", pthread_self(), sharedVar);
    sharedVar = 222;
    printf("\n thread %lu: sharedVar ahora vale %d.\n", pthread_self(), sharedVar);
    sched_yield();      /* cede paso a otro thread */
    pthread_exit(NULL); /* salida del thread */
}