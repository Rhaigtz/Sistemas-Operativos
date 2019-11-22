#include <stdio.h>
#include <pthread.h>
pthread_mutex_t mutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int n,t_f;
void* func1(void *arg){
    n++;
    n = n*3;
    printf("Valor esperado: 3, Valor obtenido: %d\n",n);
    printf("Hola, soy el thread1 y ya mande la señal para que el thread2 ejecute asique no es culpa mia si no lo hace..\n");
    t_f = 1;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void* func2(void *arg){
    pthread_mutex_lock(&mutex);
    printf("Estoy esperando la respuesta del thread1...\n");
    if(!t_f)
        pthread_cond_wait(&cond,&mutex);
    pthread_mutex_unlock(&mutex);
    n=n*2;
    printf("Valor esperado: 6, Valor obtenido: %d\n",n);
    pthread_exit(NULL);
}

int main(){
    pthread_t t1,t2;
    pthread_mutex_init(&mutex,NULL);
    pthread_create(&t1,NULL,func1,NULL);
    pthread_create(&t2, NULL,func2, NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}