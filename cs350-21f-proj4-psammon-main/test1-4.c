#include "my_mem_allocator.h"
#include "test.h"

int main(int argc, char ** argv)
{
    int ret = 0;
    int size[] = {0x100, 0x200, 0x300,
                  0x100, 0x200, 0x300,
                  0x100, 0x200, 0x300};
    char *p[9] = {NULL};
    char *ptr = NULL;
    int i = 0, cnt = 0;

    setbuf(stdout, NULL); // disable stdout buffering

    ret = my_mem_allocator_init();
    if (0 != ret) {
        goto exit;
    }

    cnt = sizeof(size)/sizeof(size[0]);

    printf(GRN "\n============== Test case 1 ==============\n" RESET);
    printf(YEL "==> testing normal mallocs ...\n" RESET);
    for (i = 0; i < cnt; i++) {
        print_free_list();

        p[i] = my_malloc(size[i]);
        if (NULL == p[i]) {
            printf("my_malloc(%d) failed!\n", size[i]);
            goto exit;
        }
        
        print_malloc_result(p[i], size[i]);
    }

    printf(GRN "\n============== Test case 2 ==============\n" RESET);
    printf(YEL "==> testing normal frees ...\n" RESET);
    cnt = sizeof(size)/sizeof(size[0]);
    for (i = 0; i < cnt; i++) { // free p[0], p[2], ....
        my_free(p[i]);
        print_free_list();
        i++;
    }

    printf(GRN "\n============== Test case 3 ==============\n" RESET);
    printf(YEL "==> testing free + coalescsing ...\n" RESET);
    for (i = 1; i < cnt; i++) {
        my_free(p[i]);
        print_free_list();
        i++;
    }

    printf(GRN "\n============== Test case 4: coalescing at head ==============\n" RESET);
    printf(YEL "==> testing coalescing at head ...\n" RESET);
    ptr = my_malloc(0x100);
    print_malloc_result(ptr, 0x100);
    print_free_list();
    my_free(ptr);
    print_free_list();

exit:
    my_mem_allocator_destroy();
    return ret;
}
