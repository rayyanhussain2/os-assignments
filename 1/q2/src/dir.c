#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>

int noVOption(int argc, char** argv, bool isV);

int main(int argc, char** argv){
    
    //Test code
    /*
    printf("%d\n", argc);
    for(int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    */

    /*
    Error codes:
    0 - Normal
    1 - directory errors
    2 - Argument / Input errors
    3 - fork errors
    */

    //Error cases
    short status;
    if(argc == 1){
        fputs("Error: Insufficient arguments\n", stderr);
        return 2;
    }else if(argc == 2){
        return noVOption(argc, argv, 0);
    }else if(argc == 3){
        if(strcmp(*(argv+1), "-r") == 0){
            //delete if exists and create
            status = mkdir(*(argv+2), 0777);
            if(status == -1){
                rmdir(*(argv + 2));
                status = mkdir(*(argv+2), 0777);
                chdir(*(argv+2));
            }
        }else if(strcmp(*(argv + 1), "-v") == 0){
            return noVOption(argc, argv, 1);
        }
    }else{
        fputs("Error: Too many arguments\n", stderr);
        return 2;
    }
    
    return 0;
}

int noVOption(int argc, char** argv, bool isV){
    short status;
    if(isV){
        fputs("Creating directory \"", stdout);
        fputs(*(argv+2), stdout);
        fputs("\"...\n", stdout);
        status = mkdir(*(argv + 2), 0777);
    }else{
        status = mkdir(*(argv+1), 0777);
    }
    
    if(status == -1){
        if(isV){
            fputs("Error: Cannot create a directory ", stderr);
            fputs(*(argv+2), stderr);
            fputs(" : Directory exists.", stderr);
            fputs("\n", stderr);
            return 1;
        }else{
            fputs("Error: Cannot create a directory ", stderr);
            fputs(*(argv+1), stderr);
            fputs(" : Directory exists.", stderr);
            fputs("\n", stderr);
            return 1;
        }
    }else if(status == 0){
        if(isV){
            fputs("Changing directory to \"", stdout);
            fputs(*(argv+2), stdout);
            fputs("\" ...\n", stdout);
            chdir(*(argv+2));
        }else{
            chdir(*(argv+1));
        }
        return 0;
    }
}