#ifndef INTERPRETADOR_G_H
#define INTERPRETADOR_G_H

#include "SPIFFS.h"

#include "GCodeParser.h"
#include "controlador.h"

#define INTERPRETADOR_DELAY 100

void vTaskInterpretadorG(void *param);

class InterpretadorG {
    public:
        InterpretadorG(Controlador& controlador): controlador(controlador){}
        void iniciarInterpretadorG();
        void imprimir();
        void cancelar();
        void taskExecutar();

    private:
        File file;
        GCodeParser GCode;
        Controlador controlador;
        bool imprimindo = false;
};

#endif