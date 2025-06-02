#include <Arduino.h>
#include "Sensors.h"
#include "Actuators.h"
#include "UserInterface.h"
#include "DateTime.h"
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

void PollAllSensors() {
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

CtrlModeSel_t ControlModeSelection() {
    static bool prevPbModeState = false;
    bool currPbModeState = pbMode.isSensorActive();
    static CtrlModeSel_t currentCtrlMode = CTRL_MODE_AUTO;
    
    /** Change mode only when pbMode is pressed and then released (rising edge) */
    if (prevPbModeState && !currPbModeState) {
        currentCtrlMode = static_cast<CtrlModeSel_t>(static_cast<int>(currentCtrlMode) + 1);
        if (currentCtrlMode > CTRL_MODE_MANUAL) {
            currentCtrlMode = CTRL_MODE_AUTO;
        }
    }
    prevPbModeState = currPbModeState;

    /** Update LEDs based on control mode */
    if(currentCtrlMode == CTRL_MODE_AUTO) {
        ledAuto.activate();
        ledManual.deactivate();
    } else {
        ledAuto.deactivate();
        ledManual.activate();
    }

    return currentCtrlMode;
}

void CntrlManualPumps(CtrlModeSel_t mode)
{
    static bool prevPbPumpSelState = false;
    static ManualPumpSel_t currentPumpSel = SELECT_PUMP_NONE;
    bool currPbPumpSelState = pbPumpSel.isSensorActive();

    /** Change pump selection only when pbPumpSel is pressed and then released (rising edge) */
    if (mode != CTRL_MODE_AUTO) {
        /** Only allow manual pump selection if mode is not AUTO */
        if (prevPbPumpSelState && !currPbPumpSelState) {
            currentPumpSel = static_cast<ManualPumpSel_t>(static_cast<int>(currentPumpSel) + 1);
            if (currentPumpSel > SELECT_PUMP_BOTH) {
                currentPumpSel = SELECT_PUMP_NONE;
            }
        }
    }
    prevPbPumpSelState = currPbPumpSelState;

    /** Activate selected pump */
    if(currentPumpSel == SELECT_PUMP_1) {
        pump1.activate();
        pump2.deactivate();
    } else if(currentPumpSel == SELECT_PUMP_2) {
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

void ShowDisplayMenus() {
    static bool firstRun = true;
    if (firstRun) {
        lcdDisplay.clearScreen();
        firstRun = false;
    }
}

void setup() {
    Serial.begin(9600);
    LogSerialn("System starting...", true);
    lcdDisplay.init();
}

void loop() {
    static uint64_t lastSensorsMillis = 0;
    static uint64_t lastActuatorsMillis = 0;
    static uint64_t lastDisplayMillis = 0;
    CtrlModeSel_t currentCtrlMode = CTRL_MODE_AUTO;
    uint64_t now = millis();

    if (now - lastSensorsMillis >= 100) {
        PollAllSensors();
        lastSensorsMillis = now;
    }

    if (now - lastActuatorsMillis >= 200) {
        currentCtrlMode = ControlModeSelection();
        CntrlManualPumps(currentCtrlMode);
        lastActuatorsMillis = now;
    }

    if (now - lastDisplayMillis >= 1000) {
        ShowDisplayMenus();
        lastDisplayMillis = now;
    }
}
