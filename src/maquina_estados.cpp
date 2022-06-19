#include <Arduino.h>
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
    matrizTransicaoEstados[IMPRIMINDO][CANCELAR].acao = A05;
    
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
    switch(acao) {
    case NENHUMA_ACAO:
        break;
    case A01: // Carregar programa
        Serial.println("Carregando programa");
        break;
    case A02: // Calibrar
        Serial.println("Iniciando calibração");
        controlador.calibrar();
        break;
    case A03: // Imprimir
        Serial.println("Iniciando impressão");
        interpretadorG.imprimir();
        break;
    case A04: // Caneta na origem
        Serial.println("Caneta na origem");
        break;
    case A05:
        Serial.println("Cancelando impressão");
        interpretadorG.cancelar();
        Serial.println("Iniciando calibração");
        controlador.calibrar();
        break;
    }
}

Estado MaquinaEstados::getEstado() 
{
    return estado;
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
        vTaskDelay(FSM_DELAY / portTICK_PERIOD_MS);
    }
}

void vTaskMaquinaEstados(void *param) {
    static_cast<MaquinaEstados *>(param)->taskExecutar();
}