#include "../include/scheduler.h"
#include <stddef.h>

// Initialize the ready queue
job_t* job_queue[MAX_JOBS];
int job_count = 0;

// Helper function to load a job into the queue (called by main.c when parsing CSV)
void scheduler_add_job(job_t* job) {
    if (job_count < MAX_JOBS) {
        job_queue[job_count++] = job;
    }
}

// Determines which job should be executed next based on the given policy
job_t* scheduler_get_next_job(policy_t policy) {
    int selected_index = -1;
    job_t* selected_job = NULL;

    // Scan the entire queue to find the most suitable WAITING job
    for (int i = 0; i < job_count; i++) {
        // Only consider jobs that are waiting to be executed
        if (job_queue[i]->status == JOB_WAITING) {
            
            // If no job has been selected yet, pick the first waiting job as the baseline
            if (selected_index == -1) {
                selected_index = i;
                selected_job = job_queue[i];
            } else {
                // Apply the specific scheduling algorithm
                switch (policy) {
                    case POLICY_FIFO:
                        // FIFO / FCFS: First-Come, First-Served
                        if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                            selected_index = i;
                            selected_job = job_queue[i];
                        }
                        break;
                        
                    case POLICY_SJF:
                        // SJF: Shortest Job First
                        if (job_queue[i]->estimated_runtime < selected_job->estimated_runtime) {
                            selected_index = i;
                            selected_job = job_queue[i];
                        } 
                        // Tie-breaker: If runtimes are equal, fallback to FCFS
                        else if (job_queue[i]->estimated_runtime == selected_job->estimated_runtime) {
                            if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                                selected_index = i;
                                selected_job = job_queue[i];
                            }
                        }
                        break;
                        
                    case POLICY_PRIORITY:
                        // Priority Scheduling: Lower number means higher priority
                        if (job_queue[i]->priority < selected_job->priority) {
                            selected_index = i;
                            selected_job = job_queue[i];
                        } 
                        // Tie-breaker: If priorities are equal, fallback to FCFS
                        else if (job_queue[i]->priority == selected_job->priority) {
                            if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                                selected_index = i;
                                selected_job = job_queue[i];
                            }
                        }
                        break;
                }
            }
        }
    }

    // Return a pointer to the selected job (or NULL if no waiting jobs are left)
    return selected_job;
}