#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define EXIT_PROGRAM return 0;
#define GRID_H 5
#define GRID_L 5
#define F_NUM 2
#define M_NUM 2
int moves[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
char chale[GRID_L][GRID_H];
int flaites[F_NUM][2];
int moderados[M_NUM][2];
int peleles[2];
/*
 * no se si la función rdtsc funcione en windows, solo la
 * probe en osx y linux.
 * lo que hace esta funcion es retornar el numero de ciclos
 * actuales de la cpu, luego se usan estos como semilla
 * para generar numeros aleatorios de forma mas dispersa.
 * existen otras formas simples de generar numeros aleatorios
 * dispersos en c como tecnicas de xorshifts, pero con este
 * truco basta.
*/
uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}
int posicion_aleatoria()
{
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % GRID_H);
}
void print_chale()
{
    int x, y;
    for (x = 0; x < GRID_L; ++x)
        for (y = 0; y < GRID_H; ++y)
            printf("%c%c", chale[x][y], (y == 4) ? '\n' : ' ');
}
void chale_inicial()
{
    int x, y, i, j;
    for (x = 0; x < GRID_L; ++x)
        for (y = 0; y < GRID_H; ++y)
            chale[x][y] = '*';
    for (x = 0; x < F_NUM; ++x)
    {
        i = posicion_aleatoria();
        j = posicion_aleatoria();
        if (chale[i][j] == '*')
        {
            chale[i][j] = 'F';
            flaites[x][0] = i;
            flaites[x][1] = j;
        }
        else
            --x;
    }
    for (x = 0; x < M_NUM; ++x)
    {
        i = posicion_aleatoria();
        j = posicion_aleatoria();
        if (chale[i][j] == '*')
        {
            chale[i][j] = 'M';
            moderados[x][0] = i;
            moderados[x][1] = j;
        }
        else
            --x;
    }
    while (chale[i][j] != 'P')
    {
        i = posicion_aleatoria();
        j = posicion_aleatoria();
        if (chale[i][j] == '*')
        {
            chale[i][j] = 'P';
            peleles[0] = i;
            peleles[1] = j;
        }
    }
}
void escapan()
{
    int mueven, i, j;
    mueven = posicion_aleatoria() % 4;
    i = peleles[0] + moves[mueven][0];
    j = peleles[1] + moves[mueven][1];
    if (i > 7 || i < 0 || j > 7 || j < 0)
    {
        printf("Movimiento Invalido\n");
        return;
    }
    if (chale[i][j] == 'F'){
        printf("PIÑATA Y SANDWICH MURIERON A TUNAZOS\n");
    }
    else if (chale[i][j] == 'M')
        printf("PIÑATA Y SANDWICH FUERON FUNADOS\n");
    else
    {
        chale[peleles[0]][peleles[1]] = '*';
        chale[i][j] = 'P';
        peleles[0] = i;
        peleles[1] = j;
    }
}
int main(void)
{
    chale_inicial();
    print_chale();
    for (int i = 0; i < 10; ++i)
    {
        printf("\n__________________\n\n");
        escapan();
        print_chale();
    }
    EXIT_PROGRAM;
}