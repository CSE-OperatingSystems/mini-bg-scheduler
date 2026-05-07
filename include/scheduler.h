#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "job.h"

// Available scheduling policies (applied within the selected queue)
typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_PRIORITY
} policy_t;

#define NUM_QUEUES 3
#define MAX_JOBS_PER_QUEUE 1000

// Function declarations
void scheduler_init(void);
void scheduler_add_job(job_t* job);
job_t* scheduler_get_next_job(policy_t policy);

#endif // SCHEDULER_H