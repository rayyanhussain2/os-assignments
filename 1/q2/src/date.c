#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
    /*
    Error codes:
    0 - Normal
    1 - file errors
    2 - Argument / Input errors
    3 - fork errors
    */

long int returnTD(struct stat* pFile){
    return pFile -> st_mtime;
}

void printrfc(long timestamp){
    //long timestamp, gets converted to utc and stored in the struct
    struct tm* time;
    time = gmtime(&timestamp);

    //printing acc to custom format
    char output[256];
    strftime(&output[0], sizeof(output), "%a, %d %b %Y %H:%M:%S +0000\n", time);

    fputs(output, stdout);
}

void printD(long timestamp, char* STRING){
    //long timestamp, gets converted to utc and stored in the struct
    struct tm* time;
    time = localtime(&timestamp);

    //printing acc to custom format
    char output[256];
    strftime(&output[0], sizeof(output), STRING, time);

    fputs(output, stdout);
    fputs("\n", stdout);
}

int main(int argc, char** argv){
    short returnStatus;
    if(argc == 1){
        fputs("Error: Insufficient arguments\n", stderr);
        return 2;
    }
    else if(argc == 2){
        struct stat file;
        returnStatus = stat(*(argv + 1), &file);
        if(returnStatus == -1){
            perror("Error");
            return 1;
        }
        printf("%li\n", returnTD(&file));
    }
    else if(argc == 3){
        if(strcmp(*(argv+1), "-R") == 0){
            struct stat file;
            returnStatus = stat(*(argv + 2), &file);
            if(returnStatus == -1){
                perror("Error");
                return 1;
            }
            printrfc(returnTD(&file));
        }
    }else if(argc == 4){
        if(strcmp(*(argv + 1), "-d") == 0){
            struct stat file;
            returnStatus = stat(*(argv + 3), &file);
            if(returnStatus == -1){
                perror("Error");
                return 1;
            }
            printD(returnTD(&file), *(argv + 2));
        }
    }else{
        fputs("Error: Too many arguments\n", stderr);
        return 2;
    }
    
    return 0;
}