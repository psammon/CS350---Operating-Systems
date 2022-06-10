#include "my_mem_allocator.h"
#include "test.h"

int main(int argc, char ** argv)
{
    int ret = 0;
    int size[] = {0x100, 0x200, 0x300, 0x400, 0x500,
                  0x100, 0x200, 0x300, 0x400, 0x500,
                  0x100, 0x200, 0x300, 0x400, 0x500,
                  0x100, 0x200, 0x300, 0x400, 0x500};
    char *p[25] = {NULL};
    char *ptr = NULL;
    int i = 0, cnt = 0;
    
    setbuf(stdout, NULL); // disable stdout buffering

    ret = my_mem_allocator_init();
    if (0 != ret) {
        goto exit;
    }

    cnt = sizeof(size)/sizeof(size[0]);

    printf(YEL "==> preparing ... (similar to test 1&2)\n" RESET);
    for (i = 0; i < cnt; i++) {
        //print_free_list();

        p[i] = my_malloc(size[i]);
        if (NULL == p[i]) {
            printf("my_malloc(%d) failed!\n", size[i]);
            goto exit;
        }
        
        //print_malloc_result(p[i], size[i]);
    }

    for (i = 0; i < cnt; i++) {
        my_free(p[i]);
        //print_free_list();
        i++;
    }

    print_free_list();

    printf(GRN "\n============== Test case 5 ==============\n" RESET);
    printf(YEL "==> testing first fit (default) with splitting ...\n" RESET);
    ptr = my_malloc(0x200);
    print_malloc_result(ptr, 0x200);
    print_free_list();

    ptr = my_malloc(0x200);
    print_malloc_result(ptr, 0x200);
    print_free_list();

    printf(GRN "\n============== Test case 6 ==============\n" RESET);
    printf(YEL "==> testing best fit with splitting ...\n" RESET);
    set_alloc_policy(AP_BEST);    
    ptr = my_malloc(0x101);
    print_malloc_result(ptr, 0x101);
    print_free_list();

    printf(GRN "\n============== Test case 7 ==============\n" RESET);
    printf(YEL "==> testing best fit without splitting ...\n" RESET);
    ptr = my_malloc(0xe0);
    print_malloc_result(ptr, 0xe0);
    print_free_list();

exit:
    my_mem_allocator_destroy();
    return ret;
}
