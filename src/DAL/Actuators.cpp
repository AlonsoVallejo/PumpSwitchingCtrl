#include "Actuators.h"

/**
 * @brief Activates the actuator by setting the output pin HIGH.
 */
void DigitalActuator::activate() {
    setOutputPin(true);
}
/**
 * @brief Deactivates the actuator by setting the output pin LOW.
 */
void DigitalActuator::deactivate() {
    setOutputPin(false);
}
/**
 * @brief Toggles the state of the actuator.
 * If the actuator is active, it deactivates it; if it's inactive, it activates it.
 */
void DigitalActuator::toggle() {
    setOutputPin(!getState());
}
/**
 * @brief Checks if the actuator is currently active.
 * @return True if the actuator is active, false otherwise.
 */
bool DigitalActuator::isActive() {
    return getState();
}
/**
 * @brief Sets the state of the actuator.
 * @param state True to activate the actuator, false to deactivate it.
 */
void DigitalActuator::setState(bool state) {
    setOutputPin(state);
}
/**
 * @brief Gets the current state of the actuator.
 * @return True if the actuator is active, false otherwise.
 */
bool DigitalActuator::getState() {
    return digitalRead(getPin()) == HIGH;
}