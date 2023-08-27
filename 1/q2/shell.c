    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include "word.c"
    #include <string.h>

    #define maxarglen 256
    #define maxargc 11

    int main(){
        short prevStatus = 100;
        char cwd[] = "[~]% ";


        char currCMD[maxarglen * maxargc];
        while(true){
            fputs(cwd, stdout);
            fgets(currCMD, maxarglen * maxargc, stdin);

            char* currArg;
            char* argv[maxargc];
            short argc = 0;

            while(argc < 11){
                if(argc > 0)
                    currArg = strtok(NULL, " ");
                else
                    currArg = strtok(currCMD, " ");

                if(currArg != NULL){
                    argv[argc] = currArg;
                    argc += 1;
                }else{
                    break;
                }
            }
            argv[argc-1][strlen(argv[argc-1]) - 1] = '\0'; 
            *(argv + argc) = NULL;

            //internal commands
            if(strcmp(argv[0], "word") == 0){
                word(argc, argv);
            }else{
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