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

    if (argument == NULL)
        snprintf(buffer,strlen(maincommand), "%s", maincommand);
    else
        snprintf(buffer,strlen(maincommand)+strlen(argument)+1, "%s %s ", maincommand, argument);

    write(tcpfd, buffer, strlen(buffer));

    if (size!=0)
    {   
        sprintf(buffer, "%d ", size);
        write(tcpfd, buffer, strlen(buffer));
        while (size>0){
            bytesRead=read(fd2,buffer,128);
            write(fd2,buffer,bytesRead);
            size-=bytesRead;
        }
    }
    write(tcpfd, "\n", strlen("\n"));
    return 0;
}