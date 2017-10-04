#include "commandlinereader.h"
#include "tcpcommandwriter.h"
#include "fpt.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

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
  int fd; /*used for TCP comunication*/
  int fd2; /*used to write to file*/
  char servername[BUFFER_MAX], buffer[BUFFER_MAX];
  char commandlinebuffer[BUFFER_MAX];
  char *args[MAXARGS + 1];
  char *token, tmp[BUFFER_MAX];

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
  }1

  while (1)
  {
    printf(">> ");
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

      if ((fd = TCPconnect(servername, port)) == -1)
        continue;

      if(tcpCommand(fd, "LST", NULL, NULL)==-1)
        continue;

      bytesRead = 0;
      if ((bytesRead = read(fd, buffer, TCP_HEADER)) == -1)
      {
        printf("error: %s\n", strerror(errno));
        close(fd);
        continue;
      }

      buffer[bytesRead]='\0';

      token = strtok(buffer, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_COMMAND_PROCTASK) != 0)
      {
        printf("list:Unexpected Reply Received\n");
        close(fd);
        continue;
      }

      token = strtok(NULL, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_ARG_EOF) == 0)
      {
        printf("list: There are no servers currently availabe\n");
        close(fd);
        continue;
      }
      else if (strcmp(token, TCP_ARG_ERR) == 0)
      {
        printf("list: The request is not currectly formulated\n");
        close(fd);
        continue;
      }
      else
      {
        bytesToRead = atoi(token) * (FPT_SIZE + 1);
      }

      printf("Available File Processing Tasks:\n");

      token = strtok(NULL, PROTOCOL_DIVIDER);
      tmp[0] = '\0';

      while (bytesToRead > 0)
      {
        if (token != NULL)
        {
          strcpy(tmp, token);
          bytesToRead -= strlen(token);
        }

        if ((bytesRead = read(fd, buffer, BUFFER_MAX-1)) == -1)
        {
          printf("error: %s\n", strerror(errno));
          close(fd);
          continue;
        }
        buffer[bytesRead]='\0'; /*terminates the buffer*/

        token = strtok(buffer, PROTOCOL_DIVIDER);
        i = 1;
        while (token != NULL)
        {
          if (strlen(token) < FPT_SIZE)
            strcat(tmp, token);
          else
            strcpy(tmp, token);
          if (strlen(tmp) == FPT_SIZE)
          {
            printf("%d\t%s\t%s\n", i, tmp, taskDescription(tmp));
            tmp[0] = '\0';
          }
          token = strtok(NULL, PROTOCOL_DIVIDER);
          i++;
        }
        bytesToRead -= bytesRead;
      }
      close(fd);
    }

    /* commando Request*/
    else if (strcmp(args[0], COMANDO_REQUEST) == 0)
    {
      if (numargs < 3)
      {
        printf("%s: Invalid Syntax try again\n", COMANDO_REQUEST);
        continue;
      }

      if ((fd = TCPconnect(servername, port)) == -1)
        continue;

      if(tcpCommand(fd, "REQ", args[1], args[2])==-1)
        continue;
      
      if ((bytesRead = read(fd, buffer, BUFFER_MAX-1)) == -1)
      {
        printf("error: %s\n", strerror(errno));
        close(fd);
        continue;
      }
      buffer[bytesRead]='\0';

      token = strtok(buffer, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_COMMAND_REPLY) != 0)
      {
        printf("list:Unexpected Reply Received\n");
        close(fd);
        continue;
      }

      token = strtok(NULL, PROTOCOL_DIVIDER);
      if (strcmp(token, TCP_ARG_EOF) == 0)
      {
        printf("request: It is not possible to answer the request. Potential invalid Processing task code\n");
        close(fd);
        continue;
      }
      else if (strcmp(token, TCP_ARG_ERR) == 0)
      {
        printf("request: The request is not currectly formulated\n");
        close(fd);
        continue;
      }
      else
      {
        strcpy(tmp, args[2]);
        strcat(tmp,"-");
        strcat(tmp,args[1]);
        strcat(tmp,"-");
        strcat(tmp,token);
      }
      token = strtok(NULL, PROTOCOL_DIVIDER);
      bytesToRead = atoi(token);

      fd2=open(tmp,O_CREAT|O_WRONLY,S_IRWXU|S_IRWXG);
      token = strtok(NULL, "");
      if(token!=NULL){
        write(fd2,token,strlen(token));
        bytesToRead-=strlen(token);
      }

      while(bytesToRead>0){
        if ((bytesRead = read(fd, buffer, BUFFER_MAX-1)) == -1)
        {
          printf("error: %s\n", strerror(errno));
          close(fd);
          continue;
        }
        buffer[bytesRead]='\0';

        if (bytesRead<=bytesToRead ){
          write(fd2,buffer,strlen(buffer));
          bytesToRead-=bytesRead;
        }
        else if (buffer[bytesRead-1]=='\n'){
          write(fd2,buffer,strlen(buffer-1));
          bytesToRead-=bytesRead;
        }
        else{
          printf("request: Error reply not correctly formulated\n");
          close(fd2);
          close(fd);
          remove(tmp);
          continue;
        }
      }
      close(fd);
      close(fd2);
    }
    /* Outros commandos desconhecidos*/
    else
    {
      printf("Unknown Command try again.\n");
    }
  }
}
