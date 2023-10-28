/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include<stdio.h>
#include<stdlib.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096
#include "dll.h"
struct node* pHead;
int currIndex;
int totalNodes;
int pageMultiple;

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init(){
    currIndex = 0;
    totalNodes = 0;
    pageMultiple = 0;
    returnNewAddr(&currIndex, &totalNodes, &pageMultiple, &pHead);

    //Phead is just a dummy node
    pHead -> size = 0;
    pHead -> pNext = NULL;
    pHead -> pPrev = NULL;
    pHead ->pAllocAddress = NULL;
    pHead -> pSubChain = NULL;
    pHead -> isHole = 0;
    pHead -> isSub = 0;
    pHead ->deleted = 0;
    //making all nodes available
}
/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 
void* mems_malloc(size_t reqSize){
    //Search for free space on the list
    struct node* pHeadIter = pHead;
    unsigned long long totalSize = 0;
    while(pHeadIter != NULL){
        //Each main list node
        //Search for subnodes
        struct node* pSubNode = pHeadIter -> pSubChain;
        while(pSubNode != NULL){
            if(pSubNode -> isHole && ((pSubNode -> size) >= reqSize)){
                //add a node distributing the size
                if((pSubNode -> size) - reqSize > 0){
                pSubNode -> isHole = 0;
                
                //setting up the new node
                struct node* pNewSubNode = returnNewAddr(&currIndex, &totalNodes, &pageMultiple, &pHead);
                pNewSubNode -> isHole = 1;
                pNewSubNode -> deleted = 0;
                pNewSubNode -> pNext = pSubNode -> pNext;
                pNewSubNode -> pPrev = pSubNode;
                pNewSubNode -> pSubChain = NULL;
                pNewSubNode -> size = (pSubNode -> size) - reqSize;
                pNewSubNode -> pAllocAddress = (pSubNode -> pAllocAddress) + reqSize; //Allocating the starting address of the memory allocated for user.
                
                //modifying the previous node
                pSubNode -> pNext = pNewSubNode;
                pSubNode -> size = reqSize;
                pSubNode -> isHole = 0;

                //modifying the next node
                if(pNewSubNode -> pNext != NULL){
                    pNewSubNode -> pNext -> pPrev = pNewSubNode;
                }

                //return the process nodes pAllocated
                return (void *) totalSize;
                }else{//if the hole is exact size that the user requested
                    pSubNode -> isHole = 0;
                    return (void *) totalSize;
                }

            }
            totalSize += pSubNode -> size;
            pSubNode = pSubNode -> pNext;

        }
        pHeadIter = pHeadIter -> pNext;
    }

    //If there were no free space on the list
    //Allocate a new page/pages
    pHeadIter = pHead;
    totalSize = 0;
    while(pHeadIter -> pNext != NULL){ 
         pHeadIter = pHeadIter -> pNext;
         totalSize += pHeadIter -> size;
    }

    struct node* pNewNode = returnNewAddr(&currIndex, &totalNodes, &pageMultiple, &pHead);
    pHeadIter -> pNext = pNewNode;
    pNewNode -> isHole = 0;
    pNewNode -> deleted = 0;
    pNewNode -> isSub = 0;
    pNewNode -> pNext = NULL;
    pNewNode -> pPrev = pHeadIter;
    pNewNode -> pSubChain = returnNewAddr(&currIndex, &totalNodes, &pageMultiple, &pHead);

    //calculating the multiple for the PAGESIZE
    int multipleCalc;
    if(reqSize % PAGE_SIZE > 0){
        multipleCalc = (reqSize / PAGE_SIZE) + 1;
    }else{
        multipleCalc = reqSize / PAGE_SIZE;
    }
    pNewNode -> pAllocAddress = (void *)mmap(NULL, multipleCalc*PAGE_SIZE, PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0);  
    pNewNode -> size = multipleCalc * PAGE_SIZE;

    //Create sublist children nodes
    struct node* pSubListNode1 = pNewNode -> pSubChain;
    pSubListNode1 -> isHole = 0;
    pSubListNode1 -> deleted = 0;
    pSubListNode1 -> isSub = 1;
    pSubListNode1 -> pNext = NULL;
    pSubListNode1 -> pPrev = NULL;
    pSubListNode1 -> pSubChain = NULL;
    pSubListNode1 -> pAllocAddress = pNewNode -> pAllocAddress;   
    pSubListNode1 -> size = reqSize;
    
    //the hole node if theres free space
    if((pNewNode -> size) > reqSize){
        pSubListNode1 -> pNext = returnNewAddr(&currIndex, &totalNodes, &pageMultiple, &pHead);
        struct node* pSubListNode2 = pSubListNode1 -> pNext;

        pSubListNode2 -> isHole = 1;
        pSubListNode2 -> deleted = 0;
        pSubListNode2 -> isSub = 1;
        pSubListNode2 -> pNext = NULL;
        pSubListNode2 -> pPrev = pSubListNode1;
        pSubListNode2 -> pSubChain = NULL;
        pSubListNode2 -> pAllocAddress = pSubListNode1 -> pAllocAddress + reqSize;   
        pSubListNode2 -> size = pNewNode -> size - pSubListNode1 -> size;        
    }   

    return (void *)(totalSize);
}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    //clears not only all user allocated pages but also the pages allocated by the freeList
    struct node* pHeadIter = pHead -> pNext;
    while(pHeadIter != NULL){
        if(munmap(pHeadIter -> pAllocAddress, pHeadIter -> size) == -1){
            perror("Fatal error: Failed to de-allocate memory.");
            exit(-1); 
        }
        pHeadIter = pHeadIter -> pNext;
    }

    if(munmap(pHead, pageMultiple * PAGE_SIZE) == -1){
        perror("Fatal error: Failed to de-allocate memory.");
        exit(-1); 
    }

    pHead = NULL;
}


/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){
    long totalAllocatedSize = 0;
    long totalFreeSize = 0;
    int i = 0, j = 0;

    struct node* pHeadIter = pHead -> pNext;
    while(pHeadIter != NULL){
        totalAllocatedSize += pHeadIter ->size;
        printf("\nMain Node %d: ", ++i);
        printf("%ld Page(s)\n", pHeadIter -> size/PAGE_SIZE);
        printf("-----------\n");

        struct node* pSubNode = pHeadIter -> pSubChain;
        j = 0;
        while(pSubNode != NULL){
            if(pSubNode -> isHole){
                totalFreeSize += pSubNode -> size;
            }  
            
            if(pSubNode -> isHole){
                printf("Chain %d Node %d - Type: Hole, Size: %li byte(s)\n", i, ++j , pSubNode -> size);
            }else{
                printf("Chain %d Node %d - Type: Process, Size: %li byte(s)\n", i, ++j , pSubNode -> size);
            }
            pSubNode = pSubNode -> pNext;
        }
        pHeadIter = pHeadIter -> pNext;
    }

    printf("\nTotal Pages allocated: %ld\n", totalAllocatedSize/PAGE_SIZE);
    printf("Total free memory: %li Byte(s)\n", totalFreeSize);

}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void* v_ptr){
    if(v_ptr < 0){
        fputs("Invalid virtual pointer\n", stdout);
        return (void *) -1;
    }
    struct node* pHeadIter = pHead -> pNext;
    while(pHeadIter != NULL){
        if((v_ptr < (void*)((long)pHeadIter -> size))){
            struct node* pSubNode = pHeadIter -> pSubChain;
            while(pSubNode != NULL){
                //Im in the one of   the subnodes. if the v_ptr - size is < 0, then thatt's the node.
                //So i would wanna return the pallocaddress + abs(v_ptr - size)
                if(v_ptr < (void*)((long)pSubNode -> size)){
                    if(!(pSubNode -> isHole)){
                    return (pSubNode -> pAllocAddress) + (long)(v_ptr);
                    }else{
                        fputs("Invalid virtual pointer\n", stdout);
                        return (void *) -1;
                    }
                }
                v_ptr -= pSubNode -> size;
                pSubNode = pSubNode -> pNext;
            }
        }else{
            v_ptr -= pHeadIter -> size;
            pHeadIter = pHeadIter -> pNext;
        }
    }
    fputs("Invalid virtual pointer\n", stdout);
    return (void *) -1;
}


//Function that merges contiguous hole nodes
void checkContiguousMerge(){
    struct node* pHeadIter = pHead -> pNext;
    while(pHeadIter != NULL){
        struct node* pSubNode = pHeadIter -> pSubChain;
        while(pSubNode -> pNext != NULL){
            if(pSubNode -> isHole && pSubNode -> pNext -> isHole){
                struct node* pNextSub = pSubNode -> pNext; //saving the node for next iteration
                
                //Merge node 1 into node 2
                //node 2 changes
                pNextSub -> size += pSubNode -> size;
                pNextSub -> pAllocAddress = pSubNode -> pAllocAddress;
                pNextSub -> pPrev = pSubNode -> pPrev;
                if(pNextSub -> pPrev != NULL)
                    pNextSub -> pPrev -> pNext = pNextSub;

                //node 1 changes
                pSubNode -> deleted = 1;


                pSubNode = pNextSub;
            }else{
                pSubNode = pSubNode -> pNext;
            }
        }
        pHeadIter = pHeadIter -> pNext;
    }
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    if(v_ptr < 0){
        fputs("Invalid virtual pointer\n", stdout);
        return;
    }
    struct node* pHeadIter = pHead -> pNext;
    while(pHeadIter != NULL){
        if((v_ptr < (void*)((long)pHeadIter -> size))){
            struct node* pSubNode = pHeadIter -> pSubChain;
            while(pSubNode != NULL){
                //Im in the one of   the subnodes. if the v_ptr - size is < 0, then thatt's the node.
                //So i would wanna return the pallocaddress + abs(v_ptr - size)
                if(v_ptr < (void*)((long)pSubNode -> size)){
                    if(!(pSubNode -> isHole)){
                        pSubNode -> isHole = 1;

                        //check for contiguous hole nodes and merge them
                        checkContiguousMerge();

                        return;
                    }else{
                        fputs("Invalid virtual pointer\n", stdout);
                        return;
                    }
                }
                v_ptr -= pSubNode -> size;
                pSubNode = pSubNode -> pNext;
            }
        }else{
            v_ptr -= pHeadIter -> size;
            pHeadIter = pHeadIter -> pNext;
        }
    }
    fputs("Invalid virtual pointer\n", stdout);
    return;
}
