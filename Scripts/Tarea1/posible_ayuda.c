#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<math.h>

void par_hijos(){
    pid_t process = fork();
    if(n == counter){
        exit(0);
    }   
    if(process == 0)
    {   
        printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
        exit(0); 
    }
    for(int i=0;i<2;i++)
        wait(NULL);     
}
//volatile int levels = 1;
volatile int count = 1;
int main()
{

    pid_t t1,t2,process;
    int i,n;
    process=getpid();                
    printf("enter the number of levels\n");
    scanf("%d",&n);
    printf("root %d\n",process);
    for(i = 1;i<n;i++){
        t1=fork();
        if(t1 == 0){
            count++;
            if(count == n)
                break;
        }
        else{
            t2=fork();
            if(t2==0){
                count= count+1;
            }
        }
        if(t1>0 && t2>0)        
            break;
        printf("child pid %d   parent pid %d\n counter: %d\n",getpid(),getppid(),count);
    }
    waitpid(t2,NULL,0);
    waitpid(t1,NULL,0);
    return 0;

}