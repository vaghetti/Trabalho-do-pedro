#include<stdio.h>
#include<string.h>
#include"pedro.h"
#include <bits/stdc++.h>
#define PORT 50000

using namespace std;

int main(int argc, char *argv[]){

	int mainSocket; //Descritor do socket principal do servidor
	int socketEnvia; //Descritor do socket do cliente ou do socket efetivo do servidor
	int socketRecebe;

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
	char espera;
	sockLen = sizeof(socketAddr);
	mainSocket=openConnection(PORT,0);	//Abre conexão principal
	cin>>espera;
	if(first){
		socketEnvia=tryConnection(argv[1], PORT, 0); //Tenta conectar ao proximo
		socketRecebe=acceptConnection(mainSocket, sockLen); //Espera por conexões	
	}else{
		socketRecebe=acceptConnection(mainSocket, sockLen); //Espera por conexões
		socketEnvia=tryConnection(argv[1], PORT, 0); //Tenta conectar ao proximo
	}
	cout<<"conenctado com "<<argv[1]<<	endl;
	while(true){
		if(!first){
			cout<<"esperando token..."<<endl;
			receiveMessage(socketRecebe,message,20*sizeof(char)); //Recebe mensagem
			cout<<"recebeu token"<<endl;
			if(rand()%100<50){
				cout<<"enviando dados"<<endl;
				usleep(3000000);
				cout<<"dados enviados";
			}else{
				cout<<"sem dados para enviar"<<endl;
			}
			usleep(1000000); //so pra nao ficar muito rapido
			sendMessage(socketEnvia,message,20*sizeof(char)); //Envia mensagem
			cout<<"passou o token"<<endl;
		}else{
			cout<<"enviando token"<<endl;
			sendMessage(socketEnvia,message,20*sizeof(char)); //Envia mensagem
			first =false;
		}		
	}
	closeConnection(socketEnvia); //Fecha socket com o cliente
	closeConnection(socketRecebe); //Fecha socket com o cliente
	closeConnection(mainSocket); //Fecha socket principal

	return 0;

}

