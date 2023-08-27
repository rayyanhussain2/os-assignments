#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int word(int argc, char* argv[]){

    //Test code
    /*
    printf("%d\n", argc);
    for(int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    */

    //Error cases
    if(argc <= 1){
        noFileError();
        return 1;
    }else{
        //checking the first argument for options
        if(strcmp(argv[1], "-n") == 0){
            //error handling
            if(argc == 2){
                noFileError();
                return 1;
            }else if(argc > 3){
                fputs("Error: Too many arguments\n", stderr);
                return 2;
            }
            //Cleared of errors, execute
            return nOption(argc, &argv[0]);
        }else if(strcmp(argv[1], "-d") == 0){
            if(argc == 3){
                fputs("Error: Insufficient arguments\n", stderr);
                return 2;
            }else if(argc > 4){
                fputs("Error: Too many arguments.\n", stdout);
                return 2;
            }else{
                return dOption(argc, &argv[0]);
            }
        }else{
            //no option passed, read the one and only file
            if(argc > 2){
                fputs("Error: Insufficient arguments\n", stderr);
                return 2;
            }
            return noOption(argc, argv);            
        }

    }
    
    return 0;
}

long long wCountFunc(FILE* p2File, bool isN){
    long long res = 0;
    bool wordBegan = false;
    char currChar;

    while(true){
        //fetch char and check for EOF
        currChar = fgetc(p2File);
        if (currChar == EOF){
            if(wordBegan)
                res += 1;
            break;
        }

        //Ignore newline
        if(currChar == '\n' && isN){
            continue;   
        }

        //If its a valid character 
        if(isalnum(currChar) && !wordBegan){
            wordBegan = true;
        }else if(!isalnum(currChar) && wordBegan){
            res += 1;
            wordBegan = false;
        }
    }

    return res;
}

void noFileError(){
    fputs("Error: No valid file provided\n", stderr);
    return;
}

//wrapper for wc -w
int nOption(int argc, char** argv){
    FILE* pFile = fopen(*(argv+2), "r");
    if(pFile == NULL){
        perror(*(argv+2));
        return 1;
    }else{
        printf("Word count: %lli\n", wCountFunc(pFile, true));
        fclose(pFile);
        return 0;
    } 
}

int dOption(int argc, char** argv){
    FILE* pFile = fopen(*(argv+2), "r");
    FILE* pFile2 = fopen(*(argv+3), "r");
    if(pFile == NULL){
        perror(*(argv+2));
        return 1;
    }else if(pFile2 == NULL){
        perror(*(argv+3));
        return 1;
    }else{
        printf("Word count delta: %lli\n", abs(wCountFunc(pFile, false) - wCountFunc(pFile2, false)));
        fclose(pFile);
        fclose(pFile2);
        return 0;
    }
}

int noOption(int argc, char** argv){
    FILE* pFile = fopen(*(argv+1), "r");
    if(pFile == NULL){
        perror(*(argv+1));
        return 1;
    }else{
        printf("Word count: %lli\n", wCountFunc(pFile, false));
        fclose(pFile);
        return 0;
    }
}
