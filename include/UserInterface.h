#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "LCD_Display.h"
#include "RealTimeClock.h"
#include "utilities.h"
#include <stdint.h>

/* Times in that each pump will be in active state */
struct PumpCycleTime {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum CtrlModeSel_t {
    CTRL_AUTO_BY_SENSORS,/* Setting selected using mode's push button */
    CTRL_MODE_MANUAL,    /* Setting selected using mode's push button */
    CTRL_AUTO_BY_TIMER   /* Setting selected using display interfaces */
};

/* Manual activation of each pump using pump's push button */
enum ManualPumpSel_t {
    SELECT_PUMP_NONE,
    SELECT_PUMP_1,
    SELECT_PUMP_2,
    SELECT_PUMP_BOTH
};

enum ScreenMode_t {
    SCREEN_MAIN,            
    SCREEN_MAIN_CFGS,
    SCREEN_CFG_CTRL_TYPE,
    SCREEN_CFG_RTC,   
    SCREEN_CFG_PUMP1_CYCLE,
    SCREEN_CFG_PUMP2_CYCLE,
};

ScreenMode_t DisplayMain(bool pbOkState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay, String Hour);
ScreenMode_t DisplayMainCfgs(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, LCD_Display &lcdDisplay);
ScreenMode_t DisplayCfgControlTypes(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay);
ScreenMode_t DisplayCfgRtc(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay, RealTimeClock &rtc_datetime);
ScreenMode_t DisplayCfgPump1Cycle(
    bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState,
    bool pbLeftState, bool pbRightState,
    PumpCycleTime (&PumpCyclesTimes)[2], LCD_Display &lcdDisplay);

ScreenMode_t DisplayCfgPump2Cycle(
    bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState,
    bool pbLeftState, bool pbRightState,
    PumpCycleTime (&PumpCyclesTimes)[2], LCD_Display &lcdDisplay);
#endif