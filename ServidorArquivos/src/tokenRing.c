#include<stdio.h>
#include<stdlib.h>

#include<string.h>
#include"communication.h"
#include<pthread.h>
#define PORT 50000


int main(int argc, char *argv[]){

	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;

	int mainSocket; //Descritor do socket principal do servidor
	int fdSocket1, fdSocket2; //Descritores do socket do cliente ou do socket efetivo do servidor
	char message[20]; 	//String para troca de mensagens
	int x;  // terá valor aleatório para entre 1 e 4, se for 1 ela usará o token.
	
	mainSocket=openConnection(PORT,0);	//Abre conexão principal
	fdSocket1=tryConnection("192.168.1.25", PORT, 0); //Tenta conectar ao nodo A;
	fdSocket2=acceptConnection(mainSocket, sockLen); //Espera que o nodo C conecte;

	while(1){
		srand(time(NULL));
		x = rand() % 4 + 1;
		receiveMessage(fdSocket1,message,20*sizeof(char)); //Recebe mensagem
		printf("\n%s\n",message);
		if(x==1){
			printf("\n\n Enviando para nodo C");
			sleep(2);
		}
		sendMessage(fdSocket2,"nodo C",20*sizeof(char)); //Envia mensagem		 
	}
	
	closeConnection(fdSocket1); //Fecha socket com nodo A
	closeConnection(fdSocket2); //Fecha socket com nodo B
	closeConnection(mainSocket); //Fecha socket principal

	
}
download:tokenRing.c
