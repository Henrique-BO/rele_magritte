#include <Arduino.h>
#include "sensor_curso.h"

void SensorCurso::iniciarSensor()
{
    Serial.print("[SensorCurso] Iniciando sensor de fim de curso no pino ");
    Serial.println(pin);

    pinMode(pin, INPUT_PULLUP);
}

bool SensorCurso::origem()
{
    return (digitalRead(pin) == LOW);
}