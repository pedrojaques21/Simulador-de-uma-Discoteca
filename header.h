#ifndef SIMULADOR_DE_UMA_DISCOTECA_HEADER_H
#define SIMULADOR_DE_UMA_DISCOTECA_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>


#define UNIXSTR_PATH "/tmp/s.2045919"
#define MAXLINE 512

struct DadosConfiguracao{
    int numZonasNaDiscoteca;
    int numPessoasCriar;
    int lotacaoMax[6];
    int inicioSimulacao;
    int fimSimulacao;
};

struct Discoteca{
    int numZona;
    int numPessoasDentro;
    int lotacaoMax;
    float tempoMaxDentro;
};

struct Pessoa{
    int zonaDiscoteca;
    bool sexo;
};

#endif //SIMULADOR_DE_UMA_DISCOTECA_HEADER_H


