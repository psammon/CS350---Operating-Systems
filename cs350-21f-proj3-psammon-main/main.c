#include "parser.h"
#include<sys/wait.h>
#include<signal.h>

//Citing proj2 (xv6) as a main source of inspiration for my answer
/*
void printcmd(struct cmd *cmd)
{
    struct backcmd *bcmd = NULL;
    struct execcmd *ecmd = NULL;
    struct listcmd *lcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct redircmd *rcmd = NULL;

    int i = 0;
    
    if(cmd == NULL)
    {
        PANIC("NULL addr!");
        return;
    }
    

    switch(cmd->type){
        case EXEC:
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
            {
                goto printcmd_exit;
            }

            MSG("COMMAND: %s", ecmd->argv[0]);
            for (i = 1; i < MAXARGS; i++)
            {            
                if (ecmd->argv[i] != NULL)
                {
                    MSG(", arg-%d: %s", i, ecmd->argv[i]);
                }
            }
            MSG("\n");

            break;

        case REDIR:
            rcmd = (struct redircmd*)cmd;

            printcmd(rcmd->cmd);

            if (0 == rcmd->fd_to_close)
            {
                MSG("... input of the above command will be redirected from file \"%s\". \n", rcmd->file);
            }
            else if (1 == rcmd->fd_to_close)
            {
                MSG("... output of the above command will be redirected to file \"%s\". \n", rcmd->file);
            }
            else
            {
                PANIC("");
            }

            break;

        case LIST:
            lcmd = (struct listcmd*)cmd;

            printcmd(lcmd->left);
            MSG("\n\n");
            printcmd(lcmd->right);
            
            break;

        case PIPE:
            pcmd = (struct pipecmd*)cmd;

            printcmd(pcmd->left);
            MSG("... output of the above command will be redrecited to serve as the input of the following command ...\n");            
            printcmd(pcmd->right);

            break;

        case BACK:
            bcmd = (struct backcmd*)cmd;

            printcmd(bcmd->cmd);
            MSG("... the above command will be executed in background. \n");    

            break;


        default:
            PANIC("");
    
    }
    
    printcmd_exit:

    return;
}
*/
void handle_sigint(int sig){
    return;
}
/*void handle_sigint2(int sig){
    printf("\nCaught signal %d but no foreground process running", sig);
    return;
}*/
void execcmd(struct cmd *cmd) {
    struct backcmd *bcmd = NULL;
    struct execcmd *ecmd = NULL;
    struct listcmd *lcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct redircmd *rcmd = NULL;

    int fd[2] = {0};
    
    if(cmd == NULL)
    {
        PANIC("NULL addr!");
        return;
    }
    

    switch(cmd->type){
        case EXEC:
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
                goto execcmd_exit;
            int status;
            pid_t pid;
            
            if((pid = fork()) < 0){
                PANIC("fork failed");
                break;
            }
            else if(pid == 0){
                execvp(ecmd->argv[0], ecmd->argv);
                printf("exec %s failed\n", ecmd->argv[0]);
                break;
            }
            else {
                signal(SIGINT, handle_sigint);
                while(wait(&status) != pid){
                }
                if(status != 0)
                    printf("\nNon-zero exit code (%i) detected\n", status/256);
            }
            break;

        case REDIR:
            rcmd = (struct redircmd*)cmd;
            int s_stdout = dup(STDOUT_FILENO), s_stdin = dup(STDIN_FILENO);

            int p = fork();
            if(p < 0){
                PANIC("fork");
            }
            else if(p == 0){
                if (0 == rcmd->fd_to_close)//< - input
                {
                    int fd0 = open(rcmd->file, rcmd->mode);
                    dup2(fd0, STDIN_FILENO);
                    close(fd0);
                }
                else if (1 == rcmd->fd_to_close)//> - output
                {
                    int fd1 = open(rcmd->file, rcmd->mode, 0600);
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);
                }
                execcmd(rcmd->cmd);
                exit(0);
            }
            while(wait(NULL) > 0){
                ;
            }
            dup2(s_stdin, STDIN_FILENO);
            dup2(s_stdout, STDOUT_FILENO);
            break;

        case LIST:
            lcmd = (struct listcmd*)cmd;
            int status2;
            if(fork() == 0){
                execcmd(lcmd->left);
                exit(0);
            }
            wait(&status2);
            execcmd(lcmd->right);
            break;

        case PIPE:
            pcmd = (struct pipecmd*)cmd;
            int saved_stdout, saved_stdin;
            pid_t cpid[2] = {0};

            if(pipe(fd) < 0){
                PANIC("PIPE");
            }
            if((cpid[0] =fork()) < 0){
                PANIC("fork");
            }
            else if(cpid[0] == 0){
                saved_stdout = dup(STDOUT_FILENO);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execcmd(pcmd->left);
                exit(0);
            }
            if((cpid[1] =fork()) < 0){
                PANIC("fork");
            }
            else if(cpid[1] == 0){
                
                saved_stdin = dup(STDIN_FILENO);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                execcmd(pcmd->right);
                exit(0);
            }
            close(fd[0]);
            close(fd[1]);
            while(wait(NULL) > 0){
                ;
            }
            while(wait(NULL) > 0){
                ;
            }
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
            break;

        case BACK:
            bcmd = (struct backcmd*)cmd;
            pid_t pidt = fork();
            if(pidt < 0){
                PANIC("fork");
            }
            else if(pidt == 0){
               execcmd(bcmd->cmd);
               exit(1);
            }
            break;


        default:
            PANIC("");
    
    }
    execcmd_exit:

    return;
}



int main(void)
{
    static char buf[1024];
    int fd;
    //signal(SIGINT, handle_sigint);
    setbuf(stdout, NULL);

    // Read and run input commands.
    
    while(getcmd(buf, sizeof(buf)) >= 0)
    {
        struct cmd * command;
        command = parsecmd(buf);
        execcmd(command); // TODO: run the parsed command instead of printing it
    }

    PANIC("getcmd error!\n");
    return 0;
}
