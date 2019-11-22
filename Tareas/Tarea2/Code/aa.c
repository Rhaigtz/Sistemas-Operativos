// C program to demonstrate working of Semaphores 
#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

sem_t *bloqueo_moderados; 

void* thread(void* arg) 
{ 
	//wait 
	sem_wait(bloqueo_moderados); 
	printf("\nEntered..\n"); 

	//critical section 
	sleep(4); 
	
	//signal 
	printf("\nJust Exiting...\n"); 
	sem_post(bloqueo_moderados); 
} 


int main() 
{ 
   	sem_unlink("bloqueo_moderados");
    bloqueo_moderados = sem_open("bloqueo_moderados", O_CREAT, 0644, 5);
	pthread_t t1,t2; 
	pthread_create(&t1,NULL,thread,NULL); 
	sleep(2); 
	pthread_create(&t2,NULL,thread,NULL); 
	pthread_join(t1,NULL); 
	pthread_join(t2,NULL); 
    sem_unlink("bloqueo_moderados");
	return 0; 
} 
