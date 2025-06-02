#include "Sensors.h"

#define DEBOUNCE_DELAY_MS 200

/**
 * @brief Polls the Sensor state and updates the SensorState variable.
 * Implements a debounce mechanism to avoid false readings due to mechanical bounce.
 */
void DigitalSensor::PollSensorState() {
    uint64_t currentTime = millis();
    static uint64_t lastActiveTime = 0;
    bool isActive = readInputPin();
    if (isActive && (currentTime - lastActiveTime > DEBOUNCE_DELAY_MS)) {
        lastActiveTime = currentTime;
        SensorState = true;  
    } else if (!isActive) {
        SensorState = false;
    }
}
/**
 * @brief Checks if the Sensor is currently active.
 * @return True if the Sensor is active, false otherwise.
 */
bool DigitalSensor::isSensorActive() {
    return SensorState;
}

