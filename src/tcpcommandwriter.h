#ifndef TCPCOMMANDWRITER_H
#define TCPCOMMANDWRITER_H

#define TCP_HEADER 7 /*numero de bytes que constituem o Header*/
#define TCP_COMMAND_LIST "LST"
#define TCP_COMMAND_PROCTASK "FPT"
#define TCP_COMMAND_REPLY "REP"
#define TCP_COMMAND_WORKREQUEST "WRQ"
#define TCP_ARG_EOF "EOF"
#define TCP_ARG_ERR "ERR"
#define TCP_REPLY_REPORT "R"
#define TCP_REPLY_FILE "F"
#define PROTOCOL_DIVIDER " \n"
#define FPT_SIZE 3

int tcpCommand(int tcpfd, char *maincommand, char *argument, char *file, int include_file_name);
int TCPconnect(char *servername, int port);
int TCPacceptint(int *fd, int port);
int UDPconnect(int port);
int filesplitter(char* file, int servers, int filecounter);
int UDPCommand(char* buffer,int bufferlen, char *maincommand, char **PTC,int lenghtPTC, int port);
int sendUDP(char *servername,int UDPport,char *msg, char* reply,int size);
int FTPcounter(char* filename, char* ftp);
int connectToWS(char *filename, char *requestedFPT, int *fd_wsservers, int max_servers);
void LSTcommand(char *filename, char *requestedFPT);

#endif
