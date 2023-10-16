#include <stdio.h>
#include <stdlib.h>

#include "dll.h"
int main(){
    struct node* pHeadNode = constructNode(NULL, NULL, NULL, NULL);

    printf("%p\n", pHeadNode -> startAddr);   

    return 0;
}