// =================================================
// ATTENTION: DO NOT CHANGE THE CONTENT IN THIS FILE
// =================================================

#ifndef _MY_MEM_ALLOC_H
#define _MY_MEM_ALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_MSG(fmt, args...)                                                             \
        do {                                                                                \
            fprintf(stderr, "[%d:%s()] "fmt, __LINE__, __FUNCTION__, ##args);               \
        } while(0)  

// size of the heap space that this allocaztor is manager
#define MY_HEAP_SIZE 0x100000 // 1 MB 

// the magic number
#define MAGIC_NUM 0x87654321

// free/allocated block header
typedef struct _block_header{
    int size; // for free block: size of the usable space in the free block 
              // for allocated block: size of the mem space returned to user
    void * next; // for free block: pointer to the next free block
                 // for allocated block: magic number
}BLOCK_HDR;

// memory allocation policy
typedef enum _enum_alloc_policy{
    AP_FIRST, // first fit
    AP_BEST  // best fit
}ALLOC_POLICY;

int my_mem_allocator_init(void);
void my_mem_allocator_destroy(void);
void set_alloc_policy(ALLOC_POLICY policy);
void print_free_list(void);
void print_malloc_result(void * addr, size_t size);
unsigned long relative_addr(unsigned long addr);

void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);


#endif //_MY_MEM_ALLOC_H
