    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include "word.c"
    #include <string.h>

    #define maxarglen 256
    #define maxargc 10


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

            while(1){
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

            if(strcmp(argv[0], "word") == 0){
                word(argc, argv);
            }
        }
        return 0;
    }