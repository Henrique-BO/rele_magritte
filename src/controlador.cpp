#include <Arduino.h>
#include "AccelStepper.h"

#include "controlador.h"
#include "definicoes_sistema.h"

Controlador::Controlador(){}

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
    xTaskCreate(
        vTaskControlador,
        "Controlador",
        1000,
        this,
        1,
        NULL
    );
}

void Controlador::taskControlar()
{
    while(1) {
        if ((pStepperX->distanceToGo() == 0) && (pStepperY->distanceToGo() == 0) && (pStepperZ->distanceToGo() == 0)) {
            chegou = true;
        } else {
            chegou = false;
            pStepperX->runSpeedToPosition();
            pStepperY->runSpeedToPosition();
            pStepperZ->runSpeedToPosition();
        }
        vTaskDelay(CONTROLADOR_DELAY / portTICK_PERIOD_MS);
    }
}

void Controlador::enviarComando(int G, float X, float Y, float Z)
{

}


void vTaskControlador(void *param)
{
    static_cast<Controlador *>(param)->taskControlar();
}
