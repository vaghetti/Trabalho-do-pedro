#include <bits/stdc++.h>
using namespace std;
const int tamDisco=16*128; //128 é 1k de bytes
const int tamBloco=8; //8 bytes
const int tamGerINodes=2*128;  //128 é 1k de bytes
const int tamGerVazio=(tamDisco-tamGerINodes)/tamBloco;
const int blocosPorINode=7;
char diretorioAtual[256]="/";
char nomeUsuario[32];
FILE *arquivo;
char *disco;

struct INode{
    char nome[64];
    bool diretorio; //false para arquivo, true para diretorio
    time_t ultimaModificacao;
    short int referencias;
    int tamanho; //se for arquivo, tamanho do arquivo, se for uma pasta, numero de arquivos na pasta
    INode* enderDireto[blocosPorINode];
    INode* enderIndireto[1];
};

char* getInput(){
    char *entrada=(char*)malloc(512*sizeof(char));
    printf("%s@Computador@%s: ",nomeUsuario,diretorioAtual);
    scanf("%s",entrada);
    return entrada;
}

void criaSistemaDeArquivos(){
    disco = (char*) calloc(tamDisco,1);
    INode *raiz = (INode*) disco + tamGerVazio;
    strcpy(raiz->nome,"/");
    raiz->diretorio=true;
    time(&raiz->ultimaModificacao);

}
INode* get INode(int byte,int bit){
    return (INode*) disco +tamGerVazio
}
int main(){

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
    //disco=(char)malloc(tamDisco);
    //disco=mmap
    while(true){
        entrada=getInput();
        printf("leu entrada %s\n",entrada);
    }

}
