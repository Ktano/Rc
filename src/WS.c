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

#define DEFAULT_UDP_PORT 58006
#define DEFAULT_TCP_PORT 59000
#define BUFFER_MAX 128
#define UDP_BUFFER_SIZE 512
#define MAX_PTC 99

int main(int argc, char **argv)
{
  int udpPort = DEFAULT_UDP_PORT, tcpPort = DEFAULT_TCP_PORT, i=0,ptctasks=0,j=0;
  int bytestoWrite, bytesRead;
  char *PTC[MAX_PTC], servername[BUFFER_MAX], UDPmessage[UDP_BUFFER_SIZE],udpReply[UDP_BUFFER_SIZE];


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
    else if (strcmp(argv[i], "-e"))
    {
      udpPort = atoi(argv[i + 1]);
      i++;
    }
    else if(strlen(argv[i])==3){
      PTC[j]=argv[i];
      ptctasks++;
    }
  }

  bytestoWrite=UDPCommand(UDPmessage,UDP_BUFFER_SIZE,"REG",PTC,MAX_PTC,udpPort);
  bytesRead=sendUDP(servername,udpPort,UDPmessage,udpReply,UDP_BUFFER_SIZE);



  bytestoWrite=UDPCommand(UDPmessage,UDP_BUFFER_SIZE,"UNR",NULL,0,udpPort);
  bytesRead=sendUDP(servername,udpPort,UDPmessage,udpReply,UDP_BUFFER_SIZE);
  return 0;
}



