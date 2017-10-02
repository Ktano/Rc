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



int main (int argc, char** argv){

    int fd, wctservers, flwservers, uppservers, lowservers;  
    char servername[BUFFER_MAX], buffer[BUFFER_MAX];

    while(1){

      if(fd = TCPaccept(GROUP_PORT)==-1)
        continue;


      if(read(fd, buffer,strlen(buffer))==TCP_COMMAND_LIST){

      }
      elseif(read(fd, buffer,strlen(buffer))==){

      }
    }

}
