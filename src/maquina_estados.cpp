#include <Arduino.h>

#include "maquina_estados.h"
#include "interface_wifi.h"

void MaquinaEstados::iniciarMaquinaEstados()
{
    Serial.println("[MaquinaEstados] Iniciando máquina de estados");
    
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
    // matrizTransicaoEstados[IMPRIMINDO][TERMINADO].estado = CALIBRANDO;
    // matrizTransicaoEstados[IMPRIMINDO][TERMINADO].acao = A02;
    matrizTransicaoEstados[IMPRIMINDO][TERMINADO].estado = IDLE;
    matrizTransicaoEstados[IMPRIMINDO][TERMINADO].acao = A04;

    matrizTransicaoEstados[IMPRIMINDO][CANCELAR].estado = IDLE;
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
        20000,
        this,
        2,
        NULL
    );
}

ProxEstadoAcao MaquinaEstados::obterProxEstadoAcao(Estado estado, Evento evento) {
    return matrizTransicaoEstados[estado][evento];
}

void MaquinaEstados::executarAcao(Acao acao) {
    switch(acao) {
    case NENHUMA_ACAO:
        break;
    case A01: // Carregar programa
        Serial.println("[MaquinaEstados] Carregando programa");
        interfaceWifi.carregando = true;
        while(interfaceWifi.carregando) vTaskDelay(pdMS_TO_TICKS(100));
        Serial.println("[MaquinaEstados] Programa carregado");
        break;
    case A02: // Calibrar
        Serial.println("[MaquinaEstados] Iniciando calibração");
        controlador.calibrar();
        break;
    case A03: // Imprimir
        Serial.println("[MaquinaEstados] Iniciando impressão");
        interpretadorG.imprimir();
        break;
    case A04: // Caneta na origem
        Serial.println("[MaquinaEstados] Caneta na origem");
        break;
    case A05:
        Serial.println("[MaquinaEstados] Cancelando impressão");
        interpretadorG.cancelar();
        controlador.cancelar();
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
        Evento evento;
        if (xQueueReceive(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
            Serial.println("[MaquinaEstados] Falha ao receber evento");
        }
        if (evento != NENHUM_EVENTO) {
            ProxEstadoAcao proxEstadoAcao = obterProxEstadoAcao(estado, evento);
            Serial.printf("[MaquinaEstados] Evento: %s; Estado: %s; Ação: %s\n", 
                          eventos_str[evento], 
                          estados_str[proxEstadoAcao.estado], 
                          acoes_str[proxEstadoAcao.acao]
            );

            executarAcao(proxEstadoAcao.acao);
            estado = proxEstadoAcao.estado;
        }
    }
}

void vTaskMaquinaEstados(void *param) {
    static_cast<MaquinaEstados *>(param)->taskExecutar();
}