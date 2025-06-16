// include other header files as needed
#include "../include/mems.h"


int main(int argc, char const *argv[])
{
    //initialise the MeMS system 
    printf("Initialising MeMS...\n");

    int* ptr[10];

    /*
    This allocates 10 arrays of 250 integers each
    */
    printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
    
    for(int i=0;i<10;i++){
        ptr[i] = (int*) mems_malloc(sizeof(int)*250); //Taking some random number and conveting it into an address and storing that in an array
        printf("Virtual address: %lu\n", (unsigned long)ptr[i]);
    }

    /*
    In this section we are tring to write value to 1st index of array[0] (here it is 0 based indexing).
    We get get value of both the 0th index and 1st index of array[0] by using function mems_get.
    Then we write value to 1st index using 1st index pointer and try to access it via 0th index pointer.

    This section is show that even if we have allocated an array using mems_malloc but we can 
    retrive MeMS physical address of any of the element from that array using mems_get. 
    */    
    printf("\n------ Assigning value to Virtual address [mems_get] -----\n");
    // how to write to the virtual address of the MeMS (this is given to show that the system works on arrays as well)
    int* phy_ptr= (int*) &ptr[0][1]; // get the address of index 1
    phy_ptr[0]=200; // put value at index 1
    int* phy_ptr2= (int*) &ptr[0][0]; // get the address of index 0
    printf("Virtual address: %lu\n",(unsigned long)ptr[0]);
    printf("Value written: %d\n", phy_ptr2[1]); // print the address of index 1 */

    /*
    This shows the stats of the MeMS system.  
    */
    printf("\n--------- Printing Stats [mems_print_stats] --------\n");
    mems_print_stats();

    /*
    This section shows the effect of freeing up space on free list and also the effect of 
    reallocating the space that will be fullfilled by the free list.
    */
   
    printf("\n--------- Freeing up the memory [mems_free] --------\n"); 
    printf("Freeing Up Pointer 1\n");
    mems_free(ptr[1]);    
    mems_print_stats();
    
    printf("Freeing Up Pointer 2\n");
    mems_free(ptr[2]);    
    mems_print_stats();

    printf("Allocating in Pointer 1\n");
    ptr[1] = (int*)mems_malloc(sizeof(int)*375);
    mems_print_stats();

    printf("Freeing All Memory...\n");
    for(int i=0;i<10;i++){
        mems_free(ptr[i]); //Will throw invalid for ptr2
    }
    mems_print_stats();

    return 0;
}
