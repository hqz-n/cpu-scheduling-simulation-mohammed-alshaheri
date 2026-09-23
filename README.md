# CPU Scheduling Algorithms Simulation Using Custom Data Structures

A complete C++ simulation of fundamental CPU scheduling algorithms implemented using **native custom data structures** without relying on external libraries (compatible with C-Free / MinGW).

---

## 📌 Project Overview
The project models CPU execution by taking arithmetic expressions alongside process metadata to calculate scheduling performance metrics:
- **Completion Time (CT)**
- **Turnaround Time (TAT)**: $TAT = CT - ArrivalTime$
- **Waiting Time (WT)**: $WT = TAT - BurstTime$

---

## 🛠️ Implemented Scheduling Algorithms
1. **First-Come, First-Served (FCFS)** — Non-preemptive scheduling using a custom FIFO Queue.
2. **Shortest Job First (SJF)** — Non-preemptive scheduling using a Min-Priority Queue sorted by Burst Time.
3. **Priority Scheduling** — Non-preemptive scheduling using a Min-Priority Queue (smaller integer = higher priority).
4. **Round Robin (RR)** — Preemptive time-sliced execution using a custom FIFO Queue.

---

## 🧱 Custom Data Structures Built from Scratch
- `ProcessList`: Singly Linked List for sequential process registration.
- `IntQueue`: Standard FIFO Queue using linked nodes.
- `IntPriorityQueue`: Ascending priority queue using sorted linked insertion ($O(1)$ peek).
- `StringStack`: LIFO Stack used for execution history reversal and verification.

---

## 💻 Environment & Compatibility
- **Language**: C++
- **IDE**: C-Free / MinGW
- **Course**: Data Structures

---

## 👨‍💻 Credits & Dedication
- **Author**: Eng. Mohammed Al-Shaheri
- **Special Thanks**: Dedicated to ENG. Mohammed Al-Sayanni
