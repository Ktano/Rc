#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 58006
#define BUFFER_MAX 80

int main(int argc, char **argv){
    int port= DEFAULT_PORT,i;
    char *servername;

    for (i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
        if(strcmp(argv[i],"-p")==0) port=atoi(argv[i+1]);
        if(strcmp(argv[i],"-n")==0) servername=argv[i+1];
    }

    printf("Connecting to %s:%d\n",servername,port);
}
