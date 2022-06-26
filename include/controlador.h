#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "AccelStepper.h"
#include "sensor_curso.h"
#include "GCodeParser.h"

// Velocidade máxima de deslocamento
#define MAX_SPEED 30000.0

// Resolução de passo de cada eixo
#define STEPS_POR_MM_X 4.750893824 // 2pi/200*R
#define STEPS_POR_MM_Y 4.486397268
#define Z_CANETA_BAIXA -30
#define Z_CANETA_ALTA 0

// Margem de caibração no eixo X (em steps)
#define MARGEM_X 10

// Período da Task do controlador
#define CONTROLADOR_DELAY_MS 10

// Wrapper da Task do controlador
void vTaskControlador(void *param);

class Controlador {
    public:
        Controlador(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ);
        void iniciarControlador();
        // void enviarComando(int G, float X, float Y, float Z);
        void enviarComando(GCodeParser *pGCode);
        void calibrar();
        void cancelar();
        void origem();
        void taskControlar();
    
    private:
        AccelStepper *pStepperX;
        AccelStepper *pStepperY;
        AccelStepper *pStepperZ;

        // velocidade de deslocamento
        float speed = 50.0; // TODO calcular velocidade efetiva

        // flags de controle
        bool calibrando = false;
        // bool flag_cancelar = false;
        bool mover = false;
        bool movendo = false;
};

extern Controlador controlador;

extern SemaphoreHandle_t xSemaphoreControlador;

#endif