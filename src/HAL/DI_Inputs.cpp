#include "DI_Inputs.h"

/**
 * @brief Constructor for DI_Inputs class.
 * Initializes the pin and sets it as an input.
 * @param pin The pin number to be used for digital input. 
 */
DI_Inputs::DI_Inputs(uint8_t pin) : pin(pin) {
    pinMode(pin, INPUT);
}

/**
 * @brief Reads the digital input from the specified pin.
 * @return True if the input is HIGH, false if it is LOW.
 */
uint8_t DI_Inputs::getPin() {
    return pin;
}

/**
 * @brief Reads the digital input from the specified pin.
 * @return True if the input is HIGH, false if it is LOW.
 */
bool DI_Inputs::readInputPin() {
    return digitalRead(pin);
}