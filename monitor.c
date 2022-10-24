#include "header.h"

void escreverOutput(){
    FILE* output_file;

    output_file = fopen("../output_logs.txt", "a");

    if(output_file == NULL){

        printf("ERROR, fila was not created!");
    }
    else{

        fprintf(output_file, "----------------- START ------------------\n");
        fprintf(output_file, "001 - 00:00:00 - Utilizador chegou à fila.\n");
        fprintf(output_file, "001 - 00:01:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_file, "002 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_file, "003 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_file, "004 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_file, "002 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_file, "003 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_file, "004 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_file, "001 - 00:10:00 - Utilizador mudou para a zona 3.\n");
        fprintf(output_file, "005 - 00:12:00 - Utilizador chegou à fila.\n");
        fprintf(output_file, "003 - 02:34:12 - Utilizador mudou para a zona 4.\n");
        fprintf(output_file, "002 - 03:50:00 - Utilizador mudou para a zona 6.\n");
        fprintf(output_file, "002 - 04:00:00 - Utilizador saiu da discoteca.\n");
        fprintf(output_file, "------------------ END -------------------\n");
        fprintf(output_file, " \n");
    }

    fclose(output_file);
}

void main(){
    escreverOutput();
    printf("The file output_logs.txt was created!\n");
}

