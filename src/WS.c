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
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>

#define DEFAULT_UDP_PORT 58006
#define DEFAULT_TCP_PORT 59000
#define BUFFER_MAX 128
#define UDP_BUFFER_SIZE 512
#define MAX_PTC 99

int processes, udpPort, tcpPort;
char UDPmessage[UDP_BUFFER_SIZE], servername[BUFFER_MAX], udpReply[UDP_BUFFER_SIZE];

void apanhaSIG();
int readTCP(int fd);

int main(int argc, char **argv)
{
  int i = 0, ptctasks = 0, j = 0, pid;
  int tcpfd;
  char *PTC[MAX_PTC];

  udpPort = DEFAULT_UDP_PORT;
  tcpPort = DEFAULT_TCP_PORT;

  /*cria vector para tratamento do sinal SIGINT*/
  if (signal(SIGINT, apanhaSIG) == SIG_ERR)
  {
    perror("ERRO ao criar tratamento de sinal");
    exit(EXIT_SUCCESS);
  }

  /* sets the default value for the server name*/
  if (gethostname(servername, BUFFER_MAX) == -1)
  {
    printf("error getting local host name: %s\n", strerror(errno));
    exit(EXIT_SUCCESS);
  }

  /* reads commands from arguments*/
  for (i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0)
    {
      tcpPort = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "-n") == 0)
    {
      strcpy(servername, argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "-e") == 0)
    {
      udpPort = atoi(argv[i + 1]);
      i++;
    }
    else if (strlen(argv[i]) == 3)
    {
      PTC[j] = argv[i];
      ptctasks++;
    }
  }

  if (UDPCommand(UDPmessage, UDP_BUFFER_SIZE, "REG", PTC, ptctasks, tcpPort) == -1)
    exit(EXIT_SUCCESS);
  if (sendUDP(servername, udpPort, UDPmessage, udpReply, UDP_BUFFER_SIZE) == -1)
    exit(EXIT_SUCCESS);

  while (1)
  {
    if ((tcpfd = TCPacceptint(tcpPort)) == -1)
      continue;

    if ((pid = fork()) == -1)
    {
      printf("ERROR: %s", strerror(errno));
      continue;
    }
    else if (pid == 0)
    {
      readTCP(tcpfd);
      /*child*/
      close(tcpfd);
      exit(EXIT_SUCCESS);
    }
    else
    {
      processes++;
      continue;
    }
  }

  return 0;
}

/*funcao para tratamento do signal*/
void apanhaSIG()
{
  int i, status, pid;
  /*cria vector para tratamento do sinal SIGINT*/
  if (signal(SIGINT, apanhaSIG) == SIG_ERR)
  {
    perror("ERRO ao criar tratamento de sinal");
    exit(EXIT_FAILURE);
  }

  UDPCommand(UDPmessage, UDP_BUFFER_SIZE, "UNR", NULL, 0, tcpPort);
  sendUDP(servername, udpPort, UDPmessage, udpReply, UDP_BUFFER_SIZE);
  for (i = 0; i < processes; i++)
  {
    pid = wait(&status);
    /*error on wait*/
    if (pid < 0)
    {
      if (errno == EINTR)
      {
        i--;
        continue;
      }
      printf("ERROR: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}

int readTCP(int fd)
{
  char buffer[BUFFER_MAX];
  int bytesRead, bytesToRead;
  int fd2;
  char ptc[BUFFER_MAX], filename[BUFFER_MAX], *token;

  if ((bytesRead = read(fd, buffer, BUFFER_MAX - 1)) == -1)
  {
    printf("error: %s\n", strerror(errno));
    return -1;
  }
  buffer[bytesRead] = '\0';

  token = strtok(buffer, PROTOCOL_DIVIDER);
  if (strcmp(token, TCP_COMMAND_WORKREQUEST) != 0)
  {
    printf("request:Unexpected request Received\n");
    tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_ERR, NULL);
    return -1;
  }

  token = strtok(NULL, PROTOCOL_DIVIDER);
  strcpy(ptc, token);

  token = strtok(NULL, PROTOCOL_DIVIDER);
  strcpy(filename, token);

  /* creates the file with the name provided*/
  if ((fd2 = open(filename, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG)) == -1)
  {
    printf("ERROR: %s\n", strerror(errno));
    tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_ERR, NULL);
    return -1;
  }

  /* saves any extra data to the file*/
  token = strtok(NULL, PROTOCOL_DIVIDER);
  if ((bytesToRead = atoi(token)) == 0)
  {
    printf("ERROR: Unexpected request Received");
    return -1;
  }

  token = strtok(NULL, "");
  if (token != NULL)
  {
    write(fd2, token, strlen(token));
    bytesToRead -= strlen(token);
  }

  /* reads and saves file*/
  while (bytesToRead > 0)
  {
    if ((bytesRead = read(fd, buffer, BUFFER_MAX - 1)) == -1)
    {
      printf("error: %s\n", strerror(errno));
      close(fd2);
      return -1;
    }
    buffer[bytesRead] = '\0';

    if (bytesRead <= bytesToRead)
    {
      write(fd2, buffer, strlen(buffer));
      bytesToRead -= bytesRead;
    }
    else if (buffer[bytesRead - 1] == '\n')
    {
      write(fd2, buffer, strlen(buffer - 1));
      bytesToRead -= bytesRead;
    }
    else
    {
      printf("request: Error reply not correctly formulated\n");
      close(fd2);
      return -1;
    }
  }
  close(fd2);

  /* WORD COUNT */
  if (strcmp(ptc, PTC_WORDCOUNT) == 0)
  {
    int wordCount;
    FILE *tfd;
    if ((wordCount = fileWordCount(filename)) == -1)
    {
      tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_ERR, NULL);
      return -1;
    }
    tfd = fopen("tmp", "w");
    fprintf(tfd, "%d", wordCount);
    fclose(tfd);
  }
  /* Longest Word */
  else if (strcmp(ptc, PTC_LONGESTWORD) == 0)
  {
    char answer[BUFFER_MAX];
    FILE *tfd;
    if (fileLongestWord(filename, answer, BUFFER_MAX) == -1)
    {
      tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_ERR, NULL);
      return -1;
    }

    tfd = fopen("tmp", "w");
    fprintf(tfd, "%s", answer);
    fclose(tfd);
  }
  /*chage all chars*/
  else if (strcmp(ptc, PTC_UPPER) == 0 || strcmp(ptc, PTC_LOWER) == 0)
  {
    if (changeAllChars(filename, "tmp", strcmp(ptc, PTC_UPPER) == 0 ? toupper : tolower) == -1)
    {
      tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_ERR, NULL);
      return -1;
    }
  }
  else
  {
    tcpCommand(fd, TCP_COMMAND_WORKREQUEST, TCP_ARG_EOF, NULL);
    return -1;
  }

  /* final reply and delete data*/
  tcpCommand(fd, TCP_COMMAND_REPLY, TCP_REPLY_REPORT, "tmp");
  remove("tmp");
  return 0;
}