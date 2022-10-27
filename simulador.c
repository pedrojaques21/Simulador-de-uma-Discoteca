// Declaraçao das variaveis globais
#include "header.h"

int numPistasNaDiscoteca,
    lotacaoMaxLocalA,
    lotacaoMaxLocalB,
    lotacaoMaxLocalC,
    lotacaoMaxLocalD,
    lotacaoMaxLocalE,
    lotacaoMaxLocalF,
    inicioSimulacao,
    fimSimulacao,
    tempoMedioNovoUser,
    primeiroLocal;

// Função responsável por ler o ficheiro de configuracao

void lerConfiguracao()
{
    FILE* config_file;

    config_file = fopen("../simulation_config.txt", "r");

    if(config_file != NULL){

        char line[50],
            param[50];
        int value;

        while(fgets(line, sizeof(line), config_file) != NULL){

            sscanf(line, "%s : %d", param , &value);

            definirVariaveis(param, value);

        }
    } else {
        printf("Failure opening file. Try again! \n");
    }

    fclose(config_file);

}

void definirVariaveis(char param[50], int value) {

    if(strcmp(param, "numPistasNaDiscoteca") == 0){

        numPistasNaDiscoteca = value;
    }

    if(strcmp(param, "lotacaoMaxLocalA") == 0) {

        lotacaoMaxLocalA = value;
    }

    if(strcmp(param, "lotacaoMaxLocalB") == 0) {
        lotacaoMaxLocalB = value;
    }

    if(strcmp(param, "lotacaoMaxLocalC") == 0) {

        lotacaoMaxLocalC = value;
    }

    if(strcmp(param, "lotacaoMaxLocalD") == 0) {

        lotacaoMaxLocalD = value;
    }

    if(strcmp(param, "lotacaoMaxLocalE") == 0) {

        lotacaoMaxLocalE = value;
    }

    if(strcmp(param, "lotacaoMaxLocalF") == 0) {

        lotacaoMaxLocalF = value;
    }

    if(strcmp(param, "inicioSimulacao") == 0) {

        inicioSimulacao = value;
    }

    if(strcmp(param, "fimSimulacao") == 0) {

        fimSimulacao = value;
    }

    if(strcmp(param, "tempoMedioNovoUser") == 0) {

        tempoMedioNovoUser = value;
    }

    if(strcmp(param, "primeiroLocal") == 0) {

        primeiroLocal = value;
    }

}

void main()
{
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
}

