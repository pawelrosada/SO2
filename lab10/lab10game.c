#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

char game[11][21];
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void convertArray()
{
	int i, j;
	for (i = 0; i < 11; i++)
		for ( j = 0; j < 21; j++)
		{
			if (game[i][j] == 'x' )
				continue;
			else
			{
				int k, l, numberMines = 0;
				for (k = i - 1; k <= i + 1; k++)
				{
					for (l = j - 1; l <= j + 1; l++)
					{
						if (k > 0 && k < 11 && l > 0 && l < 21)
						{
							if (game[k][l] == 'x')
								numberMines++;
						}
					}
				}
				char tmp[2];
				sprintf(tmp, "%d", numberMines);
				game[i][j] = tmp[0];
				numberMines = 0;
			}
		}
}

struct client
{
	int x;
	int y;
	char lastCase;
	int endGame;
};

int main(int argc, char **argv)
{
	int shmidChar, shmidInt, *numberClients;
	char *clientCase;
	char road[11][21];
	int i, j;
	srand(time(NULL));
	
	for (i = 0; i < 11; i++)
		for ( j = 0; j < 21; j++)
		{
			game[i][j] = ' ';
			road[i][j] = ' ';
		}
	for (i = 0; i < 50; i++)
	{
		int x = rand() % 11;
		int y = rand() % 21;
		if ((x == 0 || x == 10) && (y == 0 || y == 20))
			i--;
		else
			game[x][y] = 'x';
	}

	convertArray();
	
	union semun semUnion;
	key_t key = ftok(argv[0], '1');
	int idSem = semget(key, 1, IPC_CREAT | IPC_EXCL | 0777);
	if (idSem < 0)
	{
		printf("Error in semget\n");
		return 1;
	}
	semUnion.val = 1;
	int status = semctl(idSem, 0, SETVAL, semUnion);
	if (status < 0)
	{
		printf("Error in semctl\n");
		return 1;
	}
	
	struct sembuf semOperation;
	semOperation.sem_num = 0;
	semOperation.sem_op = 1;
	semOperation.sem_flg = 0;
	
	status = semop(idSem, &semOperation, 1);
	if (status < 0)
	{
		printf("Error in semop\n");
		return 1;
	}
	
	shmidChar = shmget(40000, 4 * sizeof(char), IPC_CREAT|0777);
	if (shmidChar < 0)
	{
		fprintf(stderr, "Error in shmget");
		return 1;
	}
	shmidInt = shmget(41000, sizeof(int), IPC_CREAT|0777);
	if (shmidInt < 0)
	{
		fprintf(stderr, "Error in shmget");
		return 1;
	}
	
	numberClients = (int*)shmat(shmidInt, NULL, 0);
	clientCase = (char*)shmat(shmidChar, NULL, 0);
	*numberClients = 0;
	
	semOperation.sem_op = -1;
	semOperation.sem_flg = SEM_UNDO;
	status = semop(idSem, &semOperation, 1);
	if (status < 0)
	{
		printf("Error in semop\n");
		return 1;
	}
	
	while (*numberClients == 0);
	int win = 0;
	struct client clients[4];
	
	clients[0].x = 0;
	clients[0].y = 0;
	clients[1].x = 0;
	clients[1].y = 20;
	clients[2].x = 10;
	clients[2].y = 0;
	clients[3].x = 10;
	clients[3].y = 20;

	clients[0].endGame = 0;
	clients[1].endGame = 0;
	clients[2].endGame = 0;
	clients[3].endGame = 0;
	system("clear");
	while (win == 0)
	{
		
		system("clear");
		printf("%d\n", *numberClients);
		for ( i = 0; i < *numberClients; i++)
		{
			if (clients[i].endGame == 0)
			{
				printf("%c\n", clientCase[i]);
				switch (clientCase[i])
				{
					case '8':
						if (clients[i].x - 1 >= 0)
							clients[i].x--;	
						break;
					case '7':
						if (clients[i].x - 1 >= 0 && clients[i].y - 1 >= 0)
						{
							clients[i].x--;
							clients[i].y--;
						}
						break;
					case '9':
						if (clients[i].x - 1 >= 0 && clients[i].y + 1 <= 20)
						{
							clients[i].x--;
							clients[i].y++;
						}
						break;
					case '4':
						if (clients[i].y - 1 >= 0)
							clients[i].y--;
						break;
					case '6':
						if (clients[i].y + 1 <= 20)
							clients[i].y++;
						break;
					case '2':
						if (clients[i].x + 1 <= 10)
							clients[i].x++;
						break;
					case '1':
						if (clients[i].x + 1 <= 10 && clients[i].y - 1 >= 0)
						{
							clients[i].x++;
							clients[i].y--;
						}
						break;
					case '3':
						if (clients[i].x + 1 <= 10 && clients[i].y + 1 <= 20)
						{
							clients[i].x++;
							clients[i].y++;
						}
						break;
					case '5':
						win = 1;
						printf("Exit");
						break;
				}
				clients[i].lastCase = clientCase[i];
				semOperation.sem_op = 1;
				semOperation.sem_flg = 0;
				status = semop(idSem, &semOperation, 1);
				if (status < 0)
				{
					printf("Error in semop\n");
					return 1;
				}
				clientCase[i] = ' ';
				semOperation.sem_op = -1;
				semOperation.sem_flg = SEM_UNDO;
				status = semop(idSem, &semOperation, 1);
				if (status < 0)
				{
					printf("Error in semop\n");
					return 1;
				}
			}
			else
				if(clientCase[i] == '5')
				{
					win = 1;
					printf("Exit\n");
				}
		}
		
		for(i = 0; i < *numberClients; i++)
		{
			if (game[clients[i].x][clients[i].y] != 'x')
				road[clients[i].x][clients[i].y] = '-';
			else
			{
				clients[i].endGame = 1;
				road[clients[i].x][clients[i].y] = 'x';
			}
		}
		
		for(i = 0; i < *numberClients; i++)
			if((clients[i].x == 5 || clients[i].x == 6) && (clients[i].y == 10 || clients[i].y == 11))
			{
				printf("Wygyrywa Gracz %d", i+1);
				win = 1;
			}
		
		for(i = 0; i < 11; i++)
		{
			for( j = 0; j < 21; j++)	
			{
				if(road[i][j] == '-')
					printf("%c", game[i][j]);
				else if (road[i][j] =='x')
					printf("x");
				else 
					printf("*");
			}
			printf("\n");
		}
		sleep(1);
		convertArray();	
	}
	status = semctl(idSem, 0, IPC_RMID);
	if (status < 0)
	{
		printf("Error in semctl\n");
		return 1;
	}
	
	status = shmctl(shmidChar, IPC_RMID, NULL);
	if (status < 0)
	{
		printf("Error in shmctl\n");
		return 1;
	}
	status = shmctl(shmidInt, IPC_RMID, NULL);
	if (status < 0)
	{
		printf("Error in shmctl\n");
		return 1;
	}
	
	return 0;
}
