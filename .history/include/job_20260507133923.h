#ifndef JOB_H
#define JOB_H

// Define the states of a background job
typedef enum {
    JOB_WAITING,
    JOB_RUNNING,
    JOB_DONE
} job_status_t;

// Structure to store information about a job
typedef struct {
    int job_id;
    char seller_id[32];
    int arrival_time;
    int estimated_runtime;
    int priority;
    char job_type[64];
    int start_time;
    int finish_time;
    job_status_t status;
} job_t;

#endif // JOB_H