#include "my_mem_allocator.h"
#include "test.h"

int main(int argc, char ** argv)
{
    int ret = 0;
    int size[] = {0x200, 0x200,
                  0x200, 0x200};
    char *p[4] = {NULL};
    char *ptr = NULL;
    int i = 0, cnt = 0;

    setbuf(stdout, NULL); // disable stdout buffering

    ret = my_mem_allocator_init();
    if (0 != ret) {
        goto exit;
    }

    cnt = sizeof(size)/sizeof(size[0]);

    printf(YEL "\n==> preparing ... (similar to test 1&2)\n" RESET);
    for (i = 0; i < cnt; i++) {
        //print_free_list();

        p[i] = my_malloc(size[i]);
        if (NULL == p[i]) {
            printf("my_malloc(%d) failed!\n", size[i]);
            goto exit;
        }
        
        //print_malloc_result(p[i], size[i]);
    }

    for (i = 0; i < cnt; i++) { // free p[0], p[2], ....
        my_free(p[i]);
        //print_free_list();
        i++;
    }

    print_free_list();

    printf(GRN "\n============== Test case 11 ==============\n" RESET);
    printf(YEL "==> testing realloc (invalid ptr) ...\n" RESET);
    ptr = my_realloc(p[1]+1, size[1]);
    printf("my_realloc returned 0x%lx\n", (unsigned long)ptr);
    print_free_list();

    printf(GRN "\n============== Test case 12 ==============\n" RESET);
    printf(YEL "==> testing realloc (newsize < oldsize, no shinking) ...\n" RESET);
    printf("old ptr address: 0x%lx\n", relative_addr((unsigned long)p[1]));
    ptr = my_realloc(p[1], 0x1f0);
    printf("my_realloc returned 0x%lx\n", relative_addr((unsigned long)ptr));
    print_free_list();

    printf(GRN "\n============== Test case 13 ==============\n" RESET);
    printf(YEL "==> testing realloc (newsize < oldsize, shrink the allocated block) ...\n" RESET);
    printf("old ptr address: 0x%lx\n", relative_addr((unsigned long)p[1]));
    ptr = my_realloc(p[1], 0x100);
    printf("my_realloc returned 0x%lx\n", relative_addr((unsigned long)ptr));
    print_free_list();

    printf(GRN "\n============== Test case 14 ==============\n" RESET);
    printf(YEL "==> testing realloc (newsize > oldsize, find a free block, allocate, copy and free) ...\n" RESET);
    printf("old ptr address: 0x%lx\n", relative_addr((unsigned long)p[1]));
    *((unsigned long *)p[1]) = 0xFEDCBA98;
    ptr = my_realloc(p[1], 0x400);
    printf("my_realloc returned 0x%lx\n", relative_addr((unsigned long)ptr));
    if (ptr) {
        if (0xFEDCBA98 == *((unsigned long *)ptr)) {
            printf("PASS - copy detected\n");
        }
        else {
            printf("FAILED - copy not detected\n");
        }
    }
    print_free_list();

exit:
    my_mem_allocator_destroy();
    return ret;
}
