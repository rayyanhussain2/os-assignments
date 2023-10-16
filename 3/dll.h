//implementation of a custom doubly linked list  
struct node{
    void* startAddr;
    struct node* pSubChain;
    struct node* pPrev;
    struct node* pNext;
}; 

// Relevant dll functions that would include search and add and sublist
struct node* constructNode(void* startAddr, struct node* pSubChain, struct node* pPrev, struct node* pNext){
    struct node* pHeadNode = (struct node*)malloc(sizeof(struct node));
    pHeadNode -> startAddr = startAddr;
    pHeadNode -> pSubChain = pSubChain;
    pHeadNode -> pPrev = pPrev;
    pHeadNode -> pNext = pNext;

    return pHeadNode;
}