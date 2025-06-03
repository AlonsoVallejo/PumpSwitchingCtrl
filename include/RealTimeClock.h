#ifndef REAL_TIME_CLOCK_H
#define REAL_TIME_CLOCK_H

#include <Arduino.h>
#include <RTClib.h>

class RealTimeClock {
private:
    RTC_DS3231 rtc;
public:
    RealTimeClock();
    void begin();
    DateTime GetCurrentDateTime();
    void setDateTime(const DateTime &dt);
    String getFormattedDateTime(); /* DD/MM/YY-HH:MM:SS */
};

#endif