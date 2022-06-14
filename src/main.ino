#include "definicoes_sistema.h"
#include "controlador.h"
#include "interface_wifi.h"
#include "interpretador_g.h"
#include "sensor_curso.h"

proxEstadoAcao_t matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS]; // matriz de transicao de estados
estado_t estado; // estado atual

// inicializa a matriz de transicao de estados
void initMatrizTransicao()
{
    int i;
    int j;

    for (i=0; i < NUM_ESTADOS; i++) {
        for (j=0; j < NUM_EVENTOS; j++) {
            matrizTransicaoEstados[i][j].estado = static_cast<estado_t>(i);
            matrizTransicaoEstados[i][j].acao = NENHUMA_ACAO;
        }
    }

    // Idle
    matrizTransicaoEstados[IDLE][CARREGAR].estado = IDLE;
    matrizTransicaoEstados[IDLE][CARREGAR].acao = A01;

    matrizTransicaoEstados[IDLE][CALIBRAR].estado = CALIBRANDO;
    matrizTransicaoEstados[IDLE][CALIBRAR].acao = A02;

    matrizTransicaoEstados[IDLE][IMPRIMIR].estado = IMPRIMINDO;
    matrizTransicaoEstados[IDLE][IMPRIMIR].acao = A03;

    // Imprimindo
    matrizTransicaoEstados[IMPRIMINDO][TERMINADO].estado = CALIBRANDO;
    matrizTransicaoEstados[IMPRIMINDO][TERMINADO].acao = A02;

    matrizTransicaoEstados[IMPRIMINDO][CANCELAR].estado = CALIBRANDO;
    matrizTransicaoEstados[IMPRIMINDO][CANCELAR].acao = A02;
    
    // Calibrando
    matrizTransicaoEstados[CALIBRANDO][ORIGEM].estado = IDLE;
    matrizTransicaoEstados[CALIBRANDO][ORIGEM].acao = A04;
}

evento_t obterEvento() {
    // TODO Determina se um evento ocorreu
    return NENHUM_EVENTO;
}

proxEstadoAcao_t obterProxEstadoAcao(estado_t estado, evento_t evento) {
    return matrizTransicaoEstados[estado][evento];
}

void executarAcao(acao_t acao) {
    // TODO Executa uma acao
}

void setup() {
    // Inicializa maquina de estados
    initMatrizTransicao();
    estado = IDLE;
}

void loop() {
    evento_t evento = obterEvento();
    if (evento != NENHUM_EVENTO) {
        proxEstadoAcao_t proxEstadoAcao = obterProxEstadoAcao(estado, evento);
        executarAcao(proxEstadoAcao.acao);
        estado = proxEstadoAcao.estado;
    }
}


