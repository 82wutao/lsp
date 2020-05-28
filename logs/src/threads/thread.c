#include "threads/thread.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdbool.h>

struct __common_thread_mutex {
    pthread_mutex_t thread_mutex;
    pthread_mutexattr_t thread_mutex_attr;
};
struct __common_thread_cond {
    pthread_cond_t thread_cond;
    pthread_condattr_t thread_cond_attr;
};

struct __common_thread {
    pthread_t thread_id;
    pthread_attr_t thread_attr;

    pthread_once_t thread_once;
    pthread_key_t thread_key;

};

static common_thread_t* pthread_new() {
    void* mem = calloc ( sizeof ( struct __common_thread ),1 );
    if ( mem==NULL ) {
        return NULL;
    }

    return ( common_thread_t* ) mem;
}
common_thread_t* thread_create_obj ( int detachable ) {
    common_thread_t* comm_thread = pthread_new();
    if ( comm_thread==NULL ) {
        return NULL;
    }

    pthread_attr_init ( &comm_thread->thread_attr );
    pthread_attr_setdetachstate ( &comm_thread->thread_attr,
                                  detachable?PTHREAD_CREATE_DETACHED:PTHREAD_CREATE_JOINABLE );

    return comm_thread;
}
void thread_destory_obj ( common_thread_t* thread_ptr ) {
    if ( !thread_ptr ) {
        return ;
    }

    pthread_attr_destroy ( &thread_ptr->thread_attr );
    free ( thread_ptr );
}

int thread_start ( common_thread_t* thread_ptr,
                   runnable run,
                   void* run_arg ) {
    int r = pthread_create ( &thread_ptr->thread_id,&thread_ptr->thread_attr,
                             run,run_arg );
    return r;
}
void thread_exit_current ( common_thread_t* thread_ptr,void* retVal ) {
    pthread_exit ( retVal );
}
void thread_detach ( common_thread_t* thread_ptr ) {
    pthread_detach ( thread_ptr->thread_id );
}
int thread_is_current_thread ( common_thread_t* thread_ptr ) {
    int ret = pthread_equal ( thread_ptr->thread_id,pthread_self() );
    if ( ret ) {
        return 1;
    }
    return 0;
}
int thread_equals ( common_thread_t* a,common_thread_t* b ) {
    int ret = pthread_equal ( a->thread_id,b->thread_id );
    if ( ret ) {
        return 1;
    }
    return 0;
}


void thread_join ( common_thread_t* thread_ptr,void** rcvReturn ) {
    pthread_join ( thread_ptr->thread_id,rcvReturn );
}

void thread_cancel_someone ( common_thread_t* thread_ptr ) {
    pthread_cancel ( thread_ptr->thread_id );
}

//////////
static pthread_mutex_t static_mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t static_mutex_size = sizeof ( pthread_mutex_t );

void thread_mutex_init ( common_thread_mutex_t* mutex ) {
    if (! mutex ) {
        return ;
    }

    memset ( &mutex->thread_mutex,0,static_mutex_size );
    memcpy ( &mutex->thread_mutex,&static_mutex,static_mutex_size );
}
common_thread_mutex_t* thread_mutex_create_obj ( enum Thread_mutex_type type ) {
    void* mem = calloc ( static_mutex_size,1 );
    if ( !mem ) {
        return NULL;
    }
    common_thread_mutex_t* mutex = ( common_thread_mutex_t* ) mem;

    pthread_mutexattr_init ( &mutex->thread_mutex_attr );
    switch ( type ) {
    case DEADLOCK_NOCHECK:
        pthread_mutexattr_settype ( &mutex->thread_mutex_attr,PTHREAD_MUTEX_NORMAL );
        break ;
    case ERROR_CHECK:
        pthread_mutexattr_settype ( &mutex->thread_mutex_attr,PTHREAD_MUTEX_ERRORCHECK );
        break ;
    case RECURSIVE_ENABLE:
        pthread_mutexattr_settype ( &mutex->thread_mutex_attr,PTHREAD_MUTEX_RECURSIVE );
        break ;
    case DEFAULT:
    default:
        pthread_mutexattr_settype ( &mutex->thread_mutex_attr,PTHREAD_MUTEX_DEFAULT );
    }

    pthread_mutex_init ( &mutex->thread_mutex,&mutex->thread_mutex_attr );

    return mutex;
}
void thread_mutex_destory_obj ( common_thread_mutex_t* mutex ) {
    if ( !mutex ) {
        return ;
    }
    pthread_mutexattr_destroy ( &mutex->thread_mutex_attr );
    pthread_mutex_destroy ( &mutex->thread_mutex );
    free ( mutex );
}
void thread_mutex_lock ( common_thread_mutex_t* mutex ) {
    pthread_mutex_lock ( &mutex->thread_mutex );
}
void thread_mutex_unlock ( common_thread_mutex_t* mutex ) {
    pthread_mutex_unlock ( &mutex->thread_mutex );
}
int thread_mutex_trylock ( common_thread_mutex_t* mutex ) {
    return pthread_mutex_trylock ( &mutex->thread_mutex );
}
int thread_mutex_timedlock ( common_thread_mutex_t* mutex,int ns ) {
    if ( ns<0 ) {
        thread_mutex_lock ( mutex );
        return 0;
    }
    if ( ns==0 ) {
        return thread_mutex_trylock ( mutex );
    }

    struct timespec clock;
    int ret = clock_gettime ( CLOCK_REALTIME,&clock );
    if ( ret ) {
        return ret;
    }


    clock.tv_nsec +=ns;
    return pthread_mutex_timedlock ( &mutex->thread_mutex,&clock );
}
//////
static pthread_cond_t static_cond = PTHREAD_COND_INITIALIZER;
static size_t static_cond_size = sizeof ( pthread_cond_t );

void thread_cond_init ( common_thread_cond_t* cond ) {
    if ( !cond ) {
        return ;
    }

    memset ( &cond->thread_cond,0,static_cond_size );
    memcpy ( &cond->thread_cond,&static_cond,static_cond_size );
}

common_thread_cond_t* thread_cond_create_obj(){
    // TODO
    return NULL;
}
void thread_cond_destory_obj(common_thread_cond_t* cond){
    if(!cond){return ;}
    pthread_condattr_destroy(&cond->thread_cond_attr);
        pthread_cond_destroy(&cond->thread_cond);
}
int thread_cond_signal(common_thread_cond_t* cond){
    return pthread_cond_signal(&cond->thread_cond);
}
int thread_cond_broadcast(common_thread_cond_t* cond){
    return pthread_cond_broadcast(&cond->thread_cond);
}
int thread_cond_wait(common_thread_cond_t* cond,common_thread_mutex_t* mutex){
   return pthread_cond_wait(&cond->thread_cond,&mutex->thread_mutex);
}
int thread_cond_timedwait(common_thread_cond_t* cond,common_thread_mutex_t* mutex,int ns){
    if ( ns<=0 ) {
        return thread_cond_wait ( cond,mutex );
    }

    struct timespec clock;
    int ret = clock_gettime ( CLOCK_REALTIME,&clock );
    if ( ret ) {
        return ret;
    }


    clock.tv_nsec +=ns;
    return pthread_cond_timedwait(&cond->thread_cond, &mutex->thread_mutex,&clock );
}
