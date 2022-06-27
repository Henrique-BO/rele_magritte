#include <Arduino.h>
#include "SPIFFS.h"
#include "GCodeParser.h"

#include "maquina_estados.h"
#include "controlador.h"
#include "interpretador_g.h"

void InterpretadorG_t::iniciarInterpretadorG()
{
    Serial.println("[Interpretador G] Iniciando interpretador G");

    imprimindo = false;
    xTaskCreate(
        vTaskInterpretadorG,
        "Interpretador G",
        50000,
        this,
        1,
        NULL
    );
}

void InterpretadorG_t::imprimir()
{
    Serial.println("[InterpretadorG] Iniciando impressão");
    file = SPIFFS.open("/gcode.txt", FILE_READ);
    Serial.println("[InterpretadorG] Arquivo gcode.txt aberto");
    Controlador.zerarEixos();
    imprimindo = true;
    line_number = 1;
}

void InterpretadorG_t::cancelar()
{
    imprimindo = false;
    file.close();
    Serial.println("[InterpretadorG] Impressão cancelada");
}

void InterpretadorG_t::taskExecutar()
{
    int G;
    float X, Y, Z;

    while(xSemaphoreControlador == NULL) {
        vTaskDelay(pdMS_TO_TICKS(INTERPRETADOR_DELAY));
    }
    Serial.println("[InterpretadorG] Task iniciada");

    while(1) {
        if (imprimindo) { // estado Imprimindo
            if (xSemaphoreTake(xSemaphoreControlador, (TickType_t) 0) == pdTRUE) {
                Serial.println("[InterpretadorG] Semáforo capturado");
                if (file.available()) {
                    Serial.printf("[InterpretadorG] Lendo linha %d\n", line_number++);
                    while(!GCode.AddCharToLine(file.read())); // Le uma linha inteira do arquivo
                    GCode.ParseLine();
                    Serial.println("[InterpretadorG] Linha lida");
                    
                    // controlador.enviarComando(G, X, Y, Z);
                    Controlador.enviarComando(&GCode);

                } else { // quando termina a última linha
                    Serial.println("[InterpretadorG] Programa terminado");

                    Evento evento = TERMINADO;
                    if (xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) == pdTRUE) {
                        imprimindo = false;
                        file.close();
                    } else {
                        Serial.println("[InterpretadorG] Erro ao enviar evento à fila");
                    }
                }
                xSemaphoreGive(xSemaphoreControlador);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(INTERPRETADOR_DELAY));
    }
}

void vTaskInterpretadorG(void *param)
{
    static_cast<InterpretadorG_t *>(param)->taskExecutar();
}
