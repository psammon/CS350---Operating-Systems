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

    for (i = 0; i < cnt; i++) {
        my_free(p[i]);
        //print_free_list();
        i++;
    }

    print_free_list();

    printf(GRN "\n============== Test case 8 ==============\n" RESET);
    printf(YEL "==> testing oversize malloc ...\n" RESET);    
    ptr = my_malloc(0x100000); // oversize malloc
    print_free_list();

    printf(GRN "\n============== Test case 9 ==============\n" RESET);
    printf(YEL "==> testing double free ...\n" RESET);    
    my_free(p[2]); // double free
    print_free_list();

    printf(GRN "\n============== Test case 10: error-invalid free addr ==============\n" RESET);
    printf(YEL "==> testing invalid free addr ...\n" RESET);    
    my_free(p[3]+1); // invalid free addr
    print_free_list();


exit:
    my_mem_allocator_destroy();
    return ret;
}
