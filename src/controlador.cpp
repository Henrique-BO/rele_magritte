#include <Arduino.h>
#include "AccelStepper.h"
#include <cmath>

#include "maquina_estados.h"
#include "controlador.h"
#include "sensor_curso.h"

Controlador::Controlador(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ) 
{
    pStepperX = new AccelStepper(AccelStepper::DRIVER, pinStepX, pinDirX);
    pStepperY = new AccelStepper(AccelStepper::DRIVER, pinStepY, pinDirY);
    pStepperZ = new AccelStepper(AccelStepper::DRIVER, pinStepZ, pinDirZ);

    pStepperX->setMaxSpeed(MAX_SPEED);
    pStepperY->setMaxSpeed(MAX_SPEED);
    pStepperZ->setMaxSpeed(MAX_SPEED);
    
}

void Controlador::iniciarControlador()
{
    Serial.println("[Controlador] Iniciando controlador");

    calibrando = false;
    xTaskCreate(
        vTaskControlador,
        "Controlador",
        1000,
        this,
        1,
        NULL
    );
}


void Controlador::enviarComando(int G, float X, float Y, float Z)
{
    int stepsX = (int) X / MM_POR_STEP_X;
    int stepsY = (int) Y / MM_POR_STEP_Y;
    int stepsZ = (int) Z / MM_POR_STEP_Z;

    if (G == 0) {
        pStepperX->moveTo(stepsX);
        pStepperY->moveTo(stepsY);
        pStepperZ->moveTo(stepsZ);

        pStepperX->setSpeed(speed);
        pStepperY->setSpeed(speed);
        pStepperZ->setSpeed(speed);
    } else if (G == 1) {
        pStepperX->moveTo(stepsX);
        pStepperY->moveTo(stepsY);
        pStepperZ->moveTo(stepsZ);

        // Velocidades para trajetoria reta XY
        float dist = sqrt(pow(stepsX - pStepperX->currentPosition(),2) + 
                          pow(stepsY - pStepperY->currentPosition(),2) 
                         );
        float speedX = speed * (stepsX - pStepperX->currentPosition()) / dist;
        float speedY = speed * (stepsY - pStepperY->currentPosition()) / dist;

        pStepperX->setSpeed(speedX);
        pStepperY->setSpeed(speedY);
        pStepperZ->setSpeed(speed);
    }
}

void Controlador::calibrar()
{
    calibrando = true;
    pStepperX->move(-1);
    pStepperX->setSpeed(speed);
}

void Controlador::taskControlar()
{
    while(1) {
        if (!calibrando) {
            if ((pStepperX->distanceToGo() == 0) && (pStepperY->distanceToGo() == 0) && (pStepperZ->distanceToGo() == 0)) {
                chegou = true;
            } else {
                chegou = false;
                pStepperX->runSpeedToPosition();
                pStepperY->runSpeedToPosition();
                pStepperZ->runSpeedToPosition();
            }
        } else { // calibrando
            if (sensorCurso1.origem()) {
                pStepperX->setCurrentPosition(-MARGEM_X);
                pStepperX->move(1);
                Evento evento = ORIGEM;
                while(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE);
                calibrando = false;
            }
            pStepperX->runSpeedToPosition();
        }
        vTaskDelay(CONTROLADOR_DELAY / portTICK_PERIOD_MS);
    }
}

void vTaskControlador(void *param)
{
    static_cast<Controlador *>(param)->taskControlar();
}
