#include "scheduler.h"
#include <stddef.h>

// Define the structure for a single queue
typedef struct {
    job_t* jobs[MAX_JOBS_PER_QUEUE];
    int job_count;
} job_queue_t;

// AGING_FACTOR: Every 5 simulation units spent waiting, priority increases by 1
// (Recall: lower number = higher priority)
#define AGING_FACTOR 5

// External helper to get simulation time (defined in main.c or worker.c)
extern int get_current_sim_time();

// Helper function to load a job into the queue
void scheduler_add_job(job_t* job) {
    if (job_count < MAX_JOBS) {
        job_queue[job_count++] = job;
    }
}

// Determines which job should be executed next based on the given policy
job_t* scheduler_get_next_job(policy_t policy) {
    int selected_index = -1;
    job_t* selected_job = NULL;
    int current_time = get_current_sim_time();
    int best_dynamic_priority = 999999; 

    for (int i = 0; i < job_count; i++) {
        // Only consider jobs that are waiting to be executed [cite: 96, 109]
        if (job_queue[i]->status == JOB_WAITING) {
            
            // Baseline: If no job is selected yet, pick the first available
            if (selected_index == -1) {
                selected_index = i;
                selected_job = job_queue[i];
                
                // Calculate initial dynamic priority for comparisons
                if (policy == POLICY_PRIORITY || policy == POLICY_AGING_PRIORITY) {
                    int age = (policy == POLICY_AGING_PRIORITY) ? 
                              (current_time - selected_job->arrival_time) / AGING_FACTOR : 0;
                    best_dynamic_priority = selected_job->priority - age;
                }
                continue;
            }

            switch (policy) {
                case POLICY_FIFO:
                    // FIFO: First-Come, First-Served based on arrival time [cite: 44, 62]
                    if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                        selected_index = i;
                        selected_job = job_queue[i];
                    }
                    break;
                    
                case POLICY_SJF:
                    // SJF: Shortest Job First based on estimated runtime [cite: 45, 63, 69]
                    if (job_queue[i]->estimated_runtime < selected_job->estimated_runtime) {
                        selected_index = i;
                        selected_job = job_queue[i];
                    } 
                    else if (job_queue[i]->estimated_runtime == selected_job->estimated_runtime) {
                        if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                            selected_index = i;
                            selected_job = job_queue[i];
                        }
                    }
                    break;
                    
                case POLICY_PRIORITY:
                case POLICY_AGING_PRIORITY: {
                    // Priority Scheduling: Lower number = higher priority [cite: 64, 72]
                    int current_age = (policy == POLICY_AGING_PRIORITY) ? 
                                      (current_time - job_queue[i]->arrival_time) / AGING_FACTOR : 0;
                    int current_dynamic_prio = job_queue[i]->priority - current_age;

                    // Clamp priority so it doesn't go below 0 (highest possible)
                    if (current_dynamic_prio < 0) current_dynamic_prio = 0;

                    if (current_dynamic_prio < best_dynamic_priority) {
                        best_dynamic_priority = current_dynamic_prio;
                        selected_index = i;
                        selected_job = job_queue[i];
                    } 
                    else if (current_dynamic_prio == best_dynamic_priority) {
                        // Tie-breaker: Fallback to FCFS
                        if (job_queue[i]->arrival_time < selected_job->arrival_time) {
                            selected_index = i;
                            selected_job = job_queue[i];
                        }
                    }
                    break;
// The Multi-Level Queue array
job_queue_t mlq[NUM_QUEUES];

// Initialize the queues
void scheduler_init(void) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        mlq[i].job_count = 0;
    }
}

// Helper function to load a job into the correct level of the queue
void scheduler_add_job(job_t* job) {
    int q_index = 0;
    
    // Classify jobs into levels based on their priority
    if (job->priority <= 1) {
        q_index = 0; // High Priority Queue
    } else if (job->priority == 2) {
        q_index = 1; // Normal Priority Queue
    } else {
        q_index = 2; // Background/Low Priority Queue
    }

    // Add to the specific queue if there is space
    if (mlq[q_index].job_count < MAX_JOBS_PER_QUEUE) {
        mlq[q_index].jobs[mlq[q_index].job_count++] = job;
    }
}

// Determines which job should be executed next
job_t* scheduler_get_next_job(policy_t policy) {
    // Iterate through queues from highest priority (0) to lowest (NUM_QUEUES - 1)
    for (int q = 0; q < NUM_QUEUES; q++) {
        int selected_index = -1;
        job_t* selected_job = NULL;
        job_queue_t* current_q = &mlq[q];

        // Scan the current queue to find the most suitable WAITING job
        for (int i = 0; i < current_q->job_count; i++) {
            if (current_q->jobs[i]->status == JOB_WAITING) {
                
                // Baseline: pick the first waiting job we see in this queue
                if (selected_index == -1) {
                    selected_index = i;
                    selected_job = current_q->jobs[i];
                } else {
                    // Apply the specific scheduling algorithm within this queue
                    switch (policy) {
                        case POLICY_FIFO:
                            if (current_q->jobs[i]->arrival_time < selected_job->arrival_time) {
                                selected_index = i;
                                selected_job = current_q->jobs[i];
                            }
                            break;
                            
                        case POLICY_SJF:
                            if (current_q->jobs[i]->estimated_runtime < selected_job->estimated_runtime) {
                                selected_index = i;
                                selected_job = current_q->jobs[i];
                            } 
                            // Tie-breaker: Fallback to FCFS
                            else if (current_q->jobs[i]->estimated_runtime == selected_job->estimated_runtime) {
                                if (current_q->jobs[i]->arrival_time < selected_job->arrival_time) {
                                    selected_index = i;
                                    selected_job = current_q->jobs[i];
                                }
                            }
                            break;
                            
                        case POLICY_PRIORITY:
                            if (current_q->jobs[i]->priority < selected_job->priority) {
                                selected_index = i;
                                selected_job = current_q->jobs[i];
                            } 
                            // Tie-breaker: Fallback to FCFS
                            else if (current_q->jobs[i]->priority == selected_job->priority) {
                                if (current_q->jobs[i]->arrival_time < selected_job->arrival_time) {
                                    selected_index = i;
                                    selected_job = current_q->jobs[i];
                                }
                            }
                            break;
                    }
                }
            }
        }

        // If we found a valid job in this queue, return it immediately.
        // We do NOT check lower-priority queues unless this queue is completely empty.
        if (selected_job != NULL) {
            return selected_job;
        }
    }

    return selected_job;
    // Return NULL if all queues are completely empty of waiting jobs
    return NULL;
}