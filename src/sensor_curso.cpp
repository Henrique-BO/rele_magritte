#include <Arduino.h>
#include "sensor_curso.h"

void SensorCurso::iniciarSensor()
{
    pinMode(pin, INPUT_PULLUP);
}

bool SensorCurso::origem()
{
    return digitalRead(pin);
}