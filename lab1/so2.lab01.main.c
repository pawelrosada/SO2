#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

int main (int argc, char **argv) 
{ 
	int ret, numbergroups, i; 
	opterr = 0; //no default "invalid option" info 
	gid_t *groups;
	groups = (gid_t *) malloc ( numbergroups * sizeof (gid_t) );
	if (groups == NULL) 
	{
		fprintf (stderr, "Memory alocation fail\n");
		return 1;
	}
	struct passwd* pass, *tmp = NULL;
	struct group* gr;
	
	while ((ret = getopt (argc, argv, "ug:")) != -1) 
		switch (ret) 
		{ 
			case 'u':
				while((pass = getpwent()) != NULL)
				{
					numbergroups = sysconf( _SC_NGROUPS_MAX );
					tmp=pass;
					if ( getgrouplist( tmp->pw_name, tmp->pw_gid, groups , &numbergroups) == -1) 
           					fprintf (stderr, "Array is too small: %d\n", numbergroups);
					printf ("%s ", tmp->pw_name);
   					for (i=0; i < numbergroups; i++)
					{
						if ((gr = getgrgid(groups[i])) != NULL)
							printf("%s ", gr->gr_name);
					}
					printf("\n");
				}
				break;
			case 'g': 
				if( ( gr = getgrnam( optarg ) ) == NULL ) 
				{
       					fprintf( stderr, "Bad group name \n" );
       					return 1;
    				}
    				printf( "Nazwa grupy: %s\n", gr->gr_name );
				char** user = NULL;
    				for (user = gr->gr_mem; *user != NULL; user++)
				{
					numbergroups = sysconf( _SC_NGROUPS_MAX );
       					printf( "%s ", *user );
					struct passwd *pwd = getpwnam(*user);
					if ( getgrouplist( pwd->pw_name, pwd->pw_gid, groups , &numbergroups) == -1) 
           					fprintf (stderr, "Array is too small: %d\n", numbergroups);
					printf ("\tGrupy: ");
   					for (i=0; i < numbergroups; i++)
					{
						if ((gr = getgrgid(groups[i])) != NULL)
							printf("%s ", gr->gr_name);
					}
					printf("\n");
				}
				break;
			case '?': 
				if (optopt == 'g' || optopt == 'u') 
					fprintf (stderr, "Option -%c requires an argument.\n", optopt); 
				else 
					fprintf (stderr, "Unknown option `-%c'.\n", optopt); 
				return 1; 
			default: abort (); 
		}
	if (argc == 1)
	{
		while((pass = getpwent()) != NULL)
			printf("%s\n", pass->pw_name);
	}
	return 0;
}
