#ifndef SENSOR_CURSO_H
#define SENSOR_CURSO_H

class SensorCurso {
    public:
        SensorCurso(int pin): pin(pin){}
        void iniciarSensor();
        bool origem();

    private:
        int pin;
};

#endif