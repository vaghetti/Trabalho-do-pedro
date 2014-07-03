#include <bits/stdc++.h>
using namespace std;

const int blocosPorINode=7;

struct INode{
    char nome[64];
    bool diretorio; //false para arquivo, true para diretorio
    time_t ultimaModificacao;
    short int referencias;
    int tamanho; //se for arquivo, tamanho do arquivo, se for uma pasta, numero de arquivos na pasta
    int pai;
    int enderDireto[blocosPorINode];
    int enderIndireto;
};

const int tamDisco=128 *128; //128 é 1k de bytes
const int tamBloco=8; //8 bytes
const int tamINodes=128*sizeof(INode);  //128 é 1k de bytes
const int tamGerVazioInodes=tamINodes/sizeof(INode);
const int temp=(tamDisco-tamINodes-tamGerVazioInodes)/tamBloco;
const int tamEspacoBlocos=tamDisco-temp-tamGerVazioInodes-tamINodes;
const int tamGerVazioBlocos = (tamEspacoBlocos - temp)/tamBloco;



int INODESCRIADOS=0;
INode * diretorioAtual;
char nomeUsuario[32];
FILE *arquivo;
char *disco;
bool *mapaInodes;
bool *mapaBlocos;


char * geraArvoreDiretorio();

char* getInput(){
    char *entrada=(char*)malloc(512);
    printf("%s@Computador@%s: ",nomeUsuario,geraArvoreDiretorio());
    scanf("%s",entrada);
    return entrada;
}

void mkdir(char*nome){

}
char * geraArvoreDiretorio(){
    INode *temp=diretorioAtual;
    string dir="/",nome;
    printf("gerando string de nome: \n");
    while (temp->pai!=-1){
        printf("bugou aqui\n");
        nome=temp->nome;
        dir="/"+nome+"/"+dir;
        cout<<dir<<endl;
    }
    return (char*) &(dir[0]);
}

bool* getMapInode(int posicao){
    posicao=tamGerVazioBlocos + posicao;
    return (bool*) disco+posicao;
}

bool*getMapBlocos(int posicao){
    return (bool*) disco+posicao;
}

void criaSistemaDeArquivos(){
    disco = (char*) calloc(tamDisco,1);
    INode *raiz = (INode*)disco + tamGerVazioBlocos+tamGerVazioInodes;
    strcpy(raiz->nome,"RAIZ");
    raiz->diretorio=true;
    time(&raiz->ultimaModificacao);
    raiz->pai=-1;
    diretorioAtual=raiz;

    for(int cont = 0; cont < tamGerVazioInodes; cont++){
        *getMapInode(cont) = false;
    }
    for(int cont = 0; cont < tamGerVazioBlocos; cont++){
        *getMapBlocos(cont) = false;
    }
    for(int x=0;x<tamGerVazioInodes;x++){
        printf("%d ",*getMapInode(x));
    }
    printf("\n");

}

INode* getINode(int posicao){
    posicao=tamGerVazioBlocos+tamGerVazioInodes+sizeof(INode)*posicao;
    return (INode*) disco+posicao;
}

void makeINode(int posicao,bool dir,char * nome){
    INode *novo = getINode(posicao);
    char temp[]="/";
    strcpy(novo->nome,nome);
    novo->diretorio=dir;
    time(&novo->ultimaModificacao);
}

void printInfoDisco(){
    printf("Tamanho do disco :%d\ngerencimento dos blocos %d\ninodes:%d\nVazio dos inodes %d\nEspaço para os blocos %d\n",tamDisco,tamGerVazioBlocos,tamINodes,tamGerVazioInodes,tamEspacoBlocos);
}

int main(){
    printInfoDisco();
    printf("Bem vindo ao sistema de arquivos!\nInforme seu nome: ");
    scanf("%s",nomeUsuario);
    char *entrada;


    arquivo=fopen("arquivo.bin","r+");
    if(arquivo==NULL){
        printf("O arquivo de disco ainda nao existia, criando.\n");
        arquivo=fopen("arquivo.bin","w");

    }else{
        printf("Arquivo aberto com sucesso\n");
    }
    criaSistemaDeArquivos();

    //disco=(char)malloc(tamDisco);
    //disco=mmap
    while(true){
        entrada=getInput();
        printf("leu entrada %s\n",entrada);
    }
    printf("bugou aqui\n");

}
