#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
 #include <stdio.h>

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

 strcpy(clt_message,"you got it?");
  /**/
  fd=socket(AF_INET,SOCK_STREAM,0);

 hostptr=gethostbyname("john"); 
  memset((void*)&serveraddr,(int)'\0',    sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
  serveraddr.sin_port = htons((u_short)PORT);


 connect(fd,(struct sockaddr*)&serveraddr,   sizeof(serveraddr));


 write(fd , clt_message , strlen(clt_message)+1); /*send msg*/

 read(fd, srv_message, strlen(srv_message)+1); /*get answer*/
  printf("%s\n",srv_message ); /*print it */

 close(fd);

  return 0;
}