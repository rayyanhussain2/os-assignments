#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main (int argc,char *argv[]){
    pid_t PID_of_Child = fork();
    if (PID_of_Child<0){
        fprintf(stderr,"Fork Failed");
    }
    else if (PID_of_Child==0){
        printf("Child is having ID %d\n",getpid());
        printf("My parent ID is %d\n",getppid());
    }
    else{
        printf("Parent(P) is having ID %d\n",getpid());
        wait(NULL);
        printf("ID of P's child is %d\n",PID_of_Child);
    }
}