/*
  Compilar:
    gcc -o spinlock spinlock.c -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int max;
volatile int counter = 0; // Variable compartida global
int lock = 0;             //0: SC está libre, 1: SC esta ocupada

void *mythread(void *arg)
{
    while (lock)
        ;
    lock = 1;
    char *letter = arg;
    int i; // stack (privado por thread)
    printf("%s: inicio [direccion de i: %p]\n", letter, &i);
    for (i = 0; i < max; i++)
    {
        counter = counter + 1; // compartido: solo uno
    }
    printf("%s: terminado\n", letter);
    lock = 0;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "uso: ./spinlock <n_ciclos>\n");
        exit(1);
    }
    max = atoi(argv[1]);

    pthread_t p1, p2,p3;
    printf("main: inicio [contador = %d] [%lx]\n", counter, (unsigned long)&counter);
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    pthread_create(&p2, NULL, mythread, "C");
    // join esperan a que los threads terminen
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    printf("main: terminado\n [contador: %d]\n [esperado: %d]\n", counter, max * 2);
    return 0;
}