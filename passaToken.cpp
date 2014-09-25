#include<stdio.h>
#include<string.h>
#include"pedro.h"
#include <bits/stdc++.h>
#define PORT 50000

using namespace std;

int main(int argc, char *argv[]){

	int mainSocket; //Descritor do socket principal do servidor
	int fdSocket; //Descritor do socket do cliente ou do socket efetivo do servidor

	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;
	srand(time(NULL));

	char message[]="Vai token!"; 	//String para troca de mensagens
	bool first;
	if (argc==3){
		first = true;
		cout<<"FIRST!"<<endl;
	}else
		first =false;

	sockLen = sizeof(socketAddr);
	mainSocket=openConnection(PORT,0);	//Abre conexão principal
	fdSocket=acceptConnection(mainSocket, sockLen); //Espera por conexões
	fdSocket=tryConnection(argv[1], PORT, 0); //Tenta conectar ao proximo
	while(true){
		if(!first){
			cout<<"esperando token..."<<endl;
			receiveMessage(fdSocket,message,20*sizeof(char)); //Recebe mensagem
			cout<<"recebeu token"<<endl;
			if(rand()%100<50){
				cout<<"enviando dados"<<endl;
				usleep(10000);
				cout<<"dados enviados";
			}else{
				cout<<"sem dados para enviar"<<endl;
			}
			usleep(10000); //so pra nao ficar muito rapido
			sendMessage(fdSocket,message,20*sizeof(char)); //Envia mensagem
			cout<<"passou o token"<<endl;
		}else{
			first =false;
		}		
	}
		closeConnection(fdSocket); //Fecha socket com o cliente
		closeConnection(mainSocket); //Fecha socket principal

	

	return 0;

}

