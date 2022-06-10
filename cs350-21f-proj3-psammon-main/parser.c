#include "parser.h"
#include<signal.h>

char whitespace[] = " \t\r\n\v";

char symbols[] = "<|>&;";

/*
    The peak() function 
        removes leading whitespace chars and 
        checks if the first non-whitespace char is in the string toks or not.
    -----
    ps: start of the string
    es: end of the string
    toks: string to check

    char *strchr(const char *s, int c):
        The strchr() function returns a pointer to the first occurrence of
        the character c in the string s.
 */
int peek(char **ps, char *es, char *toks)
{
    char *s;

    s = *ps;
    while(s < es && strchr(whitespace, *s)) // skipping the leading whitespace chars
    {
        s++;
    }
    
    *ps = s;
    return *s && strchr(toks, *s);
}
void handle_sigint2(int sig){
    printf("\nCtrl-C catched. But no foreground process currently running");
    return;
}

char* gets(char *buf, int max)
{
  int i, cc;
  char c;

  signal(SIGINT, handle_sigint2);
  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1) break;
    //c = getchar();
    buf[i++] = c;
    
    if(c == '\n' || c == '\r') 
        break;
  }
  
  buf[i] = '\0';
  return buf;
}

int getcmd(char *buf, int nbuf)
{
    MSG("%s", SHELL_PROMPT);
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if(buf[0] == 0) // EOF
    {
        return -1;
    }
    
    return 0;
}

struct cmd* parsecmd(char *s)//
{
    char *es;
    struct cmd *cmd;

    es = s + strlen(s);
    cmd = parseline(&s, es);
    
    peek(&s, es, ""); // remove trailing whitespace chars from the command line

    if(s != es){
        MSG("leftovers: %s\n", s);
        PANIC("syntax\n");
    }
    
    nulterminate(cmd);

    return cmd;
}

struct cmd* parseline(char **ps, char *es)
{
    struct cmd *cmd;

    cmd = parsepipe(ps, es);

    while(peek(ps, es, "&")){
        gettoken(ps, es, 0, 0);
        cmd = init_backcmd(cmd);
    }
    
    if(peek(ps, es, ";")){
        gettoken(ps, es, 0, 0);
        cmd = init_listcmd(cmd, parseline(ps, es));
    }
    
    return cmd;
}

struct cmd* parsepipe(char **ps, char *es)
{
    struct cmd *cmd;

    cmd = parseexec(ps, es);

    if(peek(ps, es, "|"))
    {
        gettoken(ps, es, 0, 0);
        cmd = init_pipecmd(cmd, parsepipe(ps, es));
    }
    
    return cmd;
}

struct cmd* parseexec(char **ps, char *es)
{
    char *q, *eq;
    int tok, argc;
    struct execcmd *cmd;
    struct cmd *ret;

    // Allocate space for the exec command
    ret = init_execcmd();
    cmd = (struct execcmd*)ret;

    argc = 0;
    ret = parseredirs(ret, ps, es);
    
    while(!peek(ps, es, "|)&;"))
    {
        if((tok=gettoken(ps, es, &q, &eq)) == 0)
            break;
        
        if(tok != 'a')
        {
            PANIC("syntax\n");
        }

        if (0 == argc && *q == 'e' && *(q+1) == 'x' && *(q+2) == 'i' && *(q+3) == 't' && q+4 == eq)
        {
            exit(0);
        }
        
        cmd->argv[argc] = q;
        cmd->eargv[argc] = eq;
        argc++;

        if(argc >= MAXARGS)
        {
            PANIC("too many args\n");
        }
        
        ret = parseredirs(ret, ps, es);
    }

    cmd->argv[argc] = 0;
    cmd->eargv[argc] = 0;

    return ret;
}

struct cmd* parseredirs(struct cmd *cmd, char **ps, char *es)
{
    int tok;
    char *q, *eq;

    while(peek(ps, es, "<>")){
        tok = gettoken(ps, es, 0, 0);
        
        if(gettoken(ps, es, &q, &eq) != 'a')
        {
            PANIC("missing file for redirection\n");
        }
        
        switch(tok){
            case '<':
                cmd = init_redircmd(cmd, q, eq, O_RDONLY, 0);
                break;

            case '>':
                cmd = init_redircmd(cmd, q, eq, O_WRONLY|O_CREAT, 1);
                break;

            case '+':  // >>
                cmd = init_redircmd(cmd, q, eq, O_WRONLY|O_CREAT, 1);
                break;
        }
    }
    
    return cmd;
}


// normal command
struct cmd* init_execcmd(void)
{
    struct execcmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = EXEC;

    return (struct cmd*)cmd;
}

// redirection (">", "<")
struct cmd* init_redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd_to_close)
{
    struct redircmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = REDIR;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->efile = efile;
    cmd->mode = mode;
    cmd->fd_to_close = fd_to_close;
    
    return (struct cmd*)cmd;
}

// pipe ("|")
struct cmd* init_pipecmd(struct cmd *left, struct cmd *right)
{
    struct pipecmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = PIPE;
    cmd->left = left;
    cmd->right = right;

    return (struct cmd*)cmd;
}

// background execution ("&")
struct cmd* init_backcmd(struct cmd *subcmd)
{
    struct backcmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = BACK;
    cmd->cmd = subcmd;
    return (struct cmd*)cmd;
}

// multimple separate commands in one line (";")
struct cmd* init_listcmd(struct cmd *left, struct cmd *right)
{
    struct listcmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = LIST;
    cmd->left = left;
    cmd->right = right;

    return (struct cmd*)cmd;
}


int gettoken(char **ps, char *es, char **q, char **eq)
{
    char *s;
    int ret;

    s = *ps;
    while(s < es && strchr(whitespace, *s)) // remove the leading white space chars
    {
        s++;
    }
    
    if(q)
    {
        *q = s;
    }
    
    ret = *s;
    
    switch(*s){
        case 0:
            break;
            
        case '|':
        case ';':
        case '&':
        case '<':
            s++;
            break;

        case '>':
            s++;
            if(*s == '>')
            {
                ret = '+';
                s++;
            }
            break;

        default:
            ret = 'a';
            while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
            {
                s++;
            }
            break;
    }
    
    if(eq)
    {
        *eq = s;
    }
    
    while(s < es && strchr(whitespace, *s)) // remove the trailing white space chars
    {
        s++;
    }
    
    *ps = s;
    
    return ret;
}


// NUL-terminate all the counted strings.
struct cmd* nulterminate(struct cmd *cmd)
{
    int i;
    struct backcmd *bcmd;
    struct execcmd *ecmd;
    struct listcmd *lcmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

    if(cmd == NULL)
    {
        return NULL;
    }
    
    switch(cmd->type){
        case EXEC:
            ecmd = (struct execcmd*)cmd;
            for(i=0; ecmd->argv[i]; i++)
            *ecmd->eargv[i] = 0;
            break;

        case REDIR:
            rcmd = (struct redircmd*)cmd;
            nulterminate(rcmd->cmd);
            *rcmd->efile = 0;
            break;

        case PIPE:
            pcmd = (struct pipecmd*)cmd;
            nulterminate(pcmd->left);
            nulterminate(pcmd->right);
            break;

        case LIST:
            lcmd = (struct listcmd*)cmd;
            nulterminate(lcmd->left);
            nulterminate(lcmd->right);
            break;

        case BACK:
            bcmd = (struct backcmd*)cmd;
            nulterminate(bcmd->cmd);
            break;
    }
    
    return cmd;
}

