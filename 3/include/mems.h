#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

//doubly linked list
struct node{
    unsigned long long index; //for copying purpose
    void* pUserReqMem;
    _Bool isSub;
    long size;
    struct node* pSubChain;
    struct node* pPrev;
    struct node* pNext;
    _Bool isHole;
    _Bool deleted;
}; 

//----------------------Helper Methods---------------------------------
struct node* findNewNode();
void mergeContiguous();

//---------------------Main API----------------------
void* mems_malloc(size_t reqSize);
int mems_free(void* v_ptr);
void mems_print_stats();