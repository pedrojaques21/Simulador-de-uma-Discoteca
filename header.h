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


#define UNIXSTR_PATH "/tmp/s.2046919"
#define MAXLINE 512

struct dadosConfiguracaoSimulacao{
    int numPistasNaDiscoteca;
    int lotacaoMaxLocalA;
    int lotacaoMaxLocalB;
    int lotacaoMaxLocalC;
    int lotacaoMaxLocalD;
    int lotacaoMaxLocalE;
    int lotacaoMaxLocalF;
    int inicioSimulacao;
    int fimSimulacao;
    int tempoMedioNovoUser;
    int primeiroLocal;
};

struct discoteca{
    int numZonas;
    int numUtilizadores;
};

struct zonaDiscoteca{
    char _idZona;
    int numUtilizadores;
    int lotacao;
    float tempoMaximo;
};

struct pessoa{
    int zonaDiscoteca;
    bool naFila;
    bool desistiu;
};

#endif //SIMULADOR_DE_UMA_DISCOTECA_HEADER_H


