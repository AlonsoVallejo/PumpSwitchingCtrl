#ifndef DI_INPUTS_H
#define DI_INPUTS_H

#include <Arduino.h>

class DI_Inputs {
private:
    uint8_t pin;
public:
    DI_Inputs(uint8_t pin); 
    bool readInputPin();
    uint8_t getPin();
};

#endif