#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "pedro.h"
#define PORT 50000
#define MAXSIZE 256
#define QUADROS 100000
int fdSocket; //Descritor do socket do cliente ou do socket efetivo do servidor

void* enviadorConfirm(void * socket){
	char message[MAXSIZE];
	int fdSocket = (int) socket;
	while(true){
		if(!strcmp(message, "tchau"))
			break;
		usleep(100);
	}
	printf("acabou enviador \n");
}

void * recebedorDados(void * socket){
	char message[MAXSIZE];
	int fdSocket = (int) socket;
	while(true){
		receiveMessage(fdSocket,message,sizeof(int));
		if((rand()%100)>75 ){
			sendMessage(fdSocket,message,sizeof(int)); //Envia mensagem
		}
		usleep(10000);
	}
	printf("acabou recebedor \n");
}

int main(int argc, char *argv[]){

	int mainSocket; //Descritor do socket principal do servidor
	srand(time(NULL));
	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;

	char message[MAXSIZE]; 	//String para troca de mensagens
	pthread_t threadRecebedor,threadEnviador;
	//Se for servidor

	fdSocket=tryConnection((char*)"localhost", PORT, 0); //Tenta conectar ao servidor
	gets(message);
	
	pthread_create(&threadRecebedor,NULL,&recebedorDados,(void*)fdSocket);
	pthread_create(&threadEnviador,NULL,&enviadorConfirm,(void*)fdSocket);
	pthread_join(threadRecebedor,NULL);
	pthread_join(threadEnviador,NULL);
	
	closeConnection(fdSocket); //Fecha socket com o servidor

	return 0;

}
