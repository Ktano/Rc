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

int FTPcounter()
{
  FILE *fp;
    int count = 0;
    int ch, len;
  char *filename = "counter.txt";
  char *ftp = "WCT";


  if(NULL==(fp=fopen(filename, "r")))
        return -1;
    len = strlen(ftp);
    for(;;){
        int i;
        if(EOF==(ch=fgetc(fp))) break;
        if((char)ch != *ftp) continue;
        for(i=1;i<len;++i){
            if(EOF==(ch = fgetc(fp))) goto end;
            if((char)ch != ftp[i]){
                fseek(fp, 1-i, SEEK_CUR);
                goto next;
            }
        }
        ++count;
        next: ;
    }
end:
    fclose(fp);
    return count;
}
