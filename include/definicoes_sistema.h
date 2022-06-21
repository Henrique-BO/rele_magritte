/**
 * @file definicoes_sistema.h
 * @brief Definições do sistema, como pinagens
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DEFINICOES_SISTEMA_H
#define DEFINICOES_SISTEMA_H

#include <Arduino.h>

#include "maquina_estados.h"

// Pinos dos motores de passo
#define PIN_STEP_X 16
#define PIN_DIR_X 17
#define PIN_STEP_Y 18
#define PIN_DIR_Y 19
#define PIN_STEP_Z 20
#define PIN_DIR_Z 21

// Pinos dos sensores de fim de curso
#define PIN_SENSOR_1 4
#define PIN_SENSOR_2 5

#endif