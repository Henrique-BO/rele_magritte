#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "AccelStepper.h"
#include "GCodeParser.h"
#include <queue>

#include "sensor_curso.h"

// Velocidade máxima de deslocamento
#define MAX_SPEED 30000.0

// Resolução de passo de cada eixo
#define STEPS_POR_MM_X 4.750893824 // 2pi/200*R
#define STEPS_POR_MM_Y 4.486397268
#define Z_CANETA_BAIXA -30
#define Z_CANETA_ALTA 0

// Margem de caibração no eixo X (em steps)
#define MARGEM_X 10

// Parâmetros de interpolação de arcos
#define ARC_TOLERANCE 0.002
#define N_ARC_CORRECTION 12
#define ARC_ANGULAR_TRAVEL_EPSILON 5e-7

// Período da Task do controlador
#define CONTROLADOR_DELAY_MS 10

typedef struct {
    int stepsX;
    int stepsY;
    int stepsZ;
} ponto_steps_t; // posição da caneta em steps

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
        // seta a posição desejada em cada eixo, calculando e configurando
        // as velocidades para obter movimento retilíneo no plano XY (se linear == true)
        void nextTarget();

        void arco(ponto_steps_t position, ponto_steps_t target, ponto_steps_t offset, bool is_clockwise_arc);

        // ponteiros para os motores de passo
        AccelStepper *pStepperX;
        AccelStepper *pStepperY;
        AccelStepper *pStepperZ;

        // velocidade de deslocamento
        float speed = 50.0; // TODO calcular velocidade efetiva

        // flags de controle
        bool calibrando = false;
        bool mover = false;
        bool movendo = false;
        bool linear = false;

        // fila de targets
        std::queue<ponto_steps_t> filaTargets;
};

extern Controlador controlador;

extern SemaphoreHandle_t xSemaphoreControlador;

#endif