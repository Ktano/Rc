#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#define PORT 58000
#define MAX 80

int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;
char msg[MAX], buffer[MAX];

int main(void){

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	memset((void*) &serveraddr, (int) '\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short) PORT);

	bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

	addrlen = sizeof(clientaddr);
	recvfrom(fd, buffer, MAX, 0, (struct sockaddr*) &clientaddr, &addrlen);
	printf("%s\n", buffer);
	sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*) &clientaddr, addrlen);

	close(fd);
}