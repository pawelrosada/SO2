#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

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
      execlp(argv[0], argv[0], argToChar, pidTmp, (char*)0);
      printf("Error processed execlp()");
    }
    
  }
  for ( i = 0; i<arg; i++)
     wait(&status);
  printf("%s\n", pidTmp);
  return 0;
}
