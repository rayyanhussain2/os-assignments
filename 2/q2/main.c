#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

double process_sched(int num, int sched_policy, int priority) {
    int status;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("Forking unsuccessful");
    } else if (child_pid == 0) {
        struct sched_param p = {.sched_priority = priority};
        if (sched_setscheduler(0, sched_policy, &p) != 0) {
            perror("Error setting scheduling policy");
            exit(EXIT_FAILURE);
        }

        execl("./Counter", "./Counter", NULL);
        perror("Error in executing Counter");
        exit(EXIT_FAILURE);
    } else {
        waitpid(child_pid, &status, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double del_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Process %d execution time: %f seconds\n", num, del_time);
        return del_time;
    }
}

int main() {
    FILE *fp;
    fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("Error opening output.txt");
        exit(EXIT_FAILURE);
    }
    double time1=process_sched(1,SCHED_OTHER ,0);
    double time2=process_sched(2, SCHED_RR, 99);
    double time3=process_sched(3, SCHED_FIFO, 99);
    fprintf(fp,"%f\n",time1);
    fprintf(fp,"%f\n",time2);
    fprintf(fp,"%f",time3);
    fclose(fp);
    return 0;
}

