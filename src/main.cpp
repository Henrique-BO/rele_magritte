#include <Arduino.h>
#include "SPIFFS.h"

#include "definicoes_sistema.h"
#include "maquina_estados.h"
#include "controlador.h"
#include "interface_wifi.h"
#include "interpretador_g.h"
#include "sensor_curso.h"

MaquinaEstados maquinaEstados;
Controlador controlador(PIN_STEP_X, PIN_DIR_X, PIN_STEP_Y, PIN_DIR_Y, PIN_STEP_Z, PIN_DIR_Z);
InterfaceWiFi interfaceWifi;
InterpretadorG interpretadorG;
SensorCurso sensorCurso1(PIN_SENSOR_1);
SensorCurso sensorCurso2(PIN_SENSOR_2);

QueueHandle_t xQueueEventos;

void setup() {
    // Inicia o serial
    Serial.begin(9600);

    // Sistema de arquivos
    if (!SPIFFS.begin()) {
        Serial.println("Erro na montagem do SPIFFS");
    }
    
    // Fila de eventos
    xQueueEventos = xQueueCreate(QUEUE_EVENTOS_SIZE, sizeof(Evento));

    // Inicializa máquina de estados
    maquinaEstados.iniciarMaquinaEstados();

    // Inicializa a Interface WiFi

    // Inicializa os Sensores de fim de curso

    // Inicializa o controlador
    controlador.iniciarControlador();

    // Inicializa o interpretador de código G
    interpretadorG.iniciarInterpretadorG(controlador);

    vTaskStartScheduler();
    while (1);
}

void loop() {

}


