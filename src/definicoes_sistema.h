#ifndef DEFINICOES_SISTEMA_H
#define DEFICINOES_SISTEMA_H

#define NUM_ESTADOS 3
#define NUM_EVENTOS 7

// ESTADOS
typedef enum estado_t {
    IDLE,
    IMPRIMINDO,
    CALIBRANDO
} estado_t;

// EVENTOS
typedef enum evento_t {
    NENHUM_EVENTO,
    CALIBRAR, // Botão Calibrar
    IMPRIMIR, // Botão Imprimir
    CANCELAR, // Botão Cancelar desenho
    TERMINADO, // Desenho terminado
    ORIGEM, // Caneta na origem
    CARREGAR // Botão Carregar programa
} evento_t;

// ACOES
typedef enum acao_t {
    NENHUMA_ACAO,
    A01,
    A02,
    A03,
    A04
} acao_t;

// struct que armazena os codigos de proximo estado e acao
typedef struct proxEstadoAcao_t {
    estado_t estado;
    acao_t acao;
} proxEstadoAcao_t;

#endif