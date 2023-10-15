#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

int main(){
    FILE *fp;
    fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("Error opening output.txt");
        exit(EXIT_FAILURE);
    }
    pid_t pids[3];
    char *policy_str[] = {"SCHED OTHER","SCHED RR","SCHED FIFO"};
    struct timespec start[3];
    for(int i=0;i<3;i++){
        
        pids[i] = fork();
        clock_gettime(CLOCK_MONOTONIC, &start[i]);
        if (pids[i]<0){
            perror("Forking Unsuccessful");
        }
        else if (pids[i]==0){
            struct sched_param p;
            int sched_policy;
            if (i==0){
                p.sched_priority =0;
                sched_policy = SCHED_OTHER;
            }
            else if (i==1){
                p.sched_priority =1;
                sched_policy = SCHED_RR;
            }
            else if (i==2){
                p.sched_priority =1;
                sched_policy = SCHED_FIFO;
            }
            int sched_check = sched_setscheduler(0,sched_policy,&p);
            if (sched_check == -1){
                perror("Scheduler error");
                exit(1);
            }
            execl("./Counter", "./Counter", NULL);
            perror("Error in executing Counter");
            exit(1);
        }
    }
    int counter =0;
    while(counter<3) {
        int status;
        pid_t process_pid = waitpid(-1, &status, WNOHANG);

        if(process_pid>0){
            int process_no =-1;
            for (int j=0;j<3;j++){
                if (process_pid == pids[j]){
                    process_no = j+1;
                    break;
                }
            }


            if (process_no != -1){
                struct timespec end;
                clock_gettime(CLOCK_MONOTONIC, &end);
                double del_time = (end.tv_sec - start[process_no-1].tv_sec) + (end.tv_nsec - start[process_no-1].tv_nsec) / 1e9;
                fprintf(fp,"%f\n",del_time);
                printf("Process %d with scheduling policy %s with execution time: %f seconds\n", process_no,policy_str[process_no-1] ,del_time);
                counter++;
            }
        }
    }
    fclose(fp);
    return 0;
}
