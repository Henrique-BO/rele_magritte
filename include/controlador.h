#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "AccelStepper.h"
// #include "definicoes_sistema.h"

#define MAX_SPEED 100.0
#define CONTROLADOR_DELAY 10 // TODO calcular a frequencia necessaria

void vTaskControlador(void *param);

class Controlador {
    public:
        Controlador();
        Controlador(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ);
        void iniciarControlador();
        void enviarComando(int G, float X, float Y, float Z);
        void calibrar();
        void origem();
        void taskControlar();

        bool chegou;
    
    private:
        AccelStepper *pStepperX;
        AccelStepper *pStepperY;
        AccelStepper *pStepperZ;
};

#endif