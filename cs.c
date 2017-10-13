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

int main(int argc, char **argv)
{

  int listenfd,connfd = 0, pid, tcp_pid, bytesRead, working_servers, servers = 0, fp;
  char buffer[BUFFER_MAX], FLWbuffer[BUFFER_MAX];
  int fd_wsservers[10], fd_position = 0;
  char requestedFPT[4], filename[BUFFER_MAX], fileToWs[BUFFER_MAX], report[BUFFER_MAX];
  int bytesToRead, fileToWsCounter = 1, i;
  char *token;
  char response[BUFFER_MAX], tasks[BUFFER_MAX];
  char from_ws[BUFFER_MAX];
  int wordcount_results[10], wordcount_merged = 0;
  char *longestword_results[10];
  struct sockaddr_in clientaddr;



  if ((pid = fork()) == -1)
  {
    printf("ERROR: %s\n", strerror(errno));
  }
  else if (pid != 0)
  {
    /*waits for a command from a user*/
    if ((listenfd = TCPlisten(GROUP_PORT)) == -1)
      printf ("ERROR: %s",strerror(errno));

    while (1)
    {

      if ((connfd = TCPacceptint(listenfd,clientaddr)) == -1)
      {
        printf ("ERROR: %s",strerror(errno));
        continue;
      }

      /* forks*/
      if ((tcp_pid = fork()) == -1)
      {
        close(connfd);
        close(listenfd);
        printf("ERROR: %s\n", strerror(errno));
        continue;
      }

      /* if is child reads and processes the command*/
      else if (tcp_pid == 0)
      {
        close(listenfd); /*child closes listenfd)*/
        bytesRead = read(connfd, buffer, BUFFER_MAX - 1);
        buffer[bytesRead] = '\0';



        tasks[0]='\0';
        token = strtok(buffer, " \n");

        if (strcmp(token, TCP_COMMAND_LIST)==0)
        {

          /*printf("List request: %s %s\n", host, port);*/

          if ( FTPcounter("file_processing_tasks.txt", PTC_WORDCOUNT) > 0)
          {
            servers++;
            strcat(tasks, " WCT");

          }
          if ( FTPcounter("file_processing_tasks.txt", PTC_LONGESTWORD) > 0)
          {
            servers++;
            strcat(tasks, " FLW");

          }
          if ( FTPcounter("file_processing_tasks.txt", PTC_UPPER) > 0)
          {
            servers++;
            strcat(tasks, " UPP");

          }
          if (FTPcounter("file_processing_tasks.txt", PTC_LOWER) > 0)
          {
            servers++;
            strcat(tasks, " LOW");

          }

          if(servers==0)
            sprintf(response, "FPT EOF\n");
          else
            sprintf(response, "FPT %d%s\n",servers,tasks);

          write(connfd, response, strlen(response));

        }

        else if (strcmp(token, "REQ") == 0)
        {

          token = strtok(NULL, PROTOCOL_DIVIDER);

          strcpy(requestedFPT, token);
          requestedFPT[3] = '\0';

          token = strtok(NULL, PROTOCOL_DIVIDER);

          bytesToRead = atoi(token);

          sprintf(filename, "input_files/%05d.txt", getpid());

          fp = open(filename, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG);

          token = strtok(buffer, "");
          write(fp, token, strlen(token));
          bytesToRead -= strlen(token);

          while (bytesToRead > 0)
          {

            bytesRead = read(connfd, buffer, BUFFER_MAX - 1);
            buffer[bytesRead] = '\0';
            write(fp, buffer, bytesRead);
            bytesToRead -= bytesRead;
          }

          close(fp);

          working_servers = FTPcounter("file_processing_tasks.txt", requestedFPT);
          filesplitter(filename, working_servers, getpid());
          fd_position = connectToWS(filename, requestedFPT, fd_wsservers, working_servers);
          if(working_servers==0){
            write(connfd,"REP EOF\n",8);
          }
          for (i = 0; i < fd_position; i++)
          {

            sprintf(fileToWs, "input_files/%05d%03d.txt", getpid(), fileToWsCounter);
            fileToWsCounter++;

            tcpCommand(fd_wsservers[i], "WRQ", requestedFPT, fileToWs, 1);
          }

          for (i = 0; i < fd_position; i++)
          {

            bytesRead = read(fd_wsservers[i], from_ws, BUFFER_MAX - 1);

            token = strtok(from_ws, PROTOCOL_DIVIDER);

            if (strcmp(token, "REP"))
            {

              token = strtok(NULL, PROTOCOL_DIVIDER);

              if (strcmp(token, "R"))
              {
                if (strcmp(requestedFPT, PTC_WORDCOUNT))
                {

                  token = strtok(NULL, PROTOCOL_DIVIDER);
                  bytesToRead = atoi(token);

                  token = strtok(NULL, PROTOCOL_DIVIDER);
                  wordcount_results[i] = atoi(token);
                }
                else if (strcmp(requestedFPT, PTC_LONGESTWORD))
                {
                  token = strtok(NULL, PROTOCOL_DIVIDER);
                  bytesToRead = atoi(token);

                  read(longestword_results, from_ws, bytesToRead);
                }
              }
              if (strcmp(token, "F"))
              {
                if (strcmp(requestedFPT, PTC_UPPER))
                {
                  token = strtok(NULL, PROTOCOL_DIVIDER);
                  bytesToRead = atoi(token);
                }
                else if (strcmp(requestedFPT, PTC_LOWER))
                {
                  token = strtok(NULL, PROTOCOL_DIVIDER);
                  bytesToRead = atoi(token);
                }
              }
            }
          }

          if (strcmp(requestedFPT, PTC_WORDCOUNT))
          {
            wordcount_merged = agregateWordCount(wordcount_results, fd_position);
            snprintf(report, 10 + 2*sizeof(wordcount_merged),"REP R %d %d", sizeof(wordcount_merged), wourdcount_merged);
            write(connfd, report, BUFFER_MAX);
          }
          else if (strcmp(requestedFPT, PTC_LONGESTWORD))
          {
            flw_total = agregateLongestWord( longestword_results, fd_position, FLWbuffer, BUFFER_MAX);
            snprintf(report, 10 + flw_total + sizeof(buffer),"REP R %d %s", flw_total, buffer);

          }
          else if (strcmp(requestedFPT, PTC_UPPER))
          {

          }
          else if (strcmp(requestedFPT, PTC_LOWER))
          {

          }


          for (i = 0; i < fd_position; i++)
          {
            close(fd_wsservers[i]);
          }
        }

        close(connfd);
        exit(EXIT_SUCCESS);
      }
      /* if is parent close the file descriptor*/
      else
        close(connfd);
    }
    exit(EXIT_SUCCESS);
  }
  else
  {
    close(connfd);
    while (1)
    {
      UDPconnect(GROUP_PORT);
    }
  }

  return 0;
}
