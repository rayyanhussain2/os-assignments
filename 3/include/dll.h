#include <sys/mman.h>
//implementation of a custom doubly linked list  
struct node{
    void* pAllocAddress;
    _Bool isSub;
    struct node* pSubChain;
    long size;
    struct node* pPrev;
    struct node* pNext;
    _Bool isHole;
    _Bool deleted;
}; 

int sizeNode = sizeof(struct node);
#define PAGE_SIZE 4096

//scans through the array of struct node pointers and returns an empty spot for a node to be inserted. 
//Incase there is no free spots, the function alllocates another additional page and copies all previous data and updates the 
//start address.
struct node* returnNewAddr(int* currIndex, int* totalNodes, int* pageMultiple, struct node** pPStartAddr){
    //first do linear scan to search for deletd nodes
    for(int i = 0; i < *currIndex; i++){
        if(((*pPStartAddr) + i) -> deleted){
            return (*(pPStartAddr) + i);
        }
    }
    
    //If there's nothing you could find from the linear search then we increment the currIndex or allocate a new page
    //checks for a free place on the page, else invokes the copy function.
    //returns a free memory address
    if(*currIndex < *totalNodes){
        *currIndex += 1;
        return *pPStartAddr + *currIndex;
    }else{
        //Create a new memory allocation with increased size
        *pageMultiple += 1;
        struct node* newStartAddr = (struct node*)mmap(NULL, ((*pageMultiple) * PAGE_SIZE), PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0);
        if(newStartAddr == MAP_FAILED){
            perror("Fatal error: Failed to allocate memory.");
            exit(-1);
        }

        if(*pageMultiple > 1){
        //copy the old stuff only if the pageMultiple is greater than 1     
            for(int i = 0; i < *currIndex; i++){
                (newStartAddr + i) -> pSubChain = ((*pPStartAddr) + i) -> pSubChain; 
                (newStartAddr + i) -> size = ((*pPStartAddr) + i)  -> size; 
                (newStartAddr + i) -> pPrev = ((*pPStartAddr) + i)  -> pPrev; 
                (newStartAddr + i) -> pNext = ((*pPStartAddr) + i)  -> pNext; 
                (newStartAddr + i) -> isHole = ((*pPStartAddr) + i)  -> isHole; 
                (newStartAddr + i) -> deleted = ((*pPStartAddr) + i)  -> deleted; 
                (newStartAddr + i) -> pSubChain = ((*pPStartAddr) + i)  -> pSubChain; 
                (newStartAddr + i) -> isSub = ((*pPStartAddr) + i)  -> isSub; 

            }

            //unallocate the old stuff
            if(munmap(*pPStartAddr, *pageMultiple * PAGE_SIZE) == -1){
                perror("Fatal error: Failed to de-allocate memory.");
                exit(-1);   
            }
        }

        //update global variables
        *totalNodes = ((*pageMultiple) * PAGE_SIZE) / sizeNode;
        *pPStartAddr  = newStartAddr; 
 
        return *pPStartAddr + *currIndex;
    }
}
 
//delete node, given a address
void deleteNode(struct node* pNodeAddress){
    pNodeAddress -> deleted = 1;
}