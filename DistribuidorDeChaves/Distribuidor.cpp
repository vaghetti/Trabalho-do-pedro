#include <iostream>
#include <cstring>
#include "pedro.h"
#include <stdlib.h>
#include <time.h>
#include <climits>

const int PORT = 50001;

using namespace std;

int criptografa(int n,int chave){
	return (n^chave);
}

struct Mensagem{
	int a,b;
};



int main(){
	int chaves[] = {123,312,1337,80085,3434,1234,32241,3413};
	char* charMsg = (char*)malloc(sizeof(char)*30);
	struct sockaddr_in socketAddr;
	socklen_t sockLen;
	sockLen = sizeof(socketAddr);
	int mainSocket=openConnection(PORT,0);	//Abre conexão principal
	srand (time(NULL));

	while(true){
		int fdSocket=acceptConnection(mainSocket, sockLen);
		receiveMessage(fdSocket,charMsg,sizeof(Mensagem));
		Mensagem* msgRecebida = (Mensagem*)charMsg;

		cout<<msgRecebida->a<<" deseja se conectar com "<<msgRecebida->b<<endl;
		Mensagem* msgResposta = (Mensagem*)malloc(sizeof(Mensagem));

		int chave = rand()%INT_MAX;
		msgResposta->a=criptografa(chave,chaves[msgRecebida->a]);
		msgResposta->b=criptografa(chave,chaves[msgRecebida->b]);
		charMsg = (char*) msgResposta;
		
		sendMessage(fdSocket,charMsg,sizeof(Mensagem));

		closeConnection(fdSocket);
	}
}
