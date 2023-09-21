Sure, here's a README file that describes the code:

---

**README**

**Title**: Process Scheduling Benchmark

**Description**:

This code measures the execution times of three different processes running under different scheduling policies: SCHED_OTHER, SCHED_RR (Round Robin), and SCHED_FIFO (First In, First Out). The execution times are logged to an "output.txt" file and then visualized using Python's Matplotlib library.

**Code Components**:

1. `process_sched` Function: This function creates child processes with specified scheduling policies and priorities. It measures the execution time of each child process and returns it.

2. `main` Function: The main function calls `child_process` three times with different scheduling policies and priorities. It logs the execution times to "output.txt" and then reads this file to collect the data for visualization.

3. Visualization: The Python script reads the execution times from "output.txt" and creates a bar chart using Matplotlib to visually compare the execution times of the different scheduling policies.

**Execution**:

1. Compile and run the C code to generate the "output.txt" file.
2. Run the Python script to generate a bar chart comparing execution times.
3. The bar chart shows the performance of processes under different scheduling policies.

**Dependencies**:

- C code: Requires a C compiler.
- Python script: Requires Python and Matplotlib library.