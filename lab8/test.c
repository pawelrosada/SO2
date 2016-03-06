#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int ret;
    char *password = malloc(sizeof(char*)), *salt = malloc(sizeof(char*)), *hash = malloc(sizeof(char*));
    
    strcpy(salt, "$6$");
    opterr = 0; //no default "invalid option" info
    while ((ret = getopt (argc, argv, "p:s:")) != -1) 
		switch (ret) 
		{
			case 'p':
				strcpy(password, optarg);
				break;
			case 's': 
				strcat(salt, optarg);
				break;
			case '?': 
				if (optopt == 'p' || optopt == 's') 
					fprintf (stderr, "Option -%c requires an argument.\n", optopt); 
				else 
					fprintf (stderr, "Unknown option `-%c'.\n", optopt); 
				return 1; 
			default: 
				abort (); 
		}
   if (strlen(password) == 0 || strlen(salt) == 3)
   {
       fprintf(stderr, "Missing arguments\n");
       return 1;
   }
   hash = crypt(password, salt);
   printf ("%s\n", hash);
   free(salt);
   free(hash);   
   free(password);
   return 0;
}