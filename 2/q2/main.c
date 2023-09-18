#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h> 
#include <sched.h>
#include <unistd.h>

void child_process(int num, int sched_policy){
    int status;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    pid_t child_pid = fork();
    if (child_pid<0){
        perror("Forking unsucessful");
    }
    else if (child_pid==0){
        struct sched_param p = {.sched_priority =0};
        sched_setscheduler(0,sched_policy,&p);
        execl("./Counter.c","./Counter.c",(char *)0);
    }
    else{
        waitpid(child_pid,&status,0);
        clock_gettime(CLOCK_MONOTONIC,&end);
        double del_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
        printf("Process %d execution time: %f seconds\n",num,del_time);
    }
}
int main(){

    child_process(1,SCHED_OTHER);
    child_process(2,SCHED_RR);
    child_process(3,SCHED_FIFO);   

    return 0;
}
