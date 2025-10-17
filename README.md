# Job Execution System (C++)

A C++ project that implements a **job execution system** using **multiple processes and Unix signals**.  
It demonstrates inter-process communication, signal handling, and job queue management.

## Overview
This project consists of two main components:

- **Job Commander (JC)**  
  - Reads commands from the command line or an input file.  
  - Spawns two child processes:
    1. One to run the **Job Executor Server (JES)**.  
    2. One to read JES’s output asynchronously.  
  - Automatically creates JES if it does not exist.  
  - Uses `sleep(1)` between commands to help with debugging and ensure proper signal handling.

- **Job Executor Server (JES)**  
  - Handles incoming commands via `SIGUSR1`.  
  - Detects child termination via `SIGCHLD` and respawns processes as needed.  
  - Processes commands and executes jobs accordingly.  

- **Helper functions (`funcs.cpp` / `funcs.h`)**  
  - `mystring`: convert `char*` to `std::string`.  
  - `rmprhase`: extract the essential part of a command.  
  - `issuejob`: store a command in the waiting queue and run jobs based on priority/order.

---

## Files
- **jobExecutor.cpp** – main program and Job Commander logic.  
- **jobExecutorServer.cpp** – Job Executor Server logic and signal handlers.  
- **funcs.cpp / funcs.h** – small utility functions for command parsing and job queue management.  

---

## Build & Run
```bash
make         # compile the project
make clean   # remove build files
./jobCommander
