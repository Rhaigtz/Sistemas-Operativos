#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#define FLAITES 6
#define MODERADOS 6
#define FILAS 15
#define COLUMNAS 15
#define PAYASOS 2
// F = FLAITES , M = MODERADOS, P = PIRAÑA, S=SANDWICH, X= ZONAS DESTRUIDAS, A = MODERADOS Y PIRAÑA Y/O SANDWICH, B = PIRAÑA Y SANDWICH
pthread_cond_t cond_gen = PTHREAD_COND_INITIALIZER;  //CONDICION QUE MANDA MODERADOS Y RECIBE FLAITES
pthread_cond_t cond_gen2 = PTHREAD_COND_INITIALIZER; //CONDICION QUE MANDA FLAITES Y RECIBE PIRAÑA
pthread_cond_t cond_gen3 = PTHREAD_COND_INITIALIZER; // CONDICION QUE MANDA PIRAÑA Y RECIBE SANDWICH
pthread_cond_t cond_gen4 = PTHREAD_COND_INITIALIZER; // CONDICION QUE MANDA SANDWICH Y RECIBE MODERADOS
pthread_mutex_t mutex;
pthread_mutex_t protect;
char chale[FILAS][COLUMNAS]; // MATRIZ QUE DEFINE A CHALE
int moderados[MODERADOS][2]; // MATRIZ QUE ALMACENA LA POSICION DE CADA MODERADO
int flaites[FLAITES][2];     // MATRIZ QUE ALMACENA LA POSICION DE CADA FLAITE
int payasos[2][2];           // MATRIZ QUE ALMACENA LA POSICION DE CADA PAYASO
int p, s;                    // VARIABLES QUE INDICAN SI PIRAÑA Y SANDWICH FUERON ATRAPADOS O SI SE LOS PITIEARON, 1: SE LOS PITIEARON O LOS ATRAPARON, 0: SIGUEN PROFUGOS
int its_your_turn = 1;       // INDICA A QUIEN LE TOCA MOVERSE, SE INICIA EN MODERADOS Y SIGUE EL ORDEN QUE INDICAN LAS VAR_COND

uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int posicion_aleatoria_x()
{ //FUNCION PARA SETEAR UN VALOR RANDOM AL MOVIMIENTO X
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % FILAS);
}
int posicion_aleatoria_y()
{ //FUNCION PARA SETEAR UN VALOR RANDOM AL MOVIMIENTO Y
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % COLUMNAS);
}
int movimiento(int num)
{ // 0: ARRIBA, 1: DERECHA, 2:ABAJO, 3: IZQUIERDA
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % num);
}
void set_chale()
{ // INICIALIZAR CHALE DE MANERA RANDOM
    int i, j, x, y;
    for (i = 0; i < FILAS; ++i)
    { //Marcar los espacios vacios de chale con un *
        for (j = 0; j < COLUMNAS; ++j)
        {
            chale[i][j] = '*';
        }
    }
    for (i = 0; i < FLAITES; ++i)
    { //Posicionar de manera aleatorea a los flaites en chale
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        if (chale[x][y] == '*')
        {
            chale[x][y] = 'F';
            flaites[i][0] = x; //Almacenar la posicion de cada flaite una vez fue posicionado
            flaites[i][1] = y;
        }
        else
            --i;
    }

    for (i = 0; i < MODERADOS; i++)
    { //Posicionar de manera aleatorea a los moderados en chale
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        if (chale[x][y] == '*')
        {
            chale[x][y] = 'M';
            moderados[i][0] = x; //Almacenar la posicion de cada moderado una vez fue posicionado
            moderados[i][1] = y;
        }
        else
            --i;
    }
    for (i = 0; i < 2; ++i)
    {
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        switch (i)
        {
        case 0:
            if (chale[x][y] == '*')
            {
                chale[x][y] = 'P';
                payasos[i][0] = x;
                payasos[i][1] = y;
            }
            else
                --i;
            break;
        case 1:
            if (chale[x][y] == '*')
            {
                chale[x][y] = 'S';
                payasos[i][0] = x;
                payasos[i][1] = y;
            }
            else
                --i;
            break;
        }
    }
}
void print_chale()
{ // IMPRIMIR MATRIZ CHALE
    int x, y;
    for (x = 0; x < FILAS; ++x)
        for (y = 0; y < COLUMNAS; ++y)
            printf("%c%c", chale[x][y], (y == COLUMNAS - 1) ? '\n' : ' ');
}
void persecusion(int i, int x, char clown)
{ //FUNCION CUANDO MODERADO SE ENCUENTRA CON PAYASOS, INDICANDO SI ESCAPA O NO
    int value = movimiento(6) + 1;
    if (x == 0)
    {
        chale[moderados[i][0] - 1][moderados[i][1]] = 'A';
        chale[moderados[i][0]][moderados[i][1]] = '*';
        moderados[i][0] = moderados[i][0] - 1;
    }
    else if (x == 1)
    {
        chale[moderados[i][0]][moderados[i][1] + 1] = 'A';
        chale[moderados[i][0]][moderados[i][1]] = '*';
        moderados[i][1] = moderados[i][1] + 1;
    }
    else if (x == 2)
    {
        chale[moderados[i][0] + 1][moderados[i][1]] = 'A';
        chale[moderados[i][0]][moderados[i][1]] = '*';
        moderados[i][0] = moderados[i][0] + 1;
    }
    else if (x == 3)
    {
        chale[moderados[i][0]][moderados[i][1] - 1] = 'A';
        chale[moderados[i][0]][moderados[i][1]] = '*';
        moderados[i][1] = moderados[i][1] - 1;
    }
    print_chale();
    printf("Encontre a Piraña/Sandwich, vere si lo puedo atrapar sin que se escape\n");
    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña, con cualquier otro valor es atrapado y debe escribir la nueva constitucion.)\n");
    printf("El numero es..\n");
    sleep(1);
    printf("...\n");
    sleep(1);
    printf("...\n");
    sleep(1);
    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
    sleep(1 / 2);
    if (value == 1 || value == 3)
    {
        if (x == 0)
        {
            chale[moderados[i][0]][moderados[i][1]] = clown;
            chale[moderados[i][0] + 1][moderados[i][1]] = 'M';
            moderados[i][0] = moderados[i][0] + 1;
        }
        else if (x == 1)
        {
            chale[moderados[i][0]][moderados[i][1]] = clown;
            chale[moderados[i][0]][moderados[i][1] - 1] = 'M';
            moderados[i][1] = moderados[i][1] - 1;
        }
        else if (x == 2)
        {
            chale[moderados[i][0]][moderados[i][1]] = clown;
            chale[moderados[i][0] - 1][moderados[i][1]] = 'M';
            moderados[i][0] = moderados[i][0] - 1;
        }
        else if (x == 3)
        {
            chale[moderados[i][0]][moderados[i][1]] = clown;
            chale[moderados[i][0]][moderados[i][1] + 1] = 'M';
            moderados[i][1] = moderados[i][1] + 1;
        }
        printf("Escapo! mejor suerte para la proxima\n");
    }
    else
    {
        printf("Wasted! Se queda escribiendo una nueva constitucion.\n");
        chale[moderados[i][0]][moderados[i][1]] = 'M';
        if (clown == 'P')
            p = 1;
        else if (clown == 'S')
            s = 1;
        else if (clown == 'B')
        {
            p = 1;
            s = 1;
        }
    }
}

static void monVar_cond_enter(){
    pthread_mutex_lock(&mutex);
}
static void monWaitMod(){
    if (its_your_turn != 1)
    {
        pthread_cond_wait(&cond_gen4, &mutex);
    }
}
static void monBlock_SC(){
    pthread_mutex_lock(&protect);
}
static void monModExit(){
    pthread_cond_signal(&cond_gen);
    pthread_mutex_unlock(&protect);
    pthread_mutex_unlock(&mutex);
}
static void monWaitFla(){
    if (its_your_turn != 2)
    {
        pthread_cond_wait(&cond_gen, &mutex);
    }
}
static void monFlaExit(){
    pthread_cond_signal(&cond_gen2);
    pthread_mutex_unlock(&protect);
    pthread_mutex_unlock(&mutex);
}
static void monWait_PinSan(int a){
    if (its_your_turn != 3 && a == 0)
    {
        pthread_cond_wait(&cond_gen2, &mutex);
    }
    else if (its_your_turn != 4 && a == 1)
    {
        pthread_cond_wait(&cond_gen3, &mutex);
    }
}
static void monPinExit(){
    pthread_cond_signal(&cond_gen3);
    pthread_mutex_unlock(&protect);
    pthread_mutex_unlock(&mutex);
}
static void monSanExit()
{
    pthread_cond_signal(&cond_gen4);
    pthread_mutex_unlock(&protect);
    pthread_mutex_unlock(&mutex);
}
void *busqueda_moderados(void *arg)
{ //FUNCION CON LA CUAL LOS THREADS MODERADOS REALIZAN SU BUSQUEDA DE LOS PAYASOS
    // sem_wait(bloqueo_moderados);        //SE UTILIZA LA FUNCION RAND() PARA ESCOJER LA DIRECCION DE MOVIMIENTO 0: ARRIBA, 1: DERECHA,                                            2:ABAJO, 3: IZQUIERDA
    while (1)
    {
        monVar_cond_enter(); //BLOQUEO VARIABLE DE CONDICION
        monWaitMod();
        monBlock_SC(); //BLOQUEO SC
        if (p == 1 && s == 1)
        { //TERMINO EJECUCION DANDO PASO AL SIGUIENTE THREAD Y SALGO
            its_your_turn = 2;
            monModExit();
            pthread_exit(NULL);
        }
        for (int i = 0; i < MODERADOS; ++i)
        { // MOVIMIENTO DE CADA MODERADO
            char clown;
            int x = movimiento(4);
            if (x == 0)
            {
                if (chale[moderados[i][0] - 1][moderados[i][1]] == 'X' || chale[moderados[i][0] - 1][moderados[i][1]] == 'F' || chale[moderados[i][0] - 1][moderados[i][1]] == 'M' || moderados[i][0] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[moderados[i][0] - 1][moderados[i][1]] == 'P')
                {
                    clown = 'P';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0] - 1][moderados[i][1]] == 'S')
                {
                    clown = 'S';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0] - 1][moderados[i][1]] == 'B')
                {
                    clown = 'B';
                    persecusion(i, x, clown);
                }
                else
                {
                    chale[moderados[i][0] - 1][moderados[i][1]] = 'M';
                    chale[moderados[i][0]][moderados[i][1]] = '*';
                    moderados[i][0] = moderados[i][0] - 1;
                }
            }
            else if (x == 1)
            {
                if (chale[moderados[i][0]][moderados[i][1] + 1] == 'X' || chale[moderados[i][0]][moderados[i][1] + 1] == 'F' || chale[moderados[i][0]][moderados[i][1] + 1] == 'M' || moderados[i][1] == COLUMNAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[moderados[i][0]][moderados[i][1] + 1] == 'P')
                {
                    clown = 'P';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0]][moderados[i][1] + 1] == 'S')
                {
                    clown = 'S';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0]][moderados[i][1] + 1] == 'B')
                {
                    clown = 'B';
                    persecusion(i, x, clown);
                }
                else
                {
                    chale[moderados[i][0]][moderados[i][1] + 1] = 'M';
                    chale[moderados[i][0]][moderados[i][1]] = '*';
                    moderados[i][1] = moderados[i][1] + 1;
                }
            }
            else if (x == 2)
            {
                if (chale[moderados[i][0] + 1][moderados[i][1]] == 'X' || chale[moderados[i][0] + 1][moderados[i][1]] == 'F' || chale[moderados[i][0] + 1][moderados[i][1]] == 'M' || moderados[i][0] == FILAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[moderados[i][0] + 1][moderados[i][1]] == 'P')
                {
                    clown = 'P';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0] + 1][moderados[i][1]] == 'S')
                {
                    clown = 'S';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0] + 1][moderados[i][1]] == 'B')
                {
                    clown = 'B';
                    persecusion(i, x, clown);
                }
                else
                {
                    chale[moderados[i][0] + 1][moderados[i][1]] = 'M';
                    chale[moderados[i][0]][moderados[i][1]] = '*';
                    moderados[i][0] = moderados[i][0] + 1;
                }
            }
            else if (x == 3)
            {
                if (chale[moderados[i][0]][moderados[i][1] - 1] == 'X' || chale[moderados[i][0]][moderados[i][1] - 1] == 'F' || chale[moderados[i][0]][moderados[i][1] - 1] == 'M' || moderados[i][1] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[moderados[i][0]][moderados[i][1] - 1] == 'P')
                {
                    clown = 'P';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0]][moderados[i][1] - 1] == 'S')
                {
                    clown = 'S';
                    persecusion(i, x, clown);
                }
                else if (chale[moderados[i][0]][moderados[i][1] - 1] == 'B')
                {
                    clown = 'B';
                    persecusion(i, x, clown);
                }
                else
                {
                    chale[moderados[i][0]][moderados[i][1] - 1] = 'M';
                    chale[moderados[i][0]][moderados[i][1]] = '*';
                    moderados[i][1] = moderados[i][1] - 1;
                }
            }
            if (p == 1 && s == 1)
            {
                its_your_turn = 2;
                monModExit();
                pthread_exit(NULL);
            }
            printf("\n__________________\n\n");
            print_chale();
            printf("\n");
            sleep(3 / 2);
            its_your_turn = 2;
        }
        its_your_turn = 2; // TERMINO EJECUCION DANDO PASO AL SIGUIENTE THREAD
        monModExit();
    }
    return NULL;
}

void *busqueda_flaites(void *arg)
{ //FUNCION CON LA CUAL LOS THREADS flaites REALIZAN SU BUSQUEDA DE LOS PAYASOS
    // sem_wait(bloqueo_moderados);        //SE UTILIZA LA FUNCION RAND() PARA ESCOJER LA DIRECCION DE MOVIMIENTO 0: ARRIBA, 1: DERECHA,                                            2:ABAJO, 3: IZQUIERDA
    while (1)
    {
        monVar_cond_enter();//BLOQUEO VARIABLE CONDICION
        monWaitFla();
        monBlock_SC();//BLOQUEO SC
        if (p == 1 && s == 1)
        { // TERMINO EJECUCION DADNO PASO AL SIGUIENTE THREAD
            its_your_turn = 3;
            monFlaExit();
            pthread_exit(NULL);
        }
        for (int i = 0; i < FLAITES; ++i)
        { //MOVIMIENTO DE CADA FLAITE
            int x = movimiento(4);
            if (x == 0)
            {
                if (chale[flaites[i][0] - 1][flaites[i][1]] == 'F' || chale[flaites[i][0] - 1][flaites[i][1]] == 'M' || flaites[i][0] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[flaites[i][0] - 1][flaites[i][1]] == 'P')
                {
                    chale[flaites[i][0] - 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] - 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    p = 1;
                }
                else if (chale[flaites[i][0] - 1][flaites[i][1]] == 'S')
                {
                    chale[flaites[i][0] - 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] - 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    s = 1;
                }
                else if (chale[flaites[i][0] - 1][flaites[i][1]] == 'B')
                {
                    chale[flaites[i][0] - 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] - 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    s = 1;
                    p = 1;
                }
                else
                {
                    chale[flaites[i][0] - 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] - 1;
                }
            }
            else if (x == 1)
            {
                if (chale[flaites[i][0]][flaites[i][1] + 1] == 'F' || chale[flaites[i][0]][flaites[i][1] + 1] == 'M' || flaites[i][1] == COLUMNAS - 1)
                {
                    printf("No me puedo mover a esa posicion \n");
                }
                else if (chale[flaites[i][0]][flaites[i][1] + 1] == 'P')
                {
                    chale[flaites[i][0]][flaites[i][1] + 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] + 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    p = 1;
                }
                else if (chale[flaites[i][0]][flaites[i][1] + 1] == 'S')
                {
                    chale[flaites[i][0]][flaites[i][1] + 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] + 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    s = 1;
                }
                else if (chale[flaites[i][0]][flaites[i][1] + 1] == 'B')
                {
                    chale[flaites[i][0]][flaites[i][1] + 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] + 1;
                    printf("Me pitie a uno de los payasos.. ups\n");
                    s = 1;
                    p = 1;
                }
                else
                {
                    chale[flaites[i][0]][flaites[i][1] + 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] + 1;
                }
            }
            else if (x == 2)
            {
                if (chale[flaites[i][0] + 1][flaites[i][1]] == 'F' || chale[flaites[i][0] + 1][flaites[i][1]] == 'M' || flaites[i][0] == FILAS - 1)
                {
                    printf("No me puedo mover a esa posicion posicion\n");
                }
                else if (chale[flaites[i][0] + 1][flaites[i][1]] == 'P')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0] + 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] + 1;
                    p = 1;
                }
                else if (chale[flaites[i][0] + 1][flaites[i][1]] == 'S')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0] + 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] + 1;
                    s = 1;
                }
                else if (chale[flaites[i][0] + 1][flaites[i][1]] == 'B')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0] + 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] + 1;
                    s = 1;
                    p = 1;
                }
                else
                {
                    chale[flaites[i][0] + 1][flaites[i][1]] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][0] = flaites[i][0] + 1;
                }
            }
            else if (x == 3)
            {
                if (chale[flaites[i][0]][flaites[i][1] - 1] == 'F' || chale[flaites[i][0]][flaites[i][1] - 1] == 'M' || flaites[i][1] == 0)
                {
                    printf("No me puedo mover a esa posicion posicion\n");
                }
                else if (chale[flaites[i][0]][flaites[i][1] - 1] == 'P')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0]][flaites[i][1] - 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] - 1;
                    p = 1;
                }
                else if (chale[flaites[i][0]][flaites[i][1] - 1] == 'S')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0]][flaites[i][1] - 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] - 1;
                    s = 1;
                }
                else if (chale[flaites[i][0]][flaites[i][1] - 1] == 'B')
                {
                    printf("Me pitie a un payaso.. ups\n");
                    chale[flaites[i][0]][flaites[i][1] - 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] - 1;
                    s = 1;
                    p = 1;
                }
                else
                {
                    chale[flaites[i][0]][flaites[i][1] - 1] = 'F';
                    chale[flaites[i][0]][flaites[i][1]] = 'X';
                    flaites[i][1] = flaites[i][1] - 1;
                }
            }
            if (p == 1 & s == 1)
            {
                printf("Los Flaites terminaron\n");
                its_your_turn = 3;
                monFlaExit();
                pthread_exit(NULL);
            }
            printf("\n__________________\n\n");
            print_chale();
            printf("\n");
            sleep(1);
        }
        its_your_turn = 3; // TERMINO EJECUCION DANDO PASO AL SIGUENTE THREAD
        monFlaExit();
    }
    return NULL;
}

void *escape(void *arg)
{ // FUNCION DE MOVIMIENTO DE LOS PAYASOS
    while (1)
    {
        monVar_cond_enter(); // BLOQUEO VAR COND SEGUN CUAL THREAD ENTRA
        monWait_PinSan((int)*(int *)arg);
        monBlock_SC();// BLOQUEO SC
        //TERMINO DE EJECUCION DEL THREAD EN CUESTION PARA DAR PASO AL SIGUIENTE
        if (p == 1 && (int)*(int *)arg == 0 && s == 1)
        {
            its_your_turn = 4;
            monPinExit();
            pthread_exit(NULL);
        }
        if (s == 1 && (int)*(int *)arg == 1 && p == 1)
        {
            its_your_turn = 1;
            monSanExit();
            pthread_exit(NULL);
        }
        int x = movimiento(4);
        switch ((int)*(int *)arg)
        { // MOVIMIENTO DEL THREAD EN CUESTION, PIÑERA 0 Y SANDWICH 1
        case 0:
            if (p == 1)
            {
                break;
            }
            if (x == 0)
            {
                if (chale[payasos[0][0] - 1][payasos[0][1]] == 'X' || payasos[0][0] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[0][0] - 1][payasos[0][1]] == 'S')
                {
                    chale[payasos[(int)*(int *)arg][0] - 1][payasos[0][1]] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                }
                else if (chale[payasos[0][0] - 1][payasos[0][1]] == 'F')
                {
                    printf("Mori por weta\n");
                    p = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    break;
                }
                else if (chale[payasos[0][0] - 1][payasos[0][1]] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Me encontre con un manifestante ingenuo, vere si puedo escapar con mi fake smile\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Debo escribir la nueva constitucion, adios\n");
                        p = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0] - 1][payasos[(int)*(int *)arg][1]] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'S';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0] - 1][payasos[(int)*(int *)arg][1]] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                    }
                }
            }
            else if (x == 1)
            {
                if (chale[payasos[0][0]][payasos[0][1] + 1] == 'X' || payasos[0][1] == COLUMNAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[0][0]][payasos[0][1] + 1] == 'S')
                {
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1] + 1] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                }
                else if (chale[payasos[0][0]][payasos[0][1] + 1] == 'F')
                {
                    printf("Mori por weta\n");
                    p = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    break;
                }
                else if (chale[payasos[0][0]][payasos[0][1] + 1] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        p = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] + 1] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'S';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] + 1] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                    }
                }
            }
            else if (x == 2)
            {
                if (chale[payasos[0][0] + 1][payasos[0][1]] == 'X' || payasos[0][0] == FILAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[0][0] + 1][payasos[0][1]] == 'S')
                {
                    chale[payasos[(int)*(int *)arg][0] + 1][payasos[0][1]] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                }
                else if (chale[payasos[0][0] + 1][payasos[0][1]] == 'F')
                {
                    printf("Mori por weta\n");
                    p = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    break;
                }
                else if (chale[payasos[0][0] + 1][payasos[0][1]] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        p = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0] + 1][payasos[(int)*(int *)arg][1]] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'S';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0] + 1][payasos[(int)*(int *)arg][1]] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                    }
                }
            }
            else if (x == 3)
            {
                if (chale[payasos[0][0]][payasos[0][1] - 1] == 'X' || payasos[0][1] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[0][0]][payasos[0][1] - 1] == 'F')
                {
                    printf("Mori por weta\n");
                    p = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    break;
                }
                else if (chale[payasos[0][0]][payasos[0][1] - 1] == 'S')
                {
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1] - 1] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                    payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                }
                else if (chale[payasos[0][0]][payasos[0][1] - 1] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        p = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[0][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'S';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] = 'P';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                    }
                }
            }
            break;
        case 1:
            if (s == 1)
            {
                break;
            }
            if (x == 0)
            {
                if (chale[payasos[1][0] - 1][payasos[1][1]] == 'X' || payasos[1][0] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[1][0] - 1][payasos[1][1]] == 'P')
                {
                    chale[payasos[(int)*(int *)arg][0] - 1][payasos[(int)*(int *)arg][1]] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                }
                else if (chale[payasos[1][0] - 1][payasos[1][1]] == 'F')
                {
                    printf("Mori por weta\n");
                    s = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    break;
                }
                else if (chale[payasos[1][0] - 1][payasos[1][1]] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Me encontre con un manifestante ingenuo, vere si puedo escapar con mi fake smile\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Debe escribir la nueva constitucion\n");
                        s = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0] - 1][payasos[(int)*(int *)arg][1]] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'P';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0] - 1][payasos[(int)*(int *)arg][1]] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] - 1;
                    }
                }
            }
            else if (x == 1)
            {
                if (chale[payasos[1][0]][payasos[1][1] + 1] == 'X' || payasos[1][1] == COLUMNAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[1][0]][payasos[1][1] + 1] == 'P')
                {
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] + 1] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                }
                else if (chale[payasos[1][0]][payasos[1][1] + 1] == 'F')
                {
                    printf("Mori por weta\n");
                    s = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    break;
                }
                else if (chale[payasos[1][0]][payasos[1][1] + 1] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        s = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] + 1] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'P';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] + 1] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] + 1;
                    }
                }
            }
            else if (x == 2)
            {
                if (chale[payasos[1][0] + 1][payasos[1][1]] == 'X' || payasos[1][0] == FILAS - 1)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[1][0] + 1][payasos[1][1]] == 'P')
                {
                    chale[payasos[(int)*(int *)arg][0] + 1][payasos[(int)*(int *)arg][1]] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                }
                else if (chale[payasos[1][0] + 1][payasos[1][1]] == 'F')
                {
                    printf("Mori por weta\n");
                    s = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    break;
                }
                else if (chale[payasos[1][0] + 1][payasos[1][1]] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        s = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0] + 1][payasos[(int)*(int *)arg][1]] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'P';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0] + 1][payasos[(int)*(int *)arg][1]] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][0] = payasos[(int)*(int *)arg][0] + 1;
                    }
                }
            }
            else if (x == 3)
            {
                if (chale[payasos[1][0]][payasos[1][1] - 1] == 'X' || payasos[1][1] == 0)
                {
                    printf("No me puedo mover a esa posicion\n");
                }
                else if (chale[payasos[1][0]][payasos[1][1] - 1] == 'F')
                {
                    printf("Mori por weta\n");
                    s = 1;
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    break;
                }
                else if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] == 'P')
                {
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] = 'B';
                    chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                    payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                }
                else if (chale[payasos[1][0]][payasos[1][1] - 1] == 'M')
                {
                    printf("Me van a pitiarme..\n");
                    int value;
                    printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                    printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                    printf("El numero es..\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    printf("...\n");
                    sleep(1);
                    value = movimiento(6) + 1;
                    printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n", value);
                    sleep(1 / 2);
                    if (value == 1 || value == 3)
                    {
                        printf("Escape!, Adios idiotas!\n");
                    }
                    else
                    {
                        printf("Wasted! Pucha la custion..\n");
                        s = 1;
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        break;
                    }
                }
                else
                {
                    if (chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] == 'B')
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = 'P';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                    }
                    else
                    {
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1] - 1] = 'S';
                        chale[payasos[(int)*(int *)arg][0]][payasos[(int)*(int *)arg][1]] = '*';
                        payasos[(int)*(int *)arg][1] = payasos[(int)*(int *)arg][1] - 1;
                    }
                }
            }
            break;
        }
        printf("\n__________________\n\n");
        print_chale();
        printf("\n");
        sleep(1);
        //TERMINO DE EJECUCION DEL THREAD EN CUESTION Y PASO AL SIGUIENTE
        if ((int)*(int *)arg == 0)
        {
            its_your_turn = 4;
            monPinExit();
        }
        if ((int)*(int *)arg == 1)
        {
            its_your_turn = 1;
            monSanExit();
        }
    }
    return NULL;
}
int main()
{
    int n1 = 0; //PIÑERA
    int n2 = 1; //SANDWICH
    pthread_t flaites, moderados, piraña, sandwich;
    set_chale();
    print_chale();
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&protect, NULL);
    pthread_create(&moderados, NULL, busqueda_moderados, NULL);
    pthread_create(&flaites, NULL, busqueda_flaites, NULL);
    pthread_create(&piraña, NULL, escape, (void *)&n1);
    pthread_create(&sandwich, NULL, escape, (void *)&n2);
    pthread_join(moderados, NULL);
    pthread_join(flaites, NULL);
    pthread_join(piraña, NULL);
    pthread_join(sandwich, NULL);
    pthread_mutex_destroy(&protect);
    pthread_mutex_destroy(&mutex);
    return 0;
}
