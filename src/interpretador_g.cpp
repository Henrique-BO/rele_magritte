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
    int i = 1;

    while(1) {
        if (imprimindo) { // estado Imprimindo
            if (controlador.chegou) { // pode receber próximo comando
                if (file.available()) {
                    // TODO ler a próxima linha enquanto executa a atual
                    Serial.print("[InterpretadorG] Lendo linha ");
                    Serial.println(i++);
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
                    Evento evento = TERMINADO;
                    if (xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) == pdTRUE) {
                        imprimindo = false;
                        file.close();
                        i = 0;
                    }
                }

            }
        }
        vTaskDelay(pdTICKS_TO_MS(INTERPRETADOR_DELAY));
    }
}

void vTaskInterpretadorG(void *param)
{
    static_cast<InterpretadorG *>(param)->taskExecutar();
}
