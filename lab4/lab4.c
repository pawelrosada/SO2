#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int flag1 = 0, flag2 = 0;
void signalSigstp()
{
  sigset_t mask;
  
  sigemptyset(&mask);
  sigaddset(&mask, SIGTSTP);
  sigprocmask(SIG_BLOCK, &mask, NULL);
  flag1 = 1;
}
void signalSigint()
{
  flag2 = 1;
}
int main (int argc, char **argv) 
{
  char text[32], tmp[32], *argToChar, *pidTmp, *processPid;
  int i, arg, status;
  
  if (argc > 1)
    arg = atoi(argv[1]);
  pid_t *processId = malloc(sizeof(pid_t) * arg);

  sprintf(tmp,"%d",getpid());
  processPid = tmp;

  sprintf(text,"%d",arg-1);
  argToChar = text;

  pidTmp = malloc(255);
  if (argc > 2)
    pidTmp = argv[2];

  strcat(pidTmp, processPid);
  strcat(pidTmp, " ");
  for (i = 0; i < arg; i++)
  {
    if((processId[i] = fork()) < 0 )
    {
      fprintf(stderr,"Error processed fork()");
      return 1;
    }
    if (processId[i] == 0)
    { 
      pid_t newSid = setsid();
      if(  newSid == -1 )
	perror( "parent: setsid" );
      setpgid(processId[i], newSid);
      execlp(argv[0], argv[0], argToChar, pidTmp, (char*)0);
      printf("Error processed execlp()");
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  struct sigaction action1, action2;
  
  action1.sa_handler = &signalSigint;
  action2.sa_handler = &signalSigstp;
  
  while(flag2 == 0)
  {
    sigaction( SIGTSTP, &action2, NULL );
    sigaction( SIGINT, &action1, NULL );
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (flag2 == 1)
  {
    for ( i = 0; i<arg; i++)
      kill(processId[i],SIGINT);
  }
  for ( i = 0; i<arg; i++)
     wait(&status);
  
  printf("%s\n", pidTmp);
  
  if (flag1 == 1)
    printf("Ctrl + Z pressed\n");
  
  return 0;
}