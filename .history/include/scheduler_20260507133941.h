#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "job.h"

// Available scheduling policies
typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_PRIORITY
} policy_t;

#define MAX_JOBS 1000

// Global variables for the Ready Queue
extern job_t* job_queue[MAX_JOBS];
extern int job_count;

// Function declarations
void scheduler_add_job(job_t* job);
job_t* scheduler_get_next_job(policy_t policy);

#endif // SCHEDULER_H