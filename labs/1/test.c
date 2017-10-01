/*Welcome, you are inside now.
1st Task: Get the host name!
You have 10 minutes.*/


//test.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


extern int errno;

int main(void)
{

int gethostname(char *name,size_t len);

char *strerror(int errnum);


char buffer[128];
if(gethostname(buffer,128)==-1)
printf("error: %s\n",strerror(errno));
else printf("host name: %s\n",buffer);
exit(0);
}
