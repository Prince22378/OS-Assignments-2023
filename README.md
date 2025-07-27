# 🧠 Operating Systems Assignments – IIIT Delhi

This repository contains a collection of Operating Systems assignments implemented in **C** on **Linux**, demonstrating foundational OS concepts through hands-on coding.

> **Guide**: Dr. Vivek Kumar  
> **Tech Stack**: C Language, Linux

---

## 📂 Contents

### 1. `Simple_Loader`
A basic memory loader implementation that mimics how OS loads executables into memory.

### 2. `Simple_Shell`
A minimal command-line shell that supports command parsing, I/O redirection, piping, and background jobs.

### 3. `Simple_Scheduler`
Simulates CPU scheduling algorithms like FCFS, Round Robin, and Priority Scheduling with performance metrics.

### 4. `Simple_SmartLoader`
An enhanced memory loader that supports lazy loading and mimics page faults and demand paging concepts.

### 5. `Simple_MultiThreader`
Demonstrates multithreading with thread creation, synchronization, and scheduling using `pthreads`.

---

## 📌 Project Highlights

- Compilation of real OS-level assignments.
- Explores concepts such as:
  - ELF file structure and loading
  - Lazy loading and page faults
  - Signals and pipes
  - Scheduling algorithms
  - Multithreading and synchronization
- Acts as a practical bridge to theoretical concepts taught in OS courses.

---

## 🛠️ How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/prajil22359/OS-Assignments-2023.git
   cd OS-Assignments-2023

2. Compile an assignment (example for Simple_Shell):
   ``` bash
   cd Simple_Shell
   make
   ./shell
   ``` 
Ensure you are running on a Linux-based system with gcc and make installed.

## 📄 License
This repository is intended for academic learning purposes only.

## 🙌 Acknowledgements
Special thanks to Dr. Vivek Kumar for guidance and for enabling practical learning through these assignments.
