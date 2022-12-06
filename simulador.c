// Declaraçao das variaveis globais
#include "header.h"

char mensagem [50];

//SOCKETS:
int sockfd = 0;

//Inicializacao de structs globais:
struct dadosConfiguracaoSimulacao dados_simulacao;
//struct discoteca discoteca;
struct zonaDiscoteca zona_discoteca[];
struct pessoa pessoas_t[30];

//Semaforos:
sem_t semaforo_enviar_informacao;

//Tarefas:
pthread_t t_id_discoteca[1];
pthread_t tIdPessoas[30];                        //Array com o id das tarefas correspondentes as pessoas

/*void escreverSimulacao (char texto[]){

    FILE* ficheiroApontador; 									//Cria um apontador para um ficheiro
    ficheiroApontador = fopen("DadosDaSimulacao.txt","a"); 						//Abre o ficheiro. "a" -> acrescenta texto
    if(ficheiroApontador == NULL){ 									//Verifica se o ficheiro existe
        printf ("Erro ao abrir o ficheiro ");
    }
    else{
        fprintf(ficheiroApontador, "%s", texto); 						//Coloca no ficheiro o que foi passado como argumento
    }
}*/

void enviarInformacao(char *ficheiroApontador, int sockfd){

    sem_wait(&semaforo_enviar_informacao);        //Coloca o semaforo em espera para mais nenhum processo poder aceder

    char buffer[MAXLINE]; 						                       // criar buffer
    int tamanho_informacao = 0;   									//tamanho buffer

    if( strcpy( buffer, ficheiroApontador ) != 0 ){ 							//Se houver mensagem
        tamanho_informacao = strlen( buffer ) + 1;  							// Adicionar um espaco para fazer disto uma string
        if( write( sockfd, buffer, tamanho_informacao ) != tamanho_informacao ){ 				// verifica se o tamanho do buffer e diferente que o tamanho da mensagem
            perror("====== Erro ao enviar os dados! ====== \n"); 			// Produz uma mensagem de erro personalizada, em vez do erro padrao
        }
        else{
            printf ( "Mensagem Enviada! \n" );
        }
    }

    //usleep(2000);
    sem_post(&semaforo_enviar_informacao); 								//Abre o trinco -> se algum processo quiser entrar pode
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

            definirVariaveis(parametro, valor);

        }
    } else {
        printf("Failure opening file. Try again! \n");
    }

    fclose(configuracao_simulacao);

}

void definirVariaveis(char parametro[50], int valor) {

    if(strcmp(parametro, "numPistasNaDiscoteca") == 0){

        dados_simulacao.numPistasNaDiscoteca = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalA") == 0) {

        dados_simulacao.lotacaoMaxLocalA = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalB") == 0) {
        dados_simulacao.lotacaoMaxLocalB = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalC") == 0) {

        dados_simulacao.lotacaoMaxLocalC = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalD") == 0) {

        dados_simulacao.lotacaoMaxLocalD = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalE") == 0) {

        dados_simulacao.lotacaoMaxLocalE = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalF") == 0) {

        dados_simulacao.lotacaoMaxLocalF = valor;
    }

    if(strcmp(parametro, "inicioSimulacao") == 0) {

        dados_simulacao.inicioSimulacao = valor;
    }

    if(strcmp(parametro, "fimSimulacao") == 0) {

        dados_simulacao.fimSimulacao = valor;
    }

    if(strcmp(parametro, "tempoMedioNovoUser") == 0) {

        dados_simulacao.tempoMedioNovoUser = valor;
    }

    if(strcmp(parametro, "primeiroLocal") == 0) {

        dados_simulacao.primeiroLocal = valor;
    }

}

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

void definirValores(){

    sem_init(&semaforo_enviar_informacao,0,1);

    lerConfiguracao();
    printf("Leitura da configuracao inicial completa! \n");

    //criarDiscoteca();//colocar o numero de zonas como parametros!!!
}

//void criaDiscoteca(){}

void * discoteca (void * ptr){
    //struct discoteca discoteca = criaDiscoteca();
}

void simulacao(int sockfd){

    definirValores();

    enviarInformacao ("**** A comecar simulacao **** \n", sockfd);

    pthread_create (&t_id_discoteca[0], NULL, discoteca, NULL); // Criacao de uma tarefa. Esta tarefa vai criar o primeiro centro de testagem
    printf("Acabei de criar a tarefa da discoteca \n");

    pthread_join(t_id_discoteca[0],NULL);
    printf("Acabei de juntar a tarefa da discoteca \n");

    enviarInformacao("**** A simular - Discoteca **** \n", sockfd);
    printf("Comecou a simulacao \n");
    
    enviarInformacao("++++Coisas a acontecer durante a simulacao++++", sockfd);

    enviarInformacao("**** Simulacao terminada **** \n", sockfd);
}

void main(int argc, char* argv[])
{
    //Criar o Socket
    sockfd = criarSocket();

    //Iniciar a simulação
    simulacao(sockfd);

    //Terminar o socket
    close(sockfd);

    return 0;
}

