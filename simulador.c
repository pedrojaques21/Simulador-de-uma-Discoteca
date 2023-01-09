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
pthread_t tarefasPessoas[200];

//Variaveis de ficheiro//
int numZonasNaDiscoteca = 0;
int numPessoasCriar = 0;
int lotacaoMax[5];
int probDesistencia = 0;
int probPedido = 0;

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
            printf("A aguardar pela conexão do Monitor! \n");
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
    FILE* configuracao_simulacao = fopen("configuracao_simulacao.txt", "r");

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

            if(strcmp(parametro, "probDesistencia") == 0) {

                probDesistencia = valor;
            }
            if(strcmp(parametro, "probPedido") == 0) {

                probPedido = valor;
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

    //printf("conteudo do buffer:%s\n",&buffer[0]);

    if(send(sockfd,buffer,lenInformacao,0)!=lenInformacao){
        perror("******Erro ao enviar dados******\n");
    }
    sleep(1);
    sem_post(&semaforo_enviarInformacao); //Abre o trinco -> se algum processo quiser entrar pode
    //printf("Informacao enviada! \n");
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

int desistencia() {

    int random = rand()%100 + 1;

    if (random < probDesistencia) {
        return 1;
    }
    else {
        return 0;
    }
}

int pedido_bar(){
    int random = rand()%100 + 1;

    if (random < probPedido) {
        return 1;
    }
    else {
        return 0;
    }
}

void * pessoa (void *null){
    struct Pessoa p = criaPessoa();
    int next;
    bool saiu = false;
    int tamanho;
    int posicao;
    int desiste;
    int estado;
    int pedido;
    int zona;
    if(p.zonaDiscoteca == 1){ //ENTRADA
        zona = p.zonaDiscoteca;
        estado = zona+1;
        sem_wait(&semaforo_filaZona1);//Podem estar até 50 pessoas na fila com prioridade
        printf("Pessoa %d do sexo %s esta a espera na zona %d \n", p.id, p.sexo, zona);
        pthread_mutex_lock(&trincoPessoa);
        //zonaGlobal[zona-1].numPessoasDentro++;
        nPessoasZona1++;
        enviarInformacao(sockfd,estado,nPessoasZona1,0,0,0,0,0);
        pthread_mutex_unlock(&trincoPessoa);
        //mudança de fila para outra zona
        tamanho = sizeof(p.historico)/sizeof(p.historico[0]);
        posicao = rand()%tamanho;
        next = p.historico[posicao];
        printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
        sem_post(&semaforo_filaZona1);
        pthread_mutex_lock(&trincoPessoa);
        p.zonaDiscoteca = next;
        //fim de mudanca
        //zonaGlobal[zona-1].numPessoasDentro--;
        nPessoasZona1--;
        enviarInformacao(sockfd,estado,nPessoasZona1,0,0,0,0,0);
        pthread_mutex_unlock(&trincoPessoa);
    }
    while (!saiu){
        if(p.zonaDiscoteca == 2){ //PISTA DE  DANCA
            zona = p.zonaDiscoteca;
            estado = zona+1;
            sem_wait(&semaforo_filaZona2);//Podem estar até 20 pessoas na fila
            printf("Pessoa %d do sexo %s entrou na fila da zona %d \n", p.id, p.sexo, zona);
            if(nPessoasZona2 == lotacaoMax[zona-2]){
                sleep(0.5);
                desiste = desistencia();
                if(desiste){
                    sem_post(&semaforo_filaZona2);
                    printf("Pessoa %d do sexo %s desistiu da fila da zona %d \n", p.id, p.sexo, zona);
                    if (tamanho > 0){
                        posicao = rand()%tamanho;
                        next = p.historico[posicao];
                        pthread_mutex_lock(&trincoPessoa);
                        for(int i=zona-2; i<tamanho-2; i++){
                            p.historico[i] = p.historico[i + 1];
                        }
                        tamanho--;
                        p.zonaDiscoteca = next;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                    else {
                        pthread_mutex_lock(&trincoPessoa);
                        p.zonaDiscoteca = 0;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                }
            }
            else if(nPessoasZona2 < lotacaoMax[zona-2]){
                sem_post(&semaforo_filaZona2);
                printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona2++;
                enviarInformacao(sockfd,estado,0,nPessoasZona2,0,0,0,0);
                pthread_mutex_unlock(&trincoPessoa);
                sleep(30);
                //mudar de zona
                if (tamanho > 0){
                    posicao = rand()%tamanho;
                    next = p.historico[posicao];
                    pthread_mutex_lock(&trincoPessoa);
                    for(int i=zona-2; i<tamanho-2; i++){
                        p.historico[i] = p.historico[i + 1];
                    }
                    tamanho--;
                    p.zonaDiscoteca = next;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                else {
                    pthread_mutex_lock(&trincoPessoa);
                    p.zonaDiscoteca = 0;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona2--;
                enviarInformacao(sockfd,estado,0,nPessoasZona2,0,0,0,0);
                pthread_mutex_unlock(&trincoPessoa);
            }
        }
        if(p.zonaDiscoteca == 3){ // PISTA MINIGOLF
            zona = p.zonaDiscoteca;
            estado = zona+1;
            sem_wait(&semaforo_filaZona3);//Podem estar até 15 pessoas na fila
            printf("Pessoa %d do sexo %s entrou na fila da zona %d \n", p.id, p.sexo, zona);
            if(nPessoasZona3 == lotacaoMax[zona-2]){
                sleep(0.5);
                desiste = desistencia();
                if(desiste){
                    sem_post(&semaforo_filaZona3);
                    printf("Pessoa %d do sexo %s desistiu da fila da zona %d \n", p.id, p.sexo, zona);
                    if (tamanho > 0){
                        posicao = rand()%tamanho;
                        next = p.historico[posicao];
                        pthread_mutex_lock(&trincoPessoa);
                        for(int i=zona-2; i<tamanho-2; i++){
                            p.historico[i] = p.historico[i + 1];
                        }
                        tamanho--;
                        p.zonaDiscoteca = next;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                    else {
                        pthread_mutex_lock(&trincoPessoa);
                        p.zonaDiscoteca = 0;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                }
            }
            else if(nPessoasZona3 < lotacaoMax[zona-2]){
                sem_post(&semaforo_filaZona3);
                printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona3++;
                enviarInformacao(sockfd,estado,0,0,nPessoasZona3,0,0,0);
                pthread_mutex_unlock(&trincoPessoa);
                sleep(20);
                //mudar de zona
                if (tamanho > 0){
                    posicao = rand()%tamanho;
                    next = p.historico[posicao];
                    pthread_mutex_lock(&trincoPessoa);
                    for(int i=zona-2; i<tamanho-2; i++){
                        p.historico[i] = p.historico[i + 1];
                    }
                    tamanho--;
                    p.zonaDiscoteca = next;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                else {
                    pthread_mutex_lock(&trincoPessoa);
                    p.zonaDiscoteca = 0;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona3--;
                enviarInformacao(sockfd,estado,0,0,nPessoasZona3,0,0,0);
                pthread_mutex_unlock(&trincoPessoa);
            }
        }
        if(p.zonaDiscoteca == 4){//WC
            zona = p.zonaDiscoteca;
            estado = zona+1;
            sem_wait(&semaforo_filaZona4);//Podem estar até 5 pessoas na fila
            printf("Pessoa %d do sexo %s entrou na fila da zona %d \n", p.id, p.sexo, zona);
            if(nPessoasZona4 == lotacaoMax[zona-2]){
                sleep(0.5);
                desiste = desistencia();
                if(desiste){
                    sem_post(&semaforo_filaZona4);
                    printf("Pessoa %d do sexo %s desistiu da fila da zona %d \n", p.id, p.sexo, zona);
                    if (tamanho > 0){
                        posicao = rand()%tamanho;
                        next = p.historico[posicao];
                        pthread_mutex_lock(&trincoPessoa);
                        for(int i=zona-2; i<tamanho-2; i++){
                            p.historico[i] = p.historico[i + 1];
                        }
                        tamanho--;
                        p.zonaDiscoteca = next;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                    else {
                        pthread_mutex_lock(&trincoPessoa);
                        p.zonaDiscoteca = 0;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                }
            }
            else if(nPessoasZona4 < lotacaoMax[zona-2]){
                sem_post(&semaforo_filaZona4);
                printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona4++;
                enviarInformacao(sockfd,estado,0,0,0,nPessoasZona4,0,0);
                pthread_mutex_unlock(&trincoPessoa);
                //mudar de zona
                if (tamanho > 0){
                    posicao = rand()%tamanho;
                    next = p.historico[posicao];
                    pthread_mutex_lock(&trincoPessoa);
                    for(int i=zona-2; i<tamanho-2; i++){
                        p.historico[i] = p.historico[i + 1];
                    }
                    tamanho--;
                    p.zonaDiscoteca = next;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                else {
                    pthread_mutex_lock(&trincoPessoa);
                    p.zonaDiscoteca = 0;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona4--;
                enviarInformacao(sockfd,estado,0,0,0,nPessoasZona4,0,0);
                pthread_mutex_unlock(&trincoPessoa);
            }
        }
        if(p.zonaDiscoteca == 5){ //SNOOKER
            zona = p.zonaDiscoteca;
            estado = zona+1;
            sem_wait(&semaforo_filaZona5);//Podem estar até 5 pessoas na fila
            printf("Pessoa %d do sexo %s entrou na fila da zona %d \n", p.id, p.sexo, zona);
            if(nPessoasZona5 == lotacaoMax[zona-2]){
                sleep(0.5);
                desiste = desistencia();
                if(desiste){
                    sem_post(&semaforo_filaZona5);
                    printf("Pessoa %d do sexo %s desistiu da fila da zona %d \n", p.id, p.sexo, zona);
                    if (tamanho > 0){
                        posicao = rand()%tamanho;
                        next = p.historico[posicao];
                        pthread_mutex_lock(&trincoPessoa);
                        for(int i=zona-2; i<tamanho-2; i++){
                            p.historico[i] = p.historico[i + 1];
                        }
                        tamanho--;
                        p.zonaDiscoteca = next;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                    else {
                        pthread_mutex_lock(&trincoPessoa);
                        p.zonaDiscoteca = 0;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                }
            }
            else if(nPessoasZona5 < lotacaoMax[zona-2]){
                sem_post(&semaforo_filaZona5);
                printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona5++;
                enviarInformacao(sockfd,estado,0,0,0,0,nPessoasZona5,0);
                pthread_mutex_unlock(&trincoPessoa);
                sleep(20);
                //mudar de zona
                if (tamanho > 0){
                    posicao = rand()%tamanho;
                    next = p.historico[posicao];
                    pthread_mutex_lock(&trincoPessoa);
                    for(int i=zona-2; i<tamanho-2; i++){
                        p.historico[i] = p.historico[i + 1];
                    }
                    tamanho--;
                    p.zonaDiscoteca = next;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                else {
                    pthread_mutex_lock(&trincoPessoa);
                    p.zonaDiscoteca = 0;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona5--;
                enviarInformacao(sockfd,estado,0,0,0,0,nPessoasZona5,0);
                pthread_mutex_unlock(&trincoPessoa);
            }
        }
        if(p.zonaDiscoteca == 6){ //BAR
            zona = p.zonaDiscoteca;
            estado = zona+1;
            sem_wait(&semaforo_filaZona6);//Podem estar até 5 pessoas na fila
            printf("Pessoa %d do sexo %s entrou na fila da zona %d \n", p.id, p.sexo, zona);
            if(nPessoasZona6 == lotacaoMax[zona-2]){
                sleep(0.5);
                desiste = desistencia();
                if(desiste){
                    sem_post(&semaforo_filaZona6);
                    printf("Pessoa %d do sexo %s desistiu da fila da zona %d \n", p.id, p.sexo, zona);
                    if (tamanho > 0){
                        posicao = rand()%tamanho;
                        next = p.historico[posicao];
                        pthread_mutex_lock(&trincoPessoa);
                        for(int i=zona-2; i<tamanho-2; i++){
                            p.historico[i] = p.historico[i + 1];
                        }
                        tamanho--;
                        p.zonaDiscoteca = next;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                    else {
                        pthread_mutex_lock(&trincoPessoa);
                        p.zonaDiscoteca = 0;
                        pthread_mutex_unlock(&trincoPessoa);
                    }
                }
            }
            else if(nPessoasZona6 < lotacaoMax[zona-2]){
                sem_post(&semaforo_filaZona6);
                printf("Pessoa %d do sexo %s entrou na zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona6++;
                enviarInformacao(sockfd,estado,0,0,0,0,0,nPessoasZona6);
                pthread_mutex_unlock(&trincoPessoa);
                sleep(15);
                pedido = pedido_bar();
                if(pedido){
                    printf("Pessoa %d do sexo %s pediu uma bebida na zona %d \n", p.id, p.sexo, zona);
                    sleep(5);
                }
                //mudar de zona
                if (tamanho > 0){
                    posicao = rand()%tamanho;
                    next = p.historico[posicao];
                    pthread_mutex_lock(&trincoPessoa);
                    for(int i=zona-2; i<tamanho-2; i++){
                        p.historico[i] = p.historico[i + 1];
                    }
                    tamanho--;
                    p.zonaDiscoteca = next;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                else {
                    pthread_mutex_lock(&trincoPessoa);
                    p.zonaDiscoteca = 0;
                    pthread_mutex_unlock(&trincoPessoa);
                }
                printf("Pessoa %d do sexo %s saiu da zona %d \n", p.id, p.sexo, zona);
                pthread_mutex_lock(&trincoPessoa);
                nPessoasZona6--;
                enviarInformacao(sockfd,estado,0,0,0,0,0,nPessoasZona6);
                pthread_mutex_unlock(&trincoPessoa);
            }
        }
        if (tamanho == 0 || p.zonaDiscoteca == 0){
            saiu = true;
            printf("Pessoa %d do sexo %s saiu da discoteca \n", p.id, p.sexo);
            //pthread_exit(NULL);
        }
    }
}
void definirValores(){
    int filaZona1 = 25;
    sem_init(&semaforo_enviarInformacao,0,1);
    sem_init(&semaforo_filaZona1,0,filaZona1);//Entrada, com prioridade a mulheres
    sem_init(&semaforo_filaZona2,0,20);//pista danca
    sem_init(&semaforo_filaZona3,0,15);//mini-golfe
    sem_init(&semaforo_filaZona4,0,5);//wc
    sem_init(&semaforo_filaZona5,0,15);//snooker
    sem_init(&semaforo_filaZona6,0,20);//Bar, com prioridade a mulheres

    lerConfiguracao();
    printf("Leitura da configuracao inicial completa! \n");

    criaDiscoteca(1, filaZona1);
    for(int i=0;i<numZonasNaDiscoteca-1;i++){
        criaDiscoteca(i+2, lotacaoMax[i]);
    }
    printf("Valores definidos! \n");

}

void simulacao(int sockfd){

    definirValores();

    enviarInformacao(sockfd,1,0,0,0,0,0,0);
    /*int e;
    for(int i=0;i<numZonasNaDiscoteca;i++){
        e = i+1;
        pthread_create(&tarefasZonas[i], NULL, discoteca, &e);
    }*/
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
/*
    for(int i=0;i<numPessoasCriar;i++){
        pthread_join(&tarefasPessoas[i], NULL);
    }*/

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
