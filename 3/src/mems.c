#include "../include/mems.h"
#include <math.h>

// Global variables
struct node* pNodes = NULL;
int pagesAllocated = 0;
int totalNodes = 0;
int currNodeIndex = 0;

struct node* findNewNode(){
    for(int i = 0; i < currNodeIndex; i++){ // a linear approach might be slower in applications that make frequent calls
        if (pNodes[i].deleted){
            return pNodes + i;
        }
    }
    
    //Handling 2 Cases - Either to return the address of next node or allocate another page, copying all previous nodes.
    if(currNodeIndex < totalNodes){
        struct node* returnAddr = pNodes + currNodeIndex;
        returnAddr -> index = currNodeIndex; //mark index of new node
        
        currNodeIndex += 1;
        return returnAddr;
    }else{
        pagesAllocated += 1;    
        void* mmapAddr = (void*) mmap(NULL, ((pagesAllocated) * PAGE_SIZE), PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0); 
        if(mmapAddr == MAP_FAILED){
            perror("Fatal error: Failed to allocate memory.");
            exit(-1);
        }

        struct node* newPNodes = (struct node*) mmapAddr;

        //Recopy only if more than one page has been allocated
        if(pagesAllocated > 1){
            for(int i = 0; i < currNodeIndex; i++){
                newPNodes[i].index = i;
                newPNodes[i].pSubChain = (pNodes[i].pSubChain != NULL)? &newPNodes[pNodes[i].pSubChain -> index] : NULL; 
                newPNodes[i].isSub = pNodes[i].isSub;
                newPNodes[i].pUserReqMem = pNodes[i].pUserReqMem;
                newPNodes[i].pPrev = (pNodes[i].pPrev != NULL)? &newPNodes[pNodes[i].pPrev -> index] : NULL; //lost in transaltion wtf
                newPNodes[i].pNext = (pNodes[i].pNext != NULL)? &newPNodes[pNodes[i].pNext -> index] : NULL; //
                newPNodes[i].isHole = pNodes[i].isHole;
                newPNodes[i].deleted = pNodes[i].deleted;
                newPNodes[i].size = pNodes[i].size;
            }
            
            //unallocate the old stuff
            if(munmap(pNodes, (pagesAllocated - 1) * PAGE_SIZE) == -1){
                perror("Fatal error: Failed to de-allocate memory.");
                exit(-1);   
            }
        }

        pNodes = newPNodes;
        totalNodes = ((pagesAllocated) * PAGE_SIZE) / sizeof(struct node);
        (pNodes[currNodeIndex]).index = currNodeIndex; 
        currNodeIndex += 1;
        
        return (struct node*) &pNodes[currNodeIndex - 1]; 
    }
}

void mergeContiguous(){
    struct node* pNodeIter = pNodes;
    while(pNodeIter != NULL){
        struct node* pSubNodeIter = pNodeIter -> pSubChain;
        while(pSubNodeIter -> pNext != NULL){
            if(pSubNodeIter -> pNext != NULL && pSubNodeIter -> isHole && pSubNodeIter -> pNext -> isHole){
                struct node* pNextSubNode = pSubNodeIter -> pNext; //saving the node for next z                
                
                //Merge 1 in 2
                pNextSubNode -> size += pSubNodeIter -> size;
                pNextSubNode -> pUserReqMem = pSubNodeIter -> pUserReqMem;
                pNextSubNode -> pPrev = pSubNodeIter -> pPrev;
                
                if(pSubNodeIter -> pPrev != NULL) //if not the first node, link 0 to 2
                    pSubNodeIter -> pPrev -> pNext = pNextSubNode;

                if (pNodeIter -> pSubChain == pSubNodeIter)
                    pNodeIter -> pSubChain = pNextSubNode;
        
                pSubNodeIter -> deleted = 1;

                pSubNodeIter = pNextSubNode;
            }else{
                pSubNodeIter = pSubNodeIter -> pNext;
            }
        }
        pNodeIter = pNodeIter -> pNext;
    }
}
 
//---------------------Main API----------------------
void* mems_malloc(size_t reqSize){
    struct node* pNodeIter = pNodes;
    while (pNodeIter != NULL){
        struct node* pSubNodeIter = pNodeIter -> pSubChain;
        while(pSubNodeIter != NULL){
            if(pSubNodeIter -> isHole && pSubNodeIter -> size >= reqSize){ //if theres available space in the hole
                if (pSubNodeIter -> size > reqSize){
                    struct node* pNewNode = findNewNode(); 
                    pSubNodeIter = &pNodes[pSubNodeIter -> index]; //Restore iter variablesif in the backend new pages were allocated. 
                    pNodeIter = &pNodes[pNodeIter -> index];

                    //New Node is the node inserted on right
                    pNewNode -> isHole = 1;
                    pNewNode -> deleted = 0;
                    pNewNode -> pNext = pSubNodeIter -> pNext;
                    pNewNode -> pPrev = pSubNodeIter;
                    pNewNode -> pSubChain = NULL;
                    pNewNode -> pUserReqMem = pSubNodeIter -> pUserReqMem + reqSize;
                    pNewNode -> size = pSubNodeIter -> size - reqSize;
                    pSubNodeIter -> size = reqSize;

                    pSubNodeIter -> pNext = pNewNode;
                    pSubNodeIter -> isHole = 0;

                }else
                    pSubNodeIter -> isHole = 0;

                return (void*) pSubNodeIter -> pUserReqMem;
            }

            pSubNodeIter = pSubNodeIter -> pNext;
        }
        pNodeIter = pNodeIter -> pNext;
    }


    //If no adequate sized hole found
    //Create master Node
    if (pNodeIter == NULL){
        if (pNodes == NULL){ //First Time page allocation for nodes
            pNodeIter = findNewNode();
            pNodeIter -> pNext = NULL;           
            pNodeIter -> pPrev = NULL;

        }else{
            pNodeIter = pNodes;
            while(pNodeIter -> pNext != NULL){
                pNodeIter = pNodeIter -> pNext;
            }
                       
            struct node* pNewMain = findNewNode();
            pNodeIter = &pNodes[pNodeIter -> index]; //Restoring iter variables 

            pNodeIter -> pNext = pNewMain;
            pNewMain -> pPrev = pNodeIter;
            pNewMain -> pNext = NULL;
            pNodeIter = pNodeIter -> pNext;
        
        }
    }
    
    //Create First subnode - A hole with >= user requested memory
    struct node* pNewSub = findNewNode();
    pNodeIter = pNewSub - 1; // Restore IterBackup
    
    //Setting up subnode
    pNodeIter -> pSubChain = pNewSub; 
    struct node* pSubNodeIter = pNewSub;
    pSubNodeIter -> pNext = NULL;
    pSubNodeIter -> isHole = 1;
    pSubNodeIter -> deleted = 0;

    //Allocating Pages for user
    int pagesToAllocate = (reqSize % PAGE_SIZE > 0)? (reqSize / PAGE_SIZE) + 1 : ceil(reqSize / PAGE_SIZE);
    pSubNodeIter -> pUserReqMem = (void *)mmap(NULL, pagesToAllocate*PAGE_SIZE, PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0);  //the first subnode has the addres
    pNodeIter -> pUserReqMem = pSubNodeIter -> pUserReqMem;
    pSubNodeIter -> size = pagesToAllocate  * PAGE_SIZE;
    pNodeIter -> size = pagesToAllocate * PAGE_SIZE;

    return mems_malloc(reqSize); //Smart 
}

int mems_free(void* v_ptr){ //match and mark hole; merge and mark deleted
    if(v_ptr < 0 || pNodes == NULL){
        fputs("Invalid virtual pointer\n", stderr);
        return 1;
    }

    struct node* pNodeIter = pNodes;

    while(pNodeIter != NULL){
        struct node* pSubNodeIter = pNodeIter -> pSubChain;
        while(pSubNodeIter != NULL){
            if(v_ptr >= pSubNodeIter -> pUserReqMem  && (((pSubNodeIter -> pUserReqMem) + pSubNodeIter -> size) > v_ptr) && !pSubNodeIter -> isHole){
                pSubNodeIter -> isHole = 1;
                mergeContiguous();
                return 0;
            }

            pSubNodeIter = pSubNodeIter -> pNext;
        }
        pNodeIter = pNodeIter -> pNext;
    }

    fputs("Invalid Virtual Address\n", stderr);
    return 1;
}

void mems_print_stats(){
    printf("----- MeMS SYSTEM STATS -----\n");
    long totalAllocatedSize = 0;
    long totalFreeSize = 0;
    long totalPages = 0;
    int i = 0, j = 0;

    struct node* pNodeIter = pNodes;
    while(pNodeIter != NULL){
        printf("MAIN (Node Index: %lld) [%lld:%lld]-> ", pNodeIter -> index, pNodeIter -> pUserReqMem, pNodeIter -> pUserReqMem + (pNodeIter -> size));
        struct node* pSubNodeIter = pNodeIter -> pSubChain; 
        while(pSubNodeIter != NULL){           
            if(pSubNodeIter -> isHole == 1){
                printf("H (Node Index: %lld), Address Range: [%lld:%lld] <-> ", pSubNodeIter -> index, pSubNodeIter -> pUserReqMem, pSubNodeIter -> pUserReqMem + ((pSubNodeIter ->  size) - 1));
                totalFreeSize += pSubNodeIter ->  size;
            }else{
                printf("P (Node Index: %lld) Address Range: [%lld:%lld] <-> ", pSubNodeIter -> index, pSubNodeIter -> pUserReqMem, pSubNodeIter -> pUserReqMem + ((pSubNodeIter ->  size) - 1));
            }
            pSubNodeIter = pSubNodeIter ->  pNext;
        }
        printf("NULL\n\n");
        
        //other important stuff about the main node
        totalPages += pNodeIter -> size/PAGE_SIZE;
        pNodeIter = pNodeIter -> pNext;
        i += 1;
    }
    
    
    printf("Pages used: %ld\n", totalPages);
    printf("Space unused: %li B\n", totalFreeSize);
    printf("Main Chain Length: %d\n", i);
    printf("Sub-Chain Length array: [");
    pNodeIter = pNodes -> pNext;
    while(pNodeIter != NULL){
        j = 0;
        struct node* pSubNodeIter = pNodeIter -> pSubChain;
        while(pSubNodeIter != NULL){
            j += 1;
            pSubNodeIter = pSubNodeIter ->  pNext;
        }

        if(pNodeIter -> pNext != NULL)
            printf("%d, ", j);
        else
            printf("%d", j);
        pNodeIter = pNodeIter -> pNext;
    }
    printf("]\n\n\n");
    
}

