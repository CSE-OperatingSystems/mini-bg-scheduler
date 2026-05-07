#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "../include/job.h"
#include "../include/worker.h"
#include "../include/scheduler.h"

// Global synchronization variables
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t job_available = PTHREAD_COND_INITIALIZER;

int completed_jobs = 0;
int total_jobs = 0;
policy_t current_policy;
struct timeval sim_start_time;

// External function from worker.c
extern int get_current_sim_time();
extern void* worker_loop(void* arg);

// Parse CSV file into an array of jobs
int load_jobs(const char* filename, job_t* all_jobs) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open workload file");
        return 0;
    }

    char line[256];
    int count = 0;
    
    // Skip header row
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0; 
    }

    while (fgets(line, sizeof(line), file)) {
        job_t* j = &all_jobs[count];
        sscanf(line, "%d,%[^,],%d,%d,%d,%s",
               &j->job_id, j->seller_id, &j->arrival_time,
               &j->estimated_runtime, &j->priority, j->job_type);
        
        j->status = JOB_WAITING;
        count++;
    }

    fclose(file);
    return count;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <csv_file> <policy(fifo/sjf/priority)> <num_workers>\n", argv[0]);
        return 1;
    }

    // 1. Parse Arguments
    const char* filename = argv[1];
    int num_workers = atoi(argv[3]);
    
    if (strcmp(argv[2], "fifo") == 0) current_policy = POLICY_FIFO;
    else if (strcmp(argv[2], "sjf") == 0) current_policy = POLICY_SJF;
    else if (strcmp(argv[2], "priority") == 0) current_policy = POLICY_PRIORITY;
    else {
        printf("Invalid policy. Use fifo, sjf, or priority.\n");
        return 1;
    }

    // 2. Load Workload
    job_t all_jobs[MAX_JOBS];
    total_jobs = load_jobs(filename, all_jobs);
    if (total_jobs == 0) return 1;

    // 3. Initialize Worker Pool
    worker_t workers[num_workers];
    gettimeofday(&sim_start_time, NULL); // Start the simulation clock

    for (int i = 0; i < num_workers; i++) {
        workers[i].worker_id = i;
        pthread_create(&workers[i].thread, NULL, worker_loop, &workers[i]);
    }

    // 4. Main Timer Loop: Dispatch jobs based on arrival_time
    for (int i = 0; i < total_jobs; i++) {
        // Wait until simulation time catches up to job's arrival time
        while (get_current_sim_time() < all_jobs[i].arrival_time) {
            usleep(10000); // Check every 10ms
        }

        pthread_mutex_lock(&queue_mutex);
        scheduler_add_job(&all_jobs[i]); // Add to Ready Queue
        pthread_cond_broadcast(&job_available); // Wake up sleeping workers
        pthread_mutex_unlock(&queue_mutex);
    }

    // 5. Cleanup and wait for all workers to finish
    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i].thread, NULL);
    }

    // CALCULATE & PRINT METRICS ---
    int total_sim_time = get_current_sim_time();
    long long total_waiting_time = 0;
    long long total_turnaround_time = 0;
    int starvation_risk_count = 0;
    int total_busy_time = 0;

    for (int i = 0; i < total_jobs; i++) {
        int wait_time = all_jobs[i].start_time - all_jobs[i].arrival_time;
        int turnaround_time = all_jobs[i].finish_time - all_jobs[i].arrival_time;
        
        total_waiting_time += wait_time;
        total_turnaround_time += turnaround_time;
        total_busy_time += all_jobs[i].estimated_runtime;

        if (wait_time > 2 * all_jobs[i].estimated_runtime) {
            starvation_risk_count++;
        }
    }

    double avg_waiting = (double)total_waiting_time / total_jobs;
    double avg_turnaround = (double)total_turnaround_time / total_jobs;
    double throughput = (double)total_jobs / total_sim_time;
    double utilization = ((double)total_busy_time / (num_workers * total_sim_time)) * 100.0;

    printf("\n============================================\n");
    printf("   FINAL PERFORMANCE SUMMARY (HCMUT - OS)\n");
    printf("============================================\n");
    printf("Policy: %-10s | Workers: %d\n", argv[2], num_workers);
    printf("Total Jobs: %-6d | Sim Time: %d\n", total_jobs, total_sim_time);
    printf("--------------------------------------------\n");
    printf("Average Waiting Time:     %.2f units\n", avg_waiting);
    printf("Average Turnaround Time:  %.2f units\n", avg_turnaround);
    printf("System Throughput:        %.3f jobs/unit\n", throughput);
    printf("Worker Utilization:       %.2f%%\n", utilization);
    printf("Starvation-risk Jobs:     %d\n", starvation_risk_count);
    printf("============================================\n");

    return 0;
}