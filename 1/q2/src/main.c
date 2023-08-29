    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include <string.h>
    #include <unistd.h>
    #include <limits.h>

    #include "../include/word.h"

    #define maxarglen 256
    #define maxargc 11

    int main(){
        short prevStatus = 100;
        char cwd[maxarglen * maxargc];


        char currCMD[maxarglen * maxargc];
        while(true){
            getcwd(cwd, sizeof(cwd));
            fputs("[", stdout);
            fputs(cwd, stdout);
            fputs("]% ", stdout);
            fgets(currCMD, maxarglen * maxargc, stdin);

            char* currArg;
            char* argv[maxargc];
            short argc = 0;
            char seekChar;
            short currindex = 0;

            while(argc < 11){
                if(argc > 0){
                    /*
                    if(currArg[currindex] == "\""){

                    }
                    currArg = strtok(NULL, "\"");
                    */
                   currArg = strtok(NULL, " ");
                }
                else
                    currArg = strtok(currCMD, " ");

                if(currArg != NULL){
                    argv[argc] = currArg;
                    argc += 1;
                    currindex += strlen(currArg);
                }else{
                    break;
                }
            }
            argv[argc-1][strlen(argv[argc-1]) - 1] = '\0'; 
            *(argv + argc) = NULL;

            //internal commands
            if(strcmp(argv[0], "word") == 0){
                word(argc, argv);
            }else if(strcmp(argv[0], "exit") == 0){
                exit(0);
            }
            
            
            //special external commands
            else if(strcmp(argv[0], "dir") == 0){
                int childStatus;
                int pid = fork();
                if(pid == 0){
                    if(argc == 1)
                        execl("bin/dir", "bin/dir", NULL);
                    else if(argc == 2)
                        execl("bin/dir", "bin/dir", *(argv+1), NULL);
                    else if(argc == 3)
                        execl("bin/dir", "bin/dir", *(argv+1), *(argv+2), NULL);
                    else
                        execl("bin/dir", "bin/dir", *(argv+1), "fsdafd", "fadsa", NULL);
                }else if(pid > 0){
                    waitpid(pid, &childStatus ,0);
                    if(childStatus == 0){
                        if(argc == 2)
                            chdir(*(argv+1));
                        else if(argc == 3)
                            chdir(*(argv + 2));
                    }
                }else{
                    perror("Error: ");
                    return 3;
                }
            }   

            //special external commands
            else if(strcmp(argv[0], "date") == 0){
                int childStatus;
                int pid = fork();
                if(pid == 0){
                    if(argc == 1)
                        execl("bin/date", "bin/date", NULL);
                    else if(argc == 2)
                        execl("bin/date", "bin/date", *(argv+1), NULL);
                    else if(argc == 3)
                        execl("bin/date", "bin/date", *(argv+1), *(argv+2), NULL);
                    else if(argc == 4)
                        execl("bin/date", "bin/date", *(argv+1), *(argv+2), *(argv + 3), NULL);
                    else
                        execl("bin/date", "bin/date", *(argv+1), "fsdafd", "fadsa", NULL);
                }else if(pid > 0){
                    wait(NULL);
                }else{
                    perror("Error: ");
                    return 3;
                }
            }   
            
            //any other ext command.
            else{
                int pid = fork();
                if(pid == 0){
                    int status = execvp(*(argv+0), argv );
                    if(status == -1){
                        fputs("Error: Command \"", stderr);
                        fputs(*(argv + 0), stderr);
                        fputs("\" not found...\n", stderr);
                    }
                }else if(pid > 0){
                    wait(NULL);
                }else{
                    perror("Error: ");
                    return 3;
                }
                
            }
        }
        return 0;
    }