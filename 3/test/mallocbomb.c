#include <stdlib.h>
#include <stdio.h>
#define COUNT 3
#include "../include/mems.h"
#define ITER 50

int main() {
    for(int i = 0; i < ITER; i++){
        mems_malloc(100); //100 bytes
        mems_print_stats();
    }

    return 0;
}
