## Assignment Tasks Overview

In this assignment, we focused on three key tasks related to operating systems:

### Task 1: Process Management
We started by creating a parent-child process relationship using the vfork() system call. The parent process was responsible for generating the Fibonacci series up to the 16th element, while the child process efficiently calculated the factorial of 4. To ensure proper synchronization, we utilized the wait() function, enabling specific print statements to appear in the order: A, C, D, B.

### Task 2: Custom Shell Commands
In the second task, we developed three distinct custom commands for a shell:
1. **word Command:** This built-in command enabled us to count the number of words in a text file. We incorporated options such as -n to ignore new line characters and -d to calculate differences in word sizes between two text files.
2. **dir Command:** A significant aspect of our shell, this external command facilitated the creation of directories. We included options like -r to remove an existing directory and -v to display detailed messages during its execution.
3. **date Command:** Another external command we implemented returned the last modified date and time of a given file. This command featured options like -d to display time described by STRING and -R to provide output in RFC 5322 format.

### Task 3: Arithmetic Calculator
Our final task centered around creating a useful arithmetic calculator. We achieved this by crafting a bash script that read inputs from "input.txt". These inputs consisted of two numbers and an operation. After performing the specified operation, we saved the result in a new text file named "output.txt". To ensure proper organization, we created a "Result" directory if it did not already exist.

Overall, this assignment allowed us to delve into fundamental operating system concepts, process management, shell command creation, and scripting techniques. By addressing each task's specific requirements and following the provided rubrics, we were able to create functional and efficient solutions.

*The above content was generated with aid from openAI's chatGPT*