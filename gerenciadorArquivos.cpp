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
    bool expansao;
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
void adicionaReferencia(int pai,int ref);
void limpaReferencias(int pos);
int findPos(char* nome);

int makeINode(bool dir,char * nome,bool expansao){
    int pos=findInodeLivre();
    INode *novo = getINode(pos);
    *getMapInode(pos)=true;
    char temp[]="/";
    strcpy(novo->nome,nome);
    novo->diretorio=dir;
    novo->pai=diretorioAtual;
    time(&novo->ultimaModificacao);
    limpaReferencias(pos);
    if(pos!=0)  //se for a raiz não tem pai
        adicionaReferencia(novo->pai,pos);
    novo->expansao=expansao;
    return pos;
}

void mkdir(char * nome){
    int pos=makeINode(true,nome,false);
    printf("criou diretorio %s\n",getINode(pos)->nome);
}

void touch(char* nome){
    int pos=makeINode(false,nome,false);
    printf("criou arquivo %s",getINode(pos)->nome);
}

void rmdir(char * nome){
    INode * inode=getINode(findPos(nome));
    while(true){
        for(int x=0;x<blocosPorINode;x++){
            if(inode->enderDireto[x]!=-1){
                printf("diretorio nao está vazio\n");
                break;
            }
        }
        if(inode->pai!=-1)
            inode=getINode(inode->pai);
    }
}

int findPos(char* nome){
    INode* inode=getINode(diretorioAtual);
    char*nomeTemp;
    while(true){
        for(int x=0;x<blocosPorINode;x++){
            if(inode->enderDireto[x]!=-1 ){
                nomeTemp=getINode(inode->enderDireto[x])->nome;
                if(strcmp(nome,nomeTemp)==0){
                    return inode->enderDireto[x];
                }
            }
        }
        if(inode->enderIndireto!=-1){
            inode=getINode(inode->enderIndireto);
        }else{
            printf("arquivo não encontrado\n");
            return -1;
        }
    }
}
void cd(char*nome){
    INode* inode=getINode(diretorioAtual);
    int pos;
    char*nomeTemp;
    if(strcmp(nome,(char*)"..")==0 && inode->pai!=-1){
        diretorioAtual=inode->pai;
        return;
    }
    pos=findPos(nome);
    if(pos!=-1 && getINode(pos)->diretorio){
        printf("mudou diretorio com sucesso\n");
        printf("diretorio atual muda de %d para ",diretorioAtual);
        diretorioAtual=pos;
        printf("%d\n",diretorioAtual);
    }


}

void adicionaReferencia(int pai,int ref){
    INode * inodePai=getINode(pai);
    for(int x=0;x<blocosPorINode;x++){
        if(inodePai->enderDireto[x]==-1){
            inodePai->enderDireto[x]=ref;
            return;
        }
    }
    //se todas referencias diretas já foram feitas, usa a indireta
    if(inodePai->enderIndireto!=-1){
        adicionaReferencia(inodePai->enderIndireto,ref);
    }else{
        inodePai->enderIndireto=makeINode(inodePai->diretorio,(char*)"expansao",true);//caso o inode esteja cheio, é criada uma
        adicionaReferencia(inodePai->enderIndireto,ref);
    }

}

void limpaReferencias(int pos){
    INode* inode=getINode(pos);
    for(int x=0;x<blocosPorINode;x++){
        inode->enderDireto[x]=-1;
    }
    inode->enderIndireto=-1;
}
void ls(){
    INode* inode=getINode(diretorioAtual);
    while(true){
        for(int x=0;x<blocosPorINode;x++){
            if(inode->enderDireto[x]!=-1){
                if(getINode(inode->enderDireto[x])->diretorio){
                    printf("diretorio- ");
                }else{
                    printf("arquivo- ");
                }
                printf(" %s\n",getINode(inode->enderDireto[x])->nome);  //mostra o nome do inode relacionado, caso exista
            }
        }
        if(inode->enderIndireto!=-1){
            inode=getINode(inode->enderIndireto);
        }else{
            break;
        }
    }
}

char * geraArvoreDiretorio(){
    INode *temp=getINode(diretorioAtual);
    string dir="";
    string nome;
    while (temp->pai!=-1){
        //printf("bugou aqui\n");
        nome=temp->nome;
        dir=nome+"/"+dir;
        //cout<<dir<<endl;
        temp=getINode(temp->pai);
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
    /*INode *raiz = (INode*)disco + tamGerVazioBlocos+tamGerVazioInodes;
    strcpy(raiz->nome,"RAIZ");
    raiz->diretorio=true;
    time(&raiz->ultimaModificacao);
    raiz->pai=-1;*/
    int pos = makeINode(true,(char*)"raiz",false);
    getINode(pos)->pai=-1;

    diretorioAtual=pos;

    for(int cont = 1; cont < tamGerVazioInodes; cont++){  //inicializa todos espacoes do gerenciador de inodes vazios menos o primeiro que e a raiz
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
    return -1;
}

INode* getINode(int posicao){
    if(posicao<0){
        printf("inode invalido\n");
        return NULL;
    }
    posicao=tamGerVazioBlocos+tamGerVazioInodes+sizeof(INode)*posicao;
    return (INode*) disco+posicao;
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
    int teste=10;
    while(true){
        printf("%s@Computador: %s~",nomeUsuario,geraArvoreDiretorio());  //FIXME!!! nao sei pq mas quando tem isso fica num printf infinito
        scanf(" %s",entrada);
        printf("leu entrada %s\n",entrada);
        if(strcmp(entrada,(char*)"mkdir")==0){
            scanf(" %s",entrada);
            mkdir(entrada);
        }

        if(strcmp(entrada,"touch")==0){
            scanf(" %s",entrada);
            touch(entrada);
        }
        if(strcmp(entrada,(char*)"ls")==0){
            ls();
        }
        if(strcmp(entrada,(char*)"cd")==0){
            scanf(" %s",entrada);
            cd(entrada);
        }
        if(strcmp(entrada,(char*)"pwd")==0){
            printf("/%s\n",geraArvoreDiretorio());
        }
        if(strcmp(entrada,(char*)"rmdir")==0);
    }
    printf("bugou aqui\n");
}
