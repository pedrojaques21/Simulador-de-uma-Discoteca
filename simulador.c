// Declaraçao das variaveis globais
#include "header.h"

char mensagem [50];

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
pthread_t tarefasPessoas[200];

//Variaveis de ficheiro// 
int numZonasNaDiscoteca = 0;
int numPessoasCriar = 0;
int lotacaoMax[6];
int inicioSimulacao = 0;
int fimSimulacao = 0;

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

            if(strcmp(parametro, "lotacaoMaxZona1") == 0) {

                lotacaoMax[0] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona2") == 0) {
                lotacaoMax[1] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona3") == 0) {

                lotacaoMax[2] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona4") == 0) {

                lotacaoMax[3] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona5") == 0) {

                lotacaoMax[4] = valor;
            }

            if(strcmp(parametro, "lotacaoMaxZona6") == 0) {

                lotacaoMax[5] = valor;
            }

            if(strcmp(parametro, "inicioSimulacao") == 0) {

                inicioSimulacao = valor;
            }

            if(strcmp(parametro, "fimSimulacao") == 0) {

                fimSimulacao = valor;
            }
        }
    } else {
        printf("Failure opening file. Try again! \n");
    }

    fclose(configuracao_simulacao);
}

void enviarInformacao(int sockfd, int estado, int numPessoasZona1, int numPessoasZona2, int numPessoasZona3, int numPessoasZona4, int numPessoasZona5, int numPessoasZona6, int tempo){	
    sem_wait(&semaforo_enviarInformacao);        //Coloca o semaforo em espera para mais nenhum processo poder aceder
	char buffer[MAXLINE];																					//Cria buffer
	int lenInformacao=0;																					//Tamanho buffer

	sprintf(buffer,"%d %d %d %d %d %d %d %d",estado,numPessoasZona1,numPessoasZona2,numPessoasZona3,numPessoasZona4,numPessoasZona5,numPessoasZona6,tempo);
	lenInformacao=strlen(buffer)+1;																			//Adicionar um espaço para fazer disto uma string

	if(send(sockfd,buffer,lenInformacao,0)!=lenInformacao){													//AQUI É ONDE OS DADOS SÃO ENVIADOS 
		perror("******Erro ao enviar dados******\n");													//Produz uma mensagem de erro personalizada
	}
	sleep(1);
    sem_post(&semaforo_enviarInformacao); 								//Abre o trinco -> se algum processo quiser entrar pode
}

void criaDiscoteca(int numZona, int lotacaoMax){
    struct Discoteca *criaZona = &(zonaGlobal[numZona]);
    criaZona->numZona = numZona;
    criaZona->numPessoasDentro = 0;
    criaZona->lotacaoMax = lotacaoMax;
    criaZona->tempoMaxDentro = 120; //Tempo máximo das pessoas dentro da zona = 120 minutos

    printf(" Criada Zona : %d PessoasDentro : %d LotacaoMax : %d \n", criaZona->numZona,criaZona->numPessoasDentro,criaZona->lotacaoMax);
}

void definirValores(){

    sem_init(&semaforo_enviarInformacao,0,1);
    sem_init(&semaforo_filaZona1,0,50);//com prioridade a mulheres
	sem_init(&semaforo_filaZona2,0,30);
    sem_init(&semaforo_filaZona3,0,15);
    sem_init(&semaforo_filaZona4,0,10);//wc
    sem_init(&semaforo_filaZona5,0,15);
    sem_init(&semaforo_filaZona6,0,20);//com prioridade a mulheres

    lerConfiguracao();
    printf("Leitura da configuracao inicial completa! \n");
    for(int i=0;i<numZonasNaDiscoteca;i++){
        criaDiscoteca(i+1, lotacaoMax[i]);
    }

}

struct Pessoa criaPessoa(){
    pthread_mutex_lock(&trincoPessoa);
    struct Pessoa p;
    p.zonaDiscoteca = 1;
    int randSexo = rand()%1;
    p.sexo = randSexo; //1 = mulher/prioridade e 0 = homem/sem prioridade
    pthread_mutex_unlock(&trincoPessoa);
    return p;
}

void * discoteca(void *apontador){//Recebe como argumento o número da zona em que a tarefa foi criada
	int *numZona=(int*) apontador;
	struct Discoteca *zonaDiscoteca=&(zonaGlobal[*numZona]);//zonaDiscoteca é um array em que cada posição é o endereço do array zonaGlobal na posição indicada por *numZona
}

void * pessoa (void * ptr){
    struct Pessoa p = criaPessoa();
    //if zona 1 .....
            //pthread_mutex_lock(&trincoOrdemJogadoresEquipa1);

        //coisas q as pessoas vão fazer

        //pthread_mutex_unlock(&trincoOrdemJogadoresEquipa1);
    //if zona 2...
}

void simulacao(int sockfd){

    definirValores();

    enviarInformacao(sockfd,1,0,0,0,0,0,0,0);

    //printDiscoteca
    for(int i=0;i<numZonasNaDiscoteca;i++){
        pthread_create (&tarefasZonas[i], NULL, discoteca, i+1); // Criacao de uma tarefa. Esta tarefa vai criar a primeira zona
    }
    printf("Acabei de criar as tarefas da discoteca \n");

    for(int i=0;i<numPessoasCriar;i++){
		pthread_create(&tarefasPessoas[i], NULL, pessoa, NULL);
	}
    printf("Acabei de criar as tarefas das pessoas \n");  

    for(int i=0;i<numPessoasCriar;i++){
		pthread_join(&tarefasPessoas[i], NULL);
	}

    for(int i=0;i<numZonasNaDiscoteca;i++){
        pthread_join(&tarefasZonas[i], NULL);
    }
    printf("Acabei de juntar as tarefas todas\n");

    enviarInformacao(sockfd,20,0,0,0,0,0,0,0);
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

