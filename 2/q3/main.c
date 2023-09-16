#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

int main(){
    struct task_struct *task;
    for_each_process(task) {
        printk(KERN_INFO "Process ID: %d, Command: %s\n", task->pid, task->comm);
    }
    
    kernel_headers
    
}