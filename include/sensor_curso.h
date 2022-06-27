#ifndef SENSOR_CURSO_H
#define SENSOR_CURSO_H

class SensorCurso_t {
    public:
        SensorCurso_t(int pin): pin(pin){}
        void iniciarSensor();
        bool origem();

    private:
        int pin;
};

extern SensorCurso_t SensorCurso;

#endif