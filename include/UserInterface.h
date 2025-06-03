#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "LCD_Display.h"

enum CtrlModeSel_t {
    CTRL_AUTO_BY_SENSORS,/* Setting selected using mode's push button */
    CTRL_MODE_MANUAL,    /* Setting selected using mode's push button */
    CTRL_AUTO_BY_TIMER   /* Setting selected using display interfaces */
};

enum ManualPumpSel_t {
    SELECT_PUMP_NONE,
    SELECT_PUMP_1,
    SELECT_PUMP_2,
    SELECT_PUMP_BOTH
};

enum ScreenMode_t {
    SCREEN_MODE_MAIN,            
    SCREEN_MODE_OPTION_SETTINGS, 
    SCREEN_MODE_SET_CTRL_MODE,
    SCREEN_MODE_SET_DATE_TIME,   
    SCREEN_MODE_SET_PUMP1_CYCLE,
    SCREEN_MODE_SET_PUMP2_CYCLE,
};

ScreenMode_t DisplayMainScreen(bool pbOkState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay, String Hour);
ScreenMode_t DisplayOptionSettingsScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, LCD_Display &lcdDisplay);
ScreenMode_t DisplaySetCtrlModeScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay);
ScreenMode_t DisplaySetDateTimeScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay);
ScreenMode_t DisplaySetPump1CycleScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay);
ScreenMode_t DisplaySetPump2CycleScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay);
#endif