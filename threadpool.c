/**
 * threadpool.c
 *
 * This file will contain your implementation of a threadpool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../pthreads/mypthread.h"

#include "threadpool.h"

// _threadpool is the internal threadpool structure that is
// cast to type "threadpool" before it given out to callers

typedef struct task_st {
    void (*routine) (void *);
    void *arg;
    struct task_st* next;
} task_t;

typedef struct _threadpool_st {
   // you should fill in this structure with whatever you need
   int available_threads;
   pthread_mutex_t mutex;
   pthread_cond_t q_empty;
   pthread_cond_t q_not_empty;
   pthread_t *threads;
   int tsize;
   task_t *thead;
   task_t *ttail;
} _threadpool;

void *thread_work(threadpool ptemp)
{
  _threadpool *pool = (_threadpool *)ptemp;
  task_t *current;   

  pthread_mutex_lock(&(pool->mutex));

  while (1) {
     while(pool->tsize == 0)
     {
         pthread_cond_wait(&(pool->q_not_empty), &(pool->mutex));
     }
  
     current = pool->thead;
     pool->tsize--;
     if (pool->tsize == 0) {
       pool->thead = NULL;
       pool->ttail = NULL;
     } 
     else
     {
        pool->thead = current->next;
     }

    /*  
    if (pool->tsize == 0)
    {
       pthread_cond_signal(&(pool->q_empty));
    }
    */

    //pool->available_threads--;
    pthread_mutex_unlock(&(pool->mutex)); 
    (current->routine) (current->arg);

    /*
    pthread_mutex_lock(&(pool->mutex));
    pool->available_threads++; 
    pthread_cond_signal(&(pool->q_empty));
    pthread_mutex_unlock(&(pool->mutex));
    */

    free(current);
  }
}

threadpool create_threadpool(int num_threads_in_pool) {

  _threadpool *pool;
  int i;

  // sanity check the argument
  if ((num_threads_in_pool <= 0) || (num_threads_in_pool > MAXT_IN_POOL))
    return NULL;

  pool = (_threadpool *) malloc(sizeof(_threadpool));
  if (pool == NULL) {
    fprintf(stderr, "Out of memory creating a new threadpool!\n");
    return NULL;
  }

  // add your code here to initialize the newly created threadpool
  pool->available_threads = num_threads_in_pool;
  pool->tsize = 0;
  pool->thead = NULL;
  pool->ttail = NULL;
  pool->threads = (pthread_t *)malloc(num_threads_in_pool * sizeof(pthread_t));
  pthread_mutex_init(&(pool->mutex), NULL);
  pthread_cond_init(&(pool->q_empty), NULL);
  pthread_cond_init(&(pool->q_not_empty), NULL);

  for (i=0; i< pool->available_threads ; i++)
  {
     if(pthread_create(&(pool->threads[i]), NULL, thread_work, pool) != 0)
     {
        fprintf(stderr, "Error during threadpool creation!\n");  
        return NULL;
     }
  }

  return (threadpool) pool;
}


void dispatch(threadpool from_me, dispatch_fn dispatch_to_here,
	      void *arg) {
  _threadpool *pool = (_threadpool *) from_me;

  // add your code here to dispatch a thread
  //printf("Inside dispatch\n");
  task_t *cur_task;
  
  cur_task = (task_t *)malloc(sizeof(task_t));
  if (cur_task == NULL)
  {
      fprintf(stderr, "Error allocating space for task\n");
      return;
  }

  cur_task->routine = dispatch_to_here;
  cur_task->arg = arg;
  cur_task->next = NULL;

  pthread_mutex_lock(&(pool->mutex));
   
  if (pool->tsize == 0)
  {
     pool->tsize++;
     pool->thead = cur_task;
     pool->ttail = cur_task;
     pthread_cond_signal(&(pool->q_not_empty));
     //pthread_cond_wait(&(pool->q_empty), &(pool->mutex));
  }
  else
  {
     pool->tsize++;
     pool->ttail->next = cur_task;
     pool->ttail = cur_task;
     pthread_cond_signal(&(pool->q_not_empty));
     //pthread_cond_wait(&(pool->q_empty), &(pool->mutex));
  }

  /*
  while(pool->available_threads == 0)
  {
     //pthread_cond_signal(&(pool->q_not_empty));
     pthread_cond_wait(&(pool->q_empty), &(pool->mutex));
  }
  */

  pthread_mutex_unlock(&(pool->mutex));

}
/*
  pthread_mutex_lock(&(pool->mutex));
  printf("After pthread_mutex_lock\n");
  if (!pool->available_threads)
  {
      pthread_cond_wait(&(pool->cond_var), &(pool->mutex));
  }
  printf("After pthread_cond_wait\n"); 
  pool->available_threads--;
  printf("Available threads = %d", pool->available_threads);
  pthread_mutex_unlock(&(pool->mutex)); 
  pthread_create(*(pool->threads + pool->available_threads), NULL, (void *)dispatch_to_here , arg);
  return;
}
*/

void destroy_threadpool(threadpool destroyme) {
  _threadpool *pool = (_threadpool *) destroyme;

  // add your code here to kill a threadpool
  pthread_mutex_destroy(&(pool->mutex));
  pthread_cond_destroy(&(pool->q_empty));
  pthread_cond_destroy(&(pool->q_not_empty));
  pool->available_threads = 0;
  free(pool->threads);
  free(pool);
  return; 
}

