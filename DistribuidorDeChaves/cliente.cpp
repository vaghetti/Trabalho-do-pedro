#include <iostream>
#include <cstring>
#include "pedro.h"
#include <stdlib.h>
#include <time.h>

const int PORTADISTRIBUIDOR = 50001;
const int PORTACONECTAPEERS = PORTADISTRIBUIDOR+1;

char* ipDistribuidorChaves = (char*)"localhost";
int idPeer;
int arrayConexoes[8],arrayChaves[8];
int chavesDistribuidor[] = {123,312,1337,80085,3434,1234,32241,3413};

pthread_t arrayThreadsRecebeMensagem[8];

using namespace std;

struct Mensagem{
	int a,b;
};

int criptografa(int n,int chave){
	return (n^chave);
}

void criptografaTexto(char* mensagem,int chave){
	int tam = strlen(mensagem);
	for(int x=0;x<tam;x++){
		mensagem[x]=mensagem[x]^chave;
	}
}

void * threadRecebeMensagens(void *idPeerAlvo){
	long idAlvo = (long) idPeerAlvo;
	char* charMsg = (char*) malloc(sizeof(char)*256);
	while(true){
		receiveMessage(arrayConexoes[idAlvo],charMsg,sizeof(Mensagem));
		criptografaTexto(charMsg,arrayChaves[idAlvo]);
		cout<<"Recebeu mensagem "<<charMsg<<" do peer "<<idAlvo<<endl;
	}
}

void *threadAbreConexoes(void* p){
	struct sockaddr_in socketAddr;
	socklen_t sockLen;
	sockLen = sizeof(socketAddr);
	char * charMsg = (char*) malloc(sizeof(char)*20);
	int mainSocketConectaPeers=openConnection(PORTACONECTAPEERS,0);	//Abre conexão principal
	while(true){
		int fdSocket=acceptConnection(mainSocketConectaPeers, sockLen);
		receiveMessage(fdSocket,charMsg,sizeof(Mensagem));
		Mensagem* msgRecebida = (Mensagem*) charMsg;
		arrayConexoes[msgRecebida->a]=fdSocket;
		arrayChaves[msgRecebida->a] = criptografa(msgRecebida->b,chavesDistribuidor[idPeer]);
		cout<<"Recebeu um pedido de abertura de conexao do peer "<<msgRecebida->a<<" usando a chave "<<msgRecebida->b<<endl;
		pthread_create(&arrayThreadsRecebeMensagem[msgRecebida->a],NULL,&threadRecebeMensagens,(void*)(long)msgRecebida->a);
	}
}

void enviaMensagemConexao(int socket,int a,int b){
	Mensagem* msg = (Mensagem*)malloc(sizeof(Mensagem));
	msg->a=a;
	msg->b=b;
	char * charMsg = (char*) malloc(sizeof(char)*20);
	sendMessage(socket,charMsg,sizeof(Mensagem));
}

void iniciaConexao(int idPeerAlvo,char* ipPeerAlvo){
	char * charMsg = (char*) malloc(sizeof(char)*20);
	int socketDistribuidor=tryConnection(ipDistribuidorChaves, PORTADISTRIBUIDOR, 0);
	enviaMensagemConexao(socketDistribuidor,idPeer,idPeerAlvo);
	receiveMessage(socketDistribuidor,charMsg,sizeof(Mensagem));  //recebe a resposta
	Mensagem* msgResposta = (Mensagem* ) charMsg;
	int chaveDescriptografada = criptografa(msgResposta->a,chavesDistribuidor[idPeer]);
	arrayChaves[idPeerAlvo] = chaveDescriptografada;
	arrayConexoes[idPeerAlvo] = tryConnection(ipPeerAlvo,PORTACONECTAPEERS,0);
	enviaMensagemConexao(arrayConexoes[idPeerAlvo],idPeer,msgResposta->b);
	pthread_create(&arrayThreadsRecebeMensagem[idPeerAlvo],NULL,&threadRecebeMensagens,(void*)(long)idPeerAlvo);
}

int main(int argc,char** argv){
	string comando;
	char texto[256];
	int idPeerAlvo;
	char* ip = (char*)malloc(sizeof(char)*30);
	cout<<"Rodou programa com ID = "<<argv[1]<<endl;
	idPeer= argv[0][0]-'0';
	while(true){
		cout<<"Digite 'conecta X IP' para se conectar a um peer ou 'msg X texto' para enviar uma mensagem para X(se este ja estiver conectado)"<<endl;
		cin>>comando>>idPeerAlvo;
		if(comando.compare("conecta")==0){
			cin>>ip;
			iniciaConexao(idPeerAlvo,ip);
		}else{
			if(comando.compare("msg")==0){
				cin>>texto;
			}else{
				cout<<"Comando nao reconhecido"<<endl;
			}
		}
	}
}