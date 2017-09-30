#include "commandlinereader.h"
#include "tcpcommandwriter.h"
#include "fpt.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_PORT 58006
#define BUFFER_MAX 128
#define MAXARGS 3

/*Comandos */
#define COMANDO_SAIR "exit"
#define COMANDO_LIST "list"
#define COMANDO_REQUEST "request"


int main(int argc, char **argv)
{
  int port = DEFAULT_PORT, i, bytesRead, bytesToRead;
  int fd; //used for TCP comunication
  char servername[BUFFER_MAX], buffer[BUFFER_MAX];
  char commandlinebuffer[BUFFER_MAX];
  char *args[MAXARGS + 1];
  char *token;

  struct hostent *hostptr;
  struct sockaddr_in serveraddr;

  /* sets the default value for the server name*/
  if (gethostname(servername, BUFFER_MAX) == -1)
  {
    printf("error getting local host name: %s\n", strerror(errno));
    exit(EXIT_SUCCESS);
  }

  /* reads the inputs from argv and sets the server name and port*/
  for (i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0)
      port = atoi(argv[i + 1]);
    if (strcmp(argv[i], "-n") == 0)
      strcpy(servername, argv[i + 1]);
  }

  fd = socket(AF_INET, SOCK_STREAM, 0); //need to test for return -1

  /* structure definition to connect to server*/
  hostptr = gethostbyname(servername);
  memset((void *)&serveraddr, (int)'\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
  serveraddr.sin_port = htons((u_short)port);

  while (1)
  {
    /* read arguments from commandline*/
    int numargs;
    numargs = readLineArguments(args, MAXARGS + 1, commandlinebuffer, BUFFER_MAX);

    /* if exit or no arguments received exit*/
    if (numargs < 0 ||
        (numargs > 0 && (strcmp(args[0], COMANDO_SAIR) == 0)))
      exit(EXIT_SUCCESS);

    /* commando List ignores if any other arguments are passed*/
    else if (strcmp(args[0], COMANDO_LIST) == 0)
    {
      if (connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
      {
        printf("error: %s\n", strerror(errno));
        continue;
      }

      tcpCommand(fd, "LST", NULL, NULL);

      bytesRead = 0;
      if ((bytesRead = read(fd, buffer, TCP_HEADER)) == -1)
      {
        printf("error: %s\n", strerror(errno));
        continue;
      }

      token = strtok(buffer, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_COMMAND_PROCTASK) != 0)
      {
        printf("list:Unexpected Reply Received");
        continue;
      }

      token = strtok(NULL, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_ARG_EOF) != 0)
      {
        printf("list: There are no servers currently availabe");
        continue;
      }
      else if (strcmp(token, TCP_ARG_ERR) != 0)
      {
        printf("list: The request is not currectly formulated");
        continue;
      }
      else
      {
        bytesToRead = atoi(token) * (FPT_SIZE + 1);
      }

      printf("Available File Processing Tasks:\n");

      while (bytesToRead > 0)
      {
        if ((bytesRead = read(fd, buffer, TCP_HEADER)) == -1)
        {
          printf("error: %s\n", strerror(errno));
          continue;
        }
        token = strtok(buffer, PROTOCOL_DIVIDER);
        i=1;
        while (token != NULL)
        {
          printf("%d/t%s/n/t%s",i, token,taskDescription(token));
          token = strtok(buffer, PROTOCOL_DIVIDER);
          i++;
        }
        bytesToRead -= bytesRead;
      }
    }

    /* commando Request*/
    else if (strcmp(args[0], COMANDO_REQUEST) == 0)
    {
      if (numargs < 3)
      {
        printf("%s: Invalid Syntax try again\n", COMANDO_REQUEST);
        continue;
      }

      if (connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
      {
        printf("error: %s\n", strerror(errno));
        continue;
      }
    }
    /* Outros commandos desconhecidos*/
    else
    {
      printf("Unknown Command try again.");
    }
  }
}
