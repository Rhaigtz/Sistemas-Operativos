#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#define FLAITES 2
#define MODERADOS 4
#define FILAS 5
#define COLUMNAS 5
                                    // F = FLAITES , M = MODERADOS, P = PIRAÑA, S= GUATON CULIAO, X= ZONAS DESTRUIDAS
pthread_cond_t cond_gen = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_gen2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_gen3 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex;
pthread_mutex_t protect;
char chale[FILAS][COLUMNAS];
int moderados[MODERADOS][2];
int flaites[FLAITES][2];
int payasos[2][2];
int p,s;
int its_your_turn = 1;

uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int posicion_aleatoria_x(){
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % FILAS);
}
int posicion_aleatoria_y(){
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % COLUMNAS);
}
int movimiento(int num){   // 0: ARRIBA, 1: DERECHA, 2:ABAJO, 3: IZQUIERDA
    srand(rdtsc());
    //srand((unsigned)time(NULL));
    return (rand() % num);   
}
void set_chale(){
    int i,j,x,y;
    for(i = 0;i<FILAS;++i){                //Marcar los espacios vacios de chale con un *
        for(j=0;j<COLUMNAS;++j){
            chale[i][j] = '*';
        }
    }
    for(i = 0;i<FLAITES;++i){              //Posicionar de manera aleatorea a los flaites en chale
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        if(chale[x][y] == '*'){
            chale[x][y] = 'F';
            flaites[i][0]=x;               //Almacenar la posicion de cada flaite una vez fue posicionado
            flaites[i][1]=y;
        }
        else
            --i;
    }
    
    for(i = 0; i<MODERADOS; i++){         //Posicionar de manera aleatorea a los moderados en chale
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        if(chale[x][y] == '*'){
            chale[x][y] = 'M';
            moderados[i][0] = x;          //Almacenar la posicion de cada moderado una vez fue posicionado
            moderados[i][1] = y;
        }
        else
            --i;        
    }
    for(i=0;i<2;++i){
        x = posicion_aleatoria_x();
        y = posicion_aleatoria_y();
        switch(i){
            case 0:
                if(chale[x][y]=='*'){
                    chale[x][y] = 'P';
                    payasos[0][0] = x;
                    payasos[0][1] = y;
                }
                else
                    --i;
                break;
            case 1:
                if(chale[x][y]=='*'){
                    chale[x][y] = 'S';
                    payasos[1][0] = x;
                    payasos[1][1] = y;
                }
                else
                    --i;
                break;
        }
    }
}
void print_chale(){
    int x, y;
    for (x = 0; x < FILAS; ++x)
        for (y = 0; y < COLUMNAS; ++y)
            printf("%c%c", chale[x][y], (y == COLUMNAS-1) ? '\n' : ' ');
}

void* busqueda_moderados(void *arg){    //FUNCION CON LA CUAL LOS THREADS MODERADOS REALIZAN SU BUSQUEDA DE LOS PAYASOS
   // sem_wait(bloqueo_moderados);        //SE UTILIZA LA FUNCION RAND() PARA ESCOJER LA DIRECCION DE MOVIMIENTO 0: ARRIBA, 1: DERECHA,                                            2:ABAJO, 3: IZQUIERDA
    while(p != 1 ){
        pthread_mutex_lock(&mutex);
        if(its_your_turn != 1){
                pthread_cond_wait(&cond_gen3,&mutex);
        }
        else{
            pthread_mutex_lock(&protect);
            for(int i = 0; i < MODERADOS; ++i){
                int x = movimiento(4);
                if(p == 1){
                    pthread_exit(NULL);
                }
                if(x == 0){
                    if(chale[moderados[i][0]-1][moderados[i][1]] == 'X' || chale[moderados[i][0]-1][moderados[i][1]] == 'F' || chale[moderados[i][0]-1][moderados[i][1]]=='M' || moderados[i][0] == 0){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[moderados[i][0]-1][moderados[i][1]] == 'S' || chale[moderados[i][0]-1][moderados[i][1]] == 'P'){
                        int value;
                        printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                        printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                        printf("El numero es..\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escapo! mejor suerte para la proxima\n");
                        else {
                            printf("Wasted! Se queda escribiendo una nueva constitucion.\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[moderados[i][0]-1][moderados[i][1]] = 'M';
                        chale[moderados[i][0]][moderados[i][1]] = '*';
                        moderados[i][0] = moderados[i][0]-1;   
                    }
                }
                if(x == 1){
                    if(chale[moderados[i][0]][moderados[i][1]+1] == 'X' || chale[moderados[i][0]][moderados[i][1]+1] == 'F' || chale[moderados[i][0]][moderados[i][1]+1]=='M' || moderados[i][1] == COLUMNAS -1){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[moderados[i][0]][moderados[i][1]+1] == 'S' || chale[moderados[i][0]][moderados[i][1]+1] == 'P'){
                        int value;
                        printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                        printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                        printf("El numero es..\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escapo! mejor suerte para la proxima\n");
                        else {
                            printf("Wasted! Se queda escribiendo una nueva constitucion.\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[moderados[i][0]][moderados[i][1]+1] = 'M';
                        chale[moderados[i][0]][moderados[i][1]] = '*';
                        moderados[i][1] = moderados[i][1]+1;   
                    }
                }
                if(x == 2){
                    if(chale[moderados[i][0]+1][moderados[i][1]] == 'X' || chale[moderados[i][0]+1][moderados[i][1]] == 'F' || chale[moderados[i][0]+1][moderados[i][1]]=='M' || moderados[i][0] == FILAS -1){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[moderados[i][0]+1][moderados[i][1]] == 'S' || chale[moderados[i][0]+1][moderados[i][1]] == 'P'){
                        int value;
                        printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                        printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                        printf("El numero es..\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escapo! mejor suerte para la proxima\n");
                        else {
                            printf("Wasted! Se queda escribiendo una nueva constitucion.\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[moderados[i][0]+1][moderados[i][1]] = 'M';
                        chale[moderados[i][0]][moderados[i][1]] = '*';
                        moderados[i][0] = moderados[i][0]+1;   
                    }
                }
                if(x == 3){
                    if(chale[moderados[i][0]][moderados[i][1]-1] == 'X' || chale[moderados[i][0]][moderados[i][1]-1] == 'F' || chale[moderados[i][0]][moderados[i][1]-1]=='M' || moderados[i][1] == 0){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[moderados[i][0]-1][moderados[i][1]] == 'S' || chale[moderados[i][0]-1][moderados[i][1]] == 'P'){
                        int value;
                        printf("Encontre a uno de los payasos, vere si lo puedo atrapar sin que se escape\n");
                        printf("(Tirar dados! si obtienes un 1 o un 3 se escapa Piraña/Sandwich cualquier otro caso, es atrapado y debe escribir la nueva constitucion.)\n");
                        printf("El numero es..\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        printf("...\n");
                        sleep(1);
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value == 1 || value == 3 )
                            printf("Escapo! mejor suerte para la proxima\n");
                        else {
                            printf("Wasted! Se queda escribiendo una nueva constitucion.\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[moderados[i][0]][moderados[i][1]-1] = 'M';
                        chale[moderados[i][0]][moderados[i][1]] = '*';
                        moderados[i][1] = moderados[i][1]-1;   
                    }
                }
                printf("\n__________________\n\n");
                print_chale();
                printf("\n");
                sleep(3/2);
            }
            its_your_turn = 2;
            pthread_mutex_unlock(&protect);
        }
        pthread_cond_signal(&cond_gen);
        pthread_mutex_unlock(&mutex);
    } 
   // sem_post(bloqueo_moderados);
    pthread_exit(NULL);
}

void* busqueda_flaites(void *arg){    //FUNCION CON LA CUAL LOS THREADS flaites REALIZAN SU BUSQUEDA DE LOS PAYASOS
    // sem_wait(bloqueo_moderados);        //SE UTILIZA LA FUNCION RAND() PARA ESCOJER LA DIRECCION DE MOVIMIENTO 0: ARRIBA, 1: DERECHA,                                            2:ABAJO, 3: IZQUIERDA
    while(p != 1){
        pthread_mutex_lock(&mutex);
        if(its_your_turn != 2){
            pthread_cond_wait(&cond_gen,&mutex);
        }
        else {
            pthread_mutex_lock(&protect);
            for(int i = 0; i < FLAITES; ++i){
                int x = movimiento(4);
                if(p == 1){
                    pthread_exit(NULL);
                }
                if(x == 0){
                    if(chale[flaites[i][0]-1][flaites[i][1]] == 'F' || chale[flaites[i][0]-1][flaites[i][1]]=='M' || flaites[i][0] == 0){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[flaites[i][0]-1][flaites[i][1]] == 'S' || chale[flaites[i][0]-1][flaites[i][1]] == 'P'){
                        printf("Me pitie a uno de los payasos.. ups\n");
                        p=1;
                    }
                    else{
                        chale[flaites[i][0]-1][flaites[i][1]] = 'F';
                        chale[flaites[i][0]][flaites[i][1]] = 'X';
                        flaites[i][0] = flaites[i][0]-1;   
                    }
                }
                else if(x == 1){
                    if(chale[flaites[i][0]][flaites[i][1]+1] == 'F' || chale[flaites[i][0]][flaites[i][1]+1]=='M' || flaites[i][1] == COLUMNAS -1){
                        printf("No me puedo mover a esa posicion \n");
                    }
                    else if(chale[flaites[i][0]][flaites[i][1]+1] == 'S' || chale[flaites[i][0]][flaites[i][1]+1] == 'P'){
                        printf("Me pitie a uno de los payasos.. ups\n");
                        p=1;
                    }
                    else{
                        chale[flaites[i][0]][flaites[i][1]+1] = 'F';
                        chale[flaites[i][0]][flaites[i][1]] = 'X';
                        flaites[i][1] = flaites[i][1]+1;   
                    }
                }
                else if(x == 2){
                    if(chale[flaites[i][0]+1][flaites[i][1]] == 'F' || chale[flaites[i][0]+1][flaites[i][1]]=='M' || flaites[i][0] == FILAS -1){
                        printf("No me puedo mover a esa posicion posicion\n");
                    }
                    else if(chale[flaites[i][0]+1][flaites[i][1]] == 'S' || chale[flaites[i][0]+1][flaites[i][1]] == 'P'){
                        printf("Me pitie a un payaso.. ups\n");
                        p=1;
                    }
                    else{
                        chale[flaites[i][0]+1][flaites[i][1]] = 'F';
                        chale[flaites[i][0]][flaites[i][1]] = 'X';
                        flaites[i][0] = flaites[i][0]+1;   
                    }
                }
                else if(x == 3){
                    if(chale[flaites[i][0]][flaites[i][1]-1] == 'F' || chale[flaites[i][0]][flaites[i][1]-1]=='M' || flaites[i][1] == 0){
                        printf("No me puedo mover a esa posicion posicion\n");
                    }
                    else if(chale[flaites[i][0]][flaites[i][1]-1] == 'S' || chale[flaites[i][0]][flaites[i][1]-1] == 'P'){
                        printf("Me pitie a un payaso.. ups\n");
                        p=1;
                    }
                    else{
                        chale[flaites[i][0]][flaites[i][1]-1] = 'F';
                        chale[flaites[i][0]][flaites[i][1]] = 'X';
                        flaites[i][1] = flaites[i][1]-1;   
                    }
                }
                printf("\n__________________\n\n");
                print_chale();
                printf("\n");
                sleep(3/2);
            }
            its_your_turn = 3;
            pthread_mutex_unlock(&protect);
        }
        pthread_cond_signal(&cond_gen2);
        pthread_mutex_unlock(&mutex);
    }
    // sem_post(bloqueo_moderados);
    pthread_exit(NULL);
}

void* escape(void *arg){
    while(p != 1){
        pthread_mutex_unlock(&mutex);
        if(its_your_turn != 3){
            pthread_cond_wait(&cond_gen2,&mutex);
        }
        else{
            pthread_mutex_lock(&mutex);
            for(int i=0; i < 2; ++i){
                int x = movimiento(4);
                if(p == 1){
                    pthread_exit(NULL);
                }
                if(x == 0){
                    if(chale[payasos[i][0]-1][payasos[i][1]] == 'X' || chale[payasos[i][0]-1][payasos[i][1]] == 'S' ||payasos[i][0] == 0){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[payasos[i][0]-1][payasos[i][1]] == 'F' || chale[payasos[i][0]-1][payasos[i][1]]=='M'){
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
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3)
                            printf("Escape!, Adios idiotas!\n");
                        else {
                            printf("Debe escribir la nueva constitucion\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[payasos[i][0]-1][payasos[i][1]] = 'P';
                        chale[payasos[i][0]][payasos[i][1]] = '*';
                        payasos[i][0] = payasos[i][0]-1;   
                    }
                }
                else if(x == 1){
                    if(chale[payasos[i][0]][payasos[i][1]+1] == 'X' || chale[payasos[i][0]][payasos[i][1]+1] == 'S' || payasos[i][1] == COLUMNAS-1){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[payasos[i][0]][payasos[i][1]+1] == 'F' || chale[payasos[i][0]][payasos[i][1]+1]=='M'){
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
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escape!, Adios idiotas!\n");
                        else {
                            printf("Wasted! Pucha la custion..\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[payasos[i][0]][payasos[i][1]+1] = 'P';
                        chale[payasos[i][0]][payasos[i][1]] = '*';
                        payasos[i][1] = payasos[i][1]+1;   
                    }
                }
                else if(x == 2){
                    if(chale[payasos[i][0]+1][payasos[i][1]] == 'X' || chale[payasos[i][0]+1][payasos[i][1]] == 'S' || payasos[i][0] == FILAS-1){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[payasos[i][0]+1][payasos[i][1]] == 'F' || chale[payasos[i][0]+1][payasos[i][1]]=='M'){
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
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escape!, Adios idiotas!\n");
                        else {
                            printf("Wasted! Pucha la custion..\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[payasos[i][0]+1][payasos[i][1]] = 'P';
                        chale[payasos[i][0]][payasos[i][1]] = '*';
                        payasos[i][0] = payasos[i][0]+1;   
                    }
                }

                else if(x == 3){
                    if(chale[payasos[i][0]][payasos[i][1]-1] == 'X' || chale[payasos[i][0]][payasos[i][1]-1] == 'S' || payasos[i][1] == 0){
                        printf("No me puedo mover a esa posicion\n");
                    }
                    else if(chale[payasos[i][0]][payasos[i][1]-1] == 'F' || chale[payasos[i][0]][payasos[i][1]-1]=='M'){
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
                        value = movimiento(6)+1;
                        printf("El valor del dado fue de: %d, por lo tanto el susodicho...\n",value);
                        sleep(1/2);
                        if(value ==1 || value == 3 )
                            printf("Escape!, Adios idiotas!\n");
                        else {
                            printf("Wasted! Pucha la custion..\n");
                            p = 1;
                        }
                    }
                    else{
                        chale[payasos[i][0]][payasos[i][1]-1] = 'P';
                        chale[payasos[i][0]][payasos[i][1]] = '*';
                        payasos[i][1] = payasos[i][1]-1;   
                    }
                }
                printf("\n__________________\n\n");
                print_chale();
                printf("\n");
                sleep(3/2);
            }
            its_your_turn = 1;
            pthread_mutex_unlock(&mutex);
        }
        pthread_cond_signal(&cond_gen3);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}


int main(){
    pthread_t flaites, moderados,piraña;
    set_chale();
    print_chale();
    pthread_mutex_init(&protect,NULL);
    pthread_mutex_init(&mutex,NULL);
   // sem_unlink("lock");
    //lock = sem_open("lock", O_CREAT, 0644, 1);
    pthread_create(&moderados,NULL,busqueda_moderados,NULL);
    pthread_create(&flaites,NULL,busqueda_flaites,NULL);
    pthread_create(&piraña,NULL,escape,NULL);
    // pthread_create(&sandwich,NULL,escape,NULL);
    pthread_join(moderados,NULL);
    pthread_join(flaites,NULL);
    pthread_join(piraña, NULL);
    // pthread_join(flaites, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&protect);
    //sem_unlink("lock");
    // pthreaad_create()
    return 0;
}