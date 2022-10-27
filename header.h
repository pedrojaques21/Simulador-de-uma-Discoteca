#ifndef SIMULADOR_DE_UMA_DISCOTECA_HEADER_H
#define SIMULADOR_DE_UMA_DISCOTECA_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void lerConfiguracao();
void definirVariaveis(char param[50], int value);
void escreverOutput();

#endif //SIMULADOR_DE_UMA_DISCOTECA_HEADER_H
