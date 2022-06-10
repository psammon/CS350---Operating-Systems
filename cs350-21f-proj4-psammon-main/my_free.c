// ADD YOUR IMPLEMENTATION IN THIS FILE
#include "my_mem_allocator.h"

extern BLOCK_HDR * fl_head; //global head pointer to the free list
extern void * my_heap; //global start address of the heap space that this allocator is managing


void coalesce(){
    BLOCK_HDR *p = fl_head, *next = NULL;
    unsigned long header_p, header_next;
    while(p->next){
        header_p = (unsigned long)p;
        header_next = (unsigned long)p->next;
        if(header_p < (unsigned long)fl_head){
            fl_head = p;
        }
        if(header_p + p->size + sizeof(BLOCK_HDR) == header_next){//checking if next free block is at the next address
            next = p->next;
            p->size += (next->size) + sizeof(BLOCK_HDR);
            p->next = next->next;
        }
        if(!p->next){
            break;
        }
        p = p->next;
    }
    if(fl_head->next){
        BLOCK_HDR *tmp = fl_head->next;
        fl_head->size += tmp->size + sizeof(BLOCK_HDR);
        fl_head->next = tmp->next;
    }
}

void my_free(void *ptr)
{    
    fl_insert(((void *)((unsigned long)ptr - sizeof(BLOCK_HDR))));
    coalesce();
    return;
}
