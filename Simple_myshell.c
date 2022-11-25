#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX_CMD_ARG 10
#define BUFSIZ 256

const char *prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZ];

void fatal(char *str){
	perror(str);
	exit(1);
}

int makelist(char *s, const char *delimiters, char** list, int MAX_LIST){	
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if( (s==NULL) || (delimiters==NULL) ) return -1;

  snew = s + strspn(s, delimiters);	/* Skip delimiters */
  if( (list[numtokens]=strtok(snew, delimiters)) == NULL )
    return numtokens;
	
  numtokens = 1;
  
  while(1){
     if( (list[numtokens]=strtok(NULL, delimiters)) == NULL)
	break;
     if(numtokens == (MAX_LIST-1)) return -1;
     numtokens++;
  }
  return numtokens;
}

void myShell_cd( int argc, char **argv ){
	char* path;
	
	if (argc == 1 || strcmp(argv[1], "~")==0){
		path = getenv("HOME");
	}
	else{
		path = argv[1];
	}
	if (chdir(path)<0)
		perror("error: ");
}
int is_background(char *cmdline)
{    
	for(int i=0; i < strlen(cmdline); i++)
	{
        	if(cmdline[i] == '&')
        	{
            		cmdline[i] = ' ';
           		return 1;
        	}
	}
	return 0;
}

static void child_handler(int signo){
	int status;
	int pid;
	while((pid = waitpid(-1, &status, WNOHANG))>0){
	}
}

void sig_handler(int signo){
	printf("\n");
}

int main(int argc, char **argv){
  int i=0;
  pid_t pid;
 	signal(SIGCHLD, (void*)child_handler);
	signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
  while (1) {
  	fputs(prompt, stdout);
	fgets(cmdline, BUFSIZ, stdin);
	int back = is_background(cmdline);

	cmdline[strlen(cmdline) -1] = '\0';
	int cargc = makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);
	if (strcmp("exit", cmdline) == 0) break;

	if (strcmp("cd", cmdvector[0]) == 0){
		myShell_cd(cargc, cmdvector);
		continue;
	}
	
	switch(pid=fork()){
	case 0:
	if(back!=1){
		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		}
	else{
		signal(SIGINT, SIG_IGN);
        	signal(SIGTSTP, SIG_IGN);
        	signal(SIGQUIT, SIG_IGN);
	}
		execvp(cmdvector[0], cmdvector);
		fatal("main()");
	case -1:
  		fatal("main()");
	default:
		if(back == 1) break;
		wait(NULL);
	}
  }
  return 0;
}
