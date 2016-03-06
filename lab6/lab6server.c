#include <stdio.h>
#include <string.h>    
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct client
{
    char *name;
    char lastMessage[2048];
    int clientSocket;
    struct sockaddr_in clientSock;
};

char* getOrder(char msg[2048])
{
    
    char *order = malloc(sizeof(char)), *buff = malloc(sizeof(char));
    int i = 0;

    if (msg[i] == '/')
    {
	*buff = msg[i];
	strcpy(order, buff);
	i++; 
	while (msg[i] != ' ')
	{
	    if (msg[i] == '\0')
		break;
	    *buff = msg[i];
	    strcat(order, buff);
	    i++;
	}
    }
    free(buff);
    return order;
}
char *getNick(char msg[2048])
{
    char *nick = malloc(sizeof(char)), *buff = malloc(sizeof(char));
    int i = 0, isNick = 0, j;

    if (msg[i] == '/')
    {
	while( msg[i] != '\0')
	{
	    if (msg[i] == ' ')
	    {
		isNick++;
		i++;
		continue;
	    }
	    if (isNick == 1)
	    {
		*buff = msg[i];
		strcat(nick, buff);
	    }
	    i++;
	}
    }
    return nick;
}
char *getMessage(char msg[2048])
{
    char *message = malloc(sizeof(char)), *buff = malloc(sizeof(char));
    int i = 0, isMsg = 0;
   
    if (msg[i] == '/')
    {
	while (msg[i] != '\0')
	{
	    if (msg[i] == ' ')
	    {
		isMsg++;
		if (isMsg == 2)
		{
		    i++;
		    continue;
		}
	    }
	    if (isMsg >= 2)
	    {
		*buff = msg[i];
		strcat(message, buff);
	    }
	    i++;
	}
    }
    return message;
}
struct client *getClientStruct(char *nick, struct client clients[10], int numberClients)
{
    struct client *tmp = NULL;
    int i;
    for (i = 0; i < numberClients; i++)
    {
	if (strcmp(clients[i].name, nick) == 0)
	{
	    tmp = &clients[i];
	    break;
	}
    }
    return tmp;
}
char *getClientsList(struct client clients[10], int numberClients)
{
    int i;
    char *list = malloc(sizeof(char));
    strcpy(list, clients[0].name);
    for(i = 1; i < numberClients; i++)
    {
	strcat(list, " \n");
	strcat(list, clients[i].name);
    }
    return list;
}
int main(int argc, char **argv)
{
    daemon(0,0);
    struct client clients[10];
    int numberClients = 0, serverSocket, clientSocket, ret, socketNumber, readSize, maxSocketNumber, i, activity;
    struct sockaddr_in server;
    char messageFromClient[2048], messageToClient[2048];
    
    fd_set socketList;
     
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        fprintf(stderr,"Error to create socket");
     
    server.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server.sin_addr.s_addr);
    
    opterr = 0; //no default "invalid option" info
    while ((ret = getopt (argc, argv, "p:q")) != -1) 
      switch (ret) 
      {
	case 'p':
		socketNumber = atoi(optarg);
		server.sin_port = htons(socketNumber);
		break;
	case 'q': 
		system("`killall lab6server`");
		exit(0);
		break;
	case '?': 
		if (optopt == 'p') 
		    fprintf (stderr, "Option -%c requires an argument.\n", optopt); 
		else 
		    fprintf (stderr, "Unknown option `-%c'.\n", optopt); 
		return 1; 
	default: 
		abort (); 
      }
    
    if(bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "Error to bind");
        return 1;
    }
    
    listen(serverSocket, 2);
    char c = sizeof(struct sockaddr_in);
    printf("Wait for connection\n");
    
    while(1)
    {
	FD_ZERO(&socketList);
        FD_SET(serverSocket, &socketList);
        maxSocketNumber = serverSocket;
         
        for ( i = 0 ; i < numberClients; i++) 
        {
            if(clients[i].clientSocket > 0)
                FD_SET( clients[i].clientSocket , &socketList);
            if(clients[i].clientSocket > maxSocketNumber)
                maxSocketNumber = clients[i].clientSocket;
	}
	activity = select( maxSocketNumber + 1 , &socketList , NULL , NULL , NULL);
    
        if (activity < 0)
            fprintf(stderr, "Select error");
          
        if (FD_ISSET(serverSocket, &socketList)) 
        {
	    strcpy(messageFromClient,"");
	    if (numberClients < 10)
	    {
		printf("\nNew connection\n");
		clientSocket = accept(serverSocket, (struct sockaddr *)&clients[numberClients].clientSock, (socklen_t*)&c);
		if (clientSocket < 0)
		{
		    fprintf(stderr,"Error to accept");
		    continue;
		}
		clients[numberClients].clientSocket = clientSocket;
		if (readSize = recv(clients[numberClients].clientSocket, messageFromClient , 2048, 0) <= 0)
		{
		    fprintf(stderr, "Error to read client name\n");
		    continue;
		}
		else
		    messageFromClient[strlen(messageFromClient)] = '\0';
		
		if (strcmp(getOrder(messageFromClient), "/nick") == 0)
		    clients[numberClients].name = getNick(messageFromClient);
		printf("Client name %s connected \n", clients[numberClients].name);
		numberClients++;
		memset(messageFromClient, '\0', 2048);
	    }
	}
	
	for (i = 0; i < numberClients; i++) 
        {  
            if (FD_ISSET( clients[i].clientSocket, &socketList)) 
            {
                if ((readSize = recv( clients[i].clientSocket, messageFromClient, 2048, 0)) == 0)
                {
		    printf("Delete\n");
                    int counter=0,j;
		    struct client tmp[10];
		    for (j = 0; j < numberClients; j++)
		    {
			
			if(clients[i].clientSocket == clients[j].clientSocket)
			    close(clients[j].clientSocket);
			else
			{
			    tmp[counter] = clients[j];
			    counter++;
			}
		    }
		    numberClients = counter;
		    for (j = 0; j < numberClients; j++)
			clients[j] = tmp[j];
                }
                else
		{
		    strcpy(clients[i].lastMessage, messageFromClient);
		    clients[i].lastMessage[strlen(clients[i].lastMessage)] = '\0';
		    memset(messageFromClient, '\0', 2048);  
		}
            }
        }
	
	for(i = 0; i < numberClients; i++)
	{
	    if(strlen(clients[i].lastMessage) != 0)
	    {
		printf("Is message: %s\n", clients[i].lastMessage);
		if ((strcmp(getOrder(clients[i].lastMessage), "/msg") == 0))
		{
		    printf("Msg\n");
		    char *nick = malloc(sizeof(char));
		    nick = getNick(clients[i].lastMessage);
		    if (nick == NULL)
		    {
			strcpy(messageToClient, "Nick needed");
			if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message");
			memset(clients[i].lastMessage, '\0', 2048);
			continue;
		    }
			
		    struct client *tmp = getClientStruct(nick, clients, numberClients);
		    if (tmp == NULL)
		    {
			strcpy(messageToClient, "Client ");
			strcat(messageToClient, nick);
			strcat(messageToClient, " disconnected");
			
			if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message");
			memset(clients[i].lastMessage, '\0', 2048);
			continue;
		    }
		    strcpy(messageToClient, nick);
		    strcat(messageToClient, ": ");
		    strcat(messageToClient, getMessage(clients[i].lastMessage));
		    if (strlen(messageToClient) == 0)
		    {
			strcpy(messageToClient, "Message needed");
			if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message");
			memset(clients[i].lastMessage, '\0', 2048);
			continue;
		    }
		    if(send(tmp->clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
		    {
			strcpy(messageToClient, "Error to send message");
			if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message\n");
		    }
		    memset(clients[i].lastMessage, '\0', 2048);
		    
		}
		else if(strcmp(getOrder(clients[i].lastMessage), "/list") == 0)
		{
		    printf("List\n");
		    strcpy(messageToClient, getClientsList(clients, numberClients));
		    
		    printf("%s\n", messageToClient);
		    if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message\n");
		    memset(clients[i].lastMessage, '\0', 2048);
		}
		else if(strcmp(getOrder(clients[i].lastMessage), "/quit") == 0)
		{
		    printf("Quit\n");
		    memset(clients[i].lastMessage, '\0', 2048);
		    int counter=0,j;
		    struct client tmp[10];
		    for (j = 0; j < numberClients; j++)
		    {
			if(clients[i].clientSocket == clients[j].clientSocket)
			    close(clients[j].clientSocket);
			else
			{
			    tmp[counter] = clients[j];
			    counter++;
			}
		    }
		    numberClients = counter;
		    for (j = 0; j < numberClients; j++)
			clients[j]= tmp[j];
		   
		}
		else
		{
		    strcpy(messageToClient, "Wrong Order");
		    if (send(clients[i].clientSocket, messageToClient, strlen(messageToClient), 0) < 0)
			    fprintf(stderr, "Error to send message\n");
		    memset(clients[i].lastMessage, '\0', 2048);
		    
		}
	    } 
	}
    }
    return 0;
    
}
