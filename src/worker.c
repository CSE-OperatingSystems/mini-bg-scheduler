#include "../include/worker.h"
#include "../include/scheduler.h"
#include "../include/dispatcher.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

// Shared global variables (defined in main.c)
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t job_available;
extern int completed_jobs;
extern int total_jobs;
extern policy_t current_policy;
extern struct timeval sim_start_time;

// Helper to calculate simulation time in "units" (1 unit = 100ms)
int get_current_sim_time() {
    struct timeval now;
    gettimeofday(&now, NULL);
    long elapsed_ms = (now.tv_sec - sim_start_time.tv_sec) * 1000 + 
                      (now.tv_usec - sim_start_time.tv_usec) / 1000;
    return (int)(elapsed_ms / 100);
}

void* worker_loop(void* arg) {
    worker_t* worker = (worker_t*)arg;

    while (1) {
        pthread_mutex_lock(&queue_mutex);

        job_t* next_job = NULL;

        // Sleep while no jobs are ready and we still have jobs left to process
        while (completed_jobs < total_jobs && 
              (next_job = scheduler_get_next_job(current_policy)) == NULL) {
            pthread_cond_wait(&job_available, &queue_mutex);
        }

        // If all jobs are processed, exit the loop
        if (completed_jobs >= total_jobs) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        // Assign job and record start time
        dispatcher_assign_job(*worker, next_job);
        next_job->start_time = get_current_sim_time();
        
        printf("[time=%d] Worker %d starts Job %d seller=%s runtime=%d priority=%d\n", 
               next_job->start_time, worker->worker_id, next_job->job_id, 
               next_job->seller_id, next_job->estimated_runtime, next_job->priority);

        pthread_mutex_unlock(&queue_mutex);

        // Scaled Simulation: usleep takes microseconds (1 unit = 100,000 us)
        usleep(next_job->estimated_runtime * 100000);

        // Job finished, update status safely
        pthread_mutex_lock(&queue_mutex);
        
        next_job->finish_time = get_current_sim_time();
        next_job->status = JOB_DONE;
        completed_jobs++;

        printf("[time=%d] Worker %d finishes Job %d\n", 
               next_job->finish_time, worker->worker_id, next_job->job_id);

        // Wake up other threads in case they are waiting to terminate
        if (completed_jobs >= total_jobs) {
            pthread_cond_broadcast(&job_available);
        }

        pthread_mutex_unlock(&queue_mutex);
    }

    return NULL;
}