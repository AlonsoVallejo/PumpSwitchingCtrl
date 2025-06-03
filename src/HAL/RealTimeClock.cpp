#include "RealTimeClock.h"

/**
 * @brief Constructor for RealTimeClock class.
 * Initializes the RTC_DS3231 object.
 */
RealTimeClock::RealTimeClock() : rtc() {}

/**
 * @brief Begins the RTC by initializing it.
 * This function checks if the RTC is connected and sets the current date and time if not already set.
 */
void RealTimeClock::begin() {
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    // Check if the RTC lost power and if so, set the date and time
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
    }
}

/**
 * @brief Gets the current date and time from the RTC.
 * @return The current DateTime object.
 */
DateTime RealTimeClock::GetCurrentDateTime() {
    return rtc.now();
}

/**
 * @brief Sets the date and time of the RTC.
 * @param dt The DateTime object to set.
 */
void RealTimeClock::setDateTime(const DateTime &dt) {
    rtc.adjust(dt);
}
/**
 * @brief Gets the current date and time formatted as a string.
 * @return A string in the format "DD/MM HH:MM:SS".
 */
String RealTimeClock::getFormattedDateTime() {
    DateTime now = GetCurrentDateTime();
    char buf[18];
    snprintf(buf, sizeof(buf), "%02d/%02d %02d:%02d:%02d",
             now.day(), now.month(), now.hour(), now.minute(), now.second());
    return String(buf);
}