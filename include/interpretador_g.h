#ifndef INTERPRETADOR_G_H
#define INTERPRETADOR_G_H

#include "SPIFFS.h"

#include "GCodeParser.h"
#include "controlador.h"

#define INTERPRETADOR_DELAY 100

void vTaskInterpretadorG(void *param);

class InterpretadorG_t {
    public:
        InterpretadorG_t(){}
        void iniciarInterpretadorG();
        void imprimir();
        void cancelar();
        void taskExecutar();

    private:
        File file;
        GCodeParser GCode;
        bool imprimindo = false;
        int line_number = 1;
};

extern InterpretadorG_t InterpretadorG;

#endif