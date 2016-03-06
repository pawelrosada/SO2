#include <stdio.h>
#include <string.h>    
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char **argv)
{
    int socketNumber = 0, ret, sock = socket(AF_INET , SOCK_STREAM , 0), readSize, max, activity;
    char *name = NULL, *ipAdress = NULL, firstMessage[100], message[2048], serverMessage[2048], *messagePointer = NULL;
    size_t len = 2048;
    struct sockaddr_in server;
    
    fd_set stdList;
    
    if (sock == -1)
    {
	fprintf(stderr,"Could not create socket");
	return 1;
    }
    
    server.sin_family = AF_INET;
    
    opterr = 0; //no default "invalid option" info
    while ((ret = getopt (argc, argv, "a:p:n:")) != -1) 
      switch (ret) 
      {
	case 'a':
	  ipAdress = optarg;
	  inet_aton(ipAdress, &server.sin_addr.s_addr);
	  break;
	case 'p': 
	  socketNumber = atoi(optarg);
	  server.sin_port = htons(socketNumber);
	  break;
	case 'n': 
	  name = optarg;
	  strcpy(firstMessage, "/nick ");
	  strcat(firstMessage, name);
	  break;
	case '?': 
	  if (optopt == 'p') 
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt); 
	  else 
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt); 
	  return 1; 
	default: abort (); 
      }
      if (socketNumber == 0 || ipAdress == NULL || name == NULL)
      {
	  fprintf(stderr, "Required all arguments \n");
	  return 1;
      }
      
     
      if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
      {
	  fprintf(stderr,"Connect failed\n");
          return 1;
      }
      else
      {
	  printf("Connected\nSending name to server\n");
	  printf("Type:\n/msg [nick] [message] - to send message\n/list - to get server list\n/quit - to quit\n");
	  
	  if( send(sock, firstMessage, strlen(firstMessage), 0) < 0)
          {
		fprintf(stderr, "Send name failed");
		return 1;
          }
          
      }
     
      while(1)
      {
	  FD_ZERO(&stdList);
	  FD_SET(sock, &stdList);
	  FD_SET(fileno(stdin), &stdList);
	  if (sock > fileno(stdin))
	      max = sock;
	  else
	      max = fileno(stdin);
	  activity = select( max + 1 , &stdList , NULL , NULL , NULL);
	  
	  if (activity < 0)
            fprintf(stderr, "Select error");
	  
	  if (FD_ISSET(fileno(stdin), &stdList)) 
	  {
	    getline (&messagePointer, &len, stdin);
	    strcpy(message, messagePointer);
	    message[strlen(message)-1] = '\0';
	    if( send(sock, message, strlen(message), 0) < 0)
		    fprintf(stderr, "Send failed");
	    else
	    {
		if (strcmp(message, "/quit") == 0)
		{
		    close(sock);
		    printf("Disconnected\n");
		    break;
		}
		memset(message, '\0', 2048);
	    }
	   
	  }
	  if (FD_ISSET(sock, &stdList)) 
	  {
	    memset(serverMessage, '\0', 2048);
	    if (readSize = recv(sock, serverMessage , 2048 , 0) <= 0)
	    {
		    fprintf(stderr, "Error to read server message\n");
		    continue;
	    }
	    else
		printf("%s\n", serverMessage);
	  }
	 
	  
      }
      return 0;
      
}
