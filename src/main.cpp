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
Controlador controlador(PIN_STEP_X, PIN_DIR_X, PIN_STEP_Y, PIN_DIR_Y, PIN_STEP_Z, PIN_DIR_Z);
InterpretadorG interpretadorG;
MaquinaEstados maquinaEstados;

QueueHandle_t xQueueEventos;

void vTaskEventoSerial(void *param)
{   
    char c;
    Evento evento;
    while(1) {
        if (Serial.available() > 0) {
            c = Serial.read();
            if ((c >= '0') && (c <= '9')) {
                evento = static_cast<Evento>(c - '0');
                xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup() {

    // Inicia o serial
    Serial.begin(115200);
    Serial.println("\n==== Relé Magritte ===");

    // Sistema de arquivos
    if (!SPIFFS.begin()) {
        Serial.println("[SPIFFS] Erro na montagem do SPIFFS");
    }
    Serial.println("[SPIFFS] SPIFFS montado");

    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file){
        Serial.print("[SPIFFS] FILE: ");
        Serial.print(file.name());
        Serial.print(" (");
        Serial.print(file.size());
        Serial.println(")");
        file = root.openNextFile();
    }
    file = SPIFFS.open("/gcode.txt", FILE_READ);
    
    // Cria fila de eventos
    xQueueEventos = xQueueCreate(QUEUE_EVENTOS_SIZE, sizeof(Evento));

    // Inicializa máquina de estados
    maquinaEstados.iniciarMaquinaEstados();

    // Inicializa a Interface WiFi
    interfaceWifi.iniciarWiFi();

    // Inicializa a interface Serial
    xTaskCreate(
        vTaskEventoSerial,
        "Evento Serial",
        1000,
        NULL,
        1,
        NULL
    );

    // Inicializa os Sensores de fim de curso
    sensorCurso1.iniciarSensor();
    sensorCurso2.iniciarSensor();

    // Inicializa o controlador
    controlador.iniciarControlador();

    // Inicializa o interpretador de código G
    interpretadorG.iniciarInterpretadorG();

    Serial.println("==== Inicialização concluída! ====\n");
}

void loop() {
}


