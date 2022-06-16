#include <Arduino.h>
#include "definicoes_sistema.h"
#include "maquina_estados.h"
#include "controlador.h"
#include "interface_wifi.h"
#include "interpretador_g.h"
#include "sensor_curso.h"

MaquinaEstados *pMaquinaEstados;
Controlador *pControlador;
InterfaceWiFi *pInterfaceWifi;
InterpretadorG *pInterpretadorG;
SensorCurso *pSensorCurso1;
SensorCurso *pSensorCurso2;

QueueHandle_t xQueueEventos;

void setup() {
    // Inicia o serial
    Serial.begin(9600);
    
    // Fila de eventos
    xQueueEventos = xQueueCreate(QUEUE_EVENTOS_SIZE, sizeof(int));

    // Inicializa máquina de estados
    pMaquinaEstados = new MaquinaEstados();
    pMaquinaEstados->iniciarMaquinaEstados();

    vTaskStartScheduler();
    while (1);
}

void loop() {

}


