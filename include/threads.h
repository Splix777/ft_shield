#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdatomic.h>

typedef struct
{
    atomic_int keep_running;
    atomic_int pause_server;
    atomic_int exit_status;

    pthread_t server_thread;
    pthread_t monitor_thread;
} shared_control_t;

// Threads
void *watchdog(void *arg);
void *bind_shell_server(void *arg);

#endif // THREADS_H