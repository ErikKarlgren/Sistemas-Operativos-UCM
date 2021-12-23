#ifndef __SO_UTILS
#define __SO_UTILS

#include <pthread.h>
// #include <semaphore.h>

// Initialize a mutex.
int init_mut(pthread_mutex_t *mutex)
{
    return pthread_mutex_init(mutex, NULL);
}
// Lock a mutex.
int lock(pthread_mutex_t *mutex)
{
    return pthread_mutex_lock(mutex);
}
// Unlock a mutex.
int unlock(pthread_mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}

// Initialize condition variable COND.
int init_vc(pthread_cond_t *cond)
{
    return pthread_cond_init(cond, NULL);
}
// Wait for condition variable COND to be signaled or broadcast.
// MUTEX is assumed to be locked before.
int wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return pthread_cond_wait(cond, mutex);
}
// Wake up one thread waiting for condition variable COND.
int signal(pthread_cond_t *cond)
{
    return pthread_cond_signal(cond);
}
// Wake up all threads waiting for condition variable COND.
int broadcast(pthread_cond_t *cond)
{
    return pthread_cond_broadcast(cond);
}

#endif