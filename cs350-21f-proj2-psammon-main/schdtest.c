#include "types.h"
#include "user.h"

/* ATTENTION: to ensure correct compilation of the base code, 
   stub functions for the system call user space wrapper functions are provided. 
   REMEMBER to disable the stub functions (by commenting the following macro) to 
   allow your implementation to work properly. */
//#define STUB_FUNCS
//#ifdef STUB_FUNCS
//void set_sched(int scheduler) {}
//int tickets_owned(int pid) {return 0;}
//int transfer_tickets(int pid, int tickets) {return 0;}
//#endif

/* IMPORTANT INSTRUCTION: the test code below should not be changed. 
   Failure to follow this instruction will lead to zero point for this part */

#define SCHEDULER_DEFAULT  0
#define SCHEDULER_STRIDE 1

#define LOOP_CNT 0x10000000

#define MAX_CHILD_COUNT 6

struct {
    int pid;
} child[MAX_CHILD_COUNT];

unsigned int avoid_optm = 0; // a variable used to avoid compiler optimization

void do_child(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
        
    avoid_optm = tmp;
}


void do_parent(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

void do_parent_partial(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT/2)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

// Create "cnt" child processes and block all of them by having them read from empty pipes 
//   until they receive the let-go signal from the parent
void create_child_process(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0;

    for (i = 0; i < child_cnt; i++)
    {
        child[i].pid = fork();

        if (child[i].pid < 0)
        {
            printf(1, "fork() failed!\n");
            exit();
        }
        else if (child[i].pid == 0) // child
        {                       
            do_child(); // doing some computation for some time
            exit();
        }
        else // parent
        {
            //do nothing here
        }
    }        
}

void print_proc_tickets(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    printf(1, "parent (pid %d) has %d tickets.\n", getpid(), tickets_owned(getpid()));
    
    for (i = 0; i < child_cnt; i++)
    {  
        printf(1, "child (pid %d) has %d tickets.\n", child[i].pid, tickets_owned(child[i].pid));     
    }    
}

void wait_on_child_processes(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    for (i = 0; i < child_cnt; i++)
    {   
       if (wait() < 0)
       {
            printf(1, "\nwait() on child-%d failed!\n", i);
       }
    }            

}


void test_case_1(void)
{
    int child_cnt = 3;
    int scheduler = SCHEDULER_DEFAULT;
    
    printf(1, "===== Test case 1: default (RR) scheduler, %d child processes =====\n", child_cnt);
    set_sched(scheduler);

    create_child_process(child_cnt);
    print_proc_tickets(child_cnt);

    enable_sched_trace(1);
    do_parent();    
    wait_on_child_processes(child_cnt);
    enable_sched_trace(0);
    
    printf(1, "\n\n");      
}

void test_case_2(void)
{
    int child_cnt = 2;
    int scheduler = SCHEDULER_STRIDE;
    
    printf(1, "===== Test case 2: stride scheduler, %d child processes, no ticket trasfer =====\n");
    set_sched(scheduler);

    create_child_process(child_cnt);
    print_proc_tickets(child_cnt);
    
    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(child_cnt);
    enable_sched_trace(0);

    printf(1, "\n\n");
}

void test_case_3(void)
{
    int child_cnt = 1;
    int tickets_transferred = 0;
    int scheduler = SCHEDULER_STRIDE;
    
    printf(1, "===== Test case 3: stride scheduler, testing return values of transfer_tickets() ... \n");

    set_sched(scheduler);
    create_child_process(child_cnt);

    tickets_transferred = 20;
    printf(1, "parent (pid: %d) tranferred %d tickets to child (pid: %d), transfer_tickets() returned %d. \n", 
            getpid(), tickets_transferred, child[0].pid, transfer_tickets(child[0].pid, tickets_transferred));
    printf(1, "parent (pid: %d) now has %d tickets.\n", getpid(), tickets_owned(getpid()));

    tickets_transferred = -2;
    printf(1, "parent (pid: %d) tranferred %d tickets to child (pid: %d), transfer_tickets() returned %d. \n", 
            getpid(), tickets_transferred, child[0].pid, transfer_tickets(child[0].pid, tickets_transferred));  
    printf(1, "parent (pid: %d) now has %d tickets.\n", getpid(), tickets_owned(getpid()));
            
    tickets_transferred = tickets_owned(getpid());
    printf(1, "parent (pid: %d) tranferred %d tickets to child (pid: %d), transfer_tickets() returned %d. \n", 
            getpid(), tickets_transferred, child[0].pid, transfer_tickets(child[0].pid, tickets_transferred));  
    printf(1, "parent (pid: %d) now has %d tickets.\n", getpid(), tickets_owned(getpid()));
    
    tickets_transferred = 20;
    printf(1, "parent (pid: %d) tranferred %d tickets to child (pid: 9999), transfer_tickets() returned %d. \n", 
            getpid(), tickets_transferred, transfer_tickets(9999, tickets_transferred));
    printf(1, "parent (pid: %d) now has %d tickets.\n", getpid(), tickets_owned(getpid()));
    
    print_proc_tickets(child_cnt);
    do_parent();
    wait_on_child_processes(child_cnt);

    printf(1, "\n\n");     
    
}

void test_case_4(void)
{
    int child_cnt = 1;
    int scheduler = SCHEDULER_STRIDE;
    int tickets_transferred = 0;
    
    printf(1, "===== Test case 4: stride scheduler, %d child, with ticket trasfer at the beginning ... =====\n", child_cnt);

    set_sched(scheduler);
    create_child_process(child_cnt);

    tickets_transferred = tickets_owned(getpid())/2;
    transfer_tickets(child[0].pid, tickets_transferred);
    printf(1, "parent (pid: %d) transferred %d tickets to child (pid: %d)\n", getpid(), tickets_transferred, child[0].pid);
    
    print_proc_tickets(child_cnt);

    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(child_cnt);
    enable_sched_trace(0);

    printf(1, "\n\n");
}

void test_case_5(void)
{
    int child_cnt = 1;
    int scheduler = SCHEDULER_STRIDE;
    int tickets_transferred = 0;
    
    printf(1, "===== Test case 5: stride scheduler, %d child, with ticket trasfer at the beginning ... =====\n", child_cnt);

    set_sched(scheduler);
    create_child_process(child_cnt);

    tickets_transferred = tickets_owned(getpid()) - 1;
    transfer_tickets(child[0].pid, tickets_transferred);
    printf(1, "parent (pid: %d) transferred %d tickets to child (pid: %d)\n", getpid(), tickets_transferred, child[0].pid);
    
    print_proc_tickets(child_cnt);

    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(child_cnt);
    enable_sched_trace(0);

    printf(1, "\n\n");
}

#if 0
void test_case_6(void)
{
    int scheduler = SCHEDULER_STRIDE;   
    int lp[2];
    int c = 0, rg_pid = 0;
    
    printf(1, "===== Test case 6: stride scheduler, initialy one child process, no transfer; parent creates another child process at run time, no transfer.\n");
    set_sched(scheduler);
    create_child_process(1);
    print_proc_tickets(1);

    enable_sched_trace(1);

    do_parent_partial();
    do 
    {
        if (pipe(lp) < 0)
        {
            printf(1, "pipe() failed!\n");
            exit();
        }
        
        rg_pid = fork();
        if (rg_pid < 0)
        {
            printf(1, "fork() failed!\n");
            exit();
        }
        else if (rg_pid == 0) // child
        {
            close(lp[1]); // close the write end on the child side
            read(lp[0], &c, 1); 
            do_child();
            exit();
        }
        else // parent
        {
            write(lp[1], "G", 1); // let go the runtime generated child processes
            do_parent();
        }
    } while(0);

    wait_on_child_processes(2);
    
    enable_sched_trace(0);
    
    printf("\n");
}
#endif

int
main(int argc, char *argv[])
{
    test_case_1();
    test_case_2();
    test_case_3();
    test_case_4();
    test_case_5();
    //test_case_6();
    
    exit(); // main process terminates
}

