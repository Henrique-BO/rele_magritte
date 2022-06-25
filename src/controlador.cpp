#include <Arduino.h>
#include "AccelStepper.h"
#include <cmath>

#include "maquina_estados.h"
#include "controlador.h"
#include "sensor_curso.h"

SemaphoreHandle_t xSemaphoreControlador;

Controlador::Controlador(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ) 
{
    pStepperX = new AccelStepper(AccelStepper::DRIVER, pinStepX, pinDirX);
    pStepperY = new AccelStepper(AccelStepper::DRIVER, pinStepY, pinDirY);
    pStepperZ = new AccelStepper(AccelStepper::DRIVER, pinStepZ, pinDirZ);
    
    pStepperX->setMaxSpeed(MAX_SPEED);
    pStepperY->setMaxSpeed(MAX_SPEED);
    pStepperZ->setMaxSpeed(MAX_SPEED);

    pStepperY->setPinsInverted(true, false, false);
}

void Controlador::iniciarControlador()
{
    Serial.println("[Controlador] Iniciando controlador");

    calibrando = false;
    xTaskCreate(
        vTaskControlador,
        "Controlador",
        10000,
        this,
        1,
        NULL
    );
}

// void Controlador::enviarComando(int G, float X, float Y, float Z)
void Controlador::enviarComando(GCodeParser *pGCode)
{
    static int G = -1;
    static int stepsX, stepsY, stepsZ;

    if (pGCode->HasWord('G')) {
        G = pGCode->GetWordValue('G');
    }
    if (pGCode->HasWord('X')) {
        int X = pGCode->GetWordValue('X');
        stepsX = (int) round(X * STEPS_POR_MM_X);
    }
    if (pGCode->HasWord('Y')) {
        int Y = pGCode->GetWordValue('Y');
        stepsY = (int) round(Y * STEPS_POR_MM_Y);
    }
    if (pGCode->HasWord('Z')) {
        int Z = pGCode->GetWordValue('Z');
        if (Z >= 0) {
            stepsZ = 0;
        } else {
            stepsZ = Z_CANETA_BAIXA;
        }
    }

    Serial.printf("[Controlador] G%d stepsX=%d stepsY=%d stepsZ=%d\n", G, stepsX, stepsY, stepsZ);

    if (G == 0) {
        Serial.println("[Controlador] Movimentação rápida");
        pStepperX->moveTo(stepsX);
        pStepperY->moveTo(stepsY);
        pStepperZ->moveTo(stepsZ);

        pStepperX->setSpeed(speed);
        pStepperY->setSpeed(speed);
        pStepperZ->setSpeed(speed);
    } else if (G == 1) {
        Serial.println("[Controlador] Movimento linear");
        pStepperX->moveTo(stepsX);
        pStepperY->moveTo(stepsY);
        pStepperZ->moveTo(stepsZ);

        // Velocidades para trajetoria reta no plano XY
        float dist = sqrt(pow(stepsX - pStepperX->currentPosition(),2) + 
                          pow(stepsY - pStepperY->currentPosition(),2) 
                         );
        float speedX = speed * (stepsX - pStepperX->currentPosition()) / dist;
        float speedY = speed * (stepsY - pStepperY->currentPosition()) / dist;

        pStepperX->setSpeed(speedX);
        pStepperY->setSpeed(speedY);
        pStepperZ->setSpeed(speed);
    }
    // TODO arcos

    mover = true;
}

void Controlador::calibrar()
{
    calibrando = true;
    mover = false;
    movendo = false;
    flag_cancelar = false;

    Serial.println("[Controlador] Iniciando calibração");
    pStepperX->setSpeed(-speed);
}

void Controlador::cancelar()
{
    // TODO levantar caneta
    movendo = false;
    flag_cancelar = true;
    Serial.println("[Controlador] Cancelando movimento");
}

void Controlador::taskControlar()
{
    // TODO quando terminar jogar o papel pra fora
    xSemaphoreGive(xSemaphoreControlador);

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(CONTROLADOR_DELAY_MS);

    while(xSemaphoreControlador == NULL) {
        vTaskDelay(xFrequency);
    }

    while(1) {
        xLastWakeTime = xTaskGetTickCount();
        if (!calibrando) {
            if (mover) {
                if (xSemaphoreTake(xSemaphoreControlador, portMAX_DELAY) == pdTRUE) {
                    Serial.println("[Controlador] Semáforo capturado, iniciando movimento");
                    Serial.printf("[Controlador] dX=%d dY=%d dZ=%d\n", pStepperX->distanceToGo(), pStepperY->distanceToGo(), pStepperZ->distanceToGo());
                    mover = false;
                    movendo = true;
                } else {
                    Serial.println("[Controlador] Falha ao capturar semáforo");
                }
            }
            if (flag_cancelar) {
                if (xSemaphoreGive(xSemaphoreControlador) != pdTRUE) {
                    Serial.println("[Controlador] Falha ao ceder semáforo");
                }
                flag_cancelar = false;
                Serial.println("[Controlador] Movimento cancelado");
            }
            if (movendo) {
                if ((pStepperX->distanceToGo() != 0) || (pStepperY->distanceToGo() != 0) || (pStepperZ->distanceToGo() != 0)) {
                    pStepperX->runSpeedToPosition();
                    pStepperY->runSpeedToPosition();
                    pStepperZ->runSpeedToPosition();
                } else {
                    if (xSemaphoreGive(xSemaphoreControlador) != pdTRUE) {
                        Serial.println("[Controlador] Falha ao ceder semáforo");
                    }
                    movendo = false;
                    Serial.println("[Controlador] Chegou");
                }
            }
        } else { // calibrando
            if (sensorCurso1.origem()) {
                pStepperX->setCurrentPosition(-MARGEM_X);
                pStepperX->moveTo(0);
                Evento evento = ORIGEM;
                if (xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
                    Serial.println("[Controlador] Erro ao enviar evento à fila");
                }
                calibrando = false;
            }
            pStepperX->runSpeed();
        }
        // Delay para obter frequência constante
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void vTaskControlador(void *param)
{
    // Cria semáforo de controlador
    xSemaphoreControlador = xSemaphoreCreateBinary();

    static_cast<Controlador *>(param)->taskControlar();
}
