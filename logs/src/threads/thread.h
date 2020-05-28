
#ifndef __THREAD__H
#define __THREAD__H

#include <pthread.h>

typedef void* (*runnable)(void*) ;

struct __common_thread;
typedef struct __common_thread common_thread_t;

struct __common_thread_mutex;
typedef struct __common_thread_mutex common_thread_mutex_t;

struct __common_thread_cond;
typedef struct __common_thread_cond common_thread_cond_t;

enum Thread_mutex_type{DEFAULT,DEADLOCK_NOCHECK,ERROR_CHECK,RECURSIVE_ENABLE};

common_thread_t* thread_create_obj(int detachable);
void thread_destory_obj(common_thread_t*);

int thread_start(common_thread_t* thread_ptr,
                  runnable thread_run,void* run_arg);
void thread_join(common_thread_t* thread_ptr,void** rcvReturn);
void thread_cancel_someone(common_thread_t* thread_ptr);
void thread_exit_current(common_thread_t* thread_ptr,void* retVal);


void thread_detach(common_thread_t* thread_ptr);
int thread_is_current_thread(common_thread_t* thread_ptr);
int thread_equals(common_thread_t* a,common_thread_t* b);
////////////////////
void thread_mutex_init ( common_thread_mutex_t* mutex ) ;
common_thread_mutex_t* thread_mutex_create_obj(enum Thread_mutex_type);
void thread_mutex_destory_obj(common_thread_mutex_t*);
void thread_mutex_lock(common_thread_mutex_t*);
int thread_mutex_trylock(common_thread_mutex_t*);
int thread_mutex_timedlock(common_thread_mutex_t*,int ns);
void thread_mutex_unlock(common_thread_mutex_t*);

/////
void thread_cond_init ( common_thread_cond_t*) ;
common_thread_cond_t* thread_cond_create_obj();
void thread_cond_destory_obj(common_thread_cond_t*);
int thread_cond_signal(common_thread_cond_t*);
int thread_cond_broadcast(common_thread_cond_t*);
int thread_cond_wait(common_thread_cond_t*,common_thread_mutex_t* mutex);
int thread_cond_timedwait(common_thread_cond_t*,common_thread_mutex_t* mutex,int ns);


#endif
