# Compiler and flags required by the assignment
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

# Directory paths
SRC_DIR = src
INC_DIR = include
LOG_DIR = logs
WORKLOAD_DIR = workloads

# Target executable name
TARGET = scheduler

# Default target when you just type 'make'
all: $(TARGET)

# Compile all .c files in the src/ directory
$(TARGET): $(SRC_DIR)/main.c $(SRC_DIR)/worker.c $(SRC_DIR)/scheduler.c $(SRC_DIR)/dispatcher.c
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $@

# Clean up executable (type: make clean)
clean:
	rm -f $(TARGET)

# --- COMMANDS TO RUN TESTS AND SAVE LOGS ---

# Type: make run_fifo
run_fifo: $(TARGET)
	./$(TARGET) $(WORKLOAD_DIR)/workload_a.csv fifo 4 > $(LOG_DIR)/run_fifo.log
	@echo "FIFO execution completed. Check results in logs/run_fifo.log"

# Type: make run_sjf
run_sjf: $(TARGET)
	./$(TARGET) $(WORKLOAD_DIR)/workload_b.csv sjf 4 > $(LOG_DIR)/run_sjf.log
	@echo "SJF execution completed. Check results in logs/run_sjf.log"

# Type: make run_priority
run_priority: $(TARGET)
	./$(TARGET) $(WORKLOAD_DIR)/workload_c.csv priority 4 > $(LOG_DIR)/run_priority.log
	@echo "Priority execution completed. Check results in logs/run_priority.log"

# Type: make run_all (Runs all 3 algorithms sequentially)
run_all: run_fifo run_sjf run_priority
	@echo "All workloads executed successfully!"