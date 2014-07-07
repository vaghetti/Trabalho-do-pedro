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
int diretorioAtual=0;
char nomeUsuario[32];
FILE *arquivo;
char *disco;
bool *mapaInodes;
bool *mapaBlocos;


char * geraArvoreDiretorio();
int findInodeLivre();
bool* getMapInode(int posicao);
INode* getINode(int posicao);

void mkdir(char * nomederp){
    int pos=findInodeLivre();
    *getMapInode(pos)=true;
    INode* novo=getINode(pos);
    strcpy(novo->nome,nomederp);
    novo->diretorio=true;
    novo->pai=diretorioAtual;
    printf("criou diretorio %s\n",novo->nome);
}


char * geraArvoreDiretorio(){
    INode *temp=getINode(diretorioAtual);
    string dir="/";
    string nome;
    while (temp->pai!=-1){
        printf("bugou aqui\n");
        nome=temp->nome;
        dir="/"+nome+"/"+dir;
        cout<<dir<<endl;
        temp=getINode(temp->pai);
    }
    char teste[]="teste";
    return teste;//(char*) &(dir[0]);
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
    diretorioAtual=0;

    for(int cont = 0; cont < tamGerVazioInodes; cont++){
        *getMapInode(cont) = false;
    }
    printf("Gerou mapa de Inodes\n");
    for(int cont = 0; cont < tamGerVazioBlocos; cont++){
        *getMapBlocos(cont) = false;
    }
    printf("gerou mapa de blocos\n");
}

int findInodeLivre(){
    for(int i=0;i<tamGerVazioInodes;i++){
        if(*getMapInode(i)==false){
            printf("encontrou inode %d livre\n",i);
            return i;
        }
    }
    printf("nao encontrou inode disponivel\n");
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
    char entrada[256];
    char temp[]="mkdir";
    int teste=10;
    while(teste>0){
        //printf("%s@Computador:~%s",nomeUsuario,geraArvoreDiretorio());  FIXME!!! nao sei pq mas quando tem isso fica num printf infinito
        scanf(" %s",entrada);
        printf("leu entrada %s\n",entrada);
        if(strcmp(entrada,temp)==0){
            char nomePasta[]="nome da pasta";
            printf("criando diretorio\n");
            mkdir(nomePasta);
        }
        teste--;
    }
    printf("bugou aqui\n");
}
