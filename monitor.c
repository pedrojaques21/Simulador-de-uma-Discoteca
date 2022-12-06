#include "header.h"

bool fim_simulacao = false;

struct discoteca discoteca;

void escreverOutput(char mensagem[])
{
    FILE *output_logs;

    output_logs = fopen("output_logs.txt", "a");

    if (output_logs == NULL)
    {

        printf("ERRO, ficheiro nao foi criado!");
    }
    else
    {
        // Acrescenta a mensagem no ficheiro de texto
        fprintf(output_logs, "%s", mensagem);
        //printf("escreve monitor \n");
        // Imprime no monitor
        printf("%s", mensagem);
    }

    fclose(output_logs);
}

void escreveMensagem()
{

    char mensagemEscreve[500];

    sprintf(mensagemEscreve, "Discoteca \n");
    escreverOutput(mensagemEscreve);
    sprintf(mensagemEscreve, "Numero da zona: %i \n", discoteca.numZonas);
    escreverOutput(mensagemEscreve);
    sprintf(mensagemEscreve, "Numero de pessoas dentro: %i \n", discoteca.numUtilizadores);
    escreverOutput(mensagemEscreve);
}

void str_echo(int sockfd)
{
    int n;
    char linha[MAXLINE];
    while (!fim_simulacao)
    {
        // printf(read(sockfd, linha, MAXLINE));               // Enquanto a simulacao estiver a ocorrer
        n = read(sockfd, linha, MAXLINE); // n guarda o tamanho do line (buffer)
        if (n == 0)
        { // nao ha mensagem a ler
            printf("Entrei no n==0");
            break;
        }
        else if (n < 0)
        {
            perror("ERRO: erro ao ler a linha");
        }
        else
        { // ha mensagem a ler
            escreveMensagem();
            printf("%s\n", linha); // imprime a mensagem
            break;
        }
    }
}

void iniciaSimulador()
{

    int sockfd, newsockfd, clilen, childpid, servlen;
    struct sockaddr_un cli_addr, serv_addr;

    // Verifica a criacao do socket stream
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("server: Nao consegue abrir o stream socket");
    }

    // Limpeza do buffer -> valores no buffer a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    unlink(UNIXSTR_PATH);

    // Liga o socket a um endereco
    if (bind(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
    {
        perror("O socket não consegue ligar ao endereco especificado");
    }

    // Conexao com o simulador
    printf("Esperar Simulador \n");
    listen(sockfd, 1);

    // Criar um novo socket
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        perror("ERRO: erro na criacao do socket");
    }

    // Criar processo para lidar com o cliente
    if ((childpid = fork()) < 0)
    {
        perror("ERRO: nao foi possivel criar o processo filho");
    }
    else if (childpid == 0)
    {
        close(sockfd);
        str_echo(newsockfd);
        exit(0);
    }

    // Fecha o socket filho
    close(newsockfd);
}


int main(int argc, char const * argv[])
{
    // Interface com o utilizador
    printf("Escolha uma das opcoes: \n");
    printf("1. Iniciar Simulacao \n");
    //printf("2. Apagar conteudo do ficheiro da simulacao \n");
    // printf("3. Terminar a simulacao  \n");
    int opcao = 0; // guarda a opcao selecionada pelo utilizador
    while (!fim_simulacao)
    {

        while (opcao != 1  /*&& opcao != 2 && opcao!=3*/)
        {
            printf("Selecione uma das opcoes: \n");
            scanf("%d", &opcao); // guarda a opcao selecionada
        }

        if (opcao == 1)
        {
            iniciaSimulador();
            printf("Opcao escolhida = %d\n", opcao);
        }
        /*else if (opcao == 2)
        {
            printf("Apaga ficheiro log");
        }*/
    }
    printf("Antes do return 0");
    return 0;
}
