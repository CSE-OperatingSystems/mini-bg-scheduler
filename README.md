# Mini Background Job Scheduler

Dự án Hệ điều hành mô phỏng một hệ thống lập lịch công việc chạy nền bằng C và POSIX Threads (`pthread`).

## Thành viên nhóm
1. Huỳnh Hoàng Anh - Core Logic, Schedulers
2. [Tên Tiến] - Worker Pool, Synchronization
3. [Tên Đức] - Metrics, Evaluation & Report

## Hướng dẫn biên dịch (Build)
Sử dụng `gcc` với các cờ tối ưu hóa và cảnh báo theo yêu cầu:
\`\`\`bash
gcc -Wall -Wextra -O2 -pthread src/*.c -I./include -o scheduler
\`\`\`
*(Hoặc chỉ cần gõ `make` nếu bạn đã thiết lập file Makefile).*

## Cách chạy (Usage)
Cú pháp chạy chương trình:
\`\`\`bash
./scheduler <đường_dẫn_file_csv> <policy> <số_lượng_worker>
\`\`\`

**Ví dụ:**
\`\`\`bash
./scheduler workloads/workload_a.csv fifo 4
./scheduler workloads/workload_b.csv sjf 4
./scheduler workloads/workload_c.csv priority 4
\`\`\`

## Các chiến lược lập lịch (Supported Policies)
- `fifo`: First-In-First-Out / First-Come-First-Serve
- `sjf`: Shortest Job First
- `priority`: Priority Scheduling

## Cách tạo workload mẫu

Dự án này đi kèm với một công cụ nhỏ viết bằng C (`src/workload_gen.c`), đi kèm với file thực thi đã được biên dịch sẵn từ file src/generate_workloads.c (`workload_gen.exe`) để tự động tạo ra các file CSV chứa dữ liệu công việc (jobs) theo đúng định dạng yêu cầu của bài tập. Các file này được thiết kế để kiểm thử các kịch bản lập lịch khác nhau. Để tạo file chứa dữ liệu mới, hãy chạy file `workload_gen.exe`:
```bash
./workload_gen.exe
```

### Giải thích các file dữ liệu đầu ra
Chương trình sẽ tự động tạo ra 3 file CSV trong thư mục `workloads`. Mỗi file tuân thủ cấu trúc `job_id, seller_id, arrival_time, estimated_runtime, priority, job_type` và phục vụ một mục đích kiểm thử cụ thể:  

`workload_a.csv` (Balanced Workload): Các công việc có thời gian chạy (runtime) tương đồng nhau và mức độ ưu tiên được trộn lẫn. Workload này dùng để kiểm tra xem một chính sách đơn giản như FIFO có đủ đáp ứng nhu cầu hay không.  

`workload_b.csv` (Mixed Short and Long Jobs): Chứa một vài công việc có thời gian chạy rất dài xuất hiện ngay từ đầu, theo sau là hàng loạt các công việc rất ngắn. Workload này được thiết kế để ép hệ thống bộc lộ "hiệu ứng đoàn tàu" (convoy effect) khi chạy bằng FIFO, qua đó thấy được sự ưu việt của SJF.  

`workload_c.csv` (Priority-Sensitive Workload): Các công việc có độ ưu tiên thấp sẽ đến trước, sau đó là một đợt các công việc có độ ưu tiên cao (số nhỏ hơn) đổ về sau. Workload này dùng để quan sát xem Priority Scheduling có cải thiện được thời gian phản hồi cho các tác vụ quan trọng không, và liệu nó có gây ra tình trạng "chết đói" (starvation) cho các công việc cũ hay không.