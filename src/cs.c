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


#define GROUP_PORT 58006
#define BUFFER_MAX 128

int main (int argc, char** argv){

    int fd, pid, bytesRead, working_servers, servers = 0;
    char buffer[BUFFER_MAX];
    int fd_wsservers[10], fd_position = 0;
    char requestedFPT[4], command[4];
    int filecounter = 1, fileBytes;
    char *token;
    FILE *fp;
    char filename = "0000", response;

    while (1)
    {

      UDPconnect(PORT);

      if(fd = TCPacceptint(GROUP_PORT)==-1)
        continue;

      if ((pid = fork()) == -1)
      {
        printf("ERROR: %s", strerror(errno));
        continue;
      }
      else if (pid == 0)
      {
        bytesRead = read(fd, buffer, BUFFER_MAX-1);
        buffer[bytesRead]='\0';

        token = strtok(buffer, PROTOCOL_DIVIDER);

        if(strcmp(token, TCP_COMMAND_LIST)){
          strcat(response, FTP);
          strcat(response, " ");
          strcat(response, servers);
          if(FTPcounter( filename, "WCT") > 0){
            servers++;
            strcat(response, " WCT");
          }if(FTPcounter( filename, "FLW") > 0){
            servers++;
            strcat(response, " FLW");
          }if(FTPcounter( filename, "UPP") > 0){
            servers++;
            strcat(response, " UPP");
          }if(FTPcounter( filename, "LOW") > 0){
            servers++;
            strcat(response, " LOW");
          }
          response[4] = servers;
          response[strlen(response)] = '\0';
          write(fd, response, strlen(response));

        }
        else if(strcmp(token, "REQ"))==0){

          token = strtok(NULL, PROTOCOL_DIVIDER);

          strcpy(requestedFPT, token);
          requestedFPT[3] = '\0';

          token = strtok(NULL, PROTOCOL_DIVIDER);

          bytesToRead = atoi(token);

          strcat(filename, filecounter);
          filecounter++;
          strcat(filename, ".txt");

          fp = fopen(filename, "wb");

          token = strtok(buffer, "");
          write(fp, token, strlen(token));
          bytesToRead -= strlen(token);

          while (bytesToRead > 0){

              bytesRead=read(fd,buffer,BUFFER_MAX-1);
              buffer[bytesRead]='\0';
              write(fp,buffer,bytesRead);
              bytesToRead -= bytesRead;
          }

          working_servers = FTPcounter( filename, requestedFPT);
          filesplitter( filename, working_servers);
          //read every line in processing tasks filename
          //find the ones that have the designed FPT
          //fetch the ip and port of that line
          //connect via TCPconnect using that ip and port
          //returning the fd and placing it in the array
          //for 0 to working_servers -1, read each file and send each to each fd
          //then expect the answer
          //close all fd's and exit
        }
        exit(EXIT_SUCCESS);
      }
      else
      {
        processes++;
        continue;
      }
    }







}
