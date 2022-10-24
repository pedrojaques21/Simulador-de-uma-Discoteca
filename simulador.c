// Declaraçao das variaveis globais
#include "header.h"

int tempoMedioChegada,
    tempoAteUnidadesSaturadas,
    tempoEsperaLocalIsolamento,
    tempoMedioTestesConvencionais,
    tempoMeddioTestesRapidos,
    numEstacoesTeste,
    numEspacosIsolamentoEmEstacao,
    inicioDeSimulacao,
    tempoDeSimulacao;

// Funçao responsavel por ler o ficheiro de configuracao

void lerConfiguracao()
{
    FILE* config_file;

    config_file = fopen("../simulation_config.txt", "r"); //r - para ler   //a - escrever, se nao tiver ficheiro, cria-o

    if(config_file != NULL){

        char line[50],
            param[50];
        int value;

        while(fgets(line, sizeof(line), config_file) != NULL){

            sscanf(line, "%s : %d", param , &value);

            if(strcmp(param, "tempoMedioChegada") == 0){
                tempoMedioChegada = value;
            }

            if(strcmp(param, "tempoAteUnidadesSaturadas") == 0) {

                tempoAteUnidadesSaturadas = value;
            }

            if(strcmp(param, "tempoEsperaLocalIsolamento") == 0) {
                tempoEsperaLocalIsolamento = value;
            }

            if(strcmp(param, "tempoMedioTestesConvencionais") == 0) {

                tempoMedioTestesConvencionais = value;
            }

            if(strcmp(param, "tempoMeddioTestesRapidos") == 0) {

                tempoMeddioTestesRapidos = value;
            }

            if(strcmp(param, "numEstacoesTeste") == 0) {

                numEstacoesTeste = value;
            }

            if(strcmp(param, "numEspacosIsolamentoEmEstacao") == 0) {

                numEspacosIsolamentoEmEstacao = value;
            }

            if(strcmp(param, "inicioDeSimulacao") == 0) {

                inicioDeSimulacao = value;
            }

            if(strcmp(param, "tempoDeSimulacao") == 0) {

                tempoDeSimulacao = value;
            }
        }
    } else {
        printf("Failure opening file. Try again! \n");
    }
    fclose(config_file);

}

void main()
{
    lerConfiguracao();
    printf("Tempo de Medio de Chegada: %d\n",tempoMedioChegada);
    printf("Tempo ate as Unidades ficarem saturadas: %d\n",tempoAteUnidadesSaturadas);
    printf("Tempo de esperar no local de isolamento: %d\n",tempoEsperaLocalIsolamento);
    printf("Tempo medio dos testes convencionais: %d\n",tempoMedioTestesConvencionais);
    printf("Tempo medio dos testes rapidos: %d\n",tempoMeddioTestesRapidos);
    printf("Numero de estações de teste: %d\n",numEstacoesTeste);
    printf("Numero de espaçoes de isolamento na estacao: %d\n",numEspacosIsolamentoEmEstacao);
    printf("Inicio da simulacao: %d\n",inicioDeSimulacao);
    printf("Tempo da simulacao: %d\n",tempoDeSimulacao);
}

