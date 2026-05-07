#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>

// Structure to store worker information
typedef struct {
    int worker_id;
    pthread_t thread;
} worker_t;

#endif // WORKER_H