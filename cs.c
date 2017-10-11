#include "tcpcommandwriter.h"
#include "fpt.h"
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
    char requestedFPT[4], filename[10], *fileToWs;
    int  bytesToRead, fileToWsCounter = 1, i;
    char *token;
    char* response = "";
    char from_ws[BUFFER_MAX];
    int wordcount_results[10];
    char *longestword_results[10];
    struct sockaddr clientaddr;
    socklen_t addr_size;
    char host[6], port[16];
    char input_directory[14] = "output_files/";
    char directory[30];

    if ((pid = fork()) == -1)
    {
        printf("ERROR: %s", strerror(errno));

    }else if (pid == 0){

        while(1){

          if((fd = TCPacceptint(GROUP_PORT)) == -1)
            continue;

          if ((tcp_pid = fork()) == -1){
                printf("ERROR: %s", strerror(errno));
                continue;
          }
          else if(tcp_pid == 0){

            bytesRead = read(fd, buffer, BUFFER_MAX-1);
            buffer[bytesRead]='\0';

            addr_size = sizeof(struct sockaddr_in);
            getsockname(fd, &clientaddr, &addr_size);
            getnameinfo((struct sockaddr *)&clientaddr, addr_size, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);

            token = strtok(buffer, PROTOCOL_DIVIDER);

            if(strcmp(token, TCP_COMMAND_LIST)){

              printf("List request: %s %s\n", host, port);
              strcat(response, "FTP");

              if((servers = FTPcounter( "file_processing_tasks.txt", PTC_WORDCOUNT)) > 0){
                servers++;
                LSTcommand("file_processing_tasks.txt", PTC_WORDCOUNT);
                strcat(response, " WCT");
              }if((servers = FTPcounter( "file_processing_tasks.txt", PTC_LONGESTWORD)) > 0){
                servers++;
                LSTcommand("file_processing_tasks.txt", PTC_LONGESTWORD);
                strcat(response, " FLW");
              }if((servers = FTPcounter( "file_processing_tasks.txt", PTC_UPPER)) > 0){
                servers++;
                LSTcommand("file_processing_tasks.txt", PTC_UPPER);
                strcat(response, " UPP");
              }if((servers = FTPcounter( "file_processing_tasks.txt", PTC_LOWER)) > 0){
                servers++;
                LSTcommand("file_processing_tasks.txt", PTC_LOWER);
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
              strcpy(directory, input_directory);
              strcat(directory, filename);

              fp = open(directory,(O_CREAT | O_WRONLY));

              token = strtok(buffer, "");
              write(fp, token, strlen(token));
              bytesToRead -= strlen(token);

              while (bytesToRead > 0){

                  bytesRead=read(fd,buffer,BUFFER_MAX-1);
                  buffer[bytesRead]='\0';
                  write(fp,buffer,bytesRead);
                  bytesToRead -= bytesRead;
              }

              close(fp);

              working_servers = FTPcounter( "file_processing_tasks.txt", requestedFPT);
              filesplitter( directory, working_servers, getpid());
              fd_position = connectToWS( "file_processing_tasks.txt", requestedFPT, fd_wsservers, working_servers);

              for(i = 0; i < fd_position; i++){

                snprintf(fileToWs, 12, "%d%02d", getpid(), fileToWsCounter);
                strcat(fileToWs, ".txt");
                fileToWsCounter++;

                tcpCommand(fd_wsservers[i], "WRQ", requestedFPT, fileToWs, 1);
              }

              for(i = 0; i < fd_position; i++){

                bytesRead = read(fd_wsservers[i], from_ws, BUFFER_MAX-1);

                token = strtok(from_ws, PROTOCOL_DIVIDER);

                if(strcmp(token,"REP")){

                  token = strtok(NULL, PROTOCOL_DIVIDER);

                  if(strcmp(token,"R")){
                    if(strcmp(requestedFPT, PTC_WORDCOUNT)){

                      token = strtok(NULL, PROTOCOL_DIVIDER);
                      bytesToRead = atoi(token);

                      token = strtok(NULL, PROTOCOL_DIVIDER);
                      wordcount_results[i] = atoi(token);

                    }
                    else if(strcmp(requestedFPT, PTC_LONGESTWORD))
                    {
                      token = strtok(NULL, PROTOCOL_DIVIDER);
                      bytesToRead = atoi(token);
                    }
                  }
                  if(strcmp(token,"F")){
                    if(strcmp(requestedFPT, PTC_UPPER))
                    {
                      token = strtok(NULL, PROTOCOL_DIVIDER);
                      bytesToRead = atoi(token);
                    }
                    else if(strcmp(requestedFPT, PTC_LOWER))
                    {
                      token = strtok(NULL, PROTOCOL_DIVIDER);
                      bytesToRead = atoi(token);
                    }
                  }
                }

              }

              for(i = 0; i < fd_position; i++){
                close(fd_wsservers[i]);
              }
            }

        exit(EXIT_SUCCESS);
      }else
        close(fd);
    }
    exit(EXIT_SUCCESS);
    }else{
      while(1){
        UDPconnect(GROUP_PORT);
      }
    }

    return 0;

}
