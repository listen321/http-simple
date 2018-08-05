#include "pthread_pool.h"


void  init(data_t* data)
{
	pthread_mutex_init(&(data->mtx), NULL);
	pthread_cond_init(&(data->cond), NULL);
    data->list =(task_t*)malloc(sizeof(task_t));
	if(data->list == NULL)
	{
		perror("malloc");
		exit(1);
	}
	data->list->next = NULL;
	data->list->hand = NULL;
	data->list->arg = NULL;
    data->ids = (pthread_t*)malloc(sizeof(pthread_t)* THREADNUM);
    if(data->ids == NULL)
	{
		perror("ids malloc");
		exit(2);
	}	
   data->_is_flag = 0;
   int i = 0;
	for(; i < THREADNUM; ++i)
	{
      int ret =  pthread_create(data->ids+i, NULL, routine, (void*)data);
	  if(ret != 0)
	  {
	  	perror("pthread_create");
		exit(3);
	  }
	}	
}
int addtask(data_t* data, void (*fun)(void *s), void* a)
{
	pthread_mutex_lock(&data->mtx);
	task_t* new_node = (task_t*)malloc(sizeof(task_t));
	if(new_node == NULL)
	{
		return -1;
	}
	new_node->next = NULL;
	new_node->hand = fun;
	new_node->arg = a;
	task_t* pcur = data->list->next;
	if(pcur == NULL)
	{
	   data->list->next = new_node; 
	}
	else
	{
         while(pcur->next)
    	{
	 	 	pcur = pcur->next;
	   	}	
		
		 pcur->next = new_node;
	}
	pthread_mutex_unlock(&data->mtx);
	pthread_cond_signal(&data->cond);
//	printf("add\n");
	return 0;
}
int getsize(data_t* data)
{
	int sum = 0;
	task_t* head = data->list;
	task_t* pcur = head->next;
	while(pcur)
	{
	    sum++;
		pcur = pcur->next;
	}
	return sum;
}
void* routine(void* arg)
{
	data_t* data = (data_t*)arg;
	task_t* head = data->list;
	
	for(;;)
	{	
       	if(data->_is_flag == 1)
		{
		  //  printf("_is_flag\n");
          //  pthread_cond_broadcast(&(data->cond));
   			break;
  	 	}
	    else if(head->next == NULL)
		{
		//	printf("wait....^   ");
			pthread_cond_wait(&(data->cond), &(data->mtx));
			pthread_mutex_unlock(&(data->mtx));
	    //  printf("...wait");
				continue;
		}
            pthread_mutex_lock(&(data->mtx));
	       // printf("-----lock----\n");
			task_t* pcur = head->next;
			head->next = pcur->next;
			pthread_mutex_unlock(&(data->mtx));
	        pthread_cond_signal(&(data->cond));
			if(pcur)
			{
		     	pcur->hand(pcur->arg);
		        free(pcur);
				pcur = NULL;
			}	
	       // printf("-----unlock----\n");
	}

}

void clear(task_t* head)
{
	if(head == NULL)
			return;
	task_t* pcur = head->next;
	while(pcur)
	{
		head = pcur->next;
		free(pcur);
		if(head)
			pcur = head->next;
	}
    free(head);

}
void destroy(data_t* data)
{
	data->_is_flag = 1;
	pthread_cond_broadcast(&data->cond);
	int i = 0;
	for(; i < THREADNUM; ++i)
	{
		pthread_join(data->ids[i], NULL);
	}
	pthread_mutex_destroy(&(data->mtx));
	pthread_cond_destroy(&(data->cond));
	clear(data->list);
}
//typedef struct hand
//{
//	int x;
//	int y;
//}hand_t;
//
//void show(void *a)
//{
//	if(a)
//	{
//	    hand_t* p = (hand_t*)a;
//		printf("hehe::%d  %d\n" , p->x,p->y);
//    }
//}
//void fun(void *a)
//{
//	printf("hahaaaaaaaaaa\n");
//}
//int main()
//{
//	data_t data;
//	hand_t h;
//	h.x = 1;
//	h.y = 2;
//	init(&data);
//    addtask(&data, show, &h);	
//    addtask(&data, show, &h);	
//    addtask(&data, show, NULL);	
//    addtask(&data, fun, NULL);	
//    addtask(&data, fun, NULL);	
//    addtask(&data, show, NULL);	
//    addtask(&data, show, NULL);	
//    sleep(1);
//    printf("sleep\n");
//    addtask(&data, fun, NULL);	
//    addtask(&data, fun, NULL);	
//    addtask(&data, fun, NULL);	
//    addtask(&data, show, NULL);	
//    sleep(3);
////  while(1)
////  {
////	addtask(&data, show, NULL);
////	sleep(1);
////  }
//	destroy(&data);
//	return 0;
//}
