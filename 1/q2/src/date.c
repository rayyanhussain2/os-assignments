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
    //long timestamp, gets converted to gm and stored in the struct
    struct tm* time;
    time = gmtime(&timestamp);

    //printing acc to custom format
    char output[256];
    strftime(&output[0], sizeof(output), "%a, %d %b %Y %H:%M:%S +0000\n", time);

    fputs(output, stdout);
}

void printDef(long timestamp){
    //long timestamp, gets converted to gm and stored in the struct
    struct tm* time;
    time = gmtime(&timestamp);

    //printing acc to custom format
    char output[256];
    strftime(&output[0], sizeof(output), "%a, %d %b %Y %H:%M:%S IST\n", time);

    fputs(output, stdout);
}

int printD(long timestamp, char* STRING){
    struct tm* time;
    time = localtime(&timestamp);
    //long timestamp, gets converted to localtime and stored in the struct

/*
"now"
"today"
"yesterday"
"tomorrow"
"in a minute"
"in an hour"
"in a day"
"in a month"
"in a year"
"a minute ago"
"an hour ago"
"a day ago"
"a month ago"
"a year ago"
*/
    char STRING2[] = "%a, %d %b %Y %H:%M:%S IST\n";
    bool isCustom = 1;
    
    char output[256];
    if(strcmp(STRING, "\"yesterday\"") == 0){
        time-> tm_mday -= 1;
        time-> tm_wday -= 1;
        time-> tm_yday -= 1;
    }else if(strcmp(STRING, "\"tomorrow\"") == 0){
        time-> tm_mday += 1;
        time-> tm_wday += 1;
        time-> tm_yday += 1;
    }else if (strcmp(STRING, "\"in_a_minute\"") == 0){
        time -> tm_min += 1;

    }else if(strcmp(STRING, "\"in_an_hour\"") == 0){
        time -> tm_hour += 1;

    }else if(strcmp(STRING, "\"in_a_day\"") == 0){
        time-> tm_mday += 1;
        time-> tm_wday += 1;
        time-> tm_yday += 1;
    }else if(strcmp(STRING, "\"in_a_month\"") == 0){
        time-> tm_mon += 1;
    }else if(strcmp(STRING, "\"in_a_year\"") == 0){
        time-> tm_year += 1;
    }else if(strcmp(STRING, "\"a_minute_ago\"") == 0){
        time -> tm_min -= 1; 

    }else if(strcmp(STRING, "\"an_hour_ago\"") == 0){
        time ->tm_hour -= 1;
    }else if(strcmp(STRING, "\"a_day_ago\"") == 0){
        time-> tm_mday -= 1;
        time-> tm_wday -= 1;
        time-> tm_yday -= 1;
    }else if(strcmp(STRING, "\"a_month_ago\"") == 0){
        time ->tm_mon -= 1;
    }else if(strcmp(STRING, "\"a_year_ago\"") == 0){
        time -> tm_year -= 1;
    }else if(strcmp(STRING, "\"today\"") == 0){
    }else{
        fputs("Error: Unkown relative time string.", stderr);
        return 2;
    }
    
    strftime(&output[0], sizeof(output), STRING2, time); 
    
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
        printDef(returnTD(&file));
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
            return printD(returnTD(&file), *(argv + 2));
        }
    }else{
        fputs("Error: Too many arguments\n", stderr);
        return 2;
    }
    
    return 0;
}