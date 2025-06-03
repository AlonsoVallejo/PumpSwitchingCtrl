#ifndef REAL_TIME_CLOCK_H
#define REAL_TIME_CLOCK_H

#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>

class RealTimeClock {
private:
    RTC_DS3231 rtc;
public:
    RealTimeClock();
    void begin();
    DateTime GetCurrentDateTime();
    void setDateTime(const DateTime &dt);
    String getFormattedDateTime();
};

#endif