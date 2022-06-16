#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"

#include "GCodeParser.h"
#include "definicoes_sistema.h"
#include "interpretador_g.h"
#include "maquina_estados.h"
#include "controlador.h"

InterpretadorG::InterpretadorG(){}

void InterpretadorG::iniciarInterpretadorG(Controlador& controlador)
{
    GCode = GCodeParser();
    this->controlador = controlador;
    imprimindo = false;

    xTaskCreate(
        vTaskInterpretadorG,
        "Interpretador G",
        1000,
        this,
        1,
        NULL
    );
}

void InterpretadorG::imprimir()
{
    file = SPIFFS.open("gcode.txt");
    imprimindo = true;
}

void InterpretadorG::cancelar()
{
    imprimindo = false;
}

void InterpretadorG::taskExecutar()
{
    int G;
    float X, Y, Z;

    while(1) {
        if (imprimindo) { // estado Imprimindo
            if (controlador.chegou) { // pode receber próximo comando
                if (file.available()) {
                    // TODO ler a próxima linha enquanto executa a atual
                    while(!GCode.AddCharToLine(file.read())); // Le uma linha inteira do arquivo
                    GCode.ParseLine();
                    
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
                    }
                }

            }
        }
        vTaskDelay(INTERPRETADOR_DELAY / portTICK_PERIOD_MS);
    }
}

void vTaskInterpretadorG(void *param)
{
    static_cast<InterpretadorG *>(param)->taskExecutar();
}
