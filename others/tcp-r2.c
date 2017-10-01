 #include <stdio.h> 
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <string.h>
 #define PORT 58000
 #define MESSAGE_MAX 99



int fd, newfd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int clientlen;


int main(int argc, char const *argv[])
{
	char clt_message[MESSAGE_MAX];
	char srv_message[MESSAGE_MAX];
	strcpy(srv_message,"we got it");
	fd = socket(AF_INET,SOCK_STREAM,0);

	memset((void*)&serveraddr,(int)'\0',sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)PORT);
	
	bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	listen(fd,5);
	clientlen = sizeof(clientaddr);
	
	newfd = accept(fd,(struct sockaddr*)&clientaddr,&clientlen);
	read(newfd, clt_message, strlen(clt_message)+1);
 	printf("%s\n",clt_message ); //got msg
 	write(newfd , srv_message , strlen(srv_message)+1); //resp
 	close(fd); close(newfd);
 	return 0;
 }