
# 🧵 CPU Scheduler – C University Project

This repository contains a full implementation of a **CPU scheduling simulator** written in C.  
It includes a main scheduler program and multiple worker programs to simulate different workloads.

---

# 📂 Project Structure

```
Scheduler-C-Uni-Project/
│
├── scheduler/
│   ├── scheduler.c          # Main CPU scheduler implementation
│   ├── scheduler_io.c       # Input/output handling utilities
│   ├── Makefile             # Builds scheduler + I/O helpers
│   ├── run.sh               # Example execution script
│   ├── sample_output.txt    # Example program output
│   ├── homogeneous.txt      # Test input dataset
│   ├── mixed.txt            # Test input dataset
│   ├── reverse.txt          # Test input dataset
│
└── work/
    ├── work.c               # Base workload
    ├── work_io.c            # Workload I/O helper
    ├── Makefile             # Builds work executables
    ├── work1, work2, ...    # Multiple work executables
```

---

# ⚙️ What the Project Does

This project simulates **CPU scheduling**, allowing you to test different workloads and job arrival patterns.

The scheduler:
- Reads a list of jobs from input files (`homogeneous.txt`, `mixed.txt`, `reverse.txt`)
- Dispatches jobs to worker programs located in `/work`
- Simulates scheduling behavior (FCFS, priority, or custom — depending on the implementation)
- Prints execution logs and timing results

---

# 🛠️ How to Compile

### 1️⃣ Build the scheduler
```
cd scheduler
make
```

This produces:
```
scheduler
scheduler_io
```

### 2️⃣ Build the work programs
```
cd ../work
make
```

This produces:
```
work1, work2, work3, ...
```

---

# 🚀 How to Run

You **must compile both folders first**.

### Option A — Run using the provided script
```
cd scheduler
./run.sh
```

### Option B — Run manually
```
./scheduler < inputfile
```

Example:
```
./scheduler homogeneous.txt
```

---

# 🧪 Input Files

The input `.txt` files define simulated processes.

Examples:
- `homogeneous.txt` → tasks with similar characteristics  
- `mixed.txt` → mixed CPU/IO workloads  
- `reverse.txt` → reverse-sorted job order  

---

# 📤 Output

The scheduler outputs:
- Execution order of tasks  
- Start/finish times  
- Performance statistics  
- Simulated CPU timeline  

See `sample_output.txt` for an example.

---

# 📘 Summary

This project demonstrates:
✔ Process scheduling simulation  
✔ C programming with multiple executables  
✔ Inter-process communication via exec calls  
✔ Job execution timing  
✔ Makefile automation  


