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

#define DEFAULT_UDP_PORT 58006
#define DEFAULT_TCP_PORT 59000
#define BUFFER_MAX 128
#define UDP_BUFFER_SIZE 512
#define MAX_PTC 99

int processes, udpPort, tcpPort;
char UDPmessage[UDP_BUFFER_SIZE], servername[BUFFER_MAX], udpReply[UDP_BUFFER_SIZE];

void apanhaSIG();

int main(int argc, char **argv)
{
  int i = 0, ptctasks = 0, j = 0, pid;
  int bytestoWrite, bytesRead;
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
  if (sendUDP(servername, udpPort, UDPmessage, udpReply, UDP_BUFFER_SIZE)) == -1)
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
      /*child*/

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