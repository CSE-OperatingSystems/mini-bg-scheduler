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