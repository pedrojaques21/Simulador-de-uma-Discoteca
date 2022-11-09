// Declaraçao das variaveis globais
#include "header.h"

char mensagem [50];

//SOCKETS:
int sockfd = 0;

//Inicializacao de structs globais:
struct dadosConfiguracaoSimulacao dadosSimulacao;
struct discoteca discoteca;
struct zonaDiscoteca[];
struct pessoa PessoasT[30];            //30 pessoas

//Semaforos:
sem_t TrincoInformacaoEnviar;//MUDAR O NOME

//Tarefas:
pthread_t tIdCentrosTeste[2];			//Array com o id das tarefas correspondentes aos centros de teste
pthread_t tIdPessoas[30];                        //Array com o id das tarefas correspondentes as pessoas



// Função responsável por ler o ficheiro de configuracao

void lerConfiguracao()
{
    FILE* configuracao_simulacao;

    configuracao_simulacao = fopen("configuracao_simulacao.txt", "r");

    if(configuracao_simulacao != NULL){

        char linha[50],
            parametroetro[50];
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

        numPistasNaDiscoteca = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalA") == 0) {

        lotacaoMaxLocalA = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalB") == 0) {
        lotacaoMaxLocalB = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalC") == 0) {

        lotacaoMaxLocalC = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalD") == 0) {

        lotacaoMaxLocalD = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalE") == 0) {

        lotacaoMaxLocalE = valor;
    }

    if(strcmp(parametro, "lotacaoMaxLocalF") == 0) {

        lotacaoMaxLocalF = valor;
    }

    if(strcmp(parametro, "inicioSimulacao") == 0) {

        inicioSimulacao = valor;
    }

    if(strcmp(parametro, "fimSimulacao") == 0) {

        fimSimulacao = valor;
    }

    if(strcmp(parametro, "tempoMedioNovoUser") == 0) {

        tempoMedioNovoUser = valor;
    }

    if(strcmp(parametro, "primeiroLocal") == 0) {

        primeiroLocal = valor;
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

    sem_init(&TrincoInformacaoEnviar,0,1);

    lerConfiguracao();
    printf("Leitura da configuracao inicial completa! \n");

    criarCentroTestagem(0);//Por alterar!!
    criarCentroTestagem(1);
}

void simular(int sockfd){

    definirValores();
    InformacaoEnviar ("**** A comecar simular **** \n", sockfd);

    // printf("Acabei de imprimir o Inicio \n");

    int zero = 0;
    //Era fixe depois mudar isto para A e B e assim estes ficavam os nomes dos centros
    int um = 1;


    // ************** Criacao dos dois centros de testagem **************************

    pthread_create (&tIdCentrosTeste[0], NULL, centroTeste, &zero); // Criacao de uma tarefa. Esta tarefa vai criar o primeiro centro de testagem
    pthread_create (&tIdCentrosTeste[1], NULL, centroTeste, &um);  //Criacao de outra tarefa. Esta tarefa vai criar o segundo centro de testagem

    printf("Acabei de cirar as tarefas dos centros de Teste \n");

    pthread_join(tIdCentrosTeste[0],NULL);
    pthread_join(tIdCentrosTeste[1],NULL);

    InformacaoEnviar("**** simular - Rastreamento Covid-19 **** \n",sockfd);
    InformacaoEnviar("1",sockfd);
    // ===============================================================================
    // ===============================================================================


    // ************** Criacao das 30 pessoas ***************************************
    /*
    pthread_create (&tIdPessoas[0], NULL, Pessoa, &zero); // Criacao de uma tarefa. Esta tarefa vai criar a primeira pessoa
    pthread_create (&tIdPessoas[1], NULL, Pessoa, &um);   //Criacao de outra tarefa. Esta tarefa vai criar a primeira pessoa

    printf("Acabei de cirar as tarefas das Pessoas \n");

    pthread_join(tIdPessoas[0],NULL);
    pthread_join(tIdPessoas[1],NULL);
    */

    //Fim da simular
    InformacaoEnviar("**** A terminar simular **** \n", sockfd);


}

void main(int argc, char* argv[])
{/*
    lerConfiguracao();
    printf("Numero de locais na discoteca: %d\n",numPistasNaDiscoteca);
    printf("Lotacao maxima do local A: %d\n",lotacaoMaxLocalA);
    printf("Lotacao maxima do local B: %d\n",lotacaoMaxLocalB);
    printf("Lotacao maxima do local C: %d\n",lotacaoMaxLocalC);
    printf("Lotacao maxima do local D: %d\n",lotacaoMaxLocalD);
    printf("Lotacao maxima do local E: %d\n",lotacaoMaxLocalE);
    printf("Lotacao maxima do local F: %d\n",lotacaoMaxLocalF);
    printf("Inicio da simulacao: %d\n",inicioSimulacao);
    printf("Fim da simulacao: %d\n",fimSimulacao);
    printf("Tempo medio para a entrada de um novo user na simulacao: %d\n",tempoMedioNovoUser);
    printf("Primeiro local a ser acedido pelos users: %d\n",primeiroLocal);
*/
    //Criar o Socket
    sockfd = criarSocket();

    //Iniciar a simulação
    simular(sockfd);

    //Terminar o socket
    close(sockfd);

    return 0;
}

