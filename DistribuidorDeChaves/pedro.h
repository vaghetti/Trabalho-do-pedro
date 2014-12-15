#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/*
	Função que cria um socket e fica esperando para receber conexões

	port: porta onde o socket vai ser aberto;
	socketType: 0 para bloqueante, 1 para não bloqueante;
	return value: descritor do socket aberto;
*/
int openConnection(unsigned short port, int socketType){

	int fdSocket;
	int nonBlock;
	struct sockaddr_in socketAddr;
	socklen_t sockLen;

	//Create socket
	if(socketType==0){ //Blocking
		if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Unable to create socket\n");
			printf("Exit %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}
	}
	else{ //Non-Blocking
		if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Unable to create socket\n");
			printf("Exit %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}
		nonBlock=fcntl(fdSocket,F_GETFL,0);
		fcntl(fdSocket,F_SETFL,nonBlock | O_NONBLOCK);
	}

	//Setting socket parameters
	memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family      = AF_INET;
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketAddr.sin_port        = htons(port);
    
	sockLen = sizeof(socketAddr);

	//Bind between socket and descriptor
	if(bind(fdSocket, (struct sockaddr *)&socketAddr, sockLen) < 0){
		perror("Unable to bind socket\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}

	//Waiting for connections. The second parameter is the size of the queue for waiting connections
	if(listen(fdSocket, 100) < 0){
		perror("Unable to listen\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}

	return fdSocket;

}

/*
	Função que tenta se conectar a um socket

	address: string com o endereço IP do host que vai receber a conexão 
	port: porta onde o socket vai ser aberto;
	socketType: 0 para bloqueante, 1 para não bloqueante;
	return value: descritor do socket aberto;
*/
int tryConnection(char *address, unsigned short port, int socketType){

	int fdSocket;
	int nonBlock;
	struct sockaddr_in socketAddr;
	
	//Create socket
	if(socketType==0){ //Blocking
		if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Unable to create socket\n");
			printf("Exit %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}
	}
	else{ //Non-Blocking
		if((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Unable to create socket\n");
			printf("Exit %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}
		nonBlock=fcntl(fdSocket,F_GETFL,0);
		fcntl(fdSocket,F_SETFL,nonBlock | O_NONBLOCK);
	}

	//Setting socket parameters
	memset(&socketAddr, 0,sizeof(socketAddr));
     	socketAddr.sin_family = AF_INET;	
     	socketAddr.sin_port   = htons(port);
     	inet_aton(address, &(socketAddr.sin_addr));	//Set the destination address
     
	//Connect to a socket
    	if(connect(fdSocket, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) < 0){
		return -1;
	}

	return fdSocket;

}

/*
	Função que aceita conexão em um socket já aberto

	fdSocket: descritor do socket já aberto
	sockLen: tamanho em bytes de uma estrutura de socket (ver código exemplo)
	return value: descritor do socket que especifica a conexão que foi estabelecida

*/
int acceptConnection(int fdSocket, socklen_t sockLen){

	int realSocket;
	struct sockaddr_in socketAddr;

	memset(&socketAddr, 0, sizeof(socketAddr));

	//Accept connection on a socket
	realSocket = accept(fdSocket, (struct sockaddr *)&socketAddr, &sockLen);

	return realSocket;

}

/*
	Função que fecha um socket aberto

	fdSocket: socket a ser fechado

*/
void closeConnection(int fdSocket){

	if((close(fdSocket)) < 0){
		perror("Unable to close socket\n");
		printf("Exit %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}

}

/*
	Função que envia uma mensagem para um socket

	fdSocket: descritor do socket onde a mensagem vai ser enviada
	data: dados a serem enviados
	size: quantidade de bytes que serão enviados
	return value: quantidade de bytes enviados

*/
int sendMessage(int fdSocket, const char *data, size_t size){

	int ret;
	
	ret=send(fdSocket, data, size, 0);
	
	if(ret < 0){
		perror("Unable to send message\n");
	}

	return ret;

}

/*
	Função que recebe uma mensagem em um socket

	fdSocket: descritor do socket que vai receber a mensagem
	data: local onde os dados recebidos serão armazenados
	size: quantidade de bytes que serão recebidos
	return value: quantidade de bytes recebidos

*/
int receiveMessage(int fdSocket, const char *data, size_t size){

	int ret;
	
	ret=recv(fdSocket, (void *)data, size, 0);

	if(ret < 0){
		perror("Unable to receive message\n");
	}

	return ret;

}
