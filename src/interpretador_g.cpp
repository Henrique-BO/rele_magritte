#include <Arduino.h>
#include "SPIFFS.h"

#include "GCodeParser.h"
#include "maquina_estados.h"
#include "controlador.h"
#include "interpretador_g.h"

void InterpretadorG::iniciarInterpretadorG()
{
    Serial.println("[Interpretador G] Iniciando interpretador G");

    imprimindo = false;
    xTaskCreate(
        vTaskInterpretadorG,
        "Interpretador G",
        10000,
        this,
        1,
        NULL
    );
}

void InterpretadorG::imprimir()
{
    Serial.println("[InterpretadorG] Iniciando impressão");
    file = SPIFFS.open("/gcode.txt", FILE_READ);
    Serial.println("[InterpretadorG] Arquivo gcode.txt aberto");
    imprimindo = true;
    line_number = 1;
}

void InterpretadorG::cancelar()
{
    imprimindo = false;
    file.close();
}

void InterpretadorG::taskExecutar()
{
    int G;
    float X, Y, Z;

    while(xSemaphoreControlador == NULL) {
        vTaskDelay(pdMS_TO_TICKS(INTERPRETADOR_DELAY));
    }

    while(1) {
        if (imprimindo && (xSemaphoreControlador != NULL)) { // estado Imprimindo
            // Serial.println("[InterpretadorG] Aguardando semáforo");
            if (xSemaphoreTake(xSemaphoreControlador, (TickType_t) 0) == pdTRUE) {
                Serial.println("[InterpretadorG] Semáforo capturado");
                if (file.available()) {
                    // TODO ler a próxima linha enquanto executa a atual
                    Serial.printf("[InterpretadorG] Lendo linha %d\n", line_number++);
                    while(!GCode.AddCharToLine(file.read())); // Le uma linha inteira do arquivo
                    GCode.ParseLine();
                    Serial.println("[InterpretadorG] Linha lida");
                    
                    if (GCode.HasWord('G')) {
                        G = GCode.GetWordValue('G');
                    }
                    if (GCode.HasWord('X')) {
                        X = GCode.GetWordValue('X');
                    }
                    if (GCode.HasWord('Y')) {
                        Y = GCode.GetWordValue('Y');
                    }
                    if (GCode.HasWord('Z')) {
                        Z = GCode.GetWordValue('Z');
                    }

                    controlador.enviarComando(G, X, Y, Z);

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
    static_cast<InterpretadorG *>(param)->taskExecutar();
}
