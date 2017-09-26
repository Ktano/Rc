#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 58006
#define BUFFER_MAX 128

int main(int argc, char **argv){
    int port= PORT,i;
    int fd; //used for comunication
    char servername[128], buffer[128];

    struct hostent *hostptr;
    struct sockaddr_in serveraddr; 

    for (i = 0; i < argc; i++)
    {
        if(strcmp(argv[i],"-p")==0) port=atoi(argv[i+1]);
        if(strcmp(argv[i],"-n")==0) strcpy(servername,argv[i+1]);
    }



    fd=socket(AF_INET,SOCK_STREAM,0); //need to test for return -1

    if(servername==NULL){
        if(gethostname(servername,BUFFER_MAX)==-1){
            printf("error getting local host name: %s\n",strerror(errno));
            exit(EXIT_SUCCESS);
        }
    }


    /* structure definition to connect to server*/
    hostptr=gethostbyname(servername);
    memset((void*)&serveraddr,(int) '\0',sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    serveraddr.sin_port = htons((u_short)PORT);

    if(connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))==-1) printf("error: %s\n",strerror(errno));
    write(fd, "LST\n",4);
    read(fd,buffer,BUFFER_MAX);
    printf("%s",buffer);
    close(fd);

}
