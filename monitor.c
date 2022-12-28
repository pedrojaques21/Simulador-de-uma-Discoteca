#include "header.h"

bool fim_simulacao = false;

struct Discoteca discoteca;

void escreverOutput(int newsockfd){

	int mensagem = 0;
    int estado = 0;
	int numPessoasZona1 = 0;
    int numPessoasZona2 = 0;
    int numPessoasZona3 = 0;
    int numPessoasZona4 = 0;
    int numPessoasZona5 = 0;
    int numPessoasZona6 = 0;
	int tempo = 0;	//tempo médio de espera nas filas?												

	FILE* output_logs = fopen("output_logs.txt", "a");//a acrescenta mensagem ao ficheiro

	if(output_logs == NULL){
		printf("Erro, ficheiro não criado");
	}
	else{
		while(estado!=20){
			char linhaRecebe[MAXLINE+1];
			mensagem=recv(newsockfd,linhaRecebe,MAXLINE,0);

			sscanf(linhaRecebe,"%d %d %d %d %d %d %d %d",&estado,&numPessoasZona1,&numPessoasZona2,&numPessoasZona3,&numPessoasZona4,&numPessoasZona5,&numPessoasZona6,&tempo);
			
			switch(estado){
				case 1:{
					fprintf(output_logs,"**** Simulacao - Discoteca ****\n");
					printf("**** Simulacao - Discoteca ****\n");
					break;
				}
				case 2:{
					fprintf(output_logs, "Pessoas na Entrada: %d\n" ,numPessoasZona1);
					printf("Pessoas na Entrada: %d\n" ,numPessoasZona1);
					break;
				}
				case 3:{
					fprintf(output_logs,"Pessoas na Pista de Dança: %d/%d\n" ,numPessoasZona2);
					printf("Pessoas na Pista de Dança: %d/%d\n" ,numPessoasZona2);
					break;
				}
				case 4:{
					fprintf(output_logs, "Pessoas na Pista de Mini-golf: %d/%d\n",numPessoasZona3);
					printf( "Pessoas na Pista de Mini-golf: %d/%d\n",numPessoasZona3);
					break;
				}
				case 5:{
                    fprintf(output_logs, "WC: %s\n", numPessoasZona4 ? "Ocupada" : "Vazia");
					printf("WC: %s\n", numPessoasZona4 ? "Ocupada" : "Vazia");
					break;
				}
                case 6:{
                    fprintf(output_logs, "Pessoas na Sala de Snooker: %d/%d\n",numPessoasZona5);
					printf( "Pessoas na Sala de Snooker: %d/%d\n",numPessoasZona5);	
					break;
                }
                case 7:{
                    fprintf(output_logs, "Pessoas no Bar: %d/%d\n",numPessoasZona6);
					printf( "Pessoas no Bar: %d/%d\n",numPessoasZona6);
					break;
                }
                case 8:{ //tempo médio
                /*
					tempoEspera += tempo;
					numPessoasIsolamentoMonitor += numPessoasIsolamento;

					tMedioIsolamento = (double)tempoEsperaIsolamento / (double)numPessoasIsolamentoMonitor;

					fprintf(output_logs,ANSI_COLOR_GREEN "Tempo médio no isolamento - Centro1: %d\n" ANSI_COLOR_RESET,tMedioIsolamento);//escreve no ficheiro
					printf(ANSI_COLOR_GREEN "Tempo médio no isolamento - Centro1: %d\n" ANSI_COLOR_RESET,tMedioIsolamento);	*/										//escreve no monitor
					break;
				}
			}
		}
	}
	if(estado==20){
		fprintf(output_logs,"**** Fim da simulacao ****\n");
		printf("**** Fim da simulacao ****\n");
		fclose(output_logs);
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
        //str_echo(newsockfd);
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
