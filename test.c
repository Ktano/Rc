#include "commandlinereader.h"
#include "commandlinereader.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"


#define PTC_1 "agora"
#define PTC_1 "agora"
#define PTC_1 "agora"
#define PTC_1 "agora"

#define MAXARGS 55 /*muda-se*/
#define BUFFER_SIZE 100





int main (int argc, char** argv) {

    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    int nProcessos=0; /* guarda o numero de processos já criados*/
    
	inicializarContas();
	inicializarTarefas();
	sinalRecebido=0;
	
	if(signal(SIGUSR1,apanhaUSR1)==SIG_ERR){
		perror("ERRO ao criar tratamento de sinal");
		exit(EXIT_SUCCESS);
	}
		
    printf("Bem-vinda/o ao i-banco\n\n");
      
    while (1) {
        int numargs;
    
        numargs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

        /* EOF (end of file) do stdin ou comando "sair" */
        if (numargs < 0 ||
	        (numargs > 0 && (strcmp(args[0], COMANDO_SAIR) == 0))) {
            int i, pid,status;
        
                if(numargs>1 && strcmp(args[1], ARGUMENTO_AGORA) == 0){
                    if (kill(0,SIGUSR1)<0){
                        printf("%s %s: ERRO\n",COMANDO_SAIR,ARGUMENTO_AGORA);
						exit(EXIT_FAILURE);
					}
                }
				
                printf("i-banco vai terminar.\n");
                printf("--\n");
				
				/*Terminates all threads*/
				for(i=0;i<NUM_TRABALHADORAS;i++)
					adicionarComando(COMANDO_SAIR_ID,0,0);
				
				for(i=0;i<nProcessos;i++){
					pid=wait(&status);
					
					/*error on wait*/
					if(pid<0){
						if (errno==EINTR){
							i--;
							continue;
						}
							perror("Error na funcao wait.\n");
							exit(EXIT_FAILURE);
					}
					
                    if(WIFEXITED(status))
						printf("FILHO TERMINADO (PID=%d; terminou normalmente)\n",pid);
					else
						printf("FILHO TERMINADO (PID=%d; terminou abruptamente)\n",pid);
				}
					
			/*Confirms all threads are terminated*/
			for(i=0;i<NUM_TRABALHADORAS;i++)
				pthread_join(tid[i],NULL);
			
            printf("--\n");
			printf("i-banco terminou.\n\n");
            exit(EXIT_SUCCESS);
        }
    
	else if (numargs == 0)
		/* Nenhum argumento; ignora e volta a pedir */
		continue;
            
	/* Debitar */
	else if (strcmp(args[0], COMANDO_DEBITAR) == 0) {
            int idConta, valor;

            if (numargs < 3) {
                printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_DEBITAR);
	           continue;
            }

            idConta = atoi(args[1]);
            valor = atoi(args[2]);
			adicionarComando(COMANDO_DEBITAR_ID,idConta,valor);

    }

    /* Creditar */
    else if (strcmp(args[0], COMANDO_CREDITAR) == 0) {
        int idConta, valor;
        
        if (numargs < 3) {
            printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_CREDITAR);
            continue;
        }

        idConta = atoi(args[1]);
        valor = atoi(args[2]);
		adicionarComando(COMANDO_CREDITAR_ID,idConta,valor);

    }

    /* Ler Saldo */
    else if (strcmp(args[0], COMANDO_LER_SALDO) == 0) {
        int idConta;

        if (numargs < 2) {
            printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_LER_SALDO);
            continue;
        }
        idConta = atoi(args[1]);
		adicionarComando(COMANDO_LER_SALDO_ID,idConta,0);
    }

    /* Simular */
    else if (strcmp(args[0], COMANDO_SIMULAR) == 0) {
		int anos,pid;
		
		
		if (numargs < 2) {
			printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_SIMULAR);
			continue;
		}
            
        anos = atoi(args[1]);
        
            /*Cria o processo*/
            pid=fork();
            
            if(pid<0){
				perror("Não foi possivel criar o processo \n");
                continue;
            }
            
			if (pid==0){
				simular(anos);
				exit(EXIT_SUCCESS);
			}
			else{
				nProcessos++;
				continue;
			}
			     
      }
	
	/*Comando desconhecido*/
    else{
      printf("Comando desconhecido. Tente de novo.\n");
    }

  } 
}

	while(1){          /* (ciclo infinito) */
                
               
		                        
		
		/*Debitar*/
		if(cmd.operacao==COMANDO_DEBITAR_ID){
			if (debitar (cmd.idConta, cmd.valor) < 0)
				printf("%s(%d, %d): Erro\n\n", COMANDO_DEBITAR, cmd.idConta, cmd.valor);
			else
				printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, cmd.idConta, cmd.valor);
		}
		
		/*Creditar*/
		else if(cmd.operacao==COMANDO_CREDITAR_ID){
			if (creditar (cmd.idConta, cmd.valor) < 0)
				printf("%s(%d, %d): Erro\n\n", COMANDO_CREDITAR, cmd.idConta, cmd.valor);
			else
				printf("%s(%d, %d): OK\n\n", COMANDO_CREDITAR, cmd.idConta, cmd.valor);
		}
		/*Ler Saldo*/
		else if(cmd.operacao==COMANDO_LER_SALDO_ID){
			int saldo;
			saldo = lerSaldo (cmd.idConta);
			
			if (saldo < 0)
				printf("%s(%d): Erro.\n\n", COMANDO_LER_SALDO, cmd.idConta);
			else
				printf("%s(%d): O saldo da conta é %d.\n\n", COMANDO_LER_SALDO, cmd.idConta, saldo);
		}
		/*Sair*/
		else if(cmd.operacao==COMANDO_SAIR_ID){
			pthread_exit(0);
		}
	}
}


void stores_in_input_files(char* ptc, char* filename, char* data){
	/*todo*/
}
void works_on_file(char* ptc, char* filename, char* data){
	/*todo*/
}
