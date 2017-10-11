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

#define GROUP_PORT 58006
#define BUFFER_MAX 128

int main (int argc, char** argv){

    int fd, pid, tcp_pid, bytesRead, working_servers, servers = 0, fp;
    char buffer[BUFFER_MAX];
    int fd_wsservers[10], fd_position = 0;
    char requestedFPT[4], filename[10], fileToSplit[5];
    int  bytesToRead, filecounter = 1;
    char *token;
    char* response = "";

    if ((pid = fork()) == -1)
    {
        printf("ERROR: %s", strerror(errno));

    }else if (pid == 0){

      if((fd = TCPacceptint(GROUP_PORT))==-1){}

      if ((tcp_pid = fork()) == -1){
            printf("ERROR: %s", strerror(errno));

      }else if(tcp_pid == 0){

        while(1){
          bytesRead = read(fd, buffer, BUFFER_MAX-1);
          buffer[bytesRead]='\0';

          token = strtok(buffer, PROTOCOL_DIVIDER);

          if(strcmp(token, TCP_COMMAND_LIST)){
            strcat(response, "FTP");
            strcat(response, "  ");
            if((servers = FTPcounter( "file_processing_tasks.txt", "WCT")) > 0){
              servers++;
              strcat(response, " WCT");
            }if((servers = FTPcounter( "file_processing_tasks.txt", "FLW")) > 0){
              servers++;
              strcat(response, " FLW");
            }if((servers = FTPcounter( "file_processing_tasks.txt", "UPP")) > 0){
              servers++;
              strcat(response, " UPP");
            }if((servers = FTPcounter( "file_processing_tasks.txt", "LOW")) > 0){
              servers++;
              strcat(response, " LOW");
            }

            response[4] = servers;
            response[strlen(response)] = '\0';
            write(fd, response, strlen(response));

          }else if(strcmp(token, "REQ")==0){

            token = strtok(NULL, PROTOCOL_DIVIDER);

            strcpy(requestedFPT, token);
            requestedFPT[3] = '\0';

            token = strtok(NULL, PROTOCOL_DIVIDER);

            bytesToRead = atoi(token);

            snprintf(filename, 5, "%d", getpid());
            strcat(filename, ".txt");

            fp = open(filename,(O_CREAT | O_WRONLY));

            token = strtok(buffer, "");
            write(fp, token, strlen(token));
            bytesToRead -= strlen(token);

            while (bytesToRead > 0){

                bytesRead=read(fd,buffer,BUFFER_MAX-1);
                buffer[bytesRead]='\0';
                write(fp,buffer,bytesRead);
                bytesToRead -= bytesRead;
            }

            working_servers = FTPcounter( "file_processing_tasks.txt", requestedFPT);
            filesplitter( filename, working_servers, getpid());
            fd_wsservers = connectToWS( filename, requestedFPT);

              //for 0 to working_servers -1, read each file and send each to each fd
              //then expect the answer
              //close all fd's and exit
          }
        }
        exit(EXIT_SUCCESS);
      }else{
        if((fd = TCPacceptint(GROUP_PORT))==-1){}

      }

    exit(EXIT_SUCCESS);
    }
    else{
      UDPconnect(GROUP_PORT);
    }
    return 0;

}
