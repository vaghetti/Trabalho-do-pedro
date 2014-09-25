#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "pedro.h"
#define PORT 50000
#define MAXSIZE 256
#define QUADROS 10
#define TIMEOUT 10

long fdSocket; //Descritor do socket do cliente ou do socket efetivo do servidor

void * recebedorDados(void * lixo){
	char message[MAXSIZE];

	while(true){
		receiveMessage(fdSocket,message,sizeof(int));
		if((rand()%100)<95 ){
			printf("recebeu %s \n",message);
			if((rand()%100)<90 ){
				sendMessage(fdSocket,message,sizeof(int)); //Envia mensagem
				printf("confirmou %s \n",message);
			}else{
				printf("falha na confirmação\n");
			}
		}
	}
	printf("acabou recebedor \n");
}

int main(int argc, char *argv[]){

	srand(time(NULL));
	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;

	char message[MAXSIZE]; 	//String para troca de mensagens
	pthread_t threadRecebedor;
	//Se for servidor

	fdSocket=openConnection(PORT,0);	//Abre conexão principal
	fdSocket=acceptConnection(fdSocket, sockLen); //Espera por conexões

	scanf("%s",message);
	
	pthread_create(&threadRecebedor,NULL,&recebedorDados,NULL);

	pthread_join(threadRecebedor,NULL);
	usleep(100000);
	closeConnection(fdSocket); //Fecha socket com o servidor

	return 0;

}
