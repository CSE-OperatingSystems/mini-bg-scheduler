#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "job.h"

// Updated scheduling policies [cite: 115-119, 175]
// Available scheduling policies (applied within the selected queue)
typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_PRIORITY,
    POLICY_AGING_PRIORITY  // Add this line
} policy_t;

#define MAX_JOBS 1000

extern job_t* job_queue[MAX_JOBS];
extern int job_count;

#define NUM_QUEUES 3
#define MAX_JOBS_PER_QUEUE 1000

// Function declarations
void scheduler_init(void);
void scheduler_add_job(job_t* job);
job_t* scheduler_get_next_job(policy_t policy);

#endif