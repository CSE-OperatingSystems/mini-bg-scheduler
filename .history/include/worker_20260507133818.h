#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>

// Cấu trúc thông tin worker 
typedef struct {
    int worker_id;
    pthread_t thread;
} worker_t;

#endif // WORKER_H