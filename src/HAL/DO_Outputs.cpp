#include "DO_Outputs.h"

/**
 * @brief Constructor for DO_Outputs class.
 * Initializes the pin and sets it as an output.
 * @param pin The pin number to be used for digital output.
 */
DO_Outputs::DO_Outputs(uint8_t pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}
/**
 * @brief Sets the state of the output pin.
 * @param state True to set the pin HIGH, false to set it LOW.
 */
void DO_Outputs::setOutputPin(bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
}
/**
 * @brief Gets the pin number associated with this output.
 * @return The pin number.
 */
uint8_t DO_Outputs::getPin() {
    return pin;
}