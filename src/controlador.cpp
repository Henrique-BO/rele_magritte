#include <Arduino.h>
#include "AccelStepper.h"
#include <cmath>

#include "maquina_estados.h"
#include "controlador.h"
#include "sensor_curso.h"

SemaphoreHandle_t xSemaphoreControlador;

Controlador_t::Controlador_t(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinStepZ, int pinDirZ) 
{
    pStepperX = new AccelStepper(AccelStepper::DRIVER, pinStepX, pinDirX);
    pStepperY = new AccelStepper(AccelStepper::DRIVER, pinStepY, pinDirY);
    pStepperZ = new AccelStepper(AccelStepper::DRIVER, pinStepZ, pinDirZ);
    
    pStepperX->setMaxSpeed(MAX_SPEED);
    pStepperY->setMaxSpeed(MAX_SPEED);
    pStepperZ->setMaxSpeed(MAX_SPEED);

    pStepperY->setPinsInverted(true, false, false);
}

void Controlador_t::iniciarControlador()
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

void Controlador_t::enviarComando(GCodeParser *pGCode)
{
    static int G = -1;
    static int stepsX, stepsY, stepsZ, stepsI, stepsJ;

    // trata as palavras recebidas por código G
    // TODO passar o tratamento para o InterpretadorG e receber uma struct com os valores
    Serial.print("[Controlador] ");
    if (pGCode->HasWord('G')) {
        G = pGCode->GetWordValue('G');
        Serial.printf("G%d ", G);
    }
    if (pGCode->HasWord('X')) {
        double X = pGCode->GetWordValue('X');
        stepsX = (int) round(X * STEPS_POR_MM_X);
        Serial.printf("X%f ", X);
    }
    if (pGCode->HasWord('Y')) {
        double Y = pGCode->GetWordValue('Y');
        stepsY = (int) round(Y * STEPS_POR_MM_Y);
        Serial.printf("Y%f ", Y);
    }
    if (pGCode->HasWord('Z')) {
        double Z = pGCode->GetWordValue('Z');
        if (Z >= 0) {
            stepsZ = Z_CANETA_ALTA;
        } else {
            stepsZ = Z_CANETA_BAIXA;
        }
        Serial.printf("Z%f ", Z);
    }
    if (pGCode->HasWord('I')) {
        double I = pGCode->GetWordValue('I');
        stepsI = (int) round(I * STEPS_POR_MM_X);
        Serial.printf("I%f ", I);
    }
    if (pGCode->HasWord('J')) {
        double J = pGCode->GetWordValue('J');
        stepsJ = (int) round(J * STEPS_POR_MM_Y);
        Serial.printf("J%f ", J);
    }
    Serial.println("");

    if (G == 0) {
        Serial.println("[Controlador] Movimentação rápida");
        linear = false;
        ponto_steps_t target = {stepsX, stepsY, stepsZ};
        filaTargets.push(target);
        
    } else if (G == 1) {
        Serial.println("[Controlador] Movimento linear");
        linear = true;
        ponto_steps_t target = {stepsX, stepsY, stepsZ};
        filaTargets.push(target);

    } else if (G == 2) {
        Serial.println("[Controlador] Movimento em arco CCW");
        linear = true;
        ponto_steps_t position = {
            pStepperX->currentPosition(),
            pStepperY->currentPosition(),
            pStepperZ->currentPosition()
        };
        ponto_steps_t target = {stepsX, stepsY, stepsZ};
        ponto_steps_t offset = {stepsI, stepsJ, 0};
        Serial.printf("[Controlador] G%d sX%d sY%d sZ%d sI%d sJ%d\n", G, stepsX, stepsY, stepsZ, stepsI, stepsJ);
        arco(position, target, offset, true);
    
    } else if (G == 3) {
        Serial.println("[Controlador] Movimento em arco CW");
        linear = true;
        ponto_steps_t position = {
            pStepperX->currentPosition(),
            pStepperY->currentPosition(),
            pStepperZ->currentPosition()
        };
        ponto_steps_t target = {stepsX, stepsY, stepsZ};
        ponto_steps_t offset = {stepsI, stepsJ, 0};
        Serial.printf("[Controlador] G%d sX%d sY%d sZ%d sI%d sJ%d\n", G, stepsX, stepsY, stepsZ, stepsI, stepsJ);
        arco(position, target, offset, false);
    
    } else {
        Serial.println("[Controlador] Comando não implementado");
        return;
    }

    mover = true;
}

void Controlador_t::calibrar()
{
    calibrando = true;
    mover = false;
    movendo = false;

    Serial.println("[Controlador] Iniciando calibração");
    pStepperX->setSpeed(-CALIBRAR_SPPED);
}

void Controlador_t::cancelar()
{
    // para os eixos e levanta a caneta
    while (!filaTargets.empty()) {
        filaTargets.pop();
    }
    ponto_steps_t target = {
        pStepperX->currentPosition(),
        pStepperY->currentPosition(),
        Z_CANETA_ALTA
    };
    filaTargets.push(target);
    linear = false;
    nextTarget();
    Serial.println("[Controlador] Cancelando movimento");
}

void Controlador_t::zerarEixos()
{
    pStepperY->setCurrentPosition(0);
    pStepperZ->setCurrentPosition(0);
}

void Controlador_t::nextTarget()
{
    float speedX, speedY;
    
    // pega próximo target na fila
    ponto_steps_t target = filaTargets.front();
    filaTargets.pop();

    pStepperX->moveTo(target.stepsX);
    pStepperY->moveTo(target.stepsY);
    pStepperZ->moveTo(target.stepsZ);

    if (linear) {
        // Velocidades para trajetoria linear no plano XY
        float dist = sqrt(pow(target.stepsX - pStepperX->currentPosition(),2) + 
                          pow(target.stepsY - pStepperY->currentPosition(),2) 
                         );
        speedX = LINEAR_SPEED * (target.stepsX - pStepperX->currentPosition()) / dist;
        speedY = LINEAR_SPEED * (target.stepsY - pStepperY->currentPosition()) / dist;
    } else {
        speedX = FAST_SPEED;
        speedY = FAST_SPEED;
    }

    pStepperX->setSpeed(speedX);
    pStepperY->setSpeed(speedY);
    pStepperZ->setSpeed(FAST_SPEED);
    Serial.printf("[Controlador] Target X=%d Y=%d Z=%d\n", pStepperX->targetPosition(), pStepperY->targetPosition(), pStepperZ->targetPosition());
}

void Controlador_t::arco(ponto_steps_t position, ponto_steps_t target, ponto_steps_t offset, bool is_clockwise_arc)
{
    Serial.println("[Controlador] Interpolando trajetória em arco");
    // baseado em https://github.com/gnea/grbl/blob/master/grbl/motion_control.c
    int centerX = position.stepsX + offset.stepsX;
    int centerY = position.stepsY + offset.stepsY;
    float rX = -offset.stepsX;
    float rY = -offset.stepsY;
    float rtX = target.stepsX - centerX;
    float rtY = target.stepsY - centerY;
    
    // CCW angle between position and target from circle center. Only one atan2() trig computation required.
    float angular_travel = atan2(rX*rtY-rY*rtX, rX*rtX+rY*rtY);
    if (is_clockwise_arc) { // Correct atan2 output per direction
        if (angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON) { angular_travel -= 2*M_PI; }
    } else {
        if (angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON) { angular_travel += 2*M_PI; }
    }

    float radius = sqrt(rX*rX + rY*rY);
    // número de segmentos de reta
    int segments = floor(fabs(0.5*angular_travel*radius)/
                          sqrt(ARC_TOLERANCE*(2*radius - ARC_TOLERANCE)) );

    if ((radius != 0) && segments) {
        float theta_per_segment = angular_travel/segments;

        float cos_T = 2.0 - theta_per_segment*theta_per_segment;
        float sin_T = theta_per_segment*0.16666667*(cos_T + 4.0);
        cos_T *= 0.5;

        float sin_Ti;
        float cos_Ti;
        float rY_tmp;
        int i;
        int count = 0;

        for (i = 1; i<segments; i++) { // Increment (segments-1).
            if (count < N_ARC_CORRECTION) {
                // Apply vector rotation matrix. ~40 usec
                rY_tmp = rX*sin_T + rY*cos_T;
                rX = rX*cos_T - rY*sin_T;
                rY = rY_tmp;
                count++;
            } else {
                // Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments. ~375 usec
                // Compute exact location by applying transformation matrix from initial radius vector(=-offset).
                cos_Ti = cos(i*theta_per_segment);
                sin_Ti = sin(i*theta_per_segment);
                rX = -offset.stepsX*cos_Ti + offset.stepsY*sin_Ti;
                rY = -offset.stepsX*sin_Ti - offset.stepsY*cos_Ti;
                count = 0;
            }

            // Update arc_target location
            position.stepsX = centerX + round(rX);
            position.stepsY = centerY + round(rY);
            filaTargets.push(position);
        }
    }
    // Ensure last segment arrives at target location.
    filaTargets.push(target);
}

void Controlador_t::taskControlar()
{
    xSemaphoreGive(xSemaphoreControlador);

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(CONTROLADOR_DELAY_MS);

    while(xSemaphoreControlador == NULL) {
        vTaskDelay(xFrequency);
    }
    Serial.println("[Controlador] Task iniciada");

    while(1) {
        xLastWakeTime = xTaskGetTickCount();
        if (!calibrando) {
            if (mover) { // comando de mover do InterpretadorG
                if (xSemaphoreTake(xSemaphoreControlador, portMAX_DELAY) == pdTRUE) {
                    Serial.println("[Controlador] Semáforo capturado, iniciando movimento");
                    nextTarget();
                    mover = false;
                    movendo = true;
                } else {
                    Serial.println("[Controlador] Falha ao capturar semáforo");
                }
            }
            if (movendo) {
                if ((pStepperX->distanceToGo() != 0) || (pStepperY->distanceToGo() != 0) || (pStepperZ->distanceToGo() != 0)) {
                    pStepperX->runSpeedToPosition();
                    pStepperY->runSpeedToPosition();
                    pStepperZ->runSpeedToPosition();
                } else { // chegou no target atual
                    if (filaTargets.empty()) {
                        if (xSemaphoreGive(xSemaphoreControlador) != pdTRUE) {
                            Serial.println("[Controlador] Falha ao ceder semáforo");
                        }
                        movendo = false;
                        Serial.println("[Controlador] Movimento concluído");
                    } else { 
                        nextTarget();
                    }
                }
            }
        } else { // calibrando
            if (SensorCurso.origem()) {
                pStepperX->setCurrentPosition(-MARGEM_X);
                pStepperX->moveTo(0);
                Evento evento = ORIGEM;
                if (xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
                    Serial.println("[Controlador] Erro ao enviar evento à fila");
                }
                calibrando = false;
                Serial.println("[Controlador] Calibração concluída");
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

    static_cast<Controlador_t *>(param)->taskControlar();
}
