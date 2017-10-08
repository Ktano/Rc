/*
// Header file with functions to create the strings
// to send through TCP
//
*/

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


int tcpCommand(int tcpfd, char* maincommand, char* argument, char* file)
{
    char buffer[128];
    struct stat buf;
    int fd2, size=0, bytesRead;

    if (file != NULL)
    {
        stat(file, &buf);
        size = buf.st_size;
        fd2 = open(file,O_RDONLY);
        if (fd2 == -1)
        {
            printf("ERRO: %s", strerror(errno));
            return -1;
        }
    }

    if (argument == NULL){
        snprintf(buffer,strlen(maincommand)+2, "%s\n", maincommand);
    	write(tcpfd, buffer, strlen(buffer));
    	return 0;
    }
    else if ( file==NULL ){
        snprintf(buffer,strlen(maincommand)+strlen(argument)+3, "%s %s\n", maincommand, argument);
    	write(tcpfd, buffer, strlen(buffer));
    	return 0;
    }
    else{
        sprintf(buffer/*,strlen(maincommand)+strlen(argument)+4*/, "%s %s %d ", maincommand, argument, size);
        write(tcpfd, buffer, strlen(buffer));
    }



    while (size>0){
            bytesRead=read(fd2,buffer,128);
            write(tcpfd,buffer,bytesRead);
            size-=bytesRead;
    }
    write(tcpfd, "\n", strlen("\n"));
    return 0;
}


/* creates a TCP connected to the provided servername on port
// returns -1 in case there is a problem creating the connection
*/
int TCPconnect(char* servername, int port){
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in serveraddr;

    if((fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
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

int TCPacceptint(int port){

  int fd, newfd;

  struct sockaddr_in serveraddr, clientaddr;
  socklen_t clientlen;

  fd = socket(AF_INET,SOCK_STREAM,0);

  memset( (void*)&serveraddr,(int) '\0', sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((u_short)port);

  bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

  listen(fd,5);

  clientlen = sizeof(clientaddr);
  newfd = accept( fd, (struct sockaddr*) &clientaddr, &clientlen);


  close(fd);
  return newfd;
}

void UDPconnect(int port){

    int fd, sourcefile;
    char* ipString;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;
    char buffer[5000], WScommand[4], writeonfile[50];
    int recConnect;
    sourcefile = open("file_processing_tasks.txt",O_WRONLY);
    if (sourcefile == -1)
    {
        printf("ERRO: %s", strerror(errno));
        return -1;
    }

    fd = socket(AF_INET,SOCK_DGRAM,0);

    memset((void*)&serveraddr,(int)'\0', sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((u_short)port);

    bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    addrlen = sizeof(clientaddr);
    recConnect = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &addrlen);

    read(buffer, WScommand, 4);

    if(strcmp(WScommand, "REG ")==0){
      if((recConnect == -1) && (sourcefile == -1)){
        sendto(fd, "RAK NOK\n", 9, 0, (struct sockaddr*)&clientaddr,addrlen);
      }else{
        strcat(writeonfile, "+ ");
        strcat(writeonfile, buffer);
        if(write( sourcefile, writeonfile, strlen(writeonfile)))
          sendto(fd, "RAK OK\n", 8, 0, (struct sockaddr*)&clientaddr,addrlen);
      }
    }
}

int filespliter(char *file) {

	FILE *sourcefile;
	FILE *partitionfile;

	char line[128], partition[9];
	int files=1, counter=1;
	int wctservers = 2;
	int ch=0, lines=1;

	sourcefile = fopen(file,"r");


	while(!feof(sourcefile)){
	  ch = fgetc(sourcefile);
	  if(ch == '\n')
	    lines++;
	}
	rewind(sourcefile);

	printf("LINES: %d\n", lines);

	sprintf(partition, "%s00%d.txt", sourcefile, files);
	partitionfile = fopen(partition, "w");

	while (fgets(line, sizeof line, sourcefile)!=NULL) {
		if ((lines % counter) == wctservers) {
			fclose(partitionfile);
			counter = 1;
			files++;
			sprintf(partition, "%s00%d.txt", sourcefile, files);
			partitionfile = fopen(partition, "w");
		}
		counter++;
		fprintf(partitionfile,"%s\n", line);
	}
	fclose(sourcefile);
	return 0;
}
