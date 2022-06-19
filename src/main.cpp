#include <Arduino.h>
#include "SPIFFS.h"

#include "definicoes_sistema.h"
#include "maquina_estados.h"
#include "controlador.h"
#include "interface_wifi.h"
#include "interpretador_g.h"
#include "sensor_curso.h"

InterfaceWiFi interfaceWifi;
SensorCurso sensorCurso1(PIN_SENSOR_1);
SensorCurso sensorCurso2(PIN_SENSOR_2);
Controlador controlador(PIN_STEP_X, PIN_DIR_X, PIN_STEP_Y, PIN_DIR_Y, PIN_STEP_Z, PIN_DIR_Z, sensorCurso1, sensorCurso2);
InterpretadorG interpretadorG(controlador);

MaquinaEstados maquinaEstados(controlador, interpretadorG);

QueueHandle_t xQueueEventos;

void setup() {
    // Inicia o serial
    Serial.begin(9600);

    // Sistema de arquivos
    if (!SPIFFS.begin()) {
        Serial.println("Erro na montagem do SPIFFS");
    }
    
    // Cria fila de eventos
    xQueueEventos = xQueueCreate(QUEUE_EVENTOS_SIZE, sizeof(Evento));

    // Inicializa máquina de estados
    maquinaEstados.iniciarMaquinaEstados();

    // Inicializa a Interface WiFi
    interfaceWifi.iniciarWiFi();

    // Inicializa os Sensores de fim de curso
    sensorCurso1.iniciarSensor();
    sensorCurso2.iniciarSensor();

    // Inicializa o controlador
    controlador.iniciarControlador();

    // Inicializa o interpretador de código G
    interpretadorG.iniciarInterpretadorG();

    vTaskStartScheduler();
    while (1);
}

void loop() {

}


