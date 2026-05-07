#include "../include/dispatcher.h"
#include <stdio.h>

// Assigns a selected job to an available worker
void dispatcher_assign_job(worker_t worker, job_t* job) {
    if (job != NULL) {
        // Change status to RUNNING to remove it from the Ready Queue's consideration
        job->status = JOB_RUNNING;
        
        // Note: The actual execution time tracking (start_time) will be handled 
        // by the worker itself in src/worker.c to reflect accurate real-time simulation.
    }
}