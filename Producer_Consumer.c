#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct{
	int time_to_sleep;
}t_arg;

typedef struct{
	int container[5];
	int size;
}queue_t;

queue_t queue = {{},0};

void * produce(void * p_arg);
void * consume(void * c_arg);
void offer(int item);
int poll();
void printQueueContent();

int main (int argc, char ** argv){
	
	if(argc != 3){
		printf("Usage: <time_to_wait_consumer> <time_to_wait_producer>\n");
		return 1;
	}
	
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond,NULL);
	pthread_t consumer,producer;
	t_arg consumer_arg = {atoi(argv[1])};
	t_arg producer_arg = {atoi(argv[2])};
	
	
	pthread_create(&consumer,NULL,consume,&consumer_arg);
	pthread_create(&producer,NULL,produce,&producer_arg);
	
	pthread_join(consumer,NULL);
	pthread_join(producer,NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}

void * produce(void * p_arg){
	t_arg * arg = (t_arg*) p_arg;
	for(int i = 0; i <= 21; i++){
		pthread_mutex_lock(&mutex);
        
        while(queue.size >= 5){
            printf("Producer is blocking!\n");
            pthread_cond_wait(&cond,&mutex); 
            printf("Producer is no longer blocking!\n");
        }
        
        offer(i);
		printQueueContent();
        
        pthread_cond_signal(&cond); 
		pthread_mutex_unlock(&mutex);
        
		sleep(arg->time_to_sleep);
	}
	printf("The Producer is finished producing!\n");
	return NULL;
}

void * consume(void * c_arg){
	t_arg * arg = (t_arg*) c_arg;
	int item;
	do{
		pthread_mutex_lock(&mutex);
        
        while(queue.size <= 0){
            printf("Consumer is blocking!\n");
            pthread_cond_wait(&cond,&mutex); 
            printf("Consumer is no longer blocking!\n");
        }
        
        item = poll();
		printQueueContent();
		
        pthread_cond_signal(&cond); 
        pthread_mutex_unlock(&mutex);
        
		sleep(arg->time_to_sleep);
	} while(item != 21);
	printf("The Consumer is finished consuming!\n");
	return NULL;	
}

/*
 * Offers an item to be inserted into the queue at it's end
*/
void offer(int item){
	queue.container[queue.size] = item;
	queue.size++;
}

/*
 * Removes and returns the first item of the queue
*/
int poll(){
	int return_val = queue.container[0];
	for(int i = 1; i < queue.size; i++)
		queue.container[i-1] = queue.container[i];
    queue.size--;
	return return_val;
}

/*
 * Utility print function
*/
void printQueueContent(){
    printf("Queue of size %d:\t", queue.size); 
	for(int i = 0; i < queue.size-1; i++)
		printf("%d -- ", queue.container[i]);
	if(queue.size > 0) 
        printf("%d",queue.container[queue.size-1]);
    printf("\n");
}

