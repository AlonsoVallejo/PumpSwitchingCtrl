#include "utilities.h"

/**
 * @brief Logs data to the serial monitor.
 * @param data The string to log.
 * @param IsLog A flag to indicate whether to log the data or not.
 */
void LogSerial(String data, bool IsLog) {
    if (IsLog) {
        Serial.print(data);
    }
}

/**
 * @brief Logs data to the serial monitor with a newline at the end.
 * @param data The string to log.
 * @param IsLog A flag to indicate whether to log the data or not.
 */
void LogSerialn(String data, bool IsLog) {
    if (IsLog) {
        Serial.println(data);
    }
}
