#include "pedro.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace std;

const int PORT = 50006;

struct Mensagem{
	int codigo,x,y;
};

int xUltimoTiro,yUltimoTiro;
char matrizLocal[10][10],matrizAdv[10][10];
int fdSocket; 
bool TERMINOU;
int mortos=0;
string saida;

// cria o print da tela 
void mostraMatrizes(){
	system("clear");
	cout<<"    voce                || adversario"<<endl<<"    ";
	for(int x=1;x<10;x++)
		cout<<x<<" ";
	cout<<"10||    ";
	for(int x=1;x<=10;x++)
		cout<<x<<" ";
	cout<<endl;
	cout<<"    --------------------||-----------------------"<<endl;
	for(int y=0;y<10;y++){
		printf("%2d| ",y+1);
		for(int x=0;x<10;x++){
			if(matrizLocal[x][y]!='0')
				cout<<matrizLocal[x][y]<<" ";
			else
				cout<<"  ";
		}
		cout<<"||";
		printf("%2d| ",y+1);
		for(int x=0;x<10;x++){
			if(matrizAdv[x][y]!='0')
				cout<<matrizAdv[x][y]<<" ";
			else
				cout<<"  ";
		}
		cout<<endl;
	}
}

void enviaMensagem(int x,int y,int codigo){
	Mensagem* msg= (Mensagem*)malloc(sizeof(Mensagem));
	msg->x=x;
	msg->y=y;
	msg->codigo=codigo;
	char* charMsg = (char*)malloc(sizeof(Mensagem));
	charMsg=(char*)msg;
	cout<<"enviando mensagem cod "<<msg->codigo<<" x = "<<msg->x<<" y = "<<msg->y<<endl;
	sendMessage(fdSocket,charMsg,sizeof(Mensagem));
}

void joga(){
	int x,y;
	cout<<"Digite as coordenadas onde deseja atirar. Digite -1 -1 para desistir."<<endl;
	cin>>x>>y;
	xUltimoTiro = x-1;
	yUltimoTiro = y-1;
	if(x==-1 && y==-1){
		enviaMensagem(x,y,6);
		cout<<"Voce desistiu do jogo. Voce perdeu."<<endl;
		TERMINOU=true;
	}else{
		enviaMensagem(x,y,1);
	}
}

int recebeMensagem(){  //retornos: 0: pode jogar 1: espera outra mensagem
	char charMsg[20];
	receiveMessage(fdSocket,charMsg,sizeof(Mensagem));
	Mensagem * msg = (Mensagem*)charMsg;
	saida.clear();
	
	//cria a reposta depois do campo
	
	saida = "Recebeu mensagem x = ";
    saida+='0'+msg->x;
    saida+=" y = ";
    saida+='0'+msg->y;
	saida+=" codigo = ";
    saida+='0'+msg->codigo;
    saida+="\n";
	msg->x--;
	msg->y--;

	// faz as verificacoes de resposta
	if(msg->codigo==1){
		if(msg->x>=0 && msg->x<10 && msg->y>=0 && msg->y<10){
			if(matrizLocal[msg->x][msg->y]=='0'){
				saida+="Inimigo atirou na agua\n";
				enviaMensagem(0,0,3);
				return 0;
			}else{
				if(matrizLocal[msg->x][msg->y]=='1'){
					saida+="Inimigo acertou um navio\n";
					mortos++;
					if(mortos==18){
						saida+="Todas suas unidades estao mortas. Voce perdeu.\n";
						enviaMensagem(0,0,5);
						TERMINOU=true;
						return 0;
					}else{
						enviaMensagem(0,0,2);
						matrizLocal[msg->x][msg->y]='X';
						return 1;
					}
				}else{
					saida+="Acertou um navio que já tinha sido atingido\n";
					enviaMensagem(0,0,2);
					return 1;
				}
			}
		}else{
			saida+="Inimigo enviou coordenadas invalidas\n";
			enviaMensagem(0,0,4);
			return 1;
		}
	}else{
		if(msg->codigo==2){
			saida+="Tiro acertou. Jogue de novo\n";
			matrizAdv[xUltimoTiro][yUltimoTiro]='V';
			return 0;
		}
		if(msg->codigo==3){
			saida+="Seu tiro acertou a agua. Aguarde a jogada do inimigo.\n";
			matrizAdv[xUltimoTiro][yUltimoTiro]='X';
			return 1;
		}
		if(msg->codigo==4){
			saida+="Voce especificou uma coordenada invalida. Jogue novamente\n";
			return 0;
		}
		if(msg->codigo==5){
			saida+="Voce venceu. Todos barcos do adversario foram destruidos.\n";
			TERMINOU=true;
			return 0;
		}
		if(msg->codigo==6){
			saida+="Voce venceu. O adversario se rendeu\n";
			TERMINOU=true;
			return 0;
		}
		return 0;
	}

}

int main(int argc,char** argv){
	TERMINOU=false;
	fstream fs ("entrada.in", fstream::in | fstream::out);
	for(int y=0;y<10;y++){
		for(int x=0;x<10;x++){
			fs>>matrizLocal[x][y];
			matrizAdv[x][y]='0';
		}
	}
	fs.close();

	int mainSocket;
	
	struct sockaddr_in socketAddr;
	socklen_t sockLen;


	char message[20];

	// verifica se eh cliente ou servidor
	if(argc==2){
		sockLen = sizeof(socketAddr);
		mainSocket=openConnection(PORT,0);	//Abre conexão principal
		fdSocket=acceptConnection(mainSocket, sockLen);
		joga();
	}else{
		fdSocket=tryConnection((char*)"192.168.208.41", PORT, 0);
	}

	int retorno;
	
	// espera o fim do jogo pra terminar
	
	while(!TERMINOU){
		retorno=1;
		while(retorno==1){
			retorno=recebeMensagem();
			mostraMatrizes();
			cout<<saida;
		}
		if(!TERMINOU){
			joga();
			mostraMatrizes();
		}
	}

	//fecha a conexao

	closeConnection(fdSocket);

	return 0;
	
}
