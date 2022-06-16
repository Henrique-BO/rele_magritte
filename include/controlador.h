#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "AccelStepper.h"
// #include "definicoes_sistema.h"

#define MAX_SPEED 100.0

class Controlador {
    public:
        Controlador(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ);
        void enviarComando(int G, float X, float Y, float Z);
        void calibrar();
        void origem();
        void controlar();

        bool chegou;
    
    private:
        AccelStepper *pStepperX;
        AccelStepper *pStepperY;
        AccelStepper *pStepperZ;
};

#endif