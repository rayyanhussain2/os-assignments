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

/*
Error codes:
0 - Normal
1 - File errors
2 - Argument / Input errors
3 - fork errors
*/

/*approach 
tackle all the errors for the input
for -n -> create a child process and exec wc -w
for -d -> check the wc method itself

*/

long long wCountFunc(FILE* p2File, bool isN);

void noFileError();

int nOption(int argc, char** argv);

int dOption(int argc, char** argv);

int noOption(int argc, char** argv);

int word(int argc, char* argv[]);

#include "../src/word.c"