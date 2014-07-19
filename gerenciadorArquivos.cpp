#include <bits/stdc++.h>
using namespace std;

const int blocosPorINode=2;

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
const int tamBloco= 2;
const int numeroInodes = 256;
const int numeroBlocos = 256;
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
void chmod(int posicao,char * permissoes);
bool verificaPermissao(int posicao,int operacao);

int makeINode(bool dir,char * nome,bool expansao,int pai){
    //printf("possiblidade de treta\n");
    int pos=findInodeLivre();
    INode *novo = getINode(pos);
    *getMapInode(pos)=true;
    strcpy(novo->nome,nome);
    novo->diretorio=dir;
    time(&novo->ultimaModificacao);  //FIXME: não funciona para criar inodes que serão usados como expansões de um arquivo o pai do arquivo tá errado que faz o adiociona referencia fazer merda
    limpaReferencias(pos);
    novo->pai=pai;
    printf("nao é treta\n");
    novo->expansao=expansao;
    novo->tamanho=0;
    novo->dono=usuarioAtual;
    novo->grupo=-1;
    chmod(pos,(char*)"331");
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
        int pos=makeINode(true,nome,false,diretorioAtual);
        getINode(getINode(pos)->pai)->tamanho++;
        adicionaReferencia(diretorioAtual,pos);
        printf("criou diretorio %s\n",getINode(pos)->nome);
    }else{
        printf("este nome já foi usado\n");
    }
}

void touch(char* nome){
    if(!nomeUsado(nome)){
        int pos=makeINode(false,nome,false,diretorioAtual);
        getINode(getINode(pos)->pai)->tamanho++;
        adicionaReferencia(diretorioAtual,pos);
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
    if(verificaPermissao(pos,1)){
        int tam= strlen(texto);
        liberaBlocos(pos);  //desfaz todas associações de blocos que o arquivo fez e sobrescreve com o novo texto, alocando apenas os textos necessarios
        INode * inode=getINode(pos);
        int bloco=1337; //inicio inutil
        printf("tam = %d",tam);
        for(int c=0;c<=tam;c++){  //<= pq o \0 tem que entrar
            printf("c= %d",c);
            if(c%tamBloco==0){
                bloco=findBlocoLivre();
                *getMapBlocos(bloco)=true;
                printf("deu merda aqui\n");
                adicionaReferencia(pos,bloco);  //aloca um bloco novo quando encher o anterior e cria a referencia para ele
                printf("deu merda depois daqui\n");
            }
            //printf("%c",texto[c]);
            *(getBloco(bloco)+c%tamBloco)=texto[c];
        }
    }else{
        printf("voce nao tem permissão para escrever nesse arquivo\n");
    }
}

void cat(char* nome){
    int pos = findPosINode(nome);
    if(verificaPermissao(pos,0)){
        INode* inode = getINode(pos);
        char* buffer=(char*)"nada";  // nada é pq o for tem que inicializar com alguma coisa no buffer se nao dá segfault
        for(int i=0,blocoAtual=0;inode->enderDireto[blocoAtual]!=-1;i++){
            if(i%tamBloco==0){
                buffer=getBloco(inode->enderDireto[blocoAtual]);
                blocoAtual++;
                //printf("\n troca bloco \n");
                if(blocoAtual==blocosPorINode){
                    blocoAtual=0;
                    if(inode->enderIndireto!=-1)
                        inode=getINode(inode->enderIndireto);
                    else
                        break;
                    //printf("\n troca inode \n");
                }
            }
            printf("%c",*(buffer+i%blocosPorINode));
        }
        printf("\n");
    }else{
        printf("Voce nao tem permissão para ler esse arquivo\n");
    }
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
    int pos = findPosINode(nome);
    if(verificaPermissao(pos,1)){
        INode* removido=getINode(pos);
        INode* pai=getINode(removido->pai);
        if(removido->tamanho==0){
            pai->tamanho--;
            removeInode(findPosINode(nome));
            removeReferencia(removido->pai,findPosINode(nome));
        }else{
            printf("A pasta %s não está vazia\n",nome);
        }
    }else{
        printf("voce nao tem permissão de escrita sobre essa pasta\n");
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
    char*nomeTemp;
    if(strcmp(nome,(char*)"..")==0 && inode->pai!=-1){
        diretorioAtual=inode->pai;
        return;
    }
    int pos=findPosINode(nome);
    if(verificaPermissao(pos,0)){
        if(pos!=-1 && getINode(pos)->diretorio){
            printf("mudou diretorio com sucesso\n");
            printf("diretorio atual muda de %d para ",diretorioAtual);
            diretorioAtual=pos;
            printf("%d\n",diretorioAtual);
        }
    }else{
        printf("voce nao tem permissão de leitura para esta pasta\n");
    }
}

void adicionaReferencia(int pai,int ref){
    INode * inodePai=getINode(pai);
    for(int x=0;x<blocosPorINode;x++){
        if(inodePai->enderDireto[x]==-1){
            printf("entrou no if da escrita direta\n");
            inodePai->enderDireto[x]=ref;
            return;
        }
    }
    //se todas referencias diretas já foram feitas, usa a indireta
    if(inodePai->enderIndireto!=-1){
        adicionaReferencia(inodePai->enderIndireto,ref);
    }else{
        inodePai->enderIndireto=makeINode(inodePai->diretorio,(char*)"expansao",true,pai);//caso o inode esteja cheio, é criada uma
        printf("criou o inode novo na posicao %d\n",inodePai->enderIndireto);
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
                    printf("dir- ");
                }else{
                    printf("arq- ");
                }

                for(int y=0;y<9;y++){
                    printf("%d",inode->permissoes[y]);
                    if((y+1)%3==0)
                        printf(" ");
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
    disco = (char*) malloc(tamDisco);
    printf("antes\n");
    int pos = makeINode(true,(char*)"raiz",false,-1);
    chmod(pos,(char*)"777"); //todo mundo pode usar o raiz
    printf("depois\n");
    getINode(pos)->pai=-1; //raiz nao tem pai
    getINode(pos)->dono= -1;  //nem dono
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
    //printf("posicao = %d\n",posicao);
    char * hurr = (char*) disco+posicao;

    return (INode*) hurr    ;
}

char* getBloco(int bloco){
    if(bloco<0 || bloco>tamGerVazioBlocos){
        printf("bloco invalido\n");
        return NULL;
    }
    int posicao=tamGerVazioBlocos+tamGerVazioInodes+tamGrupos+tamUsuarios+tamINodes+tamBloco*bloco;
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
    printf("digite a senha do novo usuario: ");
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
    printf("grupo heio\n");
}

bool * toBinario(char c){
    c=c-'0';
    bool retorno[3];
    retorno[0]=(c&1==1);
    retorno[1]=(c&2==2);
    retorno[2]=(c&4==4);
    return retorno;
}

void chmod(int posicao,char * permissoes){
    bool *arrayPermissoes;
    INode * inode=getINode(posicao);
    if(posicao==0 || inode->dono==usuarioAtual){  // só pode dar chmod se for o dono do arquivo ou estiver criando a raiz
        for(int x=0;x<3;x++){
            arrayPermissoes=toBinario(permissoes[x]);
            for(int y=0;y<3;y++){
                inode->permissoes[3*x+y]=arrayPermissoes[y];
            }
        }
    }else{
        printf("voce nao tem permissao para alterar as permissoes desse arquivo\n");
    }
}

bool pertenceAoGrupo(int posicao){
    Grupo * grupo = getGrupo(posicao);
    for(int x=0;x<usuariosPorGrupo;x++){
        if(grupo->membro[x]==usuarioAtual)
            return true;
    }
    return false;
}

bool verificaPermissao(int posicao,int operacao){
    INode * inode = getINode(posicao);
    if(inode->dono==usuarioAtual){
        if(inode->permissoes[operacao]){
            return true;
        }
    }
    if(pertenceAoGrupo(inode->grupo)){
        if(inode->permissoes[3+operacao])
            return true;
    }
    return inode->permissoes[6+operacao]; //se nenhum deu retorna a permissao pro resto
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
        printf("faça login(login) ou crie um usuario(novoUsuario): \n");
        scanf("%s",entrada);
        if(strcmp(entrada,(char*)"login")==0){
            printf("\ninforme seu usuario:");
            scanf("%s",entrada);
            logado = login(entrada); //login retorna true quando consegue logar
        }
        if(strcmp(entrada,(char*)"novoUsuario")==0){
            printf("escolha o nome do novo usuario: ");
            scanf(" %s",entrada);
            novoUsuario(entrada);
        }
        if(strcmp(entrada,(char*)"novoGrupo")==0){
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
            if(strcmp(entrada,(char*)"chmod")==0){
                char nome[32],permissao[10];
                scanf(" %s %s",permissao,nome);
                int pos = findPosINode(nome);
                chmod(pos,permissao);
            }
        }
    }
    printf("bugou aqui\n");
}
