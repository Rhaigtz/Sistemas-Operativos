/* JEROGLIFICO 4 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void child(i,n,nodo_actual){ //Funcion para encadenar hijos recursivamente
    if (nodo_actual  >= n+1){
        return;
    }
    printf("NIVEL DEL ARBOL: %d, PID:(%d), PID PADRE (%d), SOY EL NODO Nª: %d \n", i,  getpid(), getppid(),nodo_actual);
    pid_t pid= fork();
    if(pid >0){
        pid = fork();
        if (pid < 0)
            printf("ERROR\n");
        else if (pid == 0){
            nodo_actual=2*nodo_actual;
            i = i+1;
            child(i,n,nodo_actual);
        }
        waitpid(pid, NULL, 0);
        exit(0);
    }
    else {
        nodo_actual=(2*nodo_actual)+1;
        i = i+1;
        child(i,n,nodo_actual);
    }
}
int main() {
    int n,nodo_actual = 1,i = 0;
    printf("Ingrese la cantidad de procesos que desea tener en su ABB\n");
    scanf("%d",&n);
    printf("SOY EL PROCESO MAIN (PID %d) ESTOY EN EL NIVEL %d DEL ARBOL, SOY EL NODO Nº%d\n", getpid(), i,nodo_actual);
    pid_t pid = fork(); // Proceso Main se duplica
    if(pid >0){
        pid = fork(); // Proceso Main tiene 2 hijos
        if(pid == 0){
            nodo_actual=(nodo_actual)+1;
            i = i+1;    
            child(i,n,nodo_actual);
        }
    waitpid(pid,NULL,0);
    }
    else if(pid == 0){
        nodo_actual=nodo_actual+2;
        i = i+1;
        child(i,n,nodo_actual);
    }
    waitpid(pid, NULL, 0);
    return 0;
}








