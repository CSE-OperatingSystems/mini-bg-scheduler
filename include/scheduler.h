#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "job.h"

// Updated scheduling policies [cite: 115-119, 175]
typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_PRIORITY,
    POLICY_AGING_PRIORITY  // Add this line
} policy_t;

#define MAX_JOBS 1000

extern job_t* job_queue[MAX_JOBS];
extern int job_count;

void scheduler_add_job(job_t* job);
job_t* scheduler_get_next_job(policy_t policy);

#endif