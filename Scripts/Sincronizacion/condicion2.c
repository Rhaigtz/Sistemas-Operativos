/*  
  Compilar:
	gcc -o condicion2 condicion2.c -lpthread  
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int contador = 0;
pthread_mutex_t c_mutex;
pthread_cond_t c_umbral_cond;

void *incrementa_count(void *tid)
{
    int i;
    long my_id = (long)tid;

    for (i = 0; i < TCOUNT; i++)
    {
        pthread_mutex_lock(&c_mutex);
        contador++;
        /* Revisa el valor de contador y hace signal al thread en espera cuando se 
    satisface la condicion. Esto ocurre mientras mutex esta bloqueado.
    */
        if (contador == COUNT_LIMIT)
        {
            printf("incrementa_count(): thread %ld, contador = %d  Umbral alcanzado. ",
                   my_id, contador);
            pthread_cond_signal(&c_umbral_cond);
            printf("Se acaba de mandar un signal.\n");
        }
        printf("incrementa_count(): thread %ld, contador = %d, desbloqueando el mutex.\n",
               my_id, contador);
        pthread_mutex_unlock(&c_mutex);

        /* Hacer algo mientras para que los threads puedan alternar el uso de mutex. */
        sleep(1);
    }
    pthread_exit(NULL);
}

void *observa_count(void *t)
{
    long my_id = (long)t;

    printf("Inicia observa_count(): thread %ld\n", my_id);

    /*
  Se bloquea el mutex y se espera un signal. La rutina pthread_cond_wait desbloqueara
  automaticamente y atomicamente el mutex mientras espera. Ademas, si se alcanza el 
  valor COUNT_LIMIT antes que la rutina sea ejecutada por el thread en espera, la
  ejecucion saltara el ciclo para evitar que pthread_cond_wait jamas retorne. */

    pthread_mutex_lock(&c_mutex);
    while (contador < COUNT_LIMIT)
    {
        printf("observa_count(): thread %ld contador= %d. Se inicia la espera...\n", my_id, contador);
        pthread_cond_wait(&c_umbral_cond, &c_mutex);
        printf("observa_count(): thread %ld Se recibio signal por la condicion. contador= %d\n", my_id, contador);
    }
    printf("observa_count(): thread %ld Actualizando valor de contador...\n", my_id);
    contador += 125;
    printf("observa_count(): thread %ld contador ahora es = %d.\n", my_id, contador);
    printf("observa_count(): thread %ld Desbloqueando mutex.\n", my_id);
    pthread_mutex_unlock(&c_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i, rc;
    long t1 = 1, t2 = 2, t3 = 3;
    pthread_t threads[3];
    pthread_attr_t attr;

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&c_mutex, NULL);
    pthread_cond_init(&c_umbral_cond, NULL);

    /* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, observa_count, (void *)t1);
    pthread_create(&threads[1], &attr, incrementa_count, (void *)t2);
    pthread_create(&threads[2], &attr, incrementa_count, (void *)t3);

    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("Main(): Espero e hizo join con %d threads. Valor final del contador = %d. \n",
           NUM_THREADS, contador);

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&c_mutex);
    pthread_cond_destroy(&c_umbral_cond);
    pthread_exit(NULL);
}