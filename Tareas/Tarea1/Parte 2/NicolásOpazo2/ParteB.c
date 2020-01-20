#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define OR ||
#define LINE_SIZE (1 << 5)

int asamblea(){ // Funcion que recibe simplemente la eleccion
    int eleccion;
    printf("Proceso Alumno, indique que es lo que desea hacer\n");
    printf("1. Mantenerse en clases (PROCESO NORMAL)\n");
    printf("2. Irse a paro (PROCESO PARO)\n");
    printf("3. Irse a toma y a tomar simultaneamente (PROCESO TOMA)\n");
    printf("4. Finaliza la ejecucion y lo manda pa' la fonda tikitiki ti\n");
    scanf("%d",&eleccion);
    return eleccion;
}
int main(int argc, char *argv[]) {
    char line[LINE_SIZE];
    memset(line, '_', LINE_SIZE - 1);
    while(1){
        printf("%s\n", line);
        int choice = asamblea();
        pid_t alumno; //proceso main alumno
        switch (choice){
        case 1: //caso de proceso normal
            printf("%s\n", line);
            printf("Hola, soy el proceso alumno y pasare a ser normal, si me lo permite imprimire mis syscalls asociadas(Syscall fork()).\n");
            alumno = fork(); //subdividir el proceso alumno en un nuevo proceso 
            if (alumno == 0) {
                pid_t normal;
                printf("%s\n", line);
                printf("Ahora si soy normal.\n");
                printf("%s\n", line);
                printf("Voy a ejecutar system() y execl()\n");
                system("gcc result.c -o result\n");
                execl("result", "result", NULL);
            }
            else {
                printf("%s\n", line);
                printf("Soy el alumno esperando a que termine mi proceso normal con wait()\n");
                wait(NULL);
            }
            break;

        case 2: //caso proceso paro
            printf("%s\n", line);
            printf("Hola, soy el proceso alumno y pasare a ser paro, si me lo permite imprimire mis syscalls asociadas(Syscall fork()).\n");
            alumno = fork(); //subdividir proceso alumno para transformarlo en un nuevo proceso paro
            if (alumno == 0) {
                pid_t paro;
                printf("%s\n", line);
                printf("Soy paro y estoy en paro jaja saludos\n");
                printf("Se cierran los libros! nos vamos a paro (Syscall exit(), open() and close()).\n");
                close(open("librito_de_clase.txt", O_CREAT | O_WRONLY | O_APPEND, 0644));
                exit(0);
            }
            else{
                printf("Soy el proceso alumno esperando a que termine paro.(Syscall wait())\n");
                wait(NULL);
            }
            break;

        case 3: //caso proceso toma
            printf("%s\n", line);
            printf("Hola,soy el proceso alumno y pasare a ser toma, si me lo permite imprimire mis syscalls asociadas(Syscall fork())\n");
            alumno = fork(); //subdividir proceso alumno para transformarlo en el proceso toma
            if (alumno == 0) {
                printf("%s\n", line);
                printf("Revolucion!, ahora estoy en toma!(Syscall execvp(), strdup())\n");
                pid_t toma;
                printf("%s\n", line);
                printf("\n");
                char *myargs[3];
                myargs[0] = strdup("cat");
                myargs[1] = strdup("librito_de_clase.txt");
                myargs[2] = NULL;

                execvp(myargs[0], myargs);
            }
            else {
                printf("Soy el proceso alumno esperando a que suelten la toma, no quiero super examen :( (Syscall wait())\n");
                wait(NULL);
            }
            break;

        case 4: //caso de salida del proceso
            printf("%s\n", line);
            printf("Hasta la proxima joven estudiante!");
            return 0;
            break;
        }
    }

    

    return 0;
}