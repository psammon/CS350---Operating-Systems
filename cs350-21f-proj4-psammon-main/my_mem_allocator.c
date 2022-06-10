// =================================================
// ATTENTION: DO NOT CHANGE THE CONTENT IN THIS FILE
// =================================================

#include "my_mem_allocator.h"

ALLOC_POLICY alloc_policy = AP_FIRST; // default: first-fit
void * my_heap = NULL; // start address of the heap space that this allocator is managing
BLOCK_HDR * fl_head = NULL; // head pointer to the free list

// Return 0 on success, return -1 on failure
int my_mem_allocator_init(void)
{    
    my_heap = malloc(MY_HEAP_SIZE);
    if (NULL == my_heap) {
        DEBUG_MSG("Failed to initialize the memory allocator with heap size %d.\n", MY_HEAP_SIZE);
        return -1;
    }

    printf("Initialized the heap: addr 0x%08lx size 0x%x\n", (unsigned long)my_heap, MY_HEAP_SIZE);
    memset(my_heap, 0, MY_HEAP_SIZE);
    fl_head = (BLOCK_HDR *)my_heap;
    fl_head->size = MY_HEAP_SIZE - sizeof(BLOCK_HDR);
    fl_head->next = NULL;

    return 0 ;
}

void my_mem_allocator_destroy(void)
{
    if (NULL !=  my_heap) {
        free(my_heap);
        my_heap = NULL;
    }

    return;
}

void set_alloc_policy(ALLOC_POLICY policy) 
{
    alloc_policy = policy;
}

unsigned long relative_addr(unsigned long addr)
{
    return addr - (unsigned long)my_heap;
}

void print_free_list(void)
{
    unsigned long idx = 0;
    unsigned long ra = 0;
    BLOCK_HDR * p = fl_head;

    printf("----------\n");
    while (p) {
        ra = relative_addr((unsigned long)p + sizeof(BLOCK_HDR));
        printf("Free block %ld: addr 0x%lx size 0x%x\n", idx, ra, p->size);
        p = p->next;
        idx++;
    }

    return;
}

void print_malloc_result(void * addr, size_t size)
{
    unsigned long ra = relative_addr((unsigned long)addr);
    printf("----------\n");
    printf("==> my_malloc(0x%lx): addr 0x%lx\n", size, ra);

    return;
}
