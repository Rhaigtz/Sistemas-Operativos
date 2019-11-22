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
int main(int argc, char *argv[])
{
    char line[LINE_SIZE];
    memset(line, '_', LINE_SIZE - 1);
    while (1)
    {
        int answer;
        printf("%s\n", line);
        printf("ESCOJA UNA OPCIÓN(presione el número):\n");
        printf("1] Escribir una palabra en un archivo llamado letter.txt\n");
        printf("2] Imprimir el contenido del archivo letter.txt\n");
        printf("3] Presione cualquier otra tecla para finalizar este programa\n");
        scanf("%d", &answer);
        printf("%s\n", line);
        if (answer == 1)
        {
            int pid = fork();
            if (pid < 0)
            {
                perror("FORK ERROR\n");
                exit(1);
            }
            else if (pid == 0)
            {
                printf("%s\n", line);
                system("gcc -o wrt.out wrt.c");
                execl("wrt.out", "wrt.out", NULL);
            }
            else{
                wait(NULL);
            }
        }
        else if (answer == 2)
        {
            int pid = fork();
            if (pid < 0)
            {
                perror("FORK ERROR\n");
                exit(1);
            }
            else if (pid == 0)
            {
                printf("%s\n", line);
                printf("IMPRIMIENDO CONTENIDOS DE letter.txt:\n");
                char *myargs[3];
                myargs[0] = strdup("cat");
                myargs[1] = strdup("letter.txt");
                myargs[2] = NULL;
                execvp(myargs[0], myargs);
            }
            else
            {
                wait(NULL);
            }
        }
        else
        {
            printf("PROGRAMA FINALIZADO\n");
            break;
        }
        wait(NULL);
    }
    return 0;
}