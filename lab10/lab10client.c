#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

int main(int argc, char **argv)
{
	int shmidChar, shmidInt, *numberClients;
	char *clientCase;
	if(argc < 2)
	{
		printf("Podaj numer gracza jko argumen\n");
		return 1;
	}
	shmidChar = shmget(40000, 4 * sizeof(char), IPC_CREAT|0600);
	if (shmidChar < 0)
	{
		fprintf(stderr, "Error in shmget");
		return 1;
	}
	shmidInt = shmget(41000, sizeof(int), IPC_CREAT|0600);
	if (shmidInt < 0)
	{
		fprintf(stderr, "Error in shmget");
		return 1;
	}
	
	union semun semUnion;
	key_t key = ftok(argv[0], argv[1][0]);
	int idSem = semget(key, 2, IPC_CREAT | IPC_EXCL | 0777);
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
	
	numberClients = (int*)shmat(shmidInt, NULL, 0);
	clientCase = (char*)shmat(shmidChar, NULL, 0);
	
	if (*numberClients >= 4)
	{
		printf("All slots reserver\n");
		return 0;
	}
	else	
		*numberClients = *numberClients + 1;
	int myClientId = *numberClients - 1;
	
	semOperation.sem_op = -1;
	semOperation.sem_flg = SEM_UNDO;
	
	status = semop(idSem, &semOperation, 1);
	if (status < 0)
	{
		printf("Error in semop\n");
		return 1;
	}
	semOperation.sem_num = 1;
	char clientSelect[5];
	getchar();
	while(clientSelect[0] != '5')
	{

		scanf("%s",clientSelect);
		
		semOperation.sem_op = 1;
		semOperation.sem_flg = 0;
		status = semop(idSem, &semOperation, 1);
		if (status < 0)
		{
			printf("Error in semop\n");
			return 1;
		}
		clientCase[myClientId] = clientSelect[0];
		printf("clientCase: %c\n", clientCase[myClientId]);
		//printf("%c", clientCase[*numberClients - 1]);
		semOperation.sem_op = -1;
		semOperation.sem_flg = SEM_UNDO;
		status = semop(idSem, &semOperation, 1);
		if (status < 0)
		{
			printf("Error in semop\n");
			return 1;
		}
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
