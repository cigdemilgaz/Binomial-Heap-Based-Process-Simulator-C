# Process Simulator with Binomial Heap (C)

This project is a system-level process scheduler implemented in C. It uses a **Binomial Heap** data structure to manage a priority queue of processes and simulates a scheduling algorithm to find the optimal time quantum ($q$) that minimizes the Average Waiting Time (AWT).

## Key Features

* **Binomial Heap Implementation:** Efficient min-priority queue operations for process management.
* **Dynamic Priority Calculation:** Uses a non-linear priority formula based on remaining execution time and maximum execution time ($E_{max}$).
* **AWT Optimization:** Automatically iterates through different time quanta to determine the most efficient scheduling configuration.
* **Detailed Execution Logs:** Provides a step-by-step trace of the heap state and process priorities at each time step.

## Technical Details

### Priority Formula
For the first insertion, the priority is equal to the remaining execution time. For subsequent re-insertions, the priority is calculated as:
$$c = \frac{1}{\exp(-(\frac{2 \cdot \text{rem}}{3 \cdot E_{max}})^3)}$$
$$\text{Priority} = c \cdot \text{Remaining Execution Time}$$

### Binomial Heap Operations
The project implements core Binomial Heap logic:
- `heapInsert`: To add new or returning processes.
- `heapMin`: To extract the process with the highest priority (lowest numerical value).
- `heapUnion`: To maintain heap properties after extractions.

## Setup and Usage

### Prerequisites
* A C compiler (e.g., `gcc`) or an IDE like Dev-C++.
* An `input.txt` file in the same directory.

### Input Format
The program reads process data from `input.txt`. Each line should represent a process in the following format:
`ExecutionTime,ArrivalTime`

Example `input.txt`:
```text
10,0.0
5,2.0
8,4.5
```
### Output
The program outputs the Average Waiting Time (AWT) for each quantum $q$ and identifies the Best Quantum value that provides the minimum AWT for the simulation.
