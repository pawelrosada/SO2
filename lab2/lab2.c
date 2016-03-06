#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h> 
#include <unistd.h>

char *(*unamelibName)();
char *(*unamelibMachine)();
char *(*unamelibVersion)();
char *(*unamelibNodeName)();
char *(*unamelibRelease)();

int main (int argc, char **argv) 
{
  void *unamelib = dlopen ("./unamelib.so.0.1", RTLD_LAZY);
  if (!unamelib)
    dlerror();
  else
  {
    int ret, numbergroups, i; 
    opterr = 0; //no default "invalid option" info
    while ((ret = getopt (argc, argv, "asnrvm")) != -1) 
      switch (ret) 
      {
	case 's':
	  unamelibName = dlsym(unamelib, "systemName");
	  printf("%s ", unamelibName());
	  break;
	case 'r': 
	  unamelibRelease = dlsym(unamelib, "systemRelease");
	  printf("%s ", unamelibRelease());
	  break;
	case 'v':
	  unamelibVersion = dlsym(unamelib, "systemVersion");
	  printf("%s ", unamelibVersion());
	  break;
	case 'm':
	  unamelibMachine = dlsym(unamelib, "systemMachine");
	  printf("%s ", unamelibMachine());
	  break;
	case 'n':
	  unamelibNodeName = dlsym(unamelib, "systemNodeName");
	  printf("%s ", unamelibNodeName());
	  break;
	case 'a':
	  unamelibName = dlsym(unamelib, "systemName");
	  unamelibRelease = dlsym(unamelib, "systemRelease");
	  unamelibVersion = dlsym(unamelib, "systemVersion");
	  unamelibMachine = dlsym(unamelib, "systemMachine");
	  unamelibNodeName = dlsym(unamelib, "systemNodeName");
	  printf("%s ", unamelibName());
	  printf("%s ", unamelibNodeName());
	  printf("%s ", unamelibRelease());
	  printf("%s ", unamelibVersion());
	  printf("%s ", unamelibMachine());
	  break;
	case '?': 
	  printf ("|Unknown option `-%c'| ", optopt); 
      }
      printf("\n");
      dlclose(unamelib);
  }
  
  
  return 0;
}

	  
	  
    