#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SECS
#define SECS 3
#endif

#ifndef ECODE
#define ECODE 0
#endif

int main(int argc, char ** argv)
{
    int i = 0;

    sleep(SECS);

    fprintf(stderr, "sne PID=%d: ", getpid());

    for (i = 1; i < argc ; i++)
    {
        fprintf(stderr, "%s ", argv[i]);
    }

    if (argc > 1)
        fprintf(stderr, "\n");

    exit(ECODE);
}
