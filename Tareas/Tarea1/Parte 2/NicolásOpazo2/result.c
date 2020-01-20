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
    int librito = open("librito_de_clase.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    int i = 0;
    printf("Syscall open(),write() and close()\n");
    if (!librito){
        perror("OPEN ERROR\n");
        exit(1);
    }
    char str_input[MAX_STR];
    while(i == 0){
        printf("Escriba una palabra que desee escribir en su librito de clases\n");
        scanf("%s", str_input);
        str_input[strlen(str_input) + 1] = '\0';
        str_input[strlen(str_input)] = '\n';
        write(librito, str_input, strlen(str_input));
        printf("Palabra almacenada en su libro de clases, vayase a estudiar shonen\n");
        printf("Desea seguir escribiendo en su librito? (ingrese 0 para mantenerse escribiendo en el proceso, cualquier otro numero para salir del proceso)\n");
        scanf("%d", &i);
    }
    close(librito);
    return 0;
}