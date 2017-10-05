/*
// Header file with functions to create the strings
// to send through TCP
//
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int tcpCommand(int tcpfd, char *maincommand, char *argument, char *file)
{
  char buffer[128];
  struct stat buf;
  int fd2, size = 0, bytesRead;

  if (file != NULL)
  {
    stat(file, &buf);
    size = buf.st_size;
    fd2 = open(file, O_RDONLY);
    if (fd2 == -1)
    {
      printf("ERRO: %s\n", strerror(errno));
      return -1;
    }
  }

  if (argument == NULL)
  {
    snprintf(buffer, strlen(maincommand) + 2, "%s\n", maincommand);
    write(tcpfd, buffer, strlen(buffer));
    return 0;
  }
  else if (file == NULL)
  {
    snprintf(buffer, strlen(maincommand) + strlen(argument) + 3, "%s %s\n", maincommand, argument);
    write(tcpfd, buffer, strlen(buffer));
    return 0;
  }
  else
  {
    sprintf(buffer /*,strlen(maincommand)+strlen(argument)+4*/, "%s %s %d ", maincommand, argument, size);
    write(tcpfd, buffer, strlen(buffer));
  }

  while (size > 0)
  {
    bytesRead = read(fd2, buffer, 128);
    write(tcpfd, buffer, bytesRead);
    size -= bytesRead;
  }
  write(tcpfd, "\n", strlen("\n"));
  return 0;
}

/* creates a TCP connected to the provided servername on port
// returns -1 in case there is a problem creating the connection
*/
int TCPconnect(char *servername, int port)
{
  int fd;
  struct hostent *hostptr;
  struct sockaddr_in serveraddr;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("error: %s\n", strerror(errno));
    return -1;
  }

  /* structure definition to connect to server*/
  if ((hostptr = gethostbyname(servername)) == NULL)
  {
    printf("error: %s\n", strerror(errno));
    return -1;
  };
  memset((void *)&serveraddr, (int)'\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
  serveraddr.sin_port = htons((u_short)port);

  if (connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
  {
    printf("error: %s\n", strerror(errno));
    close(fd);
    return -1;
  }
  return fd;
}
