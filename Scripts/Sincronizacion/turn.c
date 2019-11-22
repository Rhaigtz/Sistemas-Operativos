/*
  Compilar:
    gcc -o turn turn.c -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int max;
volatile int counter = 0; // Variable compartida global
int turn = 1;             // A que thread le toca entrar en la SC
int flag[2] = {0, 0};     // Arreglo que indica si algun thread quiere entrar a la SC

// bool SW_test_and_set(bool *target)
// {
//     bool old_value = *target;
//     *target = true;
//     return (old_value);
// }

// int SW_compare_and_swap(int *value, int expected, int new_value)
// {
//     int previous = *value;
//     if (*value == expected)
//     {
//         *value = new_value;
//     }
//     return (previous);
// }
void *mythread1(void *arg)
{
    int me = 0;
    int other = 1 - me;
    char *letter = arg;
    int i;        // stack (privado por thread)
    flag[me] = 1; // Quiero entrar a la SC
    turn = other;
    while (flag[other] && turn == other)
        ; // Espero si no me toca
    printf("%s: inicio [direccion de i: %p]\n", letter, &i);
    for (i = 0; i < max; i++)
    {
        counter = counter + 1; // compartido: solo uno
    }
    printf("%s: terminado\n", letter);
    flag[me] = 0; // Ya no necesito ni quiero entrar a la SC
    return NULL;
}

void *mythread2(void *arg)
{
    int me = 1;
    int other = 1 - me;
    char *letter = arg;
    int i;        // stack (privado por thread)
    flag[me] = 1; // Quiero entrar a la SC
    turn = other;
    while (flag[other] && turn == other)
        ; // Espero si no me toca
    printf("%s: inicio [direccion de i: %p]\n", letter, &i);
    for (i = 0; i < max; i++)
    {
        counter = counter + 1; // compartido: solo uno
    }
    printf("%s: terminado\n", letter);
    flag[me] = 0; // Ya no necesito ni quiero entrar a la SC
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "uso: ./turn <n_ciclos>\n");
        exit(1);
    }
    max = atoi(argv[1]);

    pthread_t p1, p2;
    printf("main: inicio [contador = %d] [%lx]\n", counter, (unsigned long)&counter);
    pthread_create(&p1, NULL, mythread1, "A");
    pthread_create(&p2, NULL, mythread2, "B");
    // join esperan a que los threads terminen
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: terminado\n [contador: %d]\n [esperado: %d]\n", counter, max * 2);
    return 0;
}