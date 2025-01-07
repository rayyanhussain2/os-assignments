#include "../include/mems.h"

// Global variables
struct node* pNodes = NULL;
int pagesAlloted = 0;
int nodesPossible = 0;
int currNodeIndex = 0;

struct node* findNewNode(){
    //Search nodes array for available node candidate
    for(int i = 0; i < currNodeIndex; i++){
        if (pNodes[i].deleted){
            return pNodes + i;
        }
    }
    
    //Increase the counter or allocate more pages
    if(currNodeIndex < nodesPossible){
        currNodeIndex += 1;
        return pNodes + currNodeIndex;
    }else{
        pagesAlloted += 1;
        void* mmapAddr = mmap(NULL, ((pagesAlloted) * PAGE_SIZE), PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0);
        if(mmapAddr == MAP_FAILED){
            perror("Fatal error: Failed to allocate memory.");
            exit(-1);
        }

        struct node* newPNodes = (struct node*) mmapAddr;

        //Recopy only if more than one page has been allocated
        if(pagesAlloted > 1){
            for(int i = 0; i < currNodeIndex; i++){
                newPNodes[i].pSubChain = pNodes[i].pSubChain; 
                newPNodes[i].isSub = pNodes[i].isSub;
                newPNodes[i].pUserReqMem = pNodes[i].pUserReqMem;
                newPNodes[i].pPrev = pNodes[i].pPrev;
                newPNodes[i].pNext = pNodes[i].pNext; 
                newPNodes[i].isHole = pNodes[i].isHole;
                newPNodes[i].deleted = pNodes[i].deleted;
                newPNodes[i].size = pNodes[i].size;
            }

            //unallocate the old stuff
            if(munmap(pNodes, (pagesAlloted - 1) * PAGE_SIZE) == -1){
                perror("Fatal error: Failed to de-allocate memory.");
                exit(-1);   
            }
        }

        //update global variables
        currNodeIndex += 1;
        nodesPossible = ((pagesAlloted) * PAGE_SIZE) / sizeof(struct node);
        pNodes = newPNodes;
        return pNodes + currNodeIndex - 1;
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
            if(pSubNodeIter -> isHole && pSubNodeIter -> size >= reqSize){
                if (pSubNodeIter -> size > reqSize){
                    //split the node
                    struct node* pNewNode = findNewNode();

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

                    return (void*) pSubNodeIter -> pUserReqMem;
                }else{
                    pSubNodeIter -> isHole = 0;
                    return (void*) pSubNodeIter -> pUserReqMem;
                }
            }

            pSubNodeIter = pSubNodeIter -> pNext;
        }
        pNodeIter = pNodeIter -> pNext;
    }

    if (pNodeIter == NULL){
        //if the head itself is null, no node has been allocated yet
        if (pNodes == NULL){
            pNodeIter = findNewNode();
            pNodeIter -> pNext = NULL;           
            pNodeIter -> pPrev = NULL;
        }else{
            pNodeIter = pNodes;
            while(pNodeIter -> pNext != NULL){
                pNodeIter = pNodeIter -> pNext;
            }
            
            pNodeIter -> pNext = findNewNode();
            pNodeIter -> pNext -> pPrev = pNodeIter;
            pNodeIter -> pNext -> pNext = NULL;
            pNodeIter = pNodeIter -> pNext;
        
        }
    }
    
    //pNodeIter points at a new master node now
    //First subnode - A hole with >= user requested memory
    pNodeIter -> pSubChain = findNewNode();
    struct node* pSubNodeIter = pNodeIter -> pSubChain;
    pSubNodeIter -> pNext = NULL;
    pSubNodeIter -> isHole = 1;
    pSubNodeIter -> deleted = 0;
    int pagesAllot = (reqSize % PAGE_SIZE > 0)? (reqSize / PAGE_SIZE) + 1 : (reqSize / PAGE_SIZE);
    pSubNodeIter -> pUserReqMem = (void *)mmap(NULL, pagesAllot*PAGE_SIZE, PROT_READ | PROT_WRITE, 0x20 | MAP_PRIVATE, -1, 0);  
    pNodeIter -> pUserReqMem = pSubNodeIter -> pUserReqMem;
    pSubNodeIter -> size = pagesAllot  * PAGE_SIZE;
    pNodeIter -> size = pagesAllot * PAGE_SIZE;
    
    return mems_malloc(reqSize); //Smart
}

int mems_free(void* v_ptr){ //match and mark hole; merge and mark deleted
    if(v_ptr < 0 || pNodes == NULL){
        fputs("Invalid virtual pointer\n", stdout);
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
        printf("MAIN[%lld:%lld]-> ", pNodeIter -> pUserReqMem, pNodeIter -> pUserReqMem + (pNodeIter -> size));
        struct node* pSubNodeIter = pNodeIter -> pSubChain; 
        while(pSubNodeIter != NULL){           
            if(pSubNodeIter -> isHole == 1){
                printf("H[%lld:%lld] <-> ", pSubNodeIter -> pUserReqMem, pSubNodeIter -> pUserReqMem + ((pSubNodeIter ->  size) - 1));
                totalFreeSize += pSubNodeIter ->  size;
            }else{
                printf("P[%lld:%lld] <-> ", pSubNodeIter -> pUserReqMem, pSubNodeIter -> pUserReqMem + ((pSubNodeIter ->  size) - 1));
            }
            pSubNodeIter = pSubNodeIter ->  pNext;
        }
        printf("NULL\n");

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

