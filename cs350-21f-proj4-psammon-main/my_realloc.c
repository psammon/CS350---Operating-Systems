// ADD YOUR IMPLEMENTATION IN THIS FILE
#include "my_mem_allocator.h"

extern BLOCK_HDR * fl_head; //global head pointer to the free list
extern void * my_heap; //global start address of the heap space that this allocator is managing

void *my_realloc(void *ptr, size_t size)
{
    void * usr_ptr = NULL;
    /*
    if(*(ptr-sizeof(ptr)) != MAGIC_NUM){
        DEBUG_MSG("magic_num not found at location");
        return NULL;
    }*/
    if(!ptr){
        usr_ptr = my_malloc(size);
        if(!usr_ptr){
            DEBUG_MSG("malloc of new space failed.");
            return NULL;
        }
    } else {
        void *ptr_header = ((void *)((unsigned long)ptr - sizeof(BLOCK_HDR)));
        BLOCK_HDR * b = (BLOCK_HDR *) ptr_header;
        int bsize = b->size;
        if(size > bsize){
            usr_ptr = my_malloc(size);  
            if(!usr_ptr) return NULL;        
            memcpy(usr_ptr, ptr, bsize);
            my_free(ptr);
        }
        else {
            usr_ptr = ptr;
        }
    }

    return usr_ptr;
}
