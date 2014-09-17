#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "pedro.h"
#define PORT 50000
#define MAXSIZE 256
#define TIMEOUT 10000
#define QUADROS 1000

int confirmado[QUADROS];
int naoFalhou;
int fdSocket; //Descritor do socket do cliente ou do socket efetivo do servidor

using namespace std;
pthread_mutex_t mutexTamJanela;
void * recebedorConfirm(void * socket){
	char message[MAXSIZE];
	int fdSocket = (int) socket;
	for(int x=0;x<QUADROS;x++){
		confirmado[x]=0;  //0 = nao envou 1 = enviou e nao chegou dentro do timeout 2 = enviou e confirmou
	}
	int Nconfirmados=0;
	//receiveMessage(fdSocket,message,MAXSIZE*sizeof(char));
	while(1<2){
		receiveMessage(fdSocket,message,sizeof(int)); //Recebe mensagem
		cout<<"quadro "<<(int)message<<" foi confirmado"<<endl;
		if(confirmado[(int)message]!=2 ){
			confirmado[(int)message]=2;
			Nconfirmados++;
		}
		if(Nconfirmados==QUADROS-1)
			break;
	}
	printf("acabou recebedor \n");
}

void* enviaQuadro(void* quadro){
	sendMessage(fdSocket,(char*)quadro,sizeof(int));
	usleep(TIMEOUT);
	if(confirmado[(int)quadro]==2){
		naoFalhou++;
	}else{
		confirmado[(int)quadro]=1;
		naoFalhou=0;
	}
}

void* enviadorDados(void * socket){
	char message[MAXSIZE];
	int fdSocket = (int) socket;
	vector<pthread_t> enviadores;

	int fim = 4;
	
	int tamJanela=4;
	int atual=0;
	int inicio=0;
	while(inicio!=QUADROS){
		while(enviadores.size()<tamJanela && atual<QUADROS){
			cout<<"enviou quadro "<<atual<<endl;
			pthread_t temp;
			enviadores.push_back(temp);
			pthread_create(&enviadores[enviadores.size()-1],NULL,&enviaQuadro,(void*)atual);
			atual++;
		}
		bool falhou=false;
		for(int x=0;x<enviadores.size();x++){
			if(confirmado[inicio]==2){
				enviadores.erase(enviadores.begin());
				inicio++;
			}else{
				if(tamJanela>=2){
					falhou=true;
					tamJanela=tamJanela/2;
					break;
				}
			}
		}
		if(!falhou){
			tamJanela=min(tamJanela+5,100);
		}
	}
	printf("acabou enviador \n");
}

int main(int argc, char *argv[]){

	int mainSocket; //Descritor do socket principal do servidor
	

	//Estruturas auxiliares do socket
	struct sockaddr_in socketAddr;
	socklen_t sockLen;

	char message[MAXSIZE]; 	//String para troca de mensagens
	pthread_t threadRecebedor,threadEnviador;
	pthread_mutex_init(&mutexTamJanela,NULL);

	sockLen = sizeof(socketAddr);


	mainSocket=openConnection(PORT,0);	//Abre conexão principal
	fdSocket=acceptConnection(mainSocket, sockLen); //Espera por conexões
	
	gets(message);
	pthread_create(&threadRecebedor,NULL,&enviadorDados,(void*)fdSocket);
	pthread_create(&threadEnviador,NULL,&recebedorConfirm,(void*)fdSocket);
	
	pthread_join(threadRecebedor,NULL);
	pthread_join(threadEnviador,NULL);
	closeConnection(fdSocket); //Fecha socket com o cliente
	closeConnection(mainSocket); //Fecha socket principal

	return 0;

}
