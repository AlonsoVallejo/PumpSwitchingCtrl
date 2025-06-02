#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "LCD_Display.h"

enum CtrlModeSel_t {
    CTRL_MODE_AUTO,
    CTRL_MODE_MANUAL
};

enum ManualPumpSel_t {
    SELECT_PUMP_NONE,
    SELECT_PUMP_1,
    SELECT_PUMP_2,
    SELECT_PUMP_BOTH
};

enum CtrlAutoMode_t {
    CTRL_AUTO_BY_SENSORS,
    CTRL_AUTO_BY_TIMER
};

enum ScreenMode_t {
    SCREEN_MODE_MAIN,            /* Screen to display the hour and current CtrlAutoMode_t or Manual CTRL_MODE_MANUAL */
    SCREEN_MODE_OPTION_SETTINGS, /* Screen to display select the settings CtrlAutoMode_t and hour */
    SCREEN_MODE_SET_DATE_TIME,   /* Screen to allow the user configure the current hour */
    SCREEN_MODE_SET_PUMP1_CYCLE, /* Screen to set the activation cycle for PUMP1 if CTRL_AUTO_BY_TIMER */
    SCREEN_MODE_SET_PUMP2_CYCLE, /* Screen to set the activation cycle for PUMP2 if CTRL_AUTO_BY_TIMER */
    TOTAL_SCREEN_MODES
};

#endif