#include <Arduino.h>

#include "maquina_estados.h"
#include "interface_wifi.h"

void MaquinaEstados_t::iniciarMaquinaEstados()
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
        50000,
        this,
        2,
        NULL
    );
}

ProxEstadoAcao MaquinaEstados_t::obterProxEstadoAcao(Estado estado, Evento evento) {
    return matrizTransicaoEstados[estado][evento];
}

void MaquinaEstados_t::executarAcao(Acao acao) {
    switch(acao) {
    case NENHUMA_ACAO:
        break;
    case A01: // Carregar programa
        Serial.println("[MaquinaEstados] Carregando programa");
        InterfaceWiFi.carregando = true;
        while(InterfaceWiFi.carregando) vTaskDelay(pdMS_TO_TICKS(100));
        Serial.println("[MaquinaEstados] Programa carregado");
        break;
    case A02: // Calibrar
        Serial.println("[MaquinaEstados] Iniciando calibração");
        Controlador.calibrar();
        break;
    case A03: // Imprimir
        Serial.println("[MaquinaEstados] Iniciando impressão");
        InterpretadorG.imprimir();
        break;
    case A04: // Caneta na origem
        Serial.println("[MaquinaEstados] Concluido");
        break;
    case A05:
        Serial.println("[MaquinaEstados] Cancelando impressão");
        InterpretadorG.cancelar();
        Controlador.cancelar();
        break;
    }
}

Estado MaquinaEstados_t::getEstado() 
{
    return estado;
}

void MaquinaEstados_t::taskExecutar()
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
    static_cast<MaquinaEstados_t *>(param)->taskExecutar();
}