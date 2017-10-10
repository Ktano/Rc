/*
// Header file with functions to create the strings
// to send through TCP
//
*/
#include "tcpcommandwriter.h"
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
#include <arpa/inet.h>

#define BUFFER_MAX 128

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
      printf("ERRO: %s", strerror(errno));
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
  hostptr = gethostbyname(servername);
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

int TCPacceptint(int port)
{

  int fd, newfd;

  struct sockaddr_in serveraddr, clientaddr;
  socklen_t clientlen;

  fd = socket(AF_INET, SOCK_STREAM, 0);

  memset((void *)&serveraddr, (int)'\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((u_short)port);

  bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

  listen(fd, 5);

  clientlen = sizeof(clientaddr);
  newfd = accept(fd, (struct sockaddr *)&clientaddr, &clientlen);

  close(fd);
  return newfd;
}

int UDPconnect(int port)
{

  int fd, sourcefile, bytesRead;
  char *token;
  struct sockaddr_in serveraddr, clientaddr;
  socklen_t addrlen;
  char buffer[5000], writeonfile[50], writeonscreen[50];

  sourcefile = open("file_processing_tasks.txt", O_WRONLY);
  if (sourcefile == -1)
  {
    printf("ERRO: %s", strerror(errno));
    return -1;
  }

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  memset((void *)&serveraddr, (int)'\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((u_short)port);

  bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

  addrlen = sizeof(clientaddr);

  bytesRead = 0;
  bytesRead = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientaddr, &addrlen);

  buffer[bytesRead] = '\0';

  token = strtok(buffer, PROTOCOL_DIVIDER);

  if (strcmp(token, "REG") == 0)
  {
    if (bytesRead == -1)
    {
      sendto(fd, "RAK ERR\n", 9, 0, (struct sockaddr *)&clientaddr, addrlen);
    }
    else
    {

      token = strtok(buffer, "");
      strcat(writeonscreen, "+ ");
      strcat(writeonscreen, token);

      if (write(sourcefile, writeonfile, strlen(writeonfile)) != -1)
      {
        printf("%s\n", writeonscreen);
        sendto(fd, "RAK OK\n", 8, 0, (struct sockaddr *)&clientaddr, addrlen);
      }
      else
        sendto(fd, "RAK NOK\n", 9, 0, (struct sockaddr *)&clientaddr, addrlen);
    }
  } /*else if (strcmp(token, "UNR")==0){

    }*/
  return 0;
}

int UDPCommand(char *buffer, int bufferlen, char *maincommand, char **PTC, int lenghtPTC, int port)
{
  int total = 0, i;
  struct hostent *h;
  struct in_addr *a;
  char servername[BUFFER_MAX], *addr, cport[BUFFER_MAX];

  strncpy(buffer, maincommand, BUFFER_MAX);
  total = strlen(buffer);
  if (PTC != NULL)
  {
    for (i = 0; i < lenghtPTC; i++)
    {
      if ((total += strlen(PTC[i]) + 1) < BUFFER_MAX)
      {
        strcat(buffer, " ");
        strcat(buffer, PTC[i]);
      }
      else
      {
        printf("ERROR: buffer size is too small");
        return -1;
      }
    }
  }

  if (port != 0)
  {
    if (gethostname(servername, BUFFER_MAX) == -1)
    {
      printf("error getting local host name: %s\n", strerror(errno));
      return -1;
    }
    if ((h = gethostbyname(servername)) == NULL)
    {
      printf("error getting host: %s\n", strerror(errno));
      return -1;
    }

    a = (struct in_addr *)h->h_addr_list[0];
    addr = inet_ntoa(*a);
    sprintf(cport, "%d", port);
    if ((total += strlen(addr) + strlen(cport) + 2) < BUFFER_MAX)
    {
      strcat(buffer, " ");
      strcat(buffer, addr);
      strcat(buffer, " ");
      strcat(buffer, cport);
    }
    else
    {
      printf("ERROR: buffer size is too small");
      return -1;
    }
  }

  if ((total += 2) < BUFFER_MAX)
    strcat(buffer, "\n");
  else
  {
    printf("ERROR: buffer size is too small");
    return -1;
  }
  return total;
}

int sendUDP(char *servername, int UDPport, char *msg, char *reply, int size)
{
  int bytestoWrite, fd, addrlen, n;
  struct hostent *hostptr;
  struct sockaddr_in serveraddr;

  bytestoWrite = strlen(msg);

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }
  if ((hostptr = gethostbyname(servername)) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }
  memset((void *)&serveraddr, (int)'\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
  serveraddr.sin_port = htons((u_short)UDPport);
  addrlen = sizeof(serveraddr);

  sendto(fd, msg, bytestoWrite, 0, (struct sockaddr *)&serveraddr, (socklen_t)addrlen);

  addrlen = sizeof(serveraddr);
  n = recvfrom(fd, reply, size, 0, (struct sockaddr *)&serveraddr, (socklen_t *)&addrlen);
  reply[n] = '\0';
  close(fd);
  return n;
}

int filesplitter(char *file, int servers, int filecounter)
{

  FILE *sourcefile;
  FILE *partitionfile;

  char line[128], partition[9];
  int files = 1, counter = 1;

  int ch = 0, lines = 1;

  sourcefile = fopen(file, "r");

  while (!feof(sourcefile))
  {
    ch = fgetc(sourcefile);
    if (ch == '\n')
      lines++;
  }
  rewind(sourcefile);

  printf("LINES: %d\n", lines);

  sprintf(partition, "%d%02d.txt", filecounter, files);
  partitionfile = fopen(partition, "w");

  while (fgets(line, sizeof line, sourcefile) != NULL)
  {
    if ((lines % counter) == servers)
    {
      fclose(partitionfile);
      counter = 1;
      files++;
      sprintf(partition, "%d%02d.txt", filecounter, files);
      partitionfile = fopen(partition, "w");
    }
    counter++;
    fprintf(partitionfile, "%s\n", line);
  }
  fclose(sourcefile);
  return 0;
}

int FTPcounter(char *filename, char *ftp)
{
  FILE *fp;
  int wordcounter = 0, ch, ftplen;

  ftplen = strlen(ftp);

  if (NULL == (fp = fopen(filename, "r")))
    return -1;

  for (;;)
  {
    int i;
    if (EOF == (ch = fgetc(fp)))
      break;

    if ((char)ch != *ftp)
      continue;

    for (i = 1; i < ftplen; ++i)
    {
      if (EOF == (ch = fgetc(fp)))
      {
        goto end;
      }
      if ((char)ch != ftp[i])
      {
        fseek(fp, 1 - i, SEEK_CUR);
        goto next;
      }
    }
    ++wordcounter;
  next:;
  }
end:
  fclose(fp);
  return wordcounter;
}

int connectToWS(char *filename, char *requestedFPT, int *fd_wsservers, int max_servers)
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  char *token, ip[15];
  char search_FTP[5];
  search_FTP[0] = '+';
  int fd_position = 0, port;

  strcat(search_FTP, requestedFPT);

  fp = fopen(filename, "r");

  if (fp == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) != -1)
  {

    token = strtok(line, PROTOCOL_DIVIDER);

    if (strcmp(token, search_FTP) == 0)
    {
      token = strtok(NULL, PROTOCOL_DIVIDER);
      strcpy(ip,token);
      token = strtok(NULL, PROTOCOL_DIVIDER);
      port =atoi(token);
      fd_wsservers[fd_position] = TCPconnect(ip, port);
      fd_position++;
    }
  }

  fclose(fp);
  return fd_position+1;
}
