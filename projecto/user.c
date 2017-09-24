#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 58006
#define BUFFER_MAX 128

int main(int argc, char **argv){
    int port= DEFAULT_PORT,i;
    int fd; //used for comunication
    char *servername;

    struct hostent *hostptr;
    struct sockaddr_in serveraddr; 

    for (i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
        if(strcmp(argv[i],"-p")==0) port=atoi(argv[i+1]);
        if(strcmp(argv[i],"-n")==0) servername=argv[i+1];
    }

    printf("Connecting to %s:%d\n",servername,port);

    fd=socket(AF_INET,SOCK_STREAM,0); //need to test for return -1

    if(servername==NULL){
        servername= malloc(BUFFER_MAX*sizeof(char));
        if(gethostname(servername,128)==-1){
            printf("error getting local host name: %s\n",strerror(errno));
            exit(EXIT_SUCCESS);
        }

        printf("Server name: %s",servername);
    }

    hostptr=gethostbyname(servername);
    

    free(servername);
}
