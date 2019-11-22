#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<math.h>
volatile int counter = 1;

void par_hijos(){
for(int i=0;i<2;i++) // loop will run n times (n=5) 
    { 
        if(fork() == 0) 
        { 
            printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
            exit(0);
        } 
    } 
    // for(int i=0;i<2;i++) // loop will run n times (n=5) 
    //     wait(NULL); 
    return;
}
int main()
{

    pid_t t1,t2,process;
    int n;
    process=getpid();                
    printf("enter the number of levels\n");
    scanf("%d",&n);
    printf("root %d\n",process);
    for(int i = 1;i<n;i++){
        t1=fork();

        if(t1 > 0){
            t2=fork();
  //          par_hijos();
            if (t1>0 && t2 == 0){
                par_hijos();
            }
            else
                wait(NULL);
            
        }
        if(t1>0&&t2>0)        
            break;
    }
    waitpid(t1,NULL,0);
    waitpid(t1,NULL,0);
    return 0;
}