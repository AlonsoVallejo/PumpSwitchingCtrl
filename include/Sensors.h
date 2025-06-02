#ifndef SENSORS_H
#define SENSORS_H

#include "DI_Inputs.h"

class DigitalSensor : public DI_Inputs
{
private:
    bool SensorState = false;
public:
    DigitalSensor(uint8_t pin) : DI_Inputs(pin){};
    void PollSensorState();
    bool isSensorActive();
};

#endif