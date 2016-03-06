#include "unamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/utsname.h>


struct utsname systemInfo;

char *systemName()
{
  if (systemInfo.sysname != NULL)
    return systemInfo.sysname;
  else
  {
    fprintf(stderr, "Error to return systemName\n");
    return 1;
  }
}
char *systemNodeName()
{
  if (systemInfo.nodename != NULL)
    return systemInfo.nodename;
   else
  {
    fprintf(stderr, "Error to return systemNodeName\n");
    return 1;
  }
}
char *systemRelease()
{
  if (systemInfo.release != NULL)
    return systemInfo.release;
  else
  {
    fprintf(stderr, "Error to return release\n");
    return 1;
  }
}
char *systemVersion()
{
  if (systemInfo.version != NULL)
    return systemInfo.version;
  else
  {
    fprintf(stderr, "Error to return version\n");
    return 1;
  }
}
char *systemMachine()
{
  if (systemInfo.machine != NULL)
    return systemInfo.machine;
  else
  {
    fprintf(stderr, "Error to return machine\n");
    return 1;
  }
}

_init()
{
  printf("Unamelib connected\n");
  if( uname( &systemInfo ) == -1 ) 
  {
       fprintf(stderr, "Error to load uname\n");
       return 1;
  }
}
_fini()
{
  printf("Unamelib disconected\n");
}
