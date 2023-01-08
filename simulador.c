#include "header.h"

//SOCKETS:
int sockfd = 0;

//Inicializacao de structs globais:
struct Discoteca zonaGlobal[6];

//Semaforos:
sem_t semaforo_enviarInformacao;
sem_t semaforo_filaZona1;
sem_t semaforo_filaZona2;
sem_t semaforo_filaZona3;
sem_t semaforo_filaZona4;
sem_t semaforo_filaZona5;
sem_t semaforo_filaZona6;

//Trincos:
pthread_mutex_t trincoPessoa;

//Tarefas:
pthread_t tarefasZonas[6];
pthread_t tarefasPessoas[100];

//Variaveis de ficheiro// 
int numZonasNaDiscoteca = 0;
int numPessoasCriar = 0;
int lotacaoMax[5];

//Variaveis monitor:
int nPessoasZona1 = 0;
int nPessoasZona2 = 0;
int nPessoasZona3 = 0;
int nPessoasZona4 = 0;
int nPessoasZona5 = 0;
int nPessoasZona6 = 0;

//Variaveis globais:
int idPessoa = 1;

int criarSocket(){

    struct sockaddr_un serv_addr;
    int servlen;

    //Comecamos por criar o socket
    if ( (sockfd = socket(AF_UNIX, SOCK_STREAM,0) ) < 0 )
        perror(" Erro ao criar o stream socket! ");

    //Socket a 0 -> Limpeza
    bzero((char*)&serv_addr, sizeof(serv_addr));

    //Informa o dominio do socket
    serv_addr.sun_family=AF_UNIX;
    strcpy(serv_addr.sun_path,UNIXSTR_PATH);
    servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);

    //Realiza a ligacao com o socket
    bool simulacaoOcorre = false;
    while (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0){
        if(simulacaoOcorre == false){
            printf("Espera monitor \n");
            simulacaoOcorre = true;
        }
    }

    if(sockfd<0){
        perror ("ACCEPT ERROR");
    }

    printf ("Simulador feito \n");
    return sockfd;
}

// Função responsável por ler o ficheiro de configuracao
void lerConfiguracao()
{
    FILE* configuracao_simulacao;

    configuracao_simulacao = fopen("configuracao_simulacao.txt", "r");

    if(configuracao_simulacao != NULL){

        char linha[50],
            parametro[50];
        int valor;

        while(fgets(linha, sizeof(linha), configuracao_simulacao) != NULL){

            sscanf(linha, "%s : %d", parametro , &valor);

            if(strcmp(parametro, "numZonasNaDiscoteca") == 0){

                numZonasNaDiscoteca = valor;
            }

            if(strcmp(parametro, "numPessoasCriar") == 0){

                numPessoasCriar = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona2") == 0) {
                lotacaoMax[0] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona3") == 0) {

                lotacaoMax[1] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona4") == 0) {

                lotacaoMax[2] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona5") == 0) {

                lotacaoMax[3] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona6") == 0) {

                lotacaoMax[4] = valor;
            }
        }
    } else {
        printf("Failure opening file. Try again! \n");
    }

    fclose(configuracao_simulacao);
}

void enviarInformacao(int sockfd, int estado, int numPessoasZona1, int numPessoasZona2, int numPessoasZona3, int numPessoasZona4, int numPessoasZona5, int numPessoasZona6){
    sem_wait(&semaforo_enviarInformacao); //Coloca o semaforo em espera para mais nenhum processo poder aceder
	char buffer[MAXLINE];
	int lenInformacao=0;
	sprintf(buffer,"%d %d %d %d %d %d %d",estado,numPessoasZona1,numPessoasZona2,numPessoasZona3,numPessoasZona4,numPessoasZona5,numPessoasZona6);
	lenInformacao=strlen(buffer)+1;	//Adicionar um espaço para fazer disto uma string
	
    printf("conteudo do buffer:%s\n",&buffer[0]);

    if(send(sockfd,buffer,lenInformacao,0)!=lenInformacao){
		perror("******Erro ao enviar dados******\n");
	}
	sleep(1);
    sem_post(&semaforo_enviarInformacao); //Abre o trinco -> se algum processo quiser entrar pode
    printf("Informacao enviada! \n");
}

void criaDiscoteca(int numZona, int lotacaoMax){
    struct Discoteca *criaZona = &(zonaGlobal[numZona]);
    criaZona->numZona = numZona;
    criaZona->numPessoasDentro = 0;
    criaZona->lotacaoMax = lotacaoMax;

    printf("Criada Zona : %d PessoasDentro : %d LotacaoMax : %d \n", criaZona->numZona,criaZona->numPessoasDentro,criaZona->lotacaoMax);
}

struct Pessoa criaPessoa(){
    pthread_mutex_lock(&trincoPessoa);
    struct Pessoa p;
    p.id = idPessoa;
    idPessoa++;
    p.zonaDiscoteca = 1;
   if (rand()%2 == 1){ //1 = mulher/prioridade e 0 = homem/sem prioridade
        strcpy(p.sexo,"Feminimo");
    }
    else{
        strcpy(p.sexo,"Masculino");
    }
    int e = 2;
    for(int i=0; i<5; i++){
    p.historico[i] = e;
    e++;
    }
    pthread_mutex_unlock(&trincoPessoa);
    printf("Pessoa %d do sexo %s criada\n", p.id, p.sexo);
    return p;
}

void * discoteca(void *apontador){//Recebe como argumento o número da zona em que a tarefa foi criada
	int *numZona = (int*) apontador;
	struct Discoteca *zonaDiscoteca=&(zonaGlobal[*numZona]);//zonaDiscoteca é um array em que cada posição é o endereço do array zonaGlobal na posição indicada por *numZona
}

void * pessoa (void *null){
    struct Pessoa p = criaPessoa();
    int next;
    bool saiu = false;
    int tamanho;
    int posicao;
    if(p.zonaDiscoteca == 1){
        sem_wait(&semaforo_filaZona1);//Podem estar até 50 pessoas na fila com prioridade
        printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
        pthread_mutex_lock(&trincoPessoa);
        nPessoasZona1++;
        enviarInformacao(sockfd,p.zonaDiscoteca+1,nPessoasZona1,0,0,0,0,0);
        pthread_mutex_unlock(&trincoPessoa);
        sleep(4);

        //mudança de fila para outra zona
        tamanho = sizeof(p.historico)/sizeof(p.historico[0]);
        posicao = rand()%tamanho;
        next = p.historico[posicao];
        printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
        pthread_mutex_lock(&trincoPessoa);
        for(int i=posicao-1; i<tamanho-1; i++){
            p.historico[i] = p.historico[i + 1];
        }
        tamanho--;
        p.zonaDiscoteca = next;
        //fim de mudanca

        nPessoasZona1--;
        enviarInformacao(sockfd,2,nPessoasZona1,0,0,0,0,0);
        pthread_mutex_unlock(&trincoPessoa);
        sem_post(&semaforo_filaZona1);
    }
    while (!saiu){
        if(p.zonaDiscoteca == 2){
            sem_wait(&semaforo_filaZona2);//Podem estar até 50 pessoas na fila com prioridade
            printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
            pthread_mutex_lock(&trincoPessoa);
            nPessoasZona2++;
            enviarInformacao(sockfd,p.zonaDiscoteca+1,0,nPessoasZona2,0,0,0,0);
            pthread_mutex_unlock(&trincoPessoa);
            sleep(4);


            //colocar verificação antes de ver o array q se o tamanho for 0 pra já nem fazer mudança de zona
        }
        if(p.zonaDiscoteca == 3){
            sem_wait(&semaforo_filaZona3);//Podem estar até 50 pessoas na fila com prioridade
            printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
            pthread_mutex_lock(&trincoPessoa);
            nPessoasZona3++;
            enviarInformacao(sockfd,p.zonaDiscoteca+1,0,0,nPessoasZona3,0,0,0);
            pthread_mutex_unlock(&trincoPessoa);
            sleep(4);
        }
        if(p.zonaDiscoteca == 4){
            sem_wait(&semaforo_filaZona4);//Podem estar até 50 pessoas na fila com prioridade
            printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
            pthread_mutex_lock(&trincoPessoa);
            nPessoasZona4++;
            enviarInformacao(sockfd,p.zonaDiscoteca+1,0,0,0,nPessoasZona4,0,0);
            pthread_mutex_unlock(&trincoPessoa);
            sleep(4);
        }
        if(p.zonaDiscoteca == 5){
            sem_wait(&semaforo_filaZona5);//Podem estar até 50 pessoas na fila com prioridade
            printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
            pthread_mutex_lock(&trincoPessoa);
            nPessoasZona5++;
            enviarInformacao(sockfd,p.zonaDiscoteca+1,0,0,0,0,nPessoasZona5,0);
            pthread_mutex_unlock(&trincoPessoa);
            sleep(4);
        }
        if(p.zonaDiscoteca == 6){
            sem_wait(&semaforo_filaZona6);//Podem estar até 50 pessoas na fila com prioridade
            printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, p.zonaDiscoteca);
            pthread_mutex_lock(&trincoPessoa);
            nPessoasZona6++;
            enviarInformacao(sockfd,p.zonaDiscoteca+1,0,0,0,0,0,nPessoasZona6);
            pthread_mutex_unlock(&trincoPessoa);
            sleep(4);
        }
        if (tamanho == 0){
            saiu = true;
            printf("Pessoa %d do sexo %s saiu da discoteca \n", p.id, p.sexo);
        }
    }
}
void definirValores(){

    sem_init(&semaforo_enviarInformacao,0,1);
    sem_init(&semaforo_filaZona1,0,25);//Entrada, com prioridade a mulheres
	sem_init(&semaforo_filaZona2,0,30);//pista danca
    sem_init(&semaforo_filaZona3,0,15);//mini-golfe
    sem_init(&semaforo_filaZona4,0,10);//wc
    sem_init(&semaforo_filaZona5,0,15);//snooker
    sem_init(&semaforo_filaZona6,0,20);//Bar, com prioridade a mulheres

    lerConfiguracao();
    printf("Leitura da configuracao inicial completa! \n");
    for(int i=0;i<numZonasNaDiscoteca;i++){
        criaDiscoteca(i+1, lotacaoMax[i]);
    }
    printf("Valores definidos! \n");

}

void simulacao(int sockfd){

    definirValores();

    enviarInformacao(sockfd,1,0,0,0,0,0,0);
    int e;
    for(int i=0;i<numZonasNaDiscoteca;i++){
        e = i+1;
        pthread_create(&tarefasZonas[i], NULL, discoteca, &e);
    } 
    //Testar se dava certo:
    /*if (pthread_create(&tarefasZonas[i], NULL, discoteca, &e) != 0) {// Criacao de uma tarefa. Esta tarefa vai criar as zonas
        printf("erro na criacao da tarefa\n");
        return 1;
    }*/
    printf("Acabei de criar as tarefas da discoteca \n");
    
    for(int i=0;i<numPessoasCriar;i++){
		pthread_create(&tarefasPessoas[i], NULL, pessoa, NULL);
	}

    sleep(0.1); //para o printf não ser imprimido antes da última pessoa criada
    printf("Acabei de criar as tarefas das pessoas \n");

    for(int i=0;i<numPessoasCriar;i++){
		pthread_join(&tarefasPessoas[i], NULL);
	}

    for(int i=0;i<numZonasNaDiscoteca;i++){
        pthread_join(&tarefasZonas[i], NULL);
    }
    printf("Acabei de juntar as tarefas todas\n");

    enviarInformacao(sockfd,20,0,0,0,0,0,0);
}

int main(int argc, char* argv[])
{
    //Criar o Socket
    sockfd = criarSocket();

    //Iniciar a simulação
    simulacao(sockfd);

    //Terminar o socket
    close(sockfd);

    return 0;
}

