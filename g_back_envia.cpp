#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "pedro.h"
#define PORT 50000
#define MAXSIZE 256
#define TIMEOUT 100000
#define QUADROS 100

int confirmado[QUADROS];
int naoFalhou;
long fdSocket; //Descritor do socket do cliente ou do socket efetivo do servidor

using namespace std;
pthread_mutex_t mutexTamJanela;

void * recebedorConfirm(void * lixo){
	char message[MAXSIZE];
	for(int x=0;x<QUADROS;x++){
		confirmado[x]=0;  //0 = nao envou 1 = enviou e nao chegou dentro do timeout 2 = enviou e confirmou
	}
	int Nconfirmados=0;
	//receiveMessage(fdSocket,message,MAXSIZE*sizeof(char));
	while(true){
		//printf("mensagem anterio = %s\n",message);
		receiveMessage(fdSocket,message,sizeof(int)); //Recebe mensagem
		//printf("mensagem nova = %s\n",message);
		if((rand()%100)<90 ){
			if(confirmado[atoi(message)] != 2 ){
				int conv = atoi(message);
				if(conv == 0 || confirmado[conv-1] == 2){
					printf("quadro %d foi confirmado\n",conv);
					confirmado[conv] = 2;
					Nconfirmados++;
				}
			}
			if(Nconfirmados==QUADROS){
				break;
			}
		}else{
			printf("falha ao enviar quadro %d\n",atoi(message));
		}
	}
	printf("acabou recebedor \n");
	pthread_exit(NULL);
}

void* enviaQuadro(void* quadro){
	if((rand()%100)<95 ){		

		char message [MAXSIZE];
		long q = (long) quadro;

		printf("enviou quadro %ld \n",q);

		sprintf(message, "%ld", q); 

		sendMessage(fdSocket,message,sizeof(int));

		usleep(TIMEOUT);

		if(confirmado[q]!=2){
			confirmado[q]=1;
		}
	}
	pthread_exit(NULL);
}

void* enviadorDados(void * socket){
	char message[MAXSIZE];
	vector<pthread_t> enviadores;
	
	int tamJanela=4;
	long atual=0;
	long inicio=0;

	while(inicio < QUADROS){
		usleep(100000);

		//atual = inicio;
		while(enviadores.size() < tamJanela && atual < QUADROS){
			
			pthread_t temp;
			enviadores.push_back(temp);
			pthread_create(&enviadores[enviadores.size()-1],NULL,&enviaQuadro,(void*)atual);
			usleep(10000);
			atual++;
		}
		bool falhou=false;
		for(int x=0;x<enviadores.size();x++){
			pthread_join(enviadores[x],NULL);
			if(confirmado[inicio]==2){
				enviadores.erase(enviadores.begin());
				inicio++;
			}else{
				printf("Deu treta, reenviando apartir de %ld\n",inicio);
				for(int i = 0; i < enviadores.size();i++){

					pthread_create(&enviadores[i],NULL,&enviaQuadro,(void*)(inicio + i));
					usleep(100);
				}

				falhou=true;
				break;
			}
		}
		if(!falhou){
			tamJanela = tamJanela + 2;
		}else{
			if(tamJanela>=2)
				tamJanela= tamJanela/2;
		}
		usleep(1000000);
	}
	printf("acabou enviador \n");
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	srand(time(NULL));

	long mainSocket; //Descritor do socket principal do servidor
	
	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;
	
	for(int x=0;x<QUADROS;x++)
		confirmado[x]=0;

	char message[MAXSIZE]; 	//String para troca de mensagens
	pthread_t threadRecebedor,threadEnviador;
	pthread_mutex_init(&mutexTamJanela,NULL);

	sockLen = sizeof(socketAddr);

	fdSocket=tryConnection((char*)"localhost", PORT, 0); //Tenta conectar ao servidor
	scanf("%s",message);

	pthread_create(&threadRecebedor,NULL,&enviadorDados,NULL);
	pthread_create(&threadEnviador,NULL,&recebedorConfirm,NULL);
	
	pthread_join(threadRecebedor,NULL);
	pthread_join(threadEnviador,NULL);
	usleep(100000);
	closeConnection(fdSocket); //Fecha socket com o cliente
	//closeConnection(mainSocket); //Fecha socket principal

	return 0;

}
