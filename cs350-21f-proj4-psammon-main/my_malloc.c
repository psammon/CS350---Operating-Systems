// ADD YOUR IMPLEMENTATION IN THIS FILE

// --- FOR STUDETS: "Segmentation fault" usually happens when deferencing a null pointer.
// --- So be careful with you code logic to avoid the fault from happening.
// --- If you need to debug a seg fault, keep the above in mind so you can find out the problem quickly.            
#include "my_mem_allocator.h"

extern BLOCK_HDR * fl_head; //global head pointer to the free list
extern void * my_heap; //global start address of the heap space that this allocator is managing

BLOCK_HDR * split(BLOCK_HDR *b, size_t size){
    void *block = ((void *)((unsigned long)b + sizeof(BLOCK_HDR)));
    BLOCK_HDR * newBlock = (BLOCK_HDR *)((unsigned long)block + size);
    newBlock->size = b->size - (sizeof(BLOCK_HDR) + size);
    b->size = size;
    return newBlock;
}

void fl_remove(BLOCK_HDR * b, BLOCK_HDR * prev){
    if(!prev){
        if(b->next){
            fl_head = b->next;
        } else {
            fl_head = NULL;
        }
    } else {
        prev->next = b->next;
    }
}

void fl_insert(BLOCK_HDR * b, BLOCK_HDR * prev){
    prev = NULL;
    b->next = NULL;
    if(!fl_head || (unsigned long)fl_head > (unsigned long)b) {
        b->next = fl_head;
        fl_head = b;
    } else {
        BLOCK_HDR *p = fl_head;
        while(p->next != NULL && (unsigned long)p->next < (unsigned long)b){
            p = p->next;
        }
        b->next = p->next;
        p->next = b;
    }

}

void *my_malloc(size_t size)
{
    void * usr_ptr = NULL;
    BLOCK_HDR *p = fl_head, *prev = NULL;

    if(p == NULL){
        DEBUG_MSG("free list not initialized");
        return NULL;
    }
//Checking blocks, will move on if free block is large enough to store malloc, 
//OR we've reached the end of the free list
    while(p){ 
        if(p->size >= size + sizeof(BLOCK_HDR)) {
            usr_ptr = ((void *)((unsigned long)p + sizeof(BLOCK_HDR)));
            fl_remove(p, prev);
            p->next = MAGIC_NUM;
            if(p->size == size){
                return usr_ptr;
            }
            BLOCK_HDR *newBlock = split(p, size);
            fl_insert(newBlock, prev);
            return usr_ptr;
        } else {
            prev = p;
            p = p->next;
        }
        
    }

    DEBUG_MSG("free block not found");
    return usr_ptr;
}
