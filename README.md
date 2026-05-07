# ⚙️ Mini Background Job Scheduler

This project was developed by **Group Eternals** for **Lab 4: Operating Systems (CO2018)** - Semester **HK252** at **Ho Chi Minh City University of Technology (HCMUT)**. The objective is to design and simulate a multi-threaded background job scheduling system, mapping classical CPU scheduling algorithms to real-world backend architectures.

## 👥 Team Members

* **Huynh Hoang Anh** - Core Data Structures & Scheduling Algorithms
* **Huynh Tan Tien** - Worker Pool, Synchronization & Parser
* **Pham Minh Duc** - Metrics Calculation, Evaluation & Report

## 📋 Project Overview

The project implements a multi-threaded worker pool in C using POSIX Threads (`pthread`). It leverages synchronization mechanisms (Mutex, Condition Variables) and compares classical scheduling policies to evaluate performance, scalability, and system trade-offs:
* **FIFO (First-In-First-Out):** A baseline scheduling policy where jobs are assigned strictly in their order of arrival.
* **SJF (Shortest Job First):** Prioritizes jobs with the shortest estimated runtime to reduce average waiting time and mitigate the convoy effect.
* **Priority Scheduling:** Assigns jobs based on priority levels, introducing preemption concepts and the risk of starvation for low-priority tasks.

### ✨ Advanced Features (Bonus Implementations)
To address the limitations of classical algorithms, we implemented two advanced scheduling mechanisms:
* **Multi-Level Queue (MLQ):** Automatically classifies incoming jobs into distinct priority tiers (High, Normal, Background). The dispatcher strictly evaluates higher-tier queues before allocating resources to lower-tier ones.
* **Aging Priority (`aging`):** Solves the *Starvation* problem inherent in strict Priority Scheduling. The system dynamically boosts the priority of long-waiting jobs based on a predefined `AGING_FACTOR`, ensuring low-priority tasks are eventually executed.

## 🚀 Getting Started

### Prerequisites

* **Operating System:** Linux (Ubuntu or WSL recommended).
* **Compiler:** `gcc` with `-O2` and `-pthread` flags.
* **Tools:** `make`.

### 1. Compilation

Use the provided `Makefile` to compile the source code:
```bash
make
```
This generates the main executable: `scheduler` and the workload generator.

### 2. Automated Testing

To run the automated experiment scripts across all scheduling policies (including the Aging mechanism) and workloads:
```bash
make run_all
```
Results and performance metrics will be automatically saved into the `logs/` directory.

### 3. Manual Execution

```bash
./scheduler <csv_file_path> <policy> <number_of_workers>
```
*Example:* `./scheduler workloads/workload_c.csv aging 4`

## 📊 Workloads & Methodology

The system is evaluated against three standardized workloads generated via the `workload_gen` tool:
* **Workload A (Balanced):** Similar runtimes and mixed priorities to test baseline stability and evaluate FIFO.
* **Workload B (Mixed Short/Long):** Forces the system to exhibit the *Convoy Effect* to demonstrate SJF's superiority in handling execution bottlenecks.
* **Workload C (Priority-Sensitive):** Late-arriving high-priority jobs to test priority allocation and evaluate *Starvation* risks (Targeted by the Aging algorithm).

## 📄 Academic Report

The accompanying academic report analyzes:
* **Scheduling Metrics:** Throughput, Average Waiting Time, Turnaround Time, and Worker Utilization.
* **Trade-offs:** Analyzing why no single scheduler fits all workloads and the real-world impact of the `AGING_FACTOR`.
* **OS Concept Mapping:** How OS-level ready queues, mutexes, condition variables, and dispatchers translate to backend CI/CD or cloud job queues.

## 📂 Directory Structure

```text
.
├── Makefile                 # Build system and automated test scripts
├── README.md                # Project overview and execution guide
├── include/                 # Header files
│   ├── dispatcher.h
│   ├── job.h
│   ├── scheduler.h
│   └── worker.h
├── src/                     # C source code implementation
│   ├── dispatcher.c         # Job assignment logic
│   ├── main.c               # Parser, initialization, and metric calculation
│   ├── scheduler.c          # Scheduling policies (FIFO, SJF, Priority, MLQ, Aging)
│   └── worker.c             # Multi-threading and synchronization (Mutex/CondVars)
├── workload_gen.c           # Source code for generating workloads
├── workload_gen             # Executable for generating datasets (Linux/WSL)
├── workloads/               # Test datasets
│   ├── workload_a.csv       # Balanced Workload
│   ├── workload_b.csv       # Mixed Short/Long Jobs
│   └── workload_c.csv       # Priority-Sensitive Workload
├── logs/                    # Execution logs and performance outputs
    ├── run_fifo.log
    ├── run_sjf.log
    ├── run_priority.log
    └── run_aging.log
```

*This project is part of the undergraduate curriculum at the Faculty of Computer Science and Engineering, HCMUT.*
