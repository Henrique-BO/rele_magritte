#ifndef DEFINICOES_SISTEMA_H
#define DEFINICOES_SISTEMA_H

#include <Arduino.h>

#include "maquina_estados.h"
// #include "controlador.h"
// #include "interface_wifi.h"
// #include "interpretador_g.h"
// #include "sensor_curso.h"

// Motores de passo
#define PIN_STEP_X 1
#define PIN_DIR_X 1
#define PIN_STEP_Y 1
#define PIN_DIR_Y 1
#define PIN_STEP_Z 1
#define PIN_DIR_Z 1

// Sensores de fim de curso
#define PIN_SENSOR_1 1
#define PIN_SENSOR_2 1

#define QUEUE_EVENTOS_SIZE 5

extern QueueHandle_t xQueueEventos;

extern MaquinaEstados maquinaEstados;
// extern Controlador controlador;
// extern InterfaceWiFi interfaceWifi;
// extern InterpretadorG interpretadorG;
// extern SensorCurso sensorCurso1;
// extern SensorCurso sensorCurso2;

#endif