/*
*ESTE PROGRAMA LO DEJA ESCRIBIR UNA ORACIÓN EN UN ARCHIVO
*DE TEXTO LLAMADO LETTER.TXT.
*ABRE EL ARCHIVO, SI NO EXISTE LO CREA,
*UNA VEZ ABIERTO USTED INGRESA UNA ORACIÓN,
*UNA VEZ LA ORACIÓN HAYA SIDO INGRESADA, 
*ES ESCRITA EN EL ARCHIVO Y POSTERIORMENTE EL ARCHIVO ES
*CERRADO, TERMINANDO ASÍ ESTE PROGRAMA SU EJECUCIÓN.
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define MAX_STR (1 << 4) // 16
int main()
{
    int outfd = open("letter.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (!outfd)
    {
        perror("OPEN ERROR\n");
        exit(1);
    }
    char str_input[MAX_STR];
    printf("ESCRIBA UNA PALABRA, NO PUEDE TENER MAS DE 16 CHARS, TERMINE CON SALTO DE LINEA\n");
    scanf("%s", str_input);
    str_input[strlen(str_input) + 1] = '\0';
    str_input[strlen(str_input)] = '\n';
    write(outfd, str_input, strlen(str_input));
    close(outfd);
    printf("ORACIÓN GUARDADA EN EL ARCHIVO letter.txt\n");
    return 0;
}