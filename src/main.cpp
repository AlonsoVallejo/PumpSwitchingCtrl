#include <Arduino.h>
#include "Sensors.h"
#include "Actuators.h"
#include "UserInterface.h"
#include "RealTimeClock.h"
#include "AT24C32_nvm.h"
#include "utilities.h"
#include <Wire.h>

#define POLL_ALL_SENSORS_TIMEOUT (50)
#define CONTROL_PUMPS_TIMEOUT    (200)
#define DISPLAY_UPDATE_TIMEOUT   (400)

/* Navigation user push buttons */
#define DI_PB_UP    (2)
#define DI_PB_DOWN  (3)
#define DI_PB_LEFT  (4)
#define DI_PB_RIGHT (5)
#define DI_PB_OK    (6)
#define DI_PB_ESC   (7)

/* Control mode selection push buttons */
#define DI_PB_MODE     (8)
#define DI_PB_PUMP_SEL (9)

/* Sensors and actuators */
#define DI_WELL_SENSOR    (10)
#define DI_CISTERN_SENSOR (11)

#define DO_LED_AUTO   (14)
#define DO_LED_MANUAL (15)

#define DO_PUMP_1 (16)
#define DO_PUMP_2 (17)

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

RealTimeClock rtc_datetime;

PumpCycleTime PumpCyclesTimes[] = {
    {0, 0, 0}, /* Pump 1 cycle time (default) */
    {0, 0, 0}  /* Pump 2 cycle time (default) */
};

/**
 * @brief Saves the pump cycle times to EEPROM.
 * This function stores the configured pump cycle times in EEPROM for persistence across resets.
 * @param cycles An array of PumpCycleTime objects representing the pump cycle times.
 */
void SavePumpCyclesToEEPROM(const PumpCycleTime cycles[2]) {
    I2C_EEPROM_WriteBytes(AT24C32_START_ADDR, (const uint8_t*)cycles, sizeof(PumpCycleTime) * 2);
    LogSerialn("Pump cycles saved to AT24C32", true);
    LogSerialn("Pump 1 Cycle: " + String(cycles[0].hour) + ":" + String(cycles[0].minute) + ":" + String(cycles[0].second), true);
    LogSerialn("Pump 2 Cycle: " + String(cycles[1].hour) + ":" + String(cycles[1].minute) + ":" + String(cycles[1].second), true);
}

/**
 * @brief Loads the pump cycle times from EEPROM.
 * This function retrieves the stored pump cycle times from EEPROM.
 * @param cycles An array of PumpCycleTime objects to store the loaded pump cycle times.
 */
void LoadPumpCyclesFromEEPROM(PumpCycleTime cycles[2]) {
    I2C_EEPROM_ReadBytes(AT24C32_START_ADDR, (uint8_t*)cycles, sizeof(PumpCycleTime) * 2);

    // Check for uninitialized EEPROM (all bytes 0xFF)
    bool invalid = true;
    for (int i = 0; i < 2; ++i) {
        if (cycles[i].hour != 0xFF || cycles[i].minute != 0xFF || cycles[i].second != 0xFF) {
            invalid = false;
            break;
        }
    }
    if (invalid) {
        // Set defaults
        cycles[0] = {0, 0, 0};
        cycles[1] = {0, 0, 0};
        SavePumpCyclesToEEPROM(cycles);
        LogSerialn("EEPROM uninitialized, set default pump cycles", true);
    }

    LogSerialn("Pump cycles loaded from AT24C32", true);
    LogSerialn("Pump 1 Cycle: " + String(cycles[0].hour) + ":" + String(cycles[0].minute) + ":" + String(cycles[0].second), true);
    LogSerialn("Pump 2 Cycle: " + String(cycles[1].hour) + ":" + String(cycles[1].minute) + ":" + String(cycles[1].second), true);
}

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
    bool cisternSensorState = cisternSensor.isSensorActive();

    if(wellSensorState == SENSOR_EMPTY_LEVEL) {
        pump1.deactivate();
        pump2.deactivate();
        return;  /** If well is empty, deactivate both pumps */
    }

    if(cisternSensorState == SENSOR_FULL_LEVEL) {
        pump1.deactivate();
        pump2.deactivate();
        return;  /** If cistern is full, deactivate both pumps */
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
 * @brief Controls the pumps based on a timer. This function manages the operation of pumps based on their timer configured values.
 * It alternates between two pumps when filling the cistern and pauses operation if the well is empty.
 * The alternating pump cycle times are defined in the PumpCyclesTimes array compared with the current time.
 * The pumps will be active for the defined cycle time and then switch to the other pump.
 * If the well is empty, both pumps will be deactivated.
 */
void CntrlPumpsByTimer(void)
{
    static bool usePump1 = true;
    static DateTime lastSwitchDateTime = DateTime(0, 0, 0, 0, 0, 0);
    static bool pumpPausedByWell = false;
    static bool waitingForFull = false;
    static bool lastCisternWasFull = true;

    bool wellSensorState = wellSensor.isSensorActive();
    bool cisternSensorState = cisternSensor.isSensorActive();

    /** Get current date and time from RTC */
    DateTime now = rtc_datetime.GetCurrentDateTime();

    /** Check if both pump cycle times are set (not default) */
    bool pump1CycleValid = (PumpCyclesTimes[0].hour != 0) || (PumpCyclesTimes[0].minute != 0) || (PumpCyclesTimes[0].second != 0);
    bool pump2CycleValid = (PumpCyclesTimes[1].hour != 0) || (PumpCyclesTimes[1].minute != 0) || (PumpCyclesTimes[1].second != 0);

    static PumpCycleTime prevPumpCyclesTimes[2] = { {0,0,0}, {0,0,0} };
    if (pump1CycleValid && pump2CycleValid) {
        /* Check if either pump's cycle time has changed */ 
        if (memcmp(&PumpCyclesTimes, &prevPumpCyclesTimes, sizeof(PumpCyclesTimes)) != 0) {
            SavePumpCyclesToEEPROM(PumpCyclesTimes);
            memcpy(&prevPumpCyclesTimes, &PumpCyclesTimes, sizeof(PumpCyclesTimes));
        }
    }

    /** If either pump cycle time is default, do not start alternation, keep both pumps off and reset state */
    if (!pump1CycleValid || !pump2CycleValid) {
        pump1.deactivate();
        pump2.deactivate();
        waitingForFull = false;
        lastCisternWasFull = true;
        pumpPausedByWell = false;
        return;
    }

    /** Start cycling when cistern becomes empty */
    if ((cisternSensorState == SENSOR_EMPTY_LEVEL) && lastCisternWasFull) {
        waitingForFull = true;
        lastCisternWasFull = false;
        pumpPausedByWell = false;
        lastSwitchDateTime = now;
        /** Activate the selected pump */
        if (usePump1) {
            pump1.activate();
            pump2.deactivate();
        } else {
            pump1.deactivate();
            pump2.activate();
        }
    }

    /** Stop cycling when cistern becomes full */
    if (waitingForFull && (cisternSensorState == SENSOR_FULL_LEVEL)) {
        pump1.deactivate();
        pump2.deactivate();
        waitingForFull = false;
        lastCisternWasFull = true;
        pumpPausedByWell = false;
        return;
    }

    /** Pause pumps if well is empty */
    if (waitingForFull && (wellSensorState == SENSOR_EMPTY_LEVEL)) {
        pump1.deactivate();
        pump2.deactivate();
        pumpPausedByWell = true;
        return;
    }

    /** Resume pumps if well becomes full again */
    if (waitingForFull && pumpPausedByWell && (wellSensorState == SENSOR_FULL_LEVEL)) {
        lastSwitchDateTime = now; /** Reset timer to avoid immediate switch */
        pumpPausedByWell = false;
        /** Reactivate the current pump */
        if (usePump1) {
            pump1.activate();
            pump2.deactivate();
        } else {
            pump1.deactivate();
            pump2.activate();
        }
    }

    /** If not in a cycle, keep both pumps off */
    if (!waitingForFull) {
        pump1.deactivate();
        pump2.deactivate();
        return;
    }

    /** Calculate elapsed seconds since last switch */
    TimeSpan elapsed = now - lastSwitchDateTime;
    uint32_t elapsedSeconds = elapsed.totalseconds();

    /** Get configured cycle time for the current pump in seconds */
    uint32_t cycleSeconds = PumpCyclesTimes[usePump1 ? 0 : 1].hour * 3600UL +
                            PumpCyclesTimes[usePump1 ? 0 : 1].minute * 60UL +
                            PumpCyclesTimes[usePump1 ? 0 : 1].second;

    /** Check if it's time to switch pumps */
    if (elapsedSeconds >= cycleSeconds && cycleSeconds > 0) {
        usePump1 = !usePump1;  /** Alternate the pump */
        lastSwitchDateTime = now;
        /** Activate the new selected pump */
        if (usePump1) {
            pump1.activate();
            pump2.deactivate();
        } else {
            pump1.deactivate();
            pump2.activate();
        }
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
    static ScreenMode_t lastScreenMode = SCREEN_MAIN;

    bool pbUpState = pbUp.isSensorActive();
    bool pbDownState = pbDown.isSensorActive();
    bool pbLeftState = pbLeft.isSensorActive();
    bool pbRightState = pbRight.isSensorActive();
    bool pbOkState = pbOk.isSensorActive();
    bool pbEscState = pbEsc.isSensorActive();

    /* Clear LCD only when changing screens */ 
    if (currentScreenMode != lastScreenMode) {
        lcdDisplay.clearScreen();
        lastScreenMode = currentScreenMode;
    }

    switch(currentScreenMode) {
        case SCREEN_MAIN:
            currentScreenMode = DisplayMain(pbOkState, currCtrlMode, lcdDisplay, rtc_datetime.getFormattedDateTime());
            break;
        case SCREEN_MAIN_CFGS:
            currentScreenMode = DisplayMainCfgs(pbOkState, pbEscState, pbUpState, pbDownState, lcdDisplay);
            break;
        case SCREEN_CFG_CTRL_TYPE:
            currentScreenMode = DisplayCfgControlTypes(pbOkState, pbEscState, pbUpState, pbDownState, currCtrlMode, lcdDisplay);
            break;
        case SCREEN_CFG_RTC:
            currentScreenMode = DisplayCfgRtc(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, lcdDisplay, rtc_datetime);
            break;
        case SCREEN_CFG_PUMP1_CYCLE:
            currentScreenMode = DisplayCfgPump1Cycle(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, PumpCyclesTimes, lcdDisplay);
            break;
        case SCREEN_CFG_PUMP2_CYCLE:
            currentScreenMode = DisplayCfgPump2Cycle(pbOkState, pbEscState, pbUpState, pbDownState, pbLeftState, pbRightState, PumpCyclesTimes, lcdDisplay);
            break;
        default:
            currentScreenMode = SCREEN_MAIN;
            break;
    }
}

void setup() {
    Serial.begin(9600);
    Wire.begin(); 
    LogSerialn("Starting Water Pump Control System", true);
    lcdDisplay.init();
    rtc_datetime.begin();
    LoadPumpCyclesFromEEPROM(PumpCyclesTimes);
}

void loop() {
    static uint64_t lastSensorsMillis = 0;
    static uint64_t lastActuatorsMillis = 0;
    static uint64_t lastDisplayMillis = 0;
    static CtrlModeSel_t currentCtrlMode = CTRL_AUTO_BY_SENSORS;
    uint64_t now = millis();

    if (now - lastSensorsMillis >= POLL_ALL_SENSORS_TIMEOUT) {
        PollAllSensors();
        lastSensorsMillis = now;
    }

    if (now - lastActuatorsMillis >= CONTROL_PUMPS_TIMEOUT) {
        currentCtrlMode = ControlModeSelection(currentCtrlMode);
        
        if(currentCtrlMode == CTRL_MODE_MANUAL) {
            CntrlPumpsByManual();
        } else if(currentCtrlMode == CTRL_AUTO_BY_SENSORS) {
            CntrlPumpsBySensors();
        } else if(currentCtrlMode == CTRL_AUTO_BY_TIMER) {
            CntrlPumpsByTimer();
        } else {
            currentCtrlMode = CTRL_AUTO_BY_SENSORS;
        }

        lastActuatorsMillis = now;
    }

    if (now - lastDisplayMillis >= DISPLAY_UPDATE_TIMEOUT) {
        ShowDisplayMenus(currentCtrlMode);
        lastDisplayMillis = now;
    }
}
