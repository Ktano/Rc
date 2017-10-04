#include "commandlinereader.h"
#include "tcpcommandwriter.h"
#include "fpt.h"
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define GROUP_PORT 58006
#define BUFFER_MAX 128

int main (int argc, char** argv){

    int fd, wctservers, flwservers, uppservers, lowservers;
    char buffer[BUFFER_MAX];
    char requestedFPT[4], command[4];
    int filecounter = 1, bytesToRead;
    FILE *fp;
    char filename = "0000";

    UDPconnect(PORT);

    while(1){

      if(fd = TCPaccept(GROUP_PORT)==-1)
        continue;

      read(fd, command, 4);
      if(strcmp( command, TCP_COMMAND_LIST)){

      }
      else if(strcmp(read(fd, buffer, 4), "REQ ")==0){

        strcpy(requestedFPT, read(fd, buffer, 4));
        requestedFPT[3] = '\0';

        fscanf(fd ,"%d ", &bytesToRead);

        strcat(filename, filecounter);
        filecounter++;
        strcat(filename, ".txt");
        
        fp = fopen(filename, "wb");

        read( fd, filename, bytesToRead);
      }
    }

}
