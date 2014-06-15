#include <bits/stdc++.h>
#include<unistd.h>
using namespace std;
const int CONSTANTE=1000000;
const int NMOLDURAS=8;
const int NDISPOSITIVOS=8;

struct Processo{ //true=pronto false=bloqueado
    int tempoTotal,prioridade,tempoRestante,IDdoUsuario,IDprocesso;
    char nome[256];
    bool bloqueado;
    vector <int> tabelaDePaginas; //-1 = não está na memória
};

struct Moldura{
    int IDprocesso;
    int paginaDoProcesso;
    int idade;
};

struct Requisicao{
    int IDprocesso;
    int dispositivo;
};

void loteria();  //escalona os processos por loteria
void printBilhetes();  // mostra os bilhetes que foram geradas para o escalonamento por loteria

void escalonaPrioridade();
void printPrioridades();  //mostra as prioridades arbitradas para cada processo
bool maiorPrioridade(Processo a,Processo b);  //funcao auxiliar usada para ordenar os processos por prioridade

void circular();

void *rodaEscalonador(void *thread); //Thread que fica rodando o escalonador selecionado
void *criadorProcessos(void *processo); //Thread que fica criando processos
void rodaProcesso(int ID); //funcao que é chamada pelo escalonador, chama as funcoes de memoria e adiciona requisições no spool de IO
void apagaProcesso(int ID);
int findID(int ID); //retorna a posição no vector da ID requisitada

void trocaPosicao(int ID,int posicao);  //bota a pagina selecionada na memoria
void acessaMemoria(int ID);  //escolhe a pagina que vai ser usada e verifica se ela já está na memoria fisica

void *fazIO(void *r);  //Thread que é criada para cada uso de recurso requisitado pelo DMA
void *DMA(void* ID);  //Le as requisições do spool e atende elas se possivel



vector <Processo> tabelaDeProcessos;
Moldura molduras[NMOLDURAS];

int totalDePrioridade=0,id=1,metodo = 0;
int mudancaMoldura=0; //usado para ver qual moldura foi modificada a mais tempo primeira moldura tem idade 0,a terceira 2 etc...

pthread_mutex_t mutexTabelaDeProcessos,mutexDispositivos[NDISPOSITIVOS];
bool FINAL,dispositosLivres[NDISPOSITIVOS];

queue <Requisicao> spool;
pthread_mutex_t mutexDoSpool;
const int CAPACIDADEDMA=4;
pthread_t threadUsaDispositivo[10000];
int usadosDMA=0,ultimaReq=0;

int main(int argc, char *argv[]){
    if(argc>1){
        if(argv[1][1]=='l') //-l para loteria
            metodo=0;
        if(argv[1][1]=='p') //-p para prioridade
            metodo = 1;
        if(argv[1][1]=='c')  // -c para circular
            metodo =2;
    }
    for(int x=0;x<NDISPOSITIVOS;x++){
        pthread_mutex_init(&mutexDispositivos[x], NULL);
        dispositosLivres[x]=true;
    }
    pthread_mutex_init(&mutexTabelaDeProcessos, NULL);
    pthread_mutex_init(&mutexDoSpool, NULL);
    srand((unsigned)time(NULL));
    pthread_t escalonador;
    pthread_create(&escalonador,NULL,&rodaEscalonador,NULL);
    pthread_t leitor;
    pthread_create(&leitor,NULL,&criadorProcessos,NULL);
    pthread_t threadDMA;
    pthread_create(&threadDMA,NULL,&DMA,NULL);
    FINAL = false;
    for(int x=0;x<NMOLDURAS;x++){
        molduras[x].IDprocesso=-1;//inicializa as molduras de memoria como vazias
        molduras[x].paginaDoProcesso=-1;
    }
    while(!FINAL); //espera o fim da execução
    pthread_exit(NULL);
}

void printMemoria(){
    for(int x=0;x<NMOLDURAS;x++){
        fprintf(stderr,"% 2d(%d) ",molduras[x].IDprocesso,molduras[x].paginaDoProcesso);
    }
    fprintf(stderr,"\n");
}

void rodaProcesso(int ID){
    int p=findID(ID);
    //fprintf(stderr,"rodando processo %s, faltam %d\n",tabelaDeProcessos[p].nome,tabelaDeProcessos[p].tempoRestante);
    if(!tabelaDeProcessos[p].bloqueado){
        int acessosMemoria=rand()%10+1,tempo;
        if(rand()%100<50){ //50% de chance de fazer IO
            Requisicao req;
            req.IDprocesso=ID;
            req.dispositivo=rand()%NDISPOSITIVOS;
            fprintf(stderr,"%s requisitando dispositivo %d IO\n",tabelaDeProcessos[p].nome,req.dispositivo);
            pthread_mutex_lock(&mutexDoSpool);
            spool.push(req);
            pthread_mutex_unlock(&mutexDoSpool);
            tempo=rand()%3;
        }else{
            tempo=5;
        }
        tempo=min(tempo,tabelaDeProcessos[p].tempoRestante);  //Se o tempo que falta rodar for menor que o arbitrado

       /* acessosMemoria=min(acessosMemoria,tempo); //Se não houver tempo para todos acessos, o numero de acessos feitos é o tempo (1 segundo por acesso)
        for(int x=0;x<acessosMemoria;x++){
            acessaMemoria(ID);
            printMemoria();
        }
        if(tempo>acessosMemoria){
            usleep (CONSTANTE*(tempo-acessosMemoria));  //dorme o tempo que sobrou
        }*/
        //###############################################################
        //###PAMONHA DA MEMORIA COMENTADA PARA LEGIBILIDADE DOS PRINTS###
        //###############################################################
        if(tabelaDeProcessos[p].tempoRestante > tempo)
            tabelaDeProcessos[p].tempoRestante-=tempo;
        else
            apagaProcesso(ID);
    }else{
        fprintf(stderr,"processo %s bloqueado\n",tabelaDeProcessos[p].nome);
    }
    usleep(10000);
}

void acessaMemoria(int ID){
    int p = findID(ID);
    int posicao = rand()%tabelaDeProcessos[p].tabelaDePaginas.size()+1; //escolhe uma pagina da memoria virtual
    if(tabelaDeProcessos[p].tabelaDePaginas[posicao]==-1){
        fprintf(stderr,"posicão %d do processo %s não está na memoria, adicionando\n",posicao,tabelaDeProcessos[p].nome);
        trocaPosicao(ID,posicao);
    }else{
        fprintf(stderr,"posição %d do processo %s está na memoria\n",posicao,tabelaDeProcessos[p].nome);
    }
    usleep(CONSTANTE);
}

void trocaPosicao(int ID,int posicao){
    int p = findID(ID);
    int menor=0;
    bool breakou=false;
    for(int x=0;x<NMOLDURAS;x++){
        if(molduras[x].IDprocesso==-1){
            //fprintf(stderr,"encontrou espaco %d livre na memoria fisica\n",x);
            menor=x;
            breakou=true;
            break;
        }
        if(molduras[x].idade<molduras[menor].idade){
            menor=x;
        }
    }
    if(!breakou){
        //fprintf(stderr,"substituindo posição %d da memoria, processo atual nela = %d\n",menor,molduras[menor].processo);
        tabelaDeProcessos[molduras[menor].IDprocesso].tabelaDePaginas[molduras[menor].paginaDoProcesso]=-1; //marca o espaço da tabela como livre
    }

    mudancaMoldura++;
    molduras[menor].idade=mudancaMoldura;
    molduras[menor].IDprocesso=ID;
    molduras[menor].paginaDoProcesso=posicao;
    tabelaDeProcessos[p].tabelaDePaginas[posicao]=menor;

}

void *DMA(void* p){
    while(true){
        pthread_mutex_lock(&mutexDoSpool); //trava o mutex para ler o spool
        if(!spool.empty()){
            Requisicao req=spool.front();  //pega a primeira requisicao
            spool.pop();
            printf("leu requisicao do processo %s para o dispositivo %d\n",tabelaDeProcessos[findID(req.IDprocesso)].nome,req.dispositivo);
            if (pthread_mutex_trylock(&mutexDispositivos[req.dispositivo]) == 0 && usadosDMA<CAPACIDADEDMA){
                pthread_create(&threadUsaDispositivo[ultimaReq],NULL,&fazIO,(void*)&req);  //se passou desse if o mutex já está bloqueado e pode tocar lata
                usadosDMA++;
                ultimaReq++;
            }else{
                if(usadosDMA==CAPACIDADEDMA){
                    fprintf(stderr,"DMA cheio, sleepando.\n");
                }else{
                    fprintf(stderr,"dispositivo %d bloqueado :( movendo requisicao para o fim do spool \n",req.dispositivo);
                    spool.push(req);
                }
            }
        }else{
            fprintf(stderr,"Spool vazia\n");
        }
        pthread_mutex_unlock(&mutexDoSpool); //libera o spool
        usleep(1000000); // sleep para deixar que os outros threads usem o spool
    }
}

void *fazIO(void *r){
    Requisicao* req=(Requisicao*) r;
    int tempo=rand()%4+1;  //arbitra o tempo
        fprintf(stderr,"dispositivo livre! %s Usando o dispositivo %d.\n",tabelaDeProcessos[findID(req->IDprocesso)].nome,req->dispositivo);
        tabelaDeProcessos[findID(req->IDprocesso)].bloqueado=true;  //bloqueia o processo
        usleep(CONSTANTE*tempo);  //"faz" a entrda e saida  (não pode ser usleep se nao bloqueia in
        tabelaDeProcessos[findID(req->IDprocesso)].bloqueado=false;  //desbloqueia o processo
        fprintf(stderr,"%s terminou de usar dispositivo %d\n",tabelaDeProcessos[findID(req->IDprocesso)].nome,req->dispositivo);
    pthread_mutex_unlock(&mutexDispositivos[req->dispositivo]);  //libera o dispositivo
    pthread_mutex_lock(&mutexDoSpool);
        usadosDMA--;
    pthread_mutex_unlock(&mutexDoSpool);
    pthread_exit(NULL);
}

void *rodaEscalonador(void *thread){
    while(!FINAL){
        pthread_mutex_lock(&mutexTabelaDeProcessos);
        if(metodo==0)
            loteria();
            else
            if(metodo ==1)
                escalonaPrioridade();
                else
                    circular();
        pthread_mutex_unlock(&mutexTabelaDeProcessos);
            usleep(CONSTANTE);
    }
    pthread_exit(NULL);

}

void *criadorProcessos(void *processo){
    while(!FINAL){
        Processo temp;
        scanf("%s %d",temp.nome,&temp.tempoTotal)   ;

        if(temp.tempoTotal==0){
            FINAL=true;
            break; //para a execução
        }
        temp.tempoRestante=temp.tempoTotal;
        temp.bloqueado=false;
        temp.prioridade=rand()%10+1;
        temp.IDprocesso=id;
        temp.tabelaDePaginas.resize(rand()%NMOLDURAS*2+1);  //arbitra o tamanho da memoria virtual do programa
        for(int x=0;x<temp.tabelaDePaginas.size();x++){
            temp.tabelaDePaginas[x]=-1; //marca todas como não estando na memoria;
        }
        id++;
        fprintf(stderr,"criou processo %s com prioridade %d de duração %d\n",temp.nome,temp.prioridade,temp.tempoTotal);
        pthread_mutex_lock(&mutexTabelaDeProcessos);
        tabelaDeProcessos.push_back(temp);
        totalDePrioridade+=temp.prioridade; // só serve para o loteria
        pthread_mutex_unlock(&mutexTabelaDeProcessos);
    }
    pthread_exit(NULL);
}

void printBilhetes(){
    int acumulador=0;
    for(int x=0;x<tabelaDeProcessos.size();x++){
        fprintf(stderr,"%s(%d,%d] ",tabelaDeProcessos[x].nome,acumulador,acumulador+tabelaDeProcessos[x].prioridade);
        acumulador+=tabelaDeProcessos[x].prioridade;
    }
    fprintf(stderr,"\n");
}

void printPrioridades(){
    fprintf(stderr,"PRIORIDADES:\n");
    for(int x=0;x<tabelaDeProcessos.size();x++){
        fprintf(stderr,"%s : %d\n",tabelaDeProcessos[x].nome,tabelaDeProcessos[x].prioridade);
    }
    fprintf(stderr,"\n");
}

void loteria(){
    //printBilhetes();
    if(totalDePrioridade==0)
        return;

    int vaiRodar=rand()%totalDePrioridade+1,acumulador=0,x;
    for(x=0;x<tabelaDeProcessos.size();x++){
        acumulador+=tabelaDeProcessos[x].prioridade;
        if(vaiRodar<=acumulador){
            break;
        }
    }
    fprintf(stderr,"escolheu bilhete %d proceso  %s\n",vaiRodar,tabelaDeProcessos[x].nome);
    rodaProcesso(tabelaDeProcessos[x].IDprocesso);
}

bool maiorPrioridade(Processo a,Processo b){
    if(a.bloqueado) return false; //gambiarra medonha para por processos bloquados no final em vez de ficar tentando rodar eles e nao fazer nada
    if(b.bloqueado) return true;
    return a.prioridade>b.prioridade;
}

void escalonaPrioridade(){
    sort(tabelaDeProcessos.begin(),tabelaDeProcessos.end()+1,maiorPrioridade);  //bota o processode maior prioridade no começo
    if(tabelaDeProcessos.size()!=0)
        for(int x=0;tabelaDeProcessos[x].prioridade==tabelaDeProcessos[0].prioridade && x<tabelaDeProcessos.size();x++){  //alterna entre todos de maior prioridade
            fprintf(stderr,"rodando %s\n",tabelaDeProcessos[x].nome);
            rodaProcesso(tabelaDeProcessos[x].IDprocesso);
        }
}

void circular(){
    for(int x=0;x<tabelaDeProcessos.size();x++){
        rodaProcesso(tabelaDeProcessos[x].IDprocesso);
    }
}

int findID(int ID){
    for(int x=0;x<tabelaDeProcessos.size();x++){
        if(tabelaDeProcessos[x].IDprocesso==ID)
            return x;
    }
    return -1;
}

void apagaProcesso(int ID){
    int p = findID(ID);
    fprintf(stderr,"%s terminou\n",tabelaDeProcessos[p].nome);
    totalDePrioridade-=tabelaDeProcessos[p].prioridade; //só serve para o loteria
    for(int x=0;x<NMOLDURAS;x++){  //marca as posicoes ocupadas pelo processo como livres(-1)
        if(molduras[x].IDprocesso==ID)
            molduras[x].IDprocesso=-1;
    }
    printMemoria();
    pthread_mutex_lock(&mutexTabelaDeProcessos);
    tabelaDeProcessos.erase(tabelaDeProcessos.begin()+p);
    pthread_mutex_unlock(&mutexTabelaDeProcessos);
}
