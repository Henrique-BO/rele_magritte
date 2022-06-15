#ifndef DEFINICOES_SISTEMA_H
#define DEFICINOES_SISTEMA_H

#define NUM_ESTADOS 3
#define NUM_EVENTOS 7

// ESTADOS
typedef enum Estado {
    IDLE,
    IMPRIMINDO,
    CALIBRANDO
} Estado;

// EVENTOS
typedef enum Evento {
    NENHUM_EVENTO,
    CALIBRAR, // Botão Calibrar
    IMPRIMIR, // Botão Imprimir
    CANCELAR, // Botão Cancelar desenho
    TERMINADO, // Desenho terminado
    ORIGEM, // Caneta na origem
    CARREGAR // Botão Carregar programa
} Evento;

// ACOES
typedef enum Acao {
    NENHUMA_ACAO,
    A01,
    A02,
    A03,
    A04
} Acao;

// struct que armazena os codigos de proximo estado e acao
typedef struct ProxEstadoAcao {
    Estado estado;
    Acao acao;
} ProxEstadoAcao;

#endif