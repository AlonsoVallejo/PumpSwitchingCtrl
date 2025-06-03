#include <Arduino.h>
#include "Sensors.h"
#include "Actuators.h"
#include "UserInterface.h"
#include "RealTimeClock.h"
#include "utilities.h"

#define DI_PB_UP 2
#define DI_PB_DOWN 3
#define DI_PB_LEFT 4
#define DI_PB_RIGHT 5
#define DI_PB_OK 6
#define DI_PB_ESC 7
#define DI_PB_MODE 8
#define DI_PB_PUMP_SEL 9
#define DI_WELL_SENSOR 10
#define DI_CISTERN_SENSOR 11

#define DO_LED_AUTO 14
#define DO_LED_MANUAL 15
#define DO_PUMP_1 16
#define DO_PUMP_2 17

#define SENSOR_FULL_LEVEL  (false)
#define SENSOR_EMPTY_LEVEL (true)

DigitalSensor pbUp(DI_PB_UP);
DigitalSensor pbDown(DI_PB_DOWN);
DigitalSensor pbLeft(DI_PB_LEFT);
DigitalSensor pbRight(DI_PB_RIGHT);
DigitalSensor pbOk(DI_PB_OK);
DigitalSensor pbEsc(DI_PB_ESC);
DigitalSensor pbMode(DI_PB_MODE);
DigitalSensor pbPumpSel(DI_PB_PUMP_SEL);
DigitalSensor wellSensor(DI_WELL_SENSOR);
DigitalSensor cisternSensor(DI_CISTERN_SENSOR);

DigitalActuator ledAuto(DO_LED_AUTO);
DigitalActuator ledManual(DO_LED_MANUAL);
DigitalActuator pump1(DO_PUMP_1);
DigitalActuator pump2(DO_PUMP_2);

LCD_Display lcdDisplay(LCD_DISPLAY_I2C_ADDR, LCD_DISPLAY_COLS, LCD_DISPLAY_ROWS);

RealTimeClock rtc;
/**
 * @brief Polls all sensors to update their states.
 * This function reads the state of each sensor and updates their internal state.
 */
void PollAllSensors(void) 
{
    pbUp.PollSensorState();
    pbDown.PollSensorState();
    pbLeft.PollSensorState();
    pbRight.PollSensorState();
    pbOk.PollSensorState();
    pbEsc.PollSensorState();
    pbMode.PollSensorState();
    pbPumpSel.PollSensorState();
    wellSensor.PollSensorState();
    cisternSensor.PollSensorState();
}

/**
 * @brief Controls the selection of control modes based on push button input.
 * This function manages the control mode selection logic, including toggling between
 * manual and automatic modes based on sensor states.
 * @param Ctrlmode The current control mode selected by the user.
 * @return The updated control mode after processing the input.
 */
CtrlModeSel_t ControlModeSelection(CtrlModeSel_t Ctrlmode) {
    static bool prevPbModeState = false;
    static CtrlModeSel_t prevAutoMode = CTRL_AUTO_BY_SENSORS;
    CtrlModeSel_t retCtrlMode = Ctrlmode;
    bool currPbModeState = pbMode.isSensorActive();

    /** If input is AUTO_BY_SENSORS, always return AUTO_BY_SENSORS */
    if (Ctrlmode == CTRL_AUTO_BY_SENSORS) {
        if (prevPbModeState && !currPbModeState) {
            prevAutoMode = CTRL_AUTO_BY_SENSORS;
            retCtrlMode = CTRL_MODE_MANUAL;
        } else {
            retCtrlMode = CTRL_AUTO_BY_SENSORS;
        }
    } else if (Ctrlmode == CTRL_AUTO_BY_TIMER) {  /** If input is AUTO_BY_TIMER, always return AUTO_BY_TIMER */
        if (prevPbModeState && !currPbModeState) {
            prevAutoMode = CTRL_AUTO_BY_TIMER;
            retCtrlMode = CTRL_MODE_MANUAL;
        } else {
            retCtrlMode = CTRL_AUTO_BY_TIMER;
        }
    } else if (Ctrlmode == CTRL_MODE_MANUAL) {  /** If input is MANUAL, allow toggling back to previous AUTO mode */
        if (prevPbModeState && !currPbModeState) {
            retCtrlMode = prevAutoMode;
        } else {
            retCtrlMode = CTRL_MODE_MANUAL;
        }
    }

    prevPbModeState = currPbModeState;

    /** Update LEDs based on control mode */
    if ((retCtrlMode == CTRL_AUTO_BY_SENSORS) || (retCtrlMode == CTRL_AUTO_BY_TIMER)) {
        ledAuto.activate();
        ledManual.deactivate();
    } else {
        ledAuto.deactivate();
        ledManual.activate();
    }

    return retCtrlMode;
}

/**
 * @brief Controls the pumps based on manual selection.
 * This function allows the user to manually select which pump to activate
 * using a push button. It also checks the well sensor state to ensure
 * pumps are only activated when the well is not empty.
 */
void CntrlPumpsByManual(void)
{
    static bool prevPbPumpSelState = false;
    static ManualPumpSel_t currentPumpSel = SELECT_PUMP_NONE;
    bool currPbPumpSelState = pbPumpSel.isSensorActive();
    bool wellSensorState = wellSensor.isSensorActive();

    if(wellSensorState == SENSOR_EMPTY_LEVEL) {
        pump1.deactivate();
        pump2.deactivate();
        return;  /** If well is empty, deactivate both pumps */
    }

    /** Only allow manual pump selection if mode is not AUTO */
    if (prevPbPumpSelState && !currPbPumpSelState) {
        currentPumpSel = static_cast<ManualPumpSel_t>(static_cast<int>(currentPumpSel) + 1);
        if (currentPumpSel > SELECT_PUMP_BOTH) {
            currentPumpSel = SELECT_PUMP_NONE;
        }
    }
    prevPbPumpSelState = currPbPumpSelState;

    /** Activate selected pump */
    if(currentPumpSel == SELECT_PUMP_1 ) {
        pump1.activate();
        pump2.deactivate();
    } else if(currentPumpSel == SELECT_PUMP_2 ) {
        pump1.deactivate();
        pump2.activate();
    } else if(currentPumpSel == SELECT_PUMP_BOTH) {
        pump1.activate();
        pump2.activate();
    } else {
        pump1.deactivate();
        pump2.deactivate();
    }
}

/**
 * @brief Controls the pumps based on sensor states.
 * This function manages the operation of pumps based on the states of well and cistern sensors.
 * It alternates between two pumps when filling the cistern and pauses operation if the well is empty.
 */
void CntrlPumpsBySensors(void)
{
    /** Static variables to keep track of pump alternation and state */
    static bool usePump1 = true;
    static bool waitingForFull = false;
    static bool lastCisternWasFull = true;
    static bool pumpPausedByWell = false;

    bool wellSensorState = wellSensor.isSensorActive();
    bool cisternSensorState = cisternSensor.isSensorActive();

    /** If cistern is empty and we are not already waiting for it to fill */
    if ((cisternSensorState == SENSOR_EMPTY_LEVEL) && lastCisternWasFull) {
        waitingForFull = true;
        lastCisternWasFull = false;
        pumpPausedByWell = false;
        /** Activate the selected pump */
        if (usePump1) {
            pump1.activate();
            pump2.deactivate();
        } else {
            pump1.deactivate();
            pump2.activate();
        }
    }

    /** If we are waiting for cistern to fill and it is now full */
    if (waitingForFull && (cisternSensorState == SENSOR_FULL_LEVEL)) {
        pump1.deactivate();
        pump2.deactivate();
        waitingForFull = false;
        lastCisternWasFull = true;
        pumpPausedByWell = false;
        /** Alternate the pump for next cycle */
        usePump1 = !usePump1;
    }

    /** If well is empty while waiting for cistern to fill, pause the current pump */
    if (waitingForFull && (wellSensorState == SENSOR_EMPTY_LEVEL)) {
        pump1.deactivate();
        pump2.deactivate();
        pumpPausedByWell = true;
        return;
    }

    /** If well becomes full again and we are still waiting for cistern, resume the current pump */
    if (waitingForFull && pumpPausedByWell && (wellSensorState == SENSOR_FULL_LEVEL)) {
        if (usePump1) {
            pump1.activate();
            pump2.deactivate();
        } else {
            pump1.deactivate();
            pump2.activate();
        }
        pumpPausedByWell = false;
    }

    /** If not waiting for cistern to fill, keep both pumps off */
    if (!waitingForFull) {
        pump1.deactivate();
        pump2.deactivate();
    }
}

/**
 * @brief Displays the current screen based on the selected mode.
 * This function manages the display of different screens based on user input and the current control mode.
 * It handles the main screen, option settings, control mode selection, date/time settings, and pump cycle settings.
 * @param currCtrlMode The current control mode selected by the user.
 */
void ShowDisplayMenus(CtrlModeSel_t &currCtrlMode) {
    static ScreenMode_t currentScreenMode = SCREEN_MAIN;
    bool pbUpState = pbUp.isSensorActive();
    bool pbDownState = pbDown.isSensorActive();
    bool pbLeftState = pbLeft.isSensorActive();
    bool pbRightState = pbRight.isSensorActive();
    bool pbOkState = pbOk.isSensorActive();
    bool pbEscState = pbEsc.isSensorActive();

    lcdDisplay.clearScreen();

    switch(currentScreenMode) {
        case SCREEN_MAIN:
            currentScreenMode = DisplayMain(pbOkState, currCtrlMode, lcdDisplay, rtc.getFormattedDateTime());
            break;
        case SCREEN_MAIN_CFGS:
            currentScreenMode = DisplayMainCfgs(pbOkState, pbEscState, pbUpState, pbDownState, lcdDisplay);
            break;
        case SCREEN_CFG_CTRL_TYPE:
            currentScreenMode = DisplayCfgControlTypes(pbOkState, pbEscState, pbUpState, pbDownState, currCtrlMode, lcdDisplay);
            break;
        case SCREEN_CFG_RTC:
            currentScreenMode = DisplayCfgRtc(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, lcdDisplay, rtc);
            break;
        case SCREEN_CFG_PUMP1_CYCLE:
            currentScreenMode = DisplayCfgPump1Cycle(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, lcdDisplay);
            break;
        case SCREEN_CFG_PUMP2_CYCLE:
            currentScreenMode = DisplayCfgPump2Cycle(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, lcdDisplay);
            break;

        default:
            currentScreenMode = SCREEN_MAIN;
            break;
    }
}

void setup() {
    Serial.begin(9600);
    LogSerialn("System starting...", true);
    lcdDisplay.init();
    rtc.begin();
}

void loop() {
    static uint64_t lastSensorsMillis = 0;
    static uint64_t lastActuatorsMillis = 0;
    static uint64_t lastDisplayMillis = 0;
    static CtrlModeSel_t currentCtrlMode = CTRL_AUTO_BY_SENSORS;
    uint64_t now = millis();

    if (now - lastSensorsMillis >= 50) {
        PollAllSensors();
        lastSensorsMillis = now;
    }

    if (now - lastActuatorsMillis >= 200) {
        currentCtrlMode = ControlModeSelection(currentCtrlMode);
        
        if(currentCtrlMode == CTRL_MODE_MANUAL) {
            CntrlPumpsByManual();
        } else if(currentCtrlMode == CTRL_AUTO_BY_SENSORS) {
            CntrlPumpsBySensors();
        } else if(currentCtrlMode == CTRL_AUTO_BY_TIMER) {

        } else {

        }

        lastActuatorsMillis = now;
    }

    if (now - lastDisplayMillis >= 400) {
        ShowDisplayMenus(currentCtrlMode);
        lastDisplayMillis = now;
    }
}
