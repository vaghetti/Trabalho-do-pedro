#include <bits/stdc++.h>
#include <stdlib.h>
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

using namespace std;

const int blocosPorINode=3;

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

const int tamBloco= 2;
const int numeroInodes = 256;
const int numeroBlocos = 256;
const int tamINodes= sizeof(INode)*numeroInodes;
const int tamGerVazioInodes = numeroInodes;
const int tamGerVazioBlocos=numeroBlocos;
const int tamEspacoBlocos= numeroBlocos*tamBloco;
const int tamDisco=tamINodes+tamGerVazioInodes+tamEspacoBlocos+tamGerVazioBlocos;

int diretorioAtual=0;
//FILE *arquivo;
char *disco;
int usuarioAtual;

void liberaBlocos(int pos);
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
char * criptografa(char * senha);
void addUser(char* nome,char * senha);
vector <char * > split(char * entrada,char separador);

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
        printf("Criou diretorio %s\n",getINode(pos)->nome);
    }else{
        printf("este nome já foi usado\n");
    }
}

void touch(char* nome){
    if(!nomeUsado(nome)){
        int pos=makeINode(false,nome,false,diretorioAtual);
        getINode(getINode(pos)->pai)->tamanho++;
        printf("touch chamando adicionaReferencia para %s \n",nome);
        adicionaReferencia(diretorioAtual,pos);
        printf("Criou arquivo %s\n",getINode(pos)->nome);
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
    printf("texto recebido para gravação no echo : %s \n",texto);
    if(verificaPermissao(pos,1)){
        int tam= strlen(texto);
        liberaBlocos(pos);  //desfaz todas associações de blocos que o arquivo fez e sobrescreve com o novo texto, alocando apenas os textos necessarios
        INode * inode=getINode(pos);
        int bloco=1337; //inicio inutil
        //printf("tam = %d",tam);
        //printf("caracteres gravados pelo echo : ");
        for(int c=0;c<=tam;c++){  //<= pq o \0 tem que entrar
            if(c%tamBloco==0){
                bloco=findBlocoLivre();
                printf("\ntrocou para o bloco %d\n",bloco);
                *getMapBlocos(bloco)=true;

                adicionaReferencia(pos,bloco);  //aloca um bloco novo quando encher o anterior e cria a referencia para ele
            }

           printf("%c",texto[c]);
            *(getBloco(bloco)+c%tamBloco)=texto[c];
        }
        //printf("\n");
    }else{
        printf("voce nao tem permissão para escrever nesse arquivo\n");
    }

}

char* leArquivo(char* nome,bool chamadaDoSistema){
    int pos = findPosINode(nome);
    char * saida= (char*)malloc(256);
    memset(saida,'\0',256);
    if(chamadaDoSistema || verificaPermissao(pos,0)){
        INode* inode = getINode(pos);
        int i=0;
        while(true){
            for(int x=0;x<blocosPorINode && inode->enderDireto[x]!=-1;x++){
                char * bloco = getBloco(inode->enderDireto[x]);
                for(int y=0;y<tamBloco && bloco[y]!='\0';y++){
                    saida[i]=bloco[y];
                    i++;
                }
            }
            if(inode->enderIndireto!=-1){
                inode=getINode(inode->enderIndireto);
            }else{
                break;
            }
        }
        return saida;
    }else{
        printf("Voce nao tem permissão para ler esse arquivo\n");
    }

}

void cat(char* nome){
    printf("%s\n",leArquivo(nome,false));
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
        printf("voce nao tem permissão de escrita sobre este arquivo \n");
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

int findPosUsuario(char* nome){
    vector < char * > nomes = split(leArquivo((char*)"passwd",true),';');
    for(int x=0;x<nomes.size();x++){
        if(strcmp(split(nomes[x],':')[0],nome)==0){
            return x;
        }
    }
    printf("Usuario nao encontrado\n");
    return -1;
}

int findPosGrupo(char * nome){
    vector < char *> nomes = split(leArquivo((char*)"grupos",true),';');
    for(int x=0;x<nomes.size();x++){
        if(strcmp(split(nomes[x],',')[0],nome)==0){
            return x;
        }
    }
    printf("grupo nao encontrado \n");
}

void cd(char*nome){
    INode* inode=getINode(diretorioAtual);
    char*nomeTemp;
    if(strcmp(nome,(char*)"..")==0 && inode->pai!=-1){\
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
    //printf("adicionando referencia no inode %d(%s) para o bloco %d\n",pai,inodePai->nome,ref);
    for(int x=0;x<blocosPorINode;x++){
        if(inodePai->enderDireto[x]==-1){
            //printf("entrou no if da escrita direta\n"); // dando merda
            inodePai->enderDireto[x]=ref;
            return;
        }
    }
    //se todas referencias diretas já foram feitas, usa a indireta
    if(inodePai->enderIndireto!=-1){
       // printf("adionando referencia recursivamente");
        adicionaReferencia(inodePai->enderIndireto,ref);
    }else{
      //  printf("criou o inode novo na posicao %d\n",inodePai->enderIndireto);
        inodePai->enderIndireto=makeINode(inodePai->diretorio,(char*)"expansao",true,pai);//caso o inode esteja cheio, é criada uma
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
                    printf("%d",getINode(inode->enderDireto[x])->permissoes[y]);
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

void criaSistemaDeArquivos(){
    disco = (char*) malloc(tamDisco);
    int pos = makeINode(true,(char*)"raiz",false,-1);
    chmod(pos,(char*)"777"); //todo mundo pode usar o raiz
    getINode(pos)->pai=-1; //raiz nao tem pai
    getINode(pos)->dono= -1;  //nem dono
    diretorioAtual=pos;
    for(int cont = 1; cont < tamGerVazioInodes; cont++){  //inicializa todos espacoes do gerenciador de inodes vazios menos o primeiro que e a raiz
        *getMapInode(cont) = false;
    }
    for(int cont = 0; cont < tamGerVazioBlocos; cont++){
        *getMapBlocos(cont) = false;
    }
    touch((char*)"passwd");
    addUser((char*)"admin",(char*)"admin");
    touch((char*)"grupos");
    printf("Criou sistema de arquivos\n");
}

int findInodeLivre(){
    for(int i=0;i<tamGerVazioInodes;i++){
        if(*getMapInode(i)==false){
            //printf("encontrou inode %d livre\n",i);
            return i;
        }
    }
    printf("nao encontrou inode disponivel\n");
    return -1;
}

int findBlocoLivre(){
    for(int i=0;i<tamGerVazioBlocos;i++){
        if(*getMapBlocos(i)==false){
            //printf("encontrou bloco %d livre\n",i);
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
    posicao=tamGerVazioBlocos+tamGerVazioInodes+(sizeof(INode)*posicao);
    //printf("posicao = %d\n",posicao);
    char * hurr = (char*) disco+posicao;

    return (INode*) hurr    ;
}

char* getBloco(int bloco){
    if(bloco<0 || bloco>tamGerVazioBlocos){
        printf("bloco invalido\n");
        return NULL;
    }
    int posicao=tamGerVazioBlocos+tamGerVazioInodes+tamINodes+tamBloco*bloco;
    return disco+posicao;
}

void printInfoDisco(){
    //printf("Tamanho do disco :%d\nGerencimento dos blocos %d\nInodes:%d\nVazio dos inodes %d\nEspaço para os blocos %d\nTamanho do inode %d\n",tamDisco,tamGerVazioBlocos,tamINodes,tamGerVazioInodes,tamEspacoBlocos,sizeof(INode));
}

vector <char * > split(char * entrada,char separador){
    vector <char *> retorno;
    char * temp = (char*)calloc(256,1);
    int inicio=0;
    //printf("entrada do split: %s\n",entrada);
    int tam = strlen(entrada);
    for(int x=0;x<tam;x++){
        if(entrada[x]!=separador){
            temp[x-inicio]=entrada[x];
        }else{
            inicio=x+1;
            retorno.push_back(temp);
           // printf("dando push back em temp = %s\n",temp);
            temp = (char*)calloc(256,1);
        }
    }
    retorno.push_back(temp);
    return retorno;
}

bool login(char * nome,char * senha){
    char * usuarios=leArquivo((char*)"passwd",true);
    char * usuarioTemp;
    char * senhaTemp;
    vector<char* > combos= split(usuarios,';');
    for(int x=0;x<combos.size();x++){
        usuarioTemp=split(combos[x],':')[0];
        senhaTemp=split(combos[x],':')[1];
        printf("usuarioTemp %s senhaTemp %s\n",usuarioTemp,senhaTemp);
        if(strcmp(usuarioTemp,nome)==0 && strcmp(senhaTemp,criptografa(senha))==0){
            usuarioAtual=x;
            return true;
        }
    }
    printf("usuario ou senha incorretos\n");
    return false;

}

void addUser(char* nome,char * senha){
    if(usuarioAtual==0){
        char * atual=leArquivo((char*)"passwd",true);
        char * durr =criptografa(senha);

        char * nova = (char*)calloc(256,1);
        strcpy(nova,atual);
        strcat(nova,nome);
        strcat(nova,":");
        strcat(nova,criptografa(senha));
        strcat(nova,";");
        strcat(nova,"\0");
        printf("nova : %s\n",nova);
        echo(nova,((char*)"passwd"));

    }else{
        printf("apenas admin pode criar novos usuarios\n");
    }
}

void novoGrupo(char * nome){
   char * temp = leArquivo((char*)"grupos",true);
   strcat(temp,nome);
   strcat(temp,";");
   echo(temp,(char*)"grupos");
}

void insereNoGrupo(char* nomeGrupo,char* nome){
    printf("reimplementar insere no grupo");
}

bool * toBinario(char c){
    c=c-'0';
    int teste=c-'0';
    bool *retorno = (bool*) malloc(3);
    retorno[0]=CHECK_BIT(c, 0);
    retorno[1]=CHECK_BIT(c, 1);
    retorno[2]=CHECK_BIT(c, 2);
    //printf("%d %d %d\n",(c&1==1),(c&2==2),(c&4==4));
    return retorno;
}

void chmod(int posicao,char * permissoes){
    //printf("chdmod recebeu %s\n",permissoes);
    bool *arrayPermissoes;
    INode *inode=getINode(posicao);
    if(posicao==0 || inode->dono==usuarioAtual){  // só pode dar chmod se for o dono do arquivo ou estiver criando a raiz
        for(int x=0;x<3;x++){
            arrayPermissoes=toBinario(permissoes[x]);
            //printf("gerou array de permissoes: %d %d %d\n",arrayPermissoes[0],arrayPermissoes[1],arrayPermissoes[2]);
            for(int y=0;y<3;y++){
                //printf("botando na posicão %d\n",3*x+y);
                inode->permissoes[3*x+y]=arrayPermissoes[y];
            }
           // printf("botou no inode : %d %d %d\n",inode->permissoes[3*x],inode->permissoes[3*x+1],inode->permissoes[3*x+2]);
        }
    }else{
        printf("voce nao tem permissao para alterar as permissoes desse arquivo\n");
    }
}

bool pertenceAoGrupo(int posicao){
    vector < char *> membros = split(split(leArquivo((char*)"grupos",true),';')[posicao],',');  //melhor split
    for(int x=1;x<membros.size();x++){
        if(atoi(membros[x])==usuarioAtual){
            return true;
        }
    }
    return false;
}

bool verificaPermissao(int posicao,int operacao){
    INode * inode = getINode(posicao);
    if(usuarioAtual==0)
        return true;  //admin pode tudo
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

char * criptografa(char * senha){
    int tam=strlen(senha);
    char * teste = (char*) malloc(32);
    strcpy(teste,senha);
    for(int x=0;x<tam;x++){
        teste[x]+=3;
        //printf("leu caratecer %c x = %d \n",teste[x],x);
    }
    //printf("criptografa mudou senha %s\n",teste);
    return teste;
}

void chownArq(int usuario,int arquivo){
    INode * inode = getINode(arquivo);
    if(usuarioAtual==0 || inode->dono==usuarioAtual){
        inode->dono=usuario;
    }else{
        printf("voce nao é o dono do arquivo nem o admin\n");
    }
}

void chownGrupo(int grupo,int arquivo){
    INode * inode = getINode(arquivo);
    if(usuarioAtual==0 || inode->dono==usuarioAtual){
        inode->grupo=grupo;
    }else{
        printf("voce nao é o dono do arquivo nem o admin\n");
    }
}

char * getNomeUsuario(){
    return split(split(leArquivo((char*)"passwd",true),';')[usuarioAtual],':')[0];    //AWWW YEAH
}

void addtogroup(int usuario,int grupo){
    vector<char *> grupos= split(leArquivo((char*)"grupos",true),';');
    char * temp = (char*)calloc(1024,1);
    strcat(grupos[grupo],",");
    char buffer[30];
    sprintf(buffer,"%d",usuario);  // funcao obscura do stack overflow que converte um int em string
    strcat(grupos[grupo],buffer);
    for(int x=0;x<grupos.size()-1;x++){
        strcat(temp,grupos[x]);
        strcat(temp,";");
    }
    echo(temp,(char*)"grupos");
}

int main(){
    printInfoDisco();
    printf("Bem vindo ao sistema de arquivos!\n");
    usuarioAtual=0;

/*    arquivo=fopen("arquivo.bin","r+");
    if(arquivo==NULL){
        printf("O arquivo de disco ainda nao existia, criando.\n");
        arquivo=fopen("arquivo.bin","w");

    }else{
        printf("Arquivo aberto com sucesso\n");
    }*/
    criaSistemaDeArquivos();
    printf("arquivo passwd depois de criar o sistema de arquivos: %s\n\n",leArquivo((char*)"passwd",true));

    //disco=(char)malloc(tamDisco);
    //disco=mmap
    char entrada[256];
    int teste=10;
    bool logado=false;
    while(true){
        printf("faça login(login)\n");
        scanf("%s",entrada);
        if(strcmp(entrada,(char*)"login")==0){
            printf("informe seu usuario:");
            scanf("%s",entrada);
            printf("digite a senha: ");
            char senha[32];
            scanf("%s",senha);
            logado = login(entrada,senha); //login retorna true quando consegue logar
        }

        while(logado){
            printf("%s@MacBook Pro Retina Display 50': %s~$ ",getNomeUsuario(),geraArvoreDiretorio());
            scanf(" %s",entrada);
            printf("leu entrada %s\n",entrada);
            if(strcmp(entrada,(char*)"mkdir")==0){
                scanf(" %s",entrada);
                mkdir(entrada);
            }
            if(strcmp(entrada,(char*)"addgroup")==0){
                printf("digite o nome do grupo: ");
                scanf(" %s",entrada);
                novoGrupo(entrada);
            }

            if(strcmp(entrada,(char*)"adduser")==0){
                printf("escolha o nome do novo usuario: ");
                char senha [32];
                scanf(" %s",entrada);
                printf("digite a senha: ");
                scanf(" %s",senha);
                addUser(entrada,senha);
            }

            if(strcmp(entrada,(char*)"addtogroup")==0){
                char usuario[32],grupo[32];
                printf("digite o nome do grupo:");
                scanf(" %s",grupo);
                printf("digite o nome do usuario:");
                scanf(" %s",usuario);
                addtogroup(findPosUsuario(usuario),findPosGrupo(grupo));
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
                rmdir(entrada);  //ALTAMENTE SUSPEITO
            }
            if(strcmp(entrada,(char*)"echo")==0){
                char* nome=(char*)malloc(512);
                scanf(" %s > %s",entrada,nome);
                echo(entrada,nome);
            }
            if(strcmp(entrada,(char*)"cat")==0){
                scanf(" %s",entrada);
                cat(entrada);
            }
            if(strcmp(entrada,(char*)"logoff")==0){
                logado=false;
                usuarioAtual=0;
            }
            if(strcmp(entrada,(char*)"chmod")==0){
                char nome[32],permissao[10];
                scanf(" %s %s",permissao,nome);
                int pos = findPosINode(nome);
                chmod(pos,permissao);
            }
            if(strcmp(entrada,(char*)"chown")==0){
                char arquivo[32],usuario[32];
                scanf("%s %s",usuario,arquivo);
                chownArq(findPosUsuario(usuario),findPosINode(arquivo));
            }
            if(strcmp(entrada,(char*)"chgroup")==0){
                char arquivo[32],grupo[32];
                scanf("%s %s",grupo,arquivo);
                printf("chown precisa ser reimplementado\n");
                chownGrupo(findPosGrupo(grupo),findPosINode(arquivo));
            }
        }
    }
}
