#include "header.h"

void escreverOutput(){
    FILE* output_logs;

    output_logs = fopen("output_logs.txt", "a");

    if(output_logs == NULL){

        printf("ERRO, ficheiro nao criado!");
    }
    else{

        fprintf(output_logs, "----------------- START ------------------\n");
        fprintf(output_logs, "001 - 00:00:00 - Utilizador chegou à fila.\n");
        fprintf(output_logs, "001 - 00:01:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_logs, "002 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_logs, "003 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_logs, "004 - 00:05:00 - Utilizador chegou à fila.\n");
        fprintf(output_logs, "002 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_logs, "003 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_logs, "004 - 00:06:00 - Utilizador entrou na zona 1.\n");
        fprintf(output_logs, "001 - 00:10:00 - Utilizador mudou para a zona 3.\n");
        fprintf(output_logs, "005 - 00:12:00 - Utilizador chegou à fila.\n");
        fprintf(output_logs, "003 - 02:34:12 - Utilizador mudou para a zona 4.\n");
        fprintf(output_logs, "002 - 03:50:00 - Utilizador mudou para a zona 6.\n");
        fprintf(output_logs, "002 - 04:00:00 - Utilizador saiu da discoteca.\n");
        fprintf(output_logs, "------------------ END -------------------\n");
        fprintf(output_logs, " \n");
    }

    fclose(output_logs);
}

void main(){
    escreverOutput();
    printf("O ficheiro output_logs.txt foi criado!\n");
}

