#include "commandlinereader.h"
#include "commandlinereader.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



#define PORT 58000
#define MESSAGE_MAX 99


#define COMM_REQUEST "WRQ"
#define PTC_WCT "WCT"
#define PTC_FLW "FLW"
#define PTC_UPP "UPP"
#define PTC_LOW "LOW"

#define MAXARGS 55 /*muda-se*/
#define BUFFER_SIZE 100

int fd, newfd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int clientlen;




int main (int argc, char** argv) {

	/*max args in a command*/
	char *args[MAXARGS + 1]; 
    /*whole buffer size*/
	char buffer[BUFFER_SIZE];

	char clt_message[MESSAGE_MAX];
	char srv_message[MESSAGE_MAX];

	/*ws loop*/
	while (1) {


		fd = socket(AF_INET,SOCK_STREAM,0);

			/*structure of addr*/
		memset((void*)&serveraddr,(int)'\0',sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons((u_short)PORT);

/*  note: The backlog argument defines the maximum length to which the  queue  of  pending  connections  for  sockfd  may  grow. */

		/*binds and listens with a queue  of  pending 5 connections */
		bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		listen(fd,5);
		clientlen = sizeof(clientaddr);

		/*new fd for connection already made*/
		newfd = accept(fd,(struct sockaddr*)&clientaddr,&clientlen);
		
		/*get message*/
		read(newfd, clt_message, strlen(clt_message)+1);
		/*print received message*/
	 	printf("%s\n",clt_message ); 

	 	/*prepare new message*/
		strcpy(srv_message,"<a message?>");
		/*sends*/
	 	write(newfd , srv_message , strlen(srv_message)+1); //resp

		/*todo*/
	 	/*apply readLineArguments(args, MAXARGS+1, msg, BUFFER_SIZE);*/


	 	/*close fds*/
	 	close(fd); close(newfd);
	 	return 0;
     
		


	 	int numb_of_args;
	 	numb_of_args = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);


	 	if (numb_of_args < 5) {
                /*bad syntax! sends udp error*/
	 		continue;
	 	}
	 	else{



	 	}
	 }


	/*Comando desconhecido*/
	 else{
	 	printf("Comando desconhecido. Tente de novo.\n");
	 }


	}





	void stores_in_input_files(char* ptc, char* filename, char* data){
	/*todo*/
	}
	void works_on_file(char* ptc, char* filename, char* data){
	/*todo*/
	}
