#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "DO_Outputs.h"

class DigitalActuator : public DO_Outputs
{
public:
    DigitalActuator(uint8_t pin) : DO_Outputs(pin) {};
    void activate();
    void deactivate();
    void toggle();
    bool isActive();
    void setState(bool state);
    bool getState();
};

#endif