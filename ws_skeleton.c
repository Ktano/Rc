#include "commandlinereader.h"
#include "commandlinereader.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



#define COMM_REQUEST "WRQ"
#define PTC_WCT "WCT"
#define PTC_FLW "FLW"
#define PTC_UPP "UPP"
#define PTC_LOW "LOW"

#define MAXARGS 55 /*muda-se*/
#define BUFFER_SIZE 100





int main (int argc, char** argv) {

	/*max args in a command*/
	char *args[MAXARGS + 1]; 
    /*whole buffer size*/
	char buffer[BUFFER_SIZE];

	

	/*ws loop*/
	while (1) {
		/*connect with tcp*/
        /*receive tcp command string (msg)*/
		/*apply readLineArguments(args, MAXARGS+1, msg, BUFFER_SIZE);*/


		int numb_of_args;
		numb_of_args = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

		
		if (numb_of_args < 5) {
                /*bad syntax! sends udp error*/
			continue;
		}
		else{



		}
	}

	
	/*Comando desconhecido*/
	else{
		printf("Comando desconhecido. Tente de novo.\n");
	}


}





void stores_in_input_files(char* ptc, char* filename, char* data){
	/*todo*/
}
void works_on_file(char* ptc, char* filename, char* data){
	/*todo*/
}
