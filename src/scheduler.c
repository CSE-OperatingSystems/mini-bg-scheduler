#include "../include/scheduler.h"
#include <stddef.h>

// Configuration for Multi-Level Queues
#define NUM_QUEUES 3
#define AGING_FACTOR 1  // Aggressive aging for demonstration

// Structure for a single queue level 
typedef struct {
    job_t* jobs[MAX_JOBS_PER_QUEUE];
    int job_count;
} job_queue_t;

// The Multi-Level Queue array
static job_queue_t mlq[NUM_QUEUES];

extern int get_current_sim_time();

// Initialize the queues
void scheduler_init(void) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        mlq[i].job_count = 0;
    }
}

// Maps job priority to a specific queue level 
void scheduler_add_job(job_t* job) {
    int q_index = 0;
    
    if (job->priority <= 1) {
        q_index = 0; // High Priority Queue
    } else if (job->priority <= 3) {
        q_index = 1; // Normal Priority Queue
    } else {
        q_index = 2; // Background/Low Priority Queue
    }

    if (mlq[q_index].job_count < MAX_JOBS_PER_QUEUE) {
        mlq[q_index].jobs[mlq[q_index].job_count++] = job;
    }
}

// Integrated scheduler with MLQ support and Aging 
job_t* scheduler_get_next_job(policy_t policy) {
    int current_time = get_current_sim_time();

    // Iterate through queues from highest priority (0) to lowest (NUM_QUEUES - 1) 
    for (int q = 0; q < NUM_QUEUES; q++) {
        job_queue_t* current_q = &mlq[q];
        int selected_index = -1;
        job_t* selected_job = NULL;
        int best_dynamic_prio = 999999;

        for (int i = 0; i < current_q->job_count; i++) {
            job_t* job = current_q->jobs[i];
            
            if (job->status == JOB_WAITING) {
                // Baseline selection
                if (selected_index == -1) {
                    selected_index = i;
                    selected_job = job;
                    
                    // Initial dynamic priority for comparison
                    int age = (policy == POLICY_AGING_PRIORITY) ? 
                              (current_time - job->arrival_time) / AGING_FACTOR : 0;
                    best_dynamic_prio = job->priority - age;
                    continue;
                }

                // Policy-based comparison within the current queue level
                switch (policy) {
                    case POLICY_FIFO:
                        if (job->arrival_time < selected_job->arrival_time) {
                            selected_job = job;
                        }
                        break;

                    case POLICY_SJF:
                        if (job->estimated_runtime < selected_job->estimated_runtime) {
                            selected_job = job;
                        } else if (job->estimated_runtime == selected_job->estimated_runtime) {
                            if (job->arrival_time < selected_job->arrival_time) selected_job = job;
                        }
                        break;

                    case POLICY_PRIORITY:
                    case POLICY_AGING_PRIORITY: {
                        int age = (policy == POLICY_AGING_PRIORITY) ? 
                                  (current_time - job->arrival_time) / AGING_FACTOR : 0;
                        int dynamic_prio = job->priority - age;
                        if (dynamic_prio < 0) dynamic_prio = 0;

                        if (dynamic_prio < best_dynamic_prio) {
                            best_dynamic_prio = dynamic_prio;
                            selected_job = job;
                        } else if (dynamic_prio == best_dynamic_prio) {
                            if (job->arrival_time < selected_job->arrival_time) selected_job = job;
                        }
                        break;
                    }
                }
            }
        }

        // If a job is found in a higher-priority queue, return it immediately 
        if (selected_job != NULL) {
            return selected_job;
        }
    }
    return NULL; // All queues are empty
}