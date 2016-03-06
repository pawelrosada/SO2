#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <libgen.h>

int isInt(char *name)
{
  int i = 0, isNumber = 0;
  while (name[i] != '\0')
  {
    if (isdigit(name[i]))
      isNumber++;
    i++;
  }
  if (i == isNumber)
    return 1;
  else 
    return 0;
}
char *returnName(char *path)
{
  char *name = basename(path);
  return name;
}
int checkName(char path[255], char *pidName)
{
  FILE *file;
  char tmp[255];
  file = fopen(path,"r");
  if (file == NULL)
  {
    fprintf(stderr, "Error to open file\n");
    return 0;
  }
  else
  {
    fscanf(file, "%s %s", &tmp, &tmp);
    if (strcmp (pidName, tmp) == 0)
    {
      pclose(file);
      return 1;
    }
    else
    {
      pclose(file);
      return 0;
    }
  }
}
char *getPidId(char *pidName)
{
  char *pid = malloc(255* sizeof(char*)), tmp[255], path[255];
  DIR *directory;
  FILE *file;
  struct dirent *currentDirectory;
  
  strcpy(path, "/proc/");
  strcpy(pid,"");
  
  directory = opendir(path);
  if (directory == NULL)
  {
    fprintf(stderr, "Error to open directory\n");
    return NULL;
  }
  while ((currentDirectory = readdir(directory)) != NULL)
  {
    strcpy (path, "/proc/");
    if (isInt(currentDirectory->d_name) == 1)
    {
      strcat (path, currentDirectory->d_name);
      strcat (path, "/exe");
      
      char *bufforExe;
      readlink(path, bufforExe, 255);
      if (bufforExe != NULL)
      {
		bufforExe = returnName(bufforExe);
		if (strcmp (bufforExe, tmp) == 0)
		{
		  strcat(pid, currentDirectory->d_name);
		  strcat(pid, " ");
		}
		else
		{
		  strcpy (path, "/proc/");
		  strcat (path, currentDirectory->d_name);
		  strcat (path, "/status");
		  if (checkName(path, pidName) == 1)
		  {
			strcat(pid, currentDirectory->d_name);
			strcat(pid, " ");
		  }
		}
      }
      else
      {
		strcpy (path, "/proc/");
		strcat (path, currentDirectory->d_name);
		strcat (path, "/status");
		if (checkName(path, pidName) == 1)
		{
		  strcat(pid, currentDirectory->d_name);
		  strcat(pid, " ");
		}
      }
    }
  }
  closedir(directory);
  return pid;
}
int main (int argc, char **argv) 
{
  char *pidName, pidNameChar[255], *pidId;
  
  if (argc > 1)
  {
    int i;
    for (i = 1; i<argc; i++)
    {
      pidName = argv[i];
      strcpy(pidNameChar, pidName);
      pidId = getPidId(pidNameChar);
      if (pidId == NULL)
      {
		fprintf(stderr, "Error to search pid id\n");
		return 1;
      }
		printf("%s ", pidId);
    }
  }
  else 
  {
    fprintf(stderr, "Lack of name");
    return 1;
  }
  
  printf("\n");

  
  return 0;
}


