#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

char *getSalt(char *hash)
{
    int i = 0, tmp = 0;
    char *salt = malloc(sizeof(char*)), *tmpChar = malloc(sizeof(char*));
    while (tmp != 2)
    {
		if(hash[i] == '$')
			tmp++;
		if(tmp == 2)
		{
			i++;
			*tmpChar = hash[i];
			strcpy(salt, tmpChar);
		}
		i++;
    }
    while (hash[i] != '$')
    {
		*tmpChar = hash[i];
		strcat(salt, tmpChar);
		i++;
    }
    return salt;
}
char *getMethod(char hash[1024])
{
    int i = 0, tmp = 0;
    char *method = malloc(sizeof(char*)), tmpChar[2];
    tmpChar[1] = '\0';
    tmpChar[0] = hash[i];
    strcpy(method, tmpChar);
    if(hash[i] == '$')
		tmp++;
    i++;
    while (tmp != 2)
    {
		if(hash[i] == '$')
			tmp++;
		tmpChar[0] = hash[i];
		strcat(method, tmpChar);
		i++;
    }
    return method;
}

char hash[1024];
int isPassowrd, passwordFile, numberLines, allLines;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void getPassword(char readPassword[1024], int *lastLine, char *password)
{
    char buff[2];
    buff[1] = '\0';
    if(readPassword[*lastLine] == '\0' || strlen(readPassword) == *lastLine)
		*lastLine = strlen(readPassword);
    if(readPassword[*lastLine] != '\r' && readPassword[*lastLine] != '\n')
    {
		printf("*");
		buff[0] = readPassword[*lastLine];
		strcpy(password, buff);
    }
    else
    {
		while(1)
		{
			if(readPassword[*lastLine] == '\r')
				*lastLine = *lastLine + 1;
			else if (readPassword[*lastLine] == '\n')
				*lastLine = *lastLine + 1;
			else if(readPassword[*lastLine] == '\0')
			{
				*lastLine = strlen(readPassword);
				return;
			}
			else
				break;
		}
		buff[0] = readPassword[*lastLine];
		strcpy(password, buff);
    }
    *lastLine = *lastLine + 1;
    while (readPassword[*lastLine] != '\r' && readPassword[*lastLine] != '\0' && readPassword[*lastLine] != '\n')
    {
		buff[0] = readPassword[*lastLine];
		strcat(password, buff);
		*lastLine = *lastLine + 1;
    }
}
int getLastR(char *readPassword)
{
    int lastR, i = strlen(readPassword);
    //i--;
    while (readPassword[i] != '\r' && readPassword[i] != '\n')
		i--;
    if ( i == strlen(readPassword))
		lastR = 0;
    else
    {
		lastR = strlen(readPassword) - i;
    }
   
    return lastR;
    
}
int numberIterations;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
void *benchmark()
{
    srand(time(NULL));
    char charToCrypt[10], salt[20], *toTest = malloc(sizeof(char*));
    sprintf(charToCrypt, "%d", rand()%100000);
    strcpy(salt, "$6$salgn2rfa5");
    toTest = crypt(charToCrypt, salt);
    while(numberIterations < 1024)
    {
	    char test[10];
	    sprintf(test, "test%d", rand() + 2312 % 10000000);
	    
	    strcmp(toTest, crypt(test, salt));
	    
	    pthread_mutex_lock(&mutex);
	    numberIterations++;
	    pthread_mutex_unlock(&mutex);
	    
	    memset(charToCrypt, '\0', sizeof(charToCrypt));
    }
}

void *threads()
{
    char *salt = malloc(sizeof(char*)), *method, readPassword[1024], *tmpHash = malloc(sizeof(char*));
    char tmpPassword[1024];
    int lastLine = 0, tmpPasswordFile;
    salt = getSalt(hash);
    method = getMethod(hash);
    strcat(method, salt);
    while(isPassowrd == 0)
    {
		pthread_mutex_lock(&mutex);
		tmpPasswordFile = passwordFile;
		if(read(tmpPasswordFile, readPassword, 1024) == 0)
		{
			printf("Nie znaleziono hasla\n");
			break;
		}
		pthread_mutex_unlock(&mutex);
		
		int pom = getLastR(&readPassword[0]);
		
		pthread_mutex_lock(&mutex);
		lseek(tmpPasswordFile, -pom, SEEK_CUR);
		passwordFile = tmpPasswordFile;
		pthread_mutex_unlock(&mutex);
		int i, tmpSize = strlen(readPassword);
		for(i = 0; i < pom; i++)
			readPassword[tmpSize - pom - i] = '\0';
		
		while(1)
		{
			getPassword(readPassword, &lastLine, &tmpPassword[0]);
			tmpHash = crypt(tmpPassword, method);
			if(strcmp(tmpHash, hash) == 0)
			{
				printf("\nZnaleziono haslo: %s\n", tmpPassword);
					
				pthread_mutex_lock(&mutex);
				isPassowrd = 1;
				pthread_mutex_unlock(&mutex);
					
				break;
			}
			if (lastLine == strlen(readPassword))
				break;
				
			pthread_mutex_lock(&mutex);
			numberLines++;
			pthread_mutex_unlock(&mutex);
				
			memset(tmpPassword, '\0', sizeof(tmpPassword)); 
			memset(tmpHash, '\0', sizeof(tmpHash));
		}
		memset(readPassword, '\0', strlen(readPassword));
		lastLine = 0;
    }
    
}
int main(int argc, char **argv)
{
    memset(hash, '\0', sizeof(hash));
    isPassowrd = 0;
    numberLines = 0;
    int ret, threadsNumber = -1;
    float percent;
    char *file = malloc(sizeof(char*)), comand[50]; 
    
    opterr = 0; //no default "invalid option" info
    while ((ret = getopt (argc, argv, "p:f:t:")) != -1) 
	switch (ret) 
	{
	    case 'p':
		    strcpy(hash, optarg);
		    break;
	    case 'f': 
		    strcpy(file, optarg);
		    break;
	    case 't':
		    threadsNumber = atoi(optarg);
		    break;
	    case '?': 
		    if (optopt == 'p' || optopt == 'f') 
				fprintf (stderr, "Option -%c requires an argument.\n", optopt); 
		    else 
				fprintf (stderr, "Unknown option `-%c'.\n", optopt); 
		    return 1; 
	    default: 
		    abort (); 
	}
  
    if (threadsNumber == -1)
    {
		int numberThreads = 1, i;
		pthread_t threadId;
		while(numberThreads <= 32)
		{
			clock_t start,stop;
			for(i = 0; i < numberThreads; i++)
			{
				pthread_create(&threadId, NULL, &benchmark, NULL);
			}
			start = clock();
			while(1)
			{
				if(numberIterations >= 1024)
				{
					stop = clock();
					if (numberThreads == 1)
						printf("Time for %d thread: %.3fs\n",numberThreads, ((float)stop- (float)start) / CLOCKS_PER_SEC);
					else
						printf("Time for %d threads: %.3fs\n",numberThreads, ((float)stop- (float)start) / CLOCKS_PER_SEC);
					numberIterations = 0;
					break;
				}
			}
			numberThreads = numberThreads * 2;
		}
    }
    else
    {
		if (strlen(hash) == 0 || strlen(file) == 0)
		{
			fprintf(stderr, "Missing arguments\n");
			return 1;
		}
		if ((passwordFile = open(file, O_RDONLY)) == -1)
		{
			fprintf(stderr, "Error to open file\n");
			return 1;
		}
		strcpy(comand, "wc -l ");
		strcat(comand, file);
		strcat(comand, "> lines.txt");
		system(comand);
		FILE *fp = fopen("lines.txt", "r");
		if(fp != NULL)
		{
			fscanf(fp,"%d", &allLines);
			fclose(fp);
		}
		else
			fprintf(stderr, "Error to open file\n");
		system("rm lines.txt");
		
		if (threadsNumber > sysconf(_SC_NPROCESSORS_ONLN))
			threadsNumber == sysconf(_SC_NPROCESSORS_ONLN);
		
		pthread_t *threadId = malloc(sizeof(pthread_t) * threadsNumber);
		int i;
		for(i = 0; i < threadsNumber; i++)
			pthread_create(&threadId[i], NULL, &threads, NULL);
		while(isPassowrd == 0)
		{
			percent = ((float)numberLines  * 100 / (float)allLines);
			printf("%.2f%c \r", percent, '%');
		}
    }
    return 0;
}