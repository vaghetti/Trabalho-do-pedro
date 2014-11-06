#include<stdio.h>
#include<string.h>
#include"communication.h"
#include<pthread.h>

#define PORT 5000      //porta de comunicação
#define JANELA_MIN 2   //tamanho mínimo da janela
#define JANELA_MAX 16  //tamanho máximo da janela 

int fdSocket;	//Descritor do socket do cliente ou do socket efetivo do servidor
int janelaAtual; //variável definida para representar o estado atual da janela
char message[4];   //String para troca de mensagens
int controleJanela=0;//variável controleJanela que gerenciará o tamanho da janela

//função de recepção de mensagem,entrará no looping para a confirmação da mensagem, caso ela chegue,a mensagem será confirmada com 1, se não, ela irá esperar para recebê-la e decrementará a variável "i",sendo esta, responsável por aumentar ou diminuir a variável janelaAtual.
void *recebe(void *x){
	int i;
	
	while(1){ 
    	receiveMessage(fdSocket,message,sizeof(char)); //Recebe mensagem
    	sendMessage(fdSocket,message,sizeof(char)); //Envia mensagem
    }
}
 
/*função para o envio da mensagem. Foram definidas duas variáveis:aleatório e erro. Haverá um sorteio do caractere na tabela ascii entre A e Z e o cálculo de uma probabilidade de 20% de ocorrer erro na mensagem. No laço da repetição,verificará se mensagem apresentará o erro sorteado. Se o erro for diferente de 1,ele enviará a mensagem e aguardará para a confirmação do recebimento da mesma.*/
void *envia(void *x){
	int aleatorio, erro; //variáveis utilizadas para o sorteio no intervalo e cálculo do erro
	char message2[4]; //réplica para comparação
	
	aleatorio=rand()%25+65; 	//sorteia um caractere da tabela ascii entre A e Z
	erro=rand()%20+1;			//se erro=1, não envia (5% de chance)
	
	message[0]=aleatorio;		//conteudo
	message[1]=(int)x;			//indice da janela
	message[2]=0;				// confirmação, que acabará não sendo utilizada
	
	strcpy(message2,message);
	
	if(erro!=1){
		sendMessage(fdSocket,message,sizeof(char));
		controleJanela++;
	}
	else
		controleJanela=0;

	receiveMessage(fdSocket,message,sizeof(char));
	
	while(strcmp(message,message2)){ //condição do looping para verificar se a mensagem recebida é a esperada, que é igual a enviada
	    sendMessage(fdSocket,message2,sizeof(char));
		receiveMessage(fdSocket,message,sizeof(char));
	}

}
int main(){
    int mainSocket; //Descritor do socket principal do servidor
   
    //Estruturas auxiliares do socket
    struct sockaddr_in socketAddr;
    socklen_t sockLen;
 
    pthread_t threads[JANELA_MAX]; // Maximo de threads eh igual ao tamanho maximo da janela
	janelaAtual=JANELA_MIN;
	
	char identifica;
	printf("\nDigite E para enviar ou R para receber\n");
	scanf("%c",&identifica);
	
 
    //Condição se for o responsável por receber a mensagem
    if(identifica=='R' || identifica=='r'){
        sockLen = sizeof(socketAddr);
 
        mainSocket=openConnection(PORT,0);  //Abre conexão principal
        fdSocket=acceptConnection(mainSocket, sockLen); //Espera por conexões
        
        pthread_create(&threads[0],NULL,&recebe,NULL);
  
        closeConnection(fdSocket); //Fecha socket com o cliente
        closeConnection(mainSocket); //Fecha socket principal
 
    }
	//Condição se for o responsável por enviar a mensagem
    else{
    	int x=0,y,aleatorio,controleJanela;
	 	fdSocket=tryConnection("localhost", PORT, 0); //Tenta conectar ao servidor
	 	

    	while(1){//looping para verificação do gerenciamento da janela 
    	    
    	    if(controleJanela==5 && janelaAtual<JANELA_MAX){ 
    	    	janelaAtual=janelaAtual*2;
    	    	controleJanela=0;
    	    }
			
			y=x;
    	    
			for(;x<janelaAtual;x++)
				pthread_create(&threads[x],NULL,&envia,(void *)x); // MODIFICADO 4º item era NULL
			
			for(;y<janelaAtual;y++)
				printf("\n\nIndice: %d, Conteudo: %d",message[1],message[0]);

			controleJanela++;

			for(;y<janelaAtual;y++)
				pthread_join(threads[y],NULL);
        }
        closeConnection(fdSocket); //Fecha socket com o servidor
    }
	pthread_exit(NULL);
    return 0;
}
