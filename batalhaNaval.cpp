#include "pedro.h"
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std;

const int PORT = 50001;

struct Mensagem{
	int codigo,x,y;
};

char matrizLocal[10][10],matrizAdv[10][10];
int fdSocket; 
bool TERMINOU;
int mortos=0;
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
			cout<<matrizLocal[x][y]<<" ";
		}
		cout<<"||";
		printf("%2d| ",y+1);
		for(int x=0;x<10;x++){
			cout<<matrizAdv[x][y]<<" ";
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
	sendMessage(fdSocket,charMsg,sizeof(Mensagem));
}

void joga(){
	int x,y;
	cout<<"Digite as coordenadas onde deseja atirar. Digite -1 -1 para desistir."<<endl;
	cin>>x>>y;
	if(x==-1 && y==-1){
		enviaMensagem(x,y,6);
		cout<<"Voce desistiu do jogo. Voce perdeu."<<endl;
		TERMINOU=true;
	}else{
		matrizAdv[x-1][y-1]='X';
		enviaMensagem(x,y,1);
	}
}

int recebeMensagem(){  //retornos: 0: pode jogar 1: espera outra mensagem
	char charMsg[20];
	receiveMessage(fdSocket,charMsg,sizeof(Mensagem));
	Mensagem * msg = (Mensagem*)charMsg;
	cout<<"Recebeu mensagem x = "<<msg->x<<" y = "<<msg->y<<" codigo = "<<msg->codigo<<endl;
	msg->x--;
	msg->y--;

	if(msg->codigo==1){
		if(msg->x>=0 && msg->x<10 && msg->y>=0 && msg->y<10){
			if(matrizLocal[msg->x][msg->y]=='0'){
				cout<<"Inimigo atirou na agua"<<endl;
				enviaMensagem(0,0,3);
				return 0;
			}else{
				if(matrizLocal[msg->x][msg->y]=='1'){
					cout<<"Inimigo acertou um navio"<<endl;
					mortos++;
					if(mortos==18){
						cout<<"Todas suas unidades estao mortas. Voce perdeu."<<endl;
						enviaMensagem(0,0,5);
						return 0;
					}else{
						enviaMensagem(0,0,2);
						matrizLocal[msg->x][msg->y]='X';
						return 1;
					}
				}else{
					cout<<"Inimigo acertou um navio que já tinha sido atingido"<<endl;
					enviaMensagem(0,0,2);
					return 1;
				}
			}
		}else{
			cout<<"Inimigo enviou coordenadas invalidas"<<endl;
			enviaMensagem(0,0,4);
			return 1;
		}
	}else{
		if(msg->codigo==2){
			cout<<"Seu tiro acertou. Jogue de novo"<<endl;
			return 0;
		}
		if(msg->codigo==3){
			cout<<"Seu tiro acertou a agua. Aguarde a jogada do inimigo."<<endl;
			return 1;
		}
		if(msg->codigo==4){
			cout<<"voce especificou uma coordenada invalida. Jogue novamente"<<endl;
			return 0;
		}
		if(msg->codigo==5){
			cout<<"Voce venceu. Todos barcos do adversario foram destruidos."<<endl;
			TERMINOU=true;
			return 0;
		}
		if(msg->codigo==6){
			cout<<"Voce venceu. O adversario se rendeu"<<endl;
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

	if(argc==2){
		sockLen = sizeof(socketAddr);
		mainSocket=openConnection(PORT,0);	//Abre conexão principal
		fdSocket=acceptConnection(mainSocket, sockLen);
		joga();
	}else{
		fdSocket=tryConnection((char*)"localhost", PORT, 0);
	}

	int retorno;
	while(!TERMINOU){
		retorno=1;
		while(retorno==1){
			mostraMatrizes();
			retorno=recebeMensagem();
		}
		if(!TERMINOU)
			joga();
	}

	closeConnection(fdSocket);

	return 0;
	
}