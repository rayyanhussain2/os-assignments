# MeMS: Memory Management System

MeMS is a memory-allocation library that emulates malloc(), a heap allocation library. It utilizes the 'mmap()' system call underneath, requesting mapped memory in multiples of page size.

A free list structure is used to manage the memory requested. This is also hosted on the same mapped-memory region. 
![alt text](.freelist.png)
# Usage 
The followng APIs are provided in MeMS:

## mems_malloc(int size)
Takes memory size as an argument and provides a void pointer to the user.

## mems_free(void* v_ptr)
Frees the chunk of memory requested prior by the user. For instance, if user has requested 250 bytes of memory and the  address is 0, then providing an address anywhere between 0 - 249 will free all 250 bytes.

## mems_print_stats()
Draws the free list and prints important information like pages used, memory unused, and lengths of main chain and sub chain. 

# Sample Program
A sample program utilizing MeMS is provided to demonstrate the usage of above APIs. To run this sample program, execute the following commands:
```
$ make
$ ./example
```