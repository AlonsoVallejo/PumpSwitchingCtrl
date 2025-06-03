#include "UserInterface.h"

/**
 * @brief Displays the main screen with control mode and current time.
 * @param pbOkState State of the OK push button (not used here, but kept for interface compatibility).
 * @param mode The current control mode to display.
 * @param lcdDisplay Reference to the LCD display object.
 * @param Hour String with the current hour in "HH:MM:SS" 24-hour format.
 * @return SCREEN_MAIN (remains on main screen).
 */
ScreenMode_t DisplayMain(bool pbOkState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay, String Hour)
{
    ScreenMode_t retval = SCREEN_MAIN;

    /** Display control mode */
    String ctrlModeStr;
    switch(mode) {
        case CTRL_MODE_MANUAL:
            ctrlModeStr = "Manual";
            break;
        case CTRL_AUTO_BY_SENSORS:
            ctrlModeStr = "Sensors";
            break;
        case CTRL_AUTO_BY_TIMER:
            ctrlModeStr = "Timer";
            break;
        default:
            ctrlModeStr = "UNKNOWN";
            break;
    }
    lcdDisplay.PrintMessage("Ctrl: " + ctrlModeStr, 0, 0);

    /** Display the hour in 24-hour format */
    lcdDisplay.PrintMessage(Hour, 0, 1);

    if( pbOkState) {
        /** If OK button is pressed, switch to option settings screen */
        retval = SCREEN_MAIN_CFGS;
    } else {
        /** If OK button is not pressed, remain on the main screen */
        retval = SCREEN_MAIN;
    }

    return retval;
}

/**
 * @brief Displays the option settings screen with selectable options.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplayMainCfgs(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, LCD_Display &lcdDisplay)
{
    /** Menu options */
    const char* menuOptions[] = {
        "Cfg Ctrl Type",
        "Cfg Hour",
        "Cfg Pump1 Time",
        "Cfg Pump2 Time"
    };
    const uint8_t numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);
    static uint8_t selectedIndex = 0;
    static uint8_t topIndex = 0;
    ScreenMode_t retval = SCREEN_MAIN_CFGS;

    /** Handle navigation */
    if (pbUpState && selectedIndex > 0) {
        selectedIndex--;
        /** Scroll up if needed */
        if (selectedIndex < topIndex) {
            topIndex--;
        }
    }
    if (pbDownState && selectedIndex < numOptions - 1) {
        selectedIndex++;
        /** Scroll down if needed */
        if (selectedIndex > topIndex + 1) {
            topIndex++;
        }
    }

    /** Display two menu options with selector '>' */
    for (uint8_t i = 0; i < 2; i++) {
        uint8_t optionIdx = topIndex + i;
        if (optionIdx >= numOptions) break;
        if (optionIdx == selectedIndex) {
            lcdDisplay.PrintMessage(">" + String(menuOptions[optionIdx]), 0, i);
        } else {
            lcdDisplay.PrintMessage(" " + String(menuOptions[optionIdx]), 0, i);
        }
    }

    /** Handle selection */
    if (pbOkState) {
        switch (selectedIndex) {
            case 0:
                retval = SCREEN_CFG_CTRL_TYPE;
                break;
            case 1:
                retval = SCREEN_CFG_RTC;
                break;
            case 2:
                retval = SCREEN_CFG_PUMP1_CYCLE;
                break;
            case 3:
                retval = SCREEN_CFG_PUMP2_CYCLE;
                break;
            default:
                retval = SCREEN_MAIN_CFGS;
                break;
        }
        /** Reset selection for next entry */
        selectedIndex = 0;
        topIndex = 0;
    } else if (pbEscState) {
        retval = SCREEN_MAIN;
        /** Reset selection for next entry */
        selectedIndex = 0;
        topIndex = 0;
    }

    return retval;
}

/**
 * @brief Displays the control mode selection screen.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplayCfgControlTypes(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay) 
{
    /** Static variable to keep track of the selected menu index: 0 = Set Control, 1 = Set Hour */
    static uint8_t selectedIndex = 0;
    ScreenMode_t retval = SCREEN_CFG_CTRL_TYPE;

    /** Handle navigation */
    if (pbUpState && selectedIndex > 0) {
        selectedIndex = 0;
    }
    if (pbDownState && selectedIndex < 1) {
        selectedIndex = 1;
    }

    /** Display menu options with selector '>' */
    if (selectedIndex == 0) {
        lcdDisplay.PrintMessage(">Auto Sensors", 0, 0);
        lcdDisplay.PrintMessage(" Auto Timer", 0, 1);
    } else {
        lcdDisplay.PrintMessage(" Auto Sensors", 0, 0);
        lcdDisplay.PrintMessage(">Auto Timer", 0, 1);
    }

    /** Handle selection */
    if (pbOkState) {
        if (selectedIndex == 0) {
            mode = CTRL_AUTO_BY_SENSORS;
        } else {
            mode = CTRL_AUTO_BY_TIMER;
        }
        retval = SCREEN_MAIN_CFGS;
        /** Reset selection for next entry */
        selectedIndex = 0;
    } else if (pbEscState) {
        retval = SCREEN_MAIN;
        /** Reset selection for next entry */
        selectedIndex = 0;
    }

    return retval;
}

/**
 * @brief Displays the date and time setting screen.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param pbLeftState State of the LEFT push button.
 * @param pbRightState State of the RIGHT push button.
 * @param lcdDisplay Reference to the LCD display object.
 * @param rtc_datetime Reference to the RealTimeClock object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplayCfgRtc(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay, RealTimeClock &rtc_datetime) {
    /** Static variables to hold editable date/time and cursor position */
    static uint8_t day = 1, month = 1, year = 24, hour = 0, minute = 0, second = 0;
    static uint8_t cursorIndex = 0; /* 0=day, 1=month, 2=year, 3=hour, 4=minute, 5=second */
    static bool initialized = false;
    ScreenMode_t retval = SCREEN_CFG_RTC;

    /** On first entry, load values from RTC */
    if (!initialized) {
        DateTime now = rtc_datetime.GetCurrentDateTime();
        day = now.day();
        month = now.month();
        year = now.year() % 100;
        hour = now.hour();
        minute = now.minute();
        second = now.second();
        cursorIndex = 0;
        initialized = true;
    }

    /** Handle navigation between fields */
    if (pbLeftState && cursorIndex > 0) {
        cursorIndex--;
    }
    if (pbRightState && cursorIndex < 5) {
        cursorIndex++;
    }

    /** Handle value changes */
    if (pbUpState) {
        switch (cursorIndex) {
            case 0: if (day < 31) day++; else day = 1; break;
            case 1: if (month < 12) month++; else month = 1; break;
            case 2: if (year < 99) year++; else year = 0; break;
            case 3: if (hour < 23) hour++; else hour = 0; break;
            case 4: if (minute < 59) minute++; else minute = 0; break;
            case 5: if (second < 59) second++; else second = 0; break;
        }
    }
    if (pbDownState) {
        switch (cursorIndex) {
            case 0: if (day > 1) day--; else day = 31; break;
            case 1: if (month > 1) month--; else month = 12; break;
            case 2: if (year > 0) year--; else year = 99; break;
            case 3: if (hour > 0) hour--; else hour = 23; break;
            case 4: if (minute > 0) minute--; else minute = 59; break;
            case 5: if (second > 0) second--; else second = 59; break;
        }
    }

    /** Format date string: DD/MM/YY-HH:MM:SS */
    char dateStr[17];
    snprintf(dateStr, sizeof(dateStr), "%02u/%02u/%02u-%02u:%02u:%02u", day, month, year, hour, minute, second);
    lcdDisplay.clearScreen();
    lcdDisplay.PrintMessage(String(dateStr), 0, 0);

    /** Draw up arrow '^' under the selected field */
    char arrowLine[17] = "                ";
    uint8_t arrowPos = 0;
    switch (cursorIndex) {
        case 0: arrowPos = 0; break;  /* Day */
        case 1: arrowPos = 3; break;  /* Month */
        case 2: arrowPos = 6; break;  /* Year */
        case 3: arrowPos = 9; break;  /* Hour */
        case 4: arrowPos = 12; break; /* Minute */
        case 5: arrowPos = 15; break; /* Second */
    }
    arrowLine[arrowPos] = '^';
    lcdDisplay.PrintMessage(String(arrowLine), 0, 1);

    /** Handle OK and ESC */
    if (pbOkState) {
        /** Save new date/time to RTC */
        DateTime newdt(2000 + year, month, day, hour, minute, second);
        rtc_datetime.setDateTime(newdt);
        LogSerialn("RTC set to: " + rtc_datetime.getFormattedDateTime(), true);
        initialized = false;
        retval = SCREEN_MAIN_CFGS;
    } else if (pbEscState) {
        /** Discard changes */
        initialized = false;
        retval = SCREEN_MAIN_CFGS;
    }

    return retval;
}

/**
 * @brief Displays the pump 1 cycle setting screen. Define the time that pump 1 will be in active state before switch to other pump.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param pbLeftState State of the LEFT push button.
 * @param pbRightState State of the RIGHT push button.
 * @param pump1cycle The current cycle time for pump 1.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplayCfgPump1Cycle(
    bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState,
    bool pbLeftState, bool pbRightState,
    PumpCycleTime (&PumpCyclesTimes)[2], LCD_Display &lcdDisplay)
{
    static uint8_t hour = 0, minute = 0, second = 0;
    static uint8_t cursorIndex = 0;
    static bool initialized = false;
    ScreenMode_t retval = SCREEN_CFG_PUMP1_CYCLE;

    if (!initialized) {
        hour = PumpCyclesTimes[0].hour;
        minute = PumpCyclesTimes[0].minute;
        second = PumpCyclesTimes[0].second;
        cursorIndex = 0;
        initialized = true;
    }

    if (pbLeftState && cursorIndex > 0) cursorIndex--;
    if (pbRightState && cursorIndex < 2) cursorIndex++;

    if (pbUpState) {
        switch (cursorIndex) {
            case 0: if (hour < 23) hour++; else hour = 0; break;
            case 1: if (minute < 59) minute++; else minute = 0; break;
            case 2: if (second < 59) second++; else second = 0; break;
        }
    }
    if (pbDownState) {
        switch (cursorIndex) {
            case 0: if (hour > 0) hour--; else hour = 23; break;
            case 1: if (minute > 0) minute--; else minute = 59; break;
            case 2: if (second > 0) second--; else second = 59; break;
        }
    }

    char cycleStr[9];
    snprintf(cycleStr, sizeof(cycleStr), "%02u:%02u:%02u", hour, minute, second);
    lcdDisplay.clearScreen();
    lcdDisplay.PrintMessage(String(cycleStr), 0, 0);

    char arrowLine[9] = "       ";
    uint8_t arrowPos = cursorIndex * 3;
    arrowLine[arrowPos] = '^';
    lcdDisplay.PrintMessage(String(arrowLine), 0, 1);

    if (pbOkState) {
        PumpCyclesTimes[0].hour = hour;
        PumpCyclesTimes[0].minute = minute;
        PumpCyclesTimes[0].second = second;
        initialized = false;
        LogSerialn("Pump 1 cycle set to: " + String(cycleStr), true);
        retval = SCREEN_MAIN_CFGS;
    } else if (pbEscState) {
        initialized = false;
        retval = SCREEN_MAIN_CFGS;
    }

    return retval;
}

/**
 * @brief Displays the pump 2 cycle setting screen. Define the time that pump 2 will be in active state before switch to other pump.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param pbLeftState State of the LEFT push button.
 * @param pbRightState State of the RIGHT push button.
 * @param pump2cycle The current cycle time for pump 2.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplayCfgPump2Cycle(
    bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState,
    bool pbLeftState, bool pbRightState,
    PumpCycleTime (&PumpCyclesTimes)[2], LCD_Display &lcdDisplay)
{
    static uint8_t hour = 0, minute = 0, second = 0;
    static uint8_t cursorIndex = 0;
    static bool initialized = false;
    ScreenMode_t retval = SCREEN_CFG_PUMP2_CYCLE;

    if (!initialized) {
        hour = PumpCyclesTimes[1].hour;
        minute = PumpCyclesTimes[1].minute;
        second = PumpCyclesTimes[1].second;
        cursorIndex = 0;
        initialized = true;
    }

    if (pbLeftState && cursorIndex > 0) cursorIndex--;
    if (pbRightState && cursorIndex < 2) cursorIndex++;

    if (pbUpState) {
        switch (cursorIndex) {
            case 0: if (hour < 23) hour++; else hour = 0; break;
            case 1: if (minute < 59) minute++; else minute = 0; break;
            case 2: if (second < 59) second++; else second = 0; break;
        }
    }
    if (pbDownState) {
        switch (cursorIndex) {
            case 0: if (hour > 0) hour--; else hour = 23; break;
            case 1: if (minute > 0) minute--; else minute = 59; break;
            case 2: if (second > 0) second--; else second = 59; break;
        }
    }

    char cycleStr[9];
    snprintf(cycleStr, sizeof(cycleStr), "%02u:%02u:%02u", hour, minute, second);
    lcdDisplay.clearScreen();
    lcdDisplay.PrintMessage(String(cycleStr), 0, 0);

    char arrowLine[9] = "       ";
    uint8_t arrowPos = cursorIndex * 3;
    arrowLine[arrowPos] = '^';
    lcdDisplay.PrintMessage(String(arrowLine), 0, 1);

    if (pbOkState) {
        PumpCyclesTimes[1].hour = hour;
        PumpCyclesTimes[1].minute = minute;
        PumpCyclesTimes[1].second = second;
        initialized = false;
        LogSerialn("Pump 2 cycle set to: " + String(cycleStr), true);
        retval = SCREEN_MAIN_CFGS;
    } else if (pbEscState) {
        initialized = false;
        retval = SCREEN_MAIN_CFGS;
    }

    return retval;
}