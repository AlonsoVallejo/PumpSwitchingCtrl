#ifndef DI_OUTPUTS_H
#define DI_OUTPUTS_H

#include <Arduino.h>

class DO_Outputs
{
private:
    uint8_t pin;
public:
    DO_Outputs(uint8_t pin); 
    void setOutputPin(bool state);
    uint8_t getPin();
};


#endif