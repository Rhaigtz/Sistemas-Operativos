#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>


void change_values(int *values,int len){
    int size;
    for(int i = 0; i < 5;i++)
        scanf("%d\n",values);
}
int main(){
    int values[5] ={0,0,0,0,0};
    change_values(values,5);
    for (int i = 0;i<5;i++)
        printf("%d\n",values[i]);
    return 0;
}