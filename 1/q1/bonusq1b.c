#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int factorial(int n){
    if (n<=1) return 1;
    else return n*factorial(n-1);
}

void fibonacci(int n){
    int n1=0,n2=1,n3;
    printf("Fibonacci Series upto %d: %d,%d ",n,n1,n2);
    for (int i=2;i<n;i++){
        n3 = n1+n2;
        printf("%d",n3);
        if (i<n-1){
            printf(", ");
        }
        n1=n2;
        n2=n3;

    }
    printf("\n");
}

int main (int argc,char *argv[]){
    pid_t PID_of_Child = fork();
    int result;
    if (PID_of_Child<0){
        fprintf(stderr,"Fork Failed");
    }
    else if (PID_of_Child==0){
        result = factorial(4);
        printf("Child process: Factorial of 4: %d\n",result);
        _exit(0);
    }
    else{
        printf("Parent process:");
        fibonacci(16);
        waitpid(PID_of_Child,NULL,0);
    }
    return 0;
}