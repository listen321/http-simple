#ifndef __PTHREAD_POOL_H_
#define __PTHREAD_POOL_H_
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#define THREADNUM 3
typedef struct  task
{
	void (*hand)(void* s);
	void* arg;
	struct task* next;

}task_t;

typedef struct data
{
    pthread_mutex_t mtx;
    pthread_cond_t cond;
	task_t* list;
    pthread_t *ids;
	int _is_flag; 	
}data_t;

void init(data_t* data);
int addtask(data_t* data, void (*fun)(void *s), void* a);
void clear(task_t* head);
void* routine(void* arg);
void singnal_pool(data_t* data);
int getsize(data_t* data);
void destroy(data_t* data);


#endif 
