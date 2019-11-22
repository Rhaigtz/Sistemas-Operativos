/* JEROGLIFICO 4 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 3
void child(int i)
{
    int pid;
    printf("HIJO %d (pid %d) DEL PADRE (pid %d) \n", i,  getpid(), getppid());
    if (i == MAX)
        return;
    pid = fork();
    if (pid < 0)
        printf("ERROR\n");
    else if (pid == 0)
        child(++i);
    else
        waitpid(pid, NULL, 0);
    exit(0);
}

int main() {
    FILE *notas;
    char cadena[] = "Notas de Sistemas Opeativos: 55-45-23-56-60.\n";
    notas = fopen ( "notaso.txt", "a" );
    fwrite( cadena, sizeof(char), sizeof(cadena), notas ); 
    fclose ( notas );
    printf("Soy la Universidad! y ahora hare mi alumno prodigio (PID %d) \n", getpid());
    int i = 0;
    printf("SYSCALL: Fork para hacer alumno\n");
    int pid = fork();
    if (pid < 0){
        printf("ERROR\n");
        return 1;
    }
    else if (pid == 0)
    {
      printf("Hola soy el alumno (PID %d), por favor decide que haremos en la Universidad: \n", getpid());
      printf("1. Normal\n");
      printf("2. Paro\n");
      printf("3. Toma\n");
      char caracter;
      int aux, t=0;
      scanf("%d",&aux);
      while(t==0)
      {
        switch(aux)
        {
          case 1:
            //nos vamos Normal
            notas = fopen ( "notaso.txt", "a+t" );
            printf("\nIntroduce un texto al fichero: ");
            while((caracter = getchar()) != '\n')
            {
                printf("%c", fputc(caracter, notas));
            }
            fclose ( notas );

            t++;
            break;
          case 2:
            //Nos vamos a paro
            t++;
            break;
          case 3:
            //nos vamos a Toma
            t++;
            break;
          default:
          printf("ingrese una opcion correcta\n");
          scanf("%d",&aux);
        }
      }

    }
    else
      waitpid(pid, NULL, 0);
    return 0;
}