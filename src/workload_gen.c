#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* SELLERS[] = {"A", "B", "C", "D", "E"};
const char* JOB_TYPES[] = {"resize", "thumbnail", "compress", "policy_check", "metadata_update"};

// Helper to get random ranges
int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Helper to get random string from an array
const char* rand_choice(const char* arr[], int size) {
    return arr[rand() % size];
}

void generate_workload_a(const char* filename, int num_jobs) {
    // Workload A: Balanced - similar runtime, mixed priorities.
    FILE *file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "job_id,seller_id,arrival_time,estimated_runtime,priority,job_type\n");
    int current_time = 0;

    for (int i = 1; i <= num_jobs; i++) {
        const char* seller = rand_choice(SELLERS, 5);
        int arrival = current_time + rand_range(0, 2);
        int runtime = rand_range(3, 6); // Similar runtimes
        int priority = rand_range(1, 4); // Mixed priorities
        const char* type = rand_choice(JOB_TYPES, 5);

        fprintf(file, "%d,%s,%d,%d,%d,%s\n", i, seller, arrival, runtime, priority, type);
        current_time = arrival; // advance time slightly
    }
    fclose(file);
    printf("Generated %s\n", filename);
}

void generate_workload_b(const char* filename, int num_jobs) {
    // Workload B: Mixed - to show convoy effect, a few long jobs arrive first, then many short jobs.
    FILE *file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "job_id,seller_id,arrival_time,estimated_runtime,priority,job_type\n");
    int current_time = 0;

    for (int i = 1; i <= num_jobs; i++) {
        const char* seller = rand_choice(SELLERS, 5);
        int arrival = current_time + rand_range(0, 1);
        
        int runtime;
        if (i <= 3) {
            // First 3 jobs are very long
            runtime = rand_range(15, 20); 
        } else {
            // Remaining jobs are very short
            runtime = rand_range(1, 2);
        }
        
        int priority = rand_range(2, 3);
        const char* type = rand_choice(JOB_TYPES, 5);

        fprintf(file, "%d,%s,%d,%d,%d,%s\n", i, seller, arrival, runtime, priority, type);
        current_time = arrival;
    }
    fclose(file);
    printf("Generated %s\n", filename);
}

void generate_workload_c(const char* filename, int num_jobs) {
    // Workload C: Priority-Sensitive - High priority jobs arrive later than low priority ones.
    FILE *file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "job_id,seller_id,arrival_time,estimated_runtime,priority,job_type\n");
    int current_time = 0;

    for (int i = 1; i <= num_jobs; i++) {
        const char* seller = rand_choice(SELLERS, 5);
        int arrival = current_time + rand_range(1, 2);
        int runtime = rand_range(4, 8);
        
        int priority;
        if (i <= num_jobs / 2) {
            // First half arrive earlier but have low priority (higher number)
            priority = rand_range(4, 5);
        } else {
            // Second half arrive later but have high priority (lower number)
            priority = rand_range(1, 2);
        }
        
        const char* type = rand_choice(JOB_TYPES, 5);

        fprintf(file, "%d,%s,%d,%d,%d,%s\n", i, seller, arrival, runtime, priority, type);
        current_time = arrival;
    }
    fclose(file);
    printf("Generated %s\n", filename);
}

int main() {
    // Seed random number generator
    srand((unsigned int)time(NULL));

    int num_jobs = 20;

    generate_workload_a("../workloads/workload_a.csv", num_jobs);
    generate_workload_b("../workloads/workload_b.csv", num_jobs);
    generate_workload_c("../workloads/workload_c.csv", num_jobs);

    printf("All workload CSV files generated successfully!\n");
    return 0;
}