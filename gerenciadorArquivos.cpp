    #include <bits/stdc++.h>
using namespace std;

const int blocosPorINode=8;

struct INode{
    char nome[32];
    bool diretorio; //false para arquivo, true para diretorio
    time_t ultimaModificacao;
    short int referencias;
    int tamanho; //se for arquivo, tamanho do arquivo, se for uma pasta, numero de arquivos na pasta
    int pai;
    int enderDireto[blocosPorINode];
    int enderIndireto;
    bool expansao;
    int grupo;
    int dono;
    bool permissoes[9];
};

struct Usuario{
    char nome[32];
    char senha[32];
    bool livre;
};

const int usuariosPorGrupo=4;
struct Grupo{
    char nome[32];
    int membro[usuariosPorGrupo];
    bool livre;
};

const int tamDisco=32*1024;
const int tamBloco= 8;
const int numeroInodes = 128;
const int numeroBlocos = 64;
const int numeroUsuarios=16;
const int tamUsuarios=numeroUsuarios*sizeof(Usuario);
const int numeroGrupos=8;
const int tamGrupos=numeroGrupos*sizeof(Grupo);
const int tamINodes= sizeof(INode)*numeroInodes;
const int tamGerVazioInodes = numeroInodes;
const int tamGerVazioBlocos=numeroBlocos;
const int tamEspacoBlocos= numeroBlocos*tamBloco;

int diretorioAtual=0;
FILE *arquivo;
char *disco;
int usuarioAtual;

void liberaBlocos(int pos);
Usuario * getUsuario(int posicao);
Grupo * getGrupo(int posicao);
char * geraArvoreDiretorio();  //uma pamonha de funções que precisavam ser chamadas fora de ordem
int findInodeLivre();
bool* getMapInode(int posicao);
INode* getINode(int posicao);
void adicionaReferencia(int pai,int ref);
void limpaReferencias(int pos);
int findPosINode(char* nome);
int findBlocoLivre();
bool*getMapBlocos(int posicao);
char* getBloco(int bloco);

int makeINode(bool dir,char * nome,bool expansao){
    int pos=findInodeLivre();
    printf("pos = %d\n",pos);
    INode *novo = getINode(pos);
    *getMapInode(pos)=true;
    printf("deu merda aqui\n");
    strcpy(novo->nome,nome);
    novo->diretorio=dir;
    novo->pai=diretorioAtual;
    time(&novo->ultimaModificacao);
    limpaReferencias(pos);
    if(pos!=0)  //se for a raiz não tem pai
        adicionaReferencia(novo->pai,pos);
    novo->expansao=expansao;
    novo->tamanho=0;
    novo->dono=usuarioAtual;
    novo->grupo=-1;
    return pos;
}

bool nomeUsado(char* nome ){
    INode* inode= getINode(diretorioAtual);
    while(true){
        for(int x=0;x<blocosPorINode;x++){
            if(inode->enderDireto[x]!=-1){
                if(strcmp(getINode(inode->enderDireto[x])->nome,nome)==0){
                    return true;
                }
            }
        }
        if(inode->enderIndireto!=-1){
            inode=getINode(inode->enderIndireto);
        }else{
            break;
        }
    }
    return false;
}

void mkdir(char * nome){
    if(!nomeUsado(nome)){
        int pos=makeINode(true,nome,false);
        getINode(getINode(pos)->pai)->tamanho++;
        printf("criou diretorio %s\n",getINode(pos)->nome);
    }else{
        printf("este nome já foi usado\n");
    }
}

void touch(char* nome){
    if(!nomeUsado(nome)){
        int pos=makeINode(false,nome,false);
        getINode(getINode(pos)->pai)->tamanho++;
        printf("criou arquivo %s\n",getINode(pos)->nome);
    }else{
        printf("este nome já foi usado\n");
    }
}

void removeInode(int pos){
    INode* inode = getINode(pos);
    if(inode->enderIndireto!=-1){
        removeInode(inode->enderIndireto);
    }
    *getMapInode(pos)=false;
}

void echo(char * texto,char*nome){
    int pos = findPosINode(nome);
    int tam= strlen(texto);
    liberaBlocos(pos);  //desfaz todas associações de blocos que o arquivo fez e sobrescreve com o novo texto, alocando apenas os textos necessarios
    INode * inode=getINode(pos);
    int bloco=1337; //inicio inutil

    for(int c=0;c<=tam;c++){  //<= pq o \0 tem que entrar
        if(c%tamBloco==0){
            if(c%(tamBloco*blocosPorINode)==0 && c!=0){
                printf("\nadicionando novo inode , c= %d, blocos por inode =%d, bloco = %d",c,blocosPorINode,bloco);
                int novo=makeINode(false,(char*)"expansao",true);
                inode->enderIndireto=novo;
                inode=getINode(novo);
                //rintf("\n trocou inode \n");
            }
            bloco=findBlocoLivre();
            *getMapBlocos(bloco)=true;
            adicionaReferencia(pos,bloco);  //aloca um bloco novo quando encher o anterior e cria a referencia para ele
            //printf("\n trocou bloco\n");
        }
        //printf("%c",texto[c]);
        *(getBloco(bloco)+c%tamBloco)=texto[c];
    }
}

void cat(char* nome){
    INode* inode = getINode(findPosINode(nome));
    char* buffer=(char*)"nada";  // nada é pq o for tem que inicializar com alguma coisa no buffer se nao dá segfault
    for(int i=0,blocoAtual=0;*(buffer+i%tamBloco)!='\0' && inode->enderDireto[0]!=-1;i++){
        if(i%tamBloco==0){
            buffer=getBloco(inode->enderDireto[blocoAtual]);
            blocoAtual++;
            //printf("\n troca bloco \n");
            if(blocoAtual==blocosPorINode){
                blocoAtual=0;
                inode=getINode(inode->enderIndireto);
                //printf("\n troca inode \n");
            }
        }
        printf("%c",*(buffer+i%blocosPorINode));
    }
    printf("\n");
}

void liberaBlocos(int pos){
    INode* inode=getINode(pos);
    do{
        for(int x=0;x<blocosPorINode;x++){
            *getMapBlocos(inode->enderDireto[x])=false;
            inode->enderDireto[x]=-1;
        }
        if(inode->enderIndireto!=-1){
            inode=getINode(inode->enderIndireto);
        }else{
            break;
        }
    }while(true);
    if(inode->enderIndireto!=-1)
        removeInode(inode->enderIndireto);   //caso tenha referencia para outro inode, libera todos esses
}

void removeReferencia(int pai,int ref){
    INode*inode;
    do{
        inode = getINode(pai);
        for(int x=0;x<blocosPorINode;x++){
            if(inode->enderDireto[x]==ref){
                inode->enderDireto[x]=-1;
                return;
            }
        }
    }while(inode->enderIndireto!=-1);
    printf("Não foi encontrada a referencia requisitada no inode pai\n");
    return;
}

void rmdir(char * nome){
    //printf("posicao do inode a ser removido %d\n",findPos(nome));
    INode* removido=getINode(findPosINode(nome));
    INode* pai=getINode(removido->pai);
    if(removido->tamanho==0){
        pai->tamanho--;
        removeInode(findPosINode(nome));
        removeReferencia(removido->pai,findPosINode(nome));
    }else{
        printf("A pasta %s não está vazia\n",nome);
    }
}

int findPosINode(char* nome){
    //printf("procurando inode com nome : %s\n",nome);
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

int findPosUsuario(char * nome){
    for(int x=0;x<numeroUsuarios;x++){
        if(strcmp(getUsuario(x)->nome,nome)==0){
            return x;
        }
    }
    printf("usuario nao encontrado\n");
    return -1;
}

int findPosGrupo(char *nome){
    for(int x=0;x<numeroGrupos;x++){
        if(strcmp(getGrupo(x)->nome,nome)==0){
            return x;
        }
    }
    printf("grupo nao encontrado\n");
    return -1;
}

void cd(char*nome){
    INode* inode=getINode(diretorioAtual);
    int pos;
    char*nomeTemp;
    if(strcmp(nome,(char*)"..")==0 && inode->pai!=-1){
        diretorioAtual=inode->pai;
        return;
    }
    pos=findPosINode(nome);
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
        nome=temp->nome;
        dir=nome+"/"+dir;
        temp=getINode(temp->pai);
    }
    return (char*) &(dir[0]);
}

bool* getMapInode(int posicao){
    int aux=tamGerVazioBlocos + posicao;
    return (bool*) disco+aux;
}

bool*getMapBlocos(int posicao){
    return (bool*) disco+posicao;
}

void limpaUsuarios(){
    for(int x=0;x<numeroUsuarios;x++){
        getUsuario(x)->livre=true;
        strcpy(getUsuario(x)->nome,(char*)"vazio");
    }
}

void limpaGrupos(){
    for(int x=0;x<numeroGrupos;x++){
        getGrupo(x)->livre=true;
        for(int y=0;y<usuariosPorGrupo;y++){
            getGrupo(x)->membro[y]=-1;
        }
    }
}

void criaSistemaDeArquivos(){
    disco = (char*) calloc(tamDisco,1);
    printf("antes\n");
    int pos = makeINode(true,(char*)"raiz",false);
    printf("depois\n");
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

    limpaGrupos();
    limpaUsuarios();
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

int findBlocoLivre(){
    for(int i=0;i<tamGerVazioBlocos;i++){
        if(*getMapBlocos(i)==false){
            printf("encontrou bloco %d livre\n",i);
            return i;
        }
    }
    printf("todos blocos estão ocupados\n");
    return -1;
}

INode* getINode(int posicao){
    if(posicao<0 || posicao>tamGerVazioInodes){
        printf("inode invalido\n");
        return NULL;
    }
    posicao=tamGerVazioBlocos+tamGerVazioInodes+tamUsuarios+tamGrupos+(sizeof(INode)*posicao);

    INode * teste=(INode*) disco+1203;
    strcpy(teste->nome,(char*)"pamonha");
    printf("%s\n",teste->nome);

    return (INode*) disco+posicao;
}

char* getBloco(int bloco){
    if(bloco<0 || bloco>tamGerVazioBlocos){
        printf("bloco invalido\n");
        return NULL;
    }
    int posicao=tamGerVazioBlocos+tamGerVazioInodes+tamGrupos+tamUsuarios+tamINodes+tamBloco*bloco;
    printf("retornando posicao %d no getbloco\n",posicao);
    return disco+posicao;
}

Grupo * getGrupo(int posicao){
    return (Grupo*) disco+tamGerVazioBlocos+tamGerVazioInodes+tamUsuarios+posicao*sizeof(Grupo);
}

Usuario* getUsuario(int posicao){
    return(Usuario*) disco+tamGerVazioBlocos+tamGerVazioInodes+sizeof(Usuario)*posicao;
}

void printInfoDisco(){
    printf("Tamanho do disco :%d\ngerencimento dos blocos %d\ninodes:%d\nVazio dos inodes %d\nEspaço para os blocos %d\n Tamanho do inode %d\n",tamDisco,tamGerVazioBlocos,tamINodes,tamGerVazioInodes,tamEspacoBlocos,sizeof(INode));
}

bool login(char * nome){
    Usuario* usuario;
    char senha[64];
    for(int x=0;numeroUsuarios;x++){
        usuario=getUsuario(x);
        if(strcmp(usuario->nome,nome)==0){
            printf("digite a sua senha : ");
            scanf(" %s",senha);
            if(strcmp(usuario->senha,senha)==0){
                usuarioAtual=x;
                return true;
            }else{
                break;
            }
        }
    }
    return false;
}

int findUsuarioLivre(){
    for(int x=0;x<numeroUsuarios;x++){
        if(getUsuario(x)->livre){
            return x;
        }
    }
    printf("todos usuarios ocupados\n");
    return -1;
}

int findGrupoLivre(){
    for(int x=0;x<numeroGrupos;x++){
        if(getGrupo(x)->livre){
            return x;
        }
    }
    printf("nenhum grupo livre encontrado\n");
    return -1;
}

void novoUsuario(char* nome){
    Usuario * user=getUsuario(findUsuarioLivre());
    user->livre=false;
    strcpy(user->nome,nome);
    printf("digite a senha do novo usuario\n");
    scanf("%s",user->senha);
}

void novoGrupo(char * nome){
    Grupo * grupo = getGrupo(findGrupoLivre());
    strcpy(grupo->nome,nome);
    grupo->livre=false;
}

void insereNoGrupo(char* nomeGrupo,char* nome){
    int numGrupo=findPosGrupo(nomeGrupo);
    int numUser=findPosUsuario(nome);
    Grupo* grupo=getGrupo(numGrupo);
    for(int x=0;x<usuariosPorGrupo;x++){
        if(grupo->membro[x]==-1){
            grupo->membro[x]=numUser;
            return;
        }
    }
    printf("grupo cheio\n");
}

int main(){
    printf("ponteiro de char : %d ponteiro de inode : %d",sizeof(char*),sizeof(INode*));
    printInfoDisco();
    printf("Bem vindo ao sistema de arquivos!\n");

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
    bool logado=false;
    while(true){
        scanf("%s",entrada);
        if(strcmp(entrada,(char*)"login")){
            scanf(" %s",entrada);
            logado = login(entrada); //login retorna true quando consegue logar
        }
        if(strcmp(entrada,(char*)"novoUsuario")){
            scanf(" %s",entrada);
            novoUsuario(entrada);
        }
        if(strcmp(entrada,(char*)"novoGrupo")){
            scanf(" %s",entrada);
            novoGrupo(entrada);
        }
        if(strcmp(entrada,(char*)"insereNoGrupo")==0){
            char nome[32],nomeGrupo[32];
            scanf("%s %s",nome,nomeGrupo);
            insereNoGrupo(nome,nomeGrupo);
        }

        while(logado){
            printf("%s@MacBook Pro Retina Display 50': %s~$ ",getUsuario(usuarioAtual)->nome,geraArvoreDiretorio());
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
            if(strcmp(entrada,(char*)"rmdir")==0){
                scanf(" %s",entrada);
                rmdir(entrada);
            }
            if(strcmp(entrada,(char*)"rm")==0){
                scanf(" %s",entrada);
                rmdir(entrada);
            }
            if(strcmp(entrada,(char*)"echo")==0){
                char* nome=(char*)malloc(512);
                scanf(" %s > %s",entrada,nome);
                echo(entrada,nome);
                free(nome);
            }
            if(strcmp(entrada,(char*)"cat")==0){
                scanf(" %s",entrada);
                cat(entrada);
            }
            if(strcmp(entrada,(char*)"logoff")==0){
                logado=false;
            }
        }
    }
    printf("bugou aqui\n");
}
