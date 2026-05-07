#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------
// 1. Data Structures (As suggested in the PDF)
// ---------------------------------------------------------

typedef enum {
    JOB_WAITING,
    JOB_RUNNING,
    JOB_DONE
} job_status_t;

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

typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_PRIORITY
} policy_t;

// ---------------------------------------------------------
// 2. Command Line Argument Parser
// ---------------------------------------------------------

// Parses the command: ./scheduler <workload.csv> <policy> <workers>
int parse_arguments(int argc, char *argv[], char **filename, policy_t *policy, int *num_workers) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <workload.csv> <policy> <num_workers>\n", argv[0]);
        fprintf(stderr, "Policies: fifo, sjf, priority\n");
        return -1;
    }

    *filename = argv[1];

    // Determine the policy
    if (strcmp(argv[2], "fifo") == 0) {
        *policy = POLICY_FIFO;
    } else if (strcmp(argv[2], "sjf") == 0) {
        *policy = POLICY_SJF;
    } else if (strcmp(argv[2], "priority") == 0) {
        *policy = POLICY_PRIORITY;
    } else {
        fprintf(stderr, "Error: Unknown policy '%s'. Use fifo, sjf, or priority.\n", argv[2]);
        return -1;
    }

    // Parse number of workers
    *num_workers = atoi(argv[3]);
    if (*num_workers <= 0) {
        fprintf(stderr, "Error: Number of workers must be at least 1.\n");
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------
// 3. CSV File Parser
// ---------------------------------------------------------

// Reads the CSV and dynamically allocates an array of jobs
int parse_jobs(const char *filename, job_t **jobs, int *num_jobs) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[256];
    int capacity = 10;
    int count = 0;
    
    // Allocate initial memory for jobs
    *jobs = malloc(capacity * sizeof(job_t));
    if (!*jobs) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }

    // Skip the header line
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: Empty CSV file.\n");
        fclose(file);
        return -1;
    }

    // Read jobs line by line
    while (fgets(line, sizeof(line), file)) {
        // Resize array if capacity is reached
        if (count >= capacity) {
            capacity *= 2;
            *jobs = realloc(*jobs, capacity * sizeof(job_t));
            if (!*jobs) {
                perror("Memory reallocation failed");
                fclose(file);
                return -1;
            }
        }

        job_t *current_job = &(*jobs)[count];
        
        // Parse CSV format: job_id,seller_id,arrival_time,estimated_runtime,priority,job_type
        // Note: %[^,] reads characters until it hits a comma.
        int parsed = sscanf(line, "%d,%31[^,],%d,%d,%d,%63[^\n\r]",
               &current_job->job_id,
               current_job->seller_id,
               &current_job->arrival_time,
               &current_job->estimated_runtime,
               &current_job->priority,
               current_job->job_type);

        if (parsed == 6) {
            // Initialize runtime fields
            current_job->start_time = -1;
            current_job->finish_time = -1;
            current_job->status = JOB_WAITING;
            count++;
        } else {
            fprintf(stderr, "Warning: Skipping malformed line: %s", line);
        }
    }

    *num_jobs = count;
    fclose(file);
    return 0;
}

// ---------------------------------------------------------
// 4. Quick Test Driver (Main)
// ---------------------------------------------------------

int main(int argc, char *argv[]) {
    char *filename;
    policy_t policy;
    int num_workers;

    // 1. Parse Inputs
    if (parse_arguments(argc, argv, &filename, &policy, &num_workers) != 0) {
        return EXIT_FAILURE;
    }

    // 2. Read Jobs
    job_t *jobs = NULL;
    int num_jobs = 0;

    if (parse_jobs(filename, &jobs, &num_jobs) != 0) {
        return EXIT_FAILURE;
    }

    // 3. Print parsed data to verify
    printf("--- Scheduler Configuration ---\n");
    printf("File: %s\n", filename);
    printf("Policy: %d (0=FIFO, 1=SJF, 2=PRIORITY)\n", policy);
    printf("Workers: %d\n", num_workers);
    printf("\n--- Loaded %d Jobs ---\n", num_jobs);

    for (int i = 0; i < num_jobs; i++) {
        printf("Job %02d | Seller: %s | Arrival: %d | Runtime: %d | Priority: %d | Type: %s\n",
               jobs[i].job_id,
               jobs[i].seller_id,
               jobs[i].arrival_time,
               jobs[i].estimated_runtime,
               jobs[i].priority,
               jobs[i].job_type);
    }

    // Free dynamically allocated memory
    free(jobs);

    return EXIT_SUCCESS;
}