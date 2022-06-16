#include <Arduino.h>
#include "definicoes_sistema.h"
#include "maquina_estados.h"

void MaquinaEstados::iniciarMaquinaEstados()
{
    // inicializa a matriz de transicao de estados
    int i;
    int j;

    for (i=0; i < NUM_ESTADOS; i++) {
        for (j=0; j < NUM_EVENTOS; j++) {
            matrizTransicaoEstados[i][j].estado = static_cast<Estado>(i);
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

    // Estado inicial
    estado = IDLE;

    // Cria a Task FreeRTOS
    xTaskCreate(
        vTaskMaquinaEstados,
        "Máquina de Estados",
        1000,
        this,
        1,
        NULL
    );
}

Evento MaquinaEstados::obterEvento() {
    // TODO Determina se um evento ocorreu
    Evento evento;
    if (xQueueReceive(xQueueEventos, &evento, 0) == pdTRUE) {
        return evento;
    } else {
        return NENHUM_EVENTO;
    }
}

ProxEstadoAcao MaquinaEstados::obterProxEstadoAcao(Estado estado, Evento evento) {
    return matrizTransicaoEstados[estado][evento];
}

void MaquinaEstados::executarAcao(Acao acao) {
    // TODO Executa uma acao
}

void MaquinaEstados::taskExecutar()
{
    while (1) {
        Evento evento = obterEvento();
        if (evento != NENHUM_EVENTO) {
            ProxEstadoAcao proxEstadoAcao = obterProxEstadoAcao(estado, evento);
            executarAcao(proxEstadoAcao.acao);
            estado = proxEstadoAcao.estado;

            Serial.print("Estado: ");
            Serial.print(estado);
            Serial.print("\tEvento: ");
            Serial.print(evento);
            Serial.print("\tAcao: ");
            Serial.println(proxEstadoAcao.acao);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void vTaskMaquinaEstados(void *param) {
    static_cast<MaquinaEstados *>(param)->taskExecutar();
}