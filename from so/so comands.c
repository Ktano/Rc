/*
// Projeto SO - exercicio 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2016-17
*/

/*
// Projecto realizado por:
// Pedro Caetano, N:56564
// Ricardo Almeida, N:63108
*/

#include "commandlinereader.h"
#include "contas.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>



#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"
#define ARGUMENTO_AGORA "agora"

#define COMANDO_DEBITAR_ID 1
#define COMANDO_CREDITAR_ID 2
#define COMANDO_LER_SALDO_ID 3
#define COMANDO_SIMULAR_ID 4
#define COMANDO_SAIR_ID 5

#define MAXARGS 3
#define BUFFER_SIZE 100

#define NUM_TRABALHADORAS 3
#define CMD_BUFFER_DIM (NUM_TRABALHADORAS * 2)


typedef struct{
	int operacao;
	int idConta;
	int valor;
} comando_t;

comando_t cmd_buffer[CMD_BUFFER_DIM];
int buff_write_idx = 0, buff_read_idx = 0;
/*Vector para por as tarefas*/
pthread_t tid[NUM_TRABALHADORAS];
sem_t semLeitura,semEscrita;
pthread_mutex_t bufferReadMutex, bufferWriteMutex;

/*
// Prototipos
*/
void apanhaUSR1(int s);
void inicializarTarefas();
void *trabalhadora();
void adicionarComando(int Comando, int idConta, int valor);


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

void apanhaUSR1(int s){
	if(signal(SIGUSR1,apanhaUSR1)==SIG_ERR){
		perror("ERRO ao criar tratamento de sinal");
		exit(EXIT_SUCCESS);
	}
	sinalRecebido=1;
}



/*inicializarTarefas()
 * 
 * Cria e inicializa as tarefas dentro do vector tid[NUM_TRABALHADORAS].
 * 
 * Mutexes e semaforos:
 * Cria mutexes para por em seguranca a leitura e escrita no buffer.
 * Cria os semaforos para leitura e escrita no buffer: semLeitura e semEscrita.
 * 
 */

void inicializarTarefas(){
	int i=0,pthread;
	
	pthread_mutex_init(&bufferReadMutex,NULL);
	pthread_mutex_init(&bufferWriteMutex,NULL);
	
	sem_init(&semLeitura,0,0);
	sem_init(&semEscrita,0,CMD_BUFFER_DIM);
        
	for(i=0;i<NUM_TRABALHADORAS;i++){
            
                /*Cria e inicializa as NUM_TRABALHADORAS tarefas com a funcao *trabalhadora    */
		pthread=pthread_create(&tid[i],0,trabalhadora,NULL); 
		if(pthread!=0){
			fprintf(stderr,"Erro ao criar thread\n");
			continue;
		}
	}
}

/**
 *      Copia a estrutura na posicao corrente do vector buffer para uma variavel.
 *      Com essa variavel usa o comando, id da conta e valor necessarios para a tarefa respectiva do comando.
 * 
 */

void *trabalhadora(){
	comando_t cmd;
	while(1){          /* (ciclo infinito) */
                
               
		sem_wait(&semLeitura);                              
                
		pthread_mutex_lock(&bufferReadMutex);    
		cmd=cmd_buffer[buff_read_idx];                        /*Copia a estrutura do vector para a variavel cmd.*/
		buff_read_idx=(buff_read_idx+1)%CMD_BUFFER_DIM;
		pthread_mutex_unlock(&bufferReadMutex);             
                
		sem_post(&semEscrita);                                 
		
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

/**
 *  Dado o comando a realizar e os argumentos cria a estrutura da tarefa e mete-a no buffer.
 *  Uma vez no buffer pode ser lido e executado por uma trabalhadora, assim que for possivel.
 */

void adicionarComando(int Comando, int idConta, int valor){
	sem_wait(&semEscrita);                        
    
	pthread_mutex_lock(&bufferWriteMutex);
	cmd_buffer[buff_write_idx].operacao=(Comando);
	cmd_buffer[buff_write_idx].idConta=(idConta);
	cmd_buffer[buff_write_idx].valor=(valor);
	buff_write_idx=(buff_write_idx+1)%CMD_BUFFER_DIM;
	pthread_mutex_unlock(&bufferWriteMutex);
        
	sem_post(&semLeitura);
}
