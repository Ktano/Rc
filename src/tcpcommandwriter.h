
#ifndef TCPCOMMANDWRITER_H
#define TCPCOMMANDWRITER_H

#define TCP_HEADER 7 /*numero de bytes que constituem o Header*/
#define TCP_COMMAND_LIST "LST"
#define TCP_COMMAND_PROCTASK "FPT"
#define TCP_COMMAND_REPLY "REP"
#define TCP_ARG_EOF "EOF"
#define TCP_ARG_ERR "ERR"
#define PROTOCOL_DIVIDER " \n"
#define FPT_SIZE 3

int tcpCommand(int tcpfd, char* maincommand, char* argument, char* file);
int TCPconnect(char* servername, int port);
int TCPaccept();
void UDPconnect();

#endif

