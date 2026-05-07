#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "worker.h"
#include "job.h"

// Function declaration for assigning a job to a worker
void dispatcher_assign_job(worker_t worker, job_t* job);

#endif // DISPATCHER_H