#include "worker.h"
#include "scheduler.h"
#include "dispatcher.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

extern pthread_mutex_t queue_mutex;
extern pthread_cond_t job_available;
extern policy_t current_policy; // Policy được cấu hình từ main

void* worker_loop(void* arg) {
    worker_t* worker = (worker_t*)arg;

    while (1) {
        pthread_mutex_lock(&queue_mutex);

        // Chờ nếu hàng đợi trống và chưa kết thúc mô phỏng
        while (/* Hàng đợi trống */ && /* Còn job chưa xử lý */) {
            pthread_cond_wait(&job_available, &queue_mutex);
        }

        // Thoát vòng lặp nếu tất cả job đã xử lý xong
        if (/* Đã xử lý xong toàn bộ job */) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        // Lấy job ra bằng scheduler
        job_t* next_job = scheduler_get_next_job(current_policy); [cite: 90]
        
        pthread_mutex_unlock(&queue_mutex);

        if (next_job != NULL) {
            // Dispatcher gán job và Worker thực thi (mô phỏng thời gian)
            dispatcher_assign_job(*worker, *next_job); [cite: 91]
            
            // In log Start [cite: 80, 81]
            printf("[time=%d] Worker %d starts Job %d...\n", current_time, worker->worker_id, next_job->job_id);
            
            // Scaled Simulation: 1 đơn vị thời gian = 0.1 giây [cite: 132]
            usleep(next_job->estimated_runtime * 100000); 
            
            // Cập nhật trạng thái và In log Finish [cite: 82, 83]
            next_job->status = JOB_DONE; [cite: 98]
            next_job->finish_time = current_time;
        }
    }
    return NULL;
}