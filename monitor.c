#include "header.h"

void escreverOutput(){
    FILE* output_file;

    output_file = fopen("../output.txt", "a");

    if(output_file == NULL){

        printf("ERROR, fila was not created!");
    }
    else{

        fprintf(output_file, "-------- START -----\n");
        fprintf(output_file, "tempoMedioChegada: 10\n");
        fprintf(output_file, "tempoAteUnidadesSaturadas: 60\n");
        fprintf(output_file, "tempoEsperaLocalIsolamento: 20\n");
        fprintf(output_file, "tempoMedioTestesConvencionais: 4\n");
        fprintf(output_file, "tempoMeddioTestesRapidos: 2\n");
        fprintf(output_file, "numEstacoesTeste: 2\n");
        fprintf(output_file, "numEspacosIsolamentoEmEstacao: 20\n");
        fprintf(output_file, "inicioDeSimulação: 1\n");
        fprintf(output_file, "tempoDeSimulação: 15\n");
        fprintf(output_file, "--------- END ------\n");
        fprintf(output_file, " \n");
    }

    fclose(output_file);
}

void main(){
    escreverOutput();
    printf("The file output.txt was created!\n");
}

