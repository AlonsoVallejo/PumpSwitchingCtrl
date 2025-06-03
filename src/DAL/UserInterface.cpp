#include "UserInterface.h"

/**
 * @brief Displays the main screen with control mode and current time.
 * @param pbOkState State of the OK push button (not used here, but kept for interface compatibility).
 * @param mode The current control mode to display.
 * @param lcdDisplay Reference to the LCD display object.
 * @param Hour String with the current hour in "HH:MM:SS" 24-hour format.
 * @return SCREEN_MODE_MAIN (remains on main screen).
 */
ScreenMode_t DisplayMainScreen(bool pbOkState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay, String Hour)
{
    ScreenMode_t retval = SCREEN_MODE_MAIN;

    /** Display control mode */
    String ctrlModeStr;
    switch(mode) {
        case CTRL_MODE_MANUAL:
            ctrlModeStr = "MANUAL";
            break;
        case CTRL_AUTO_BY_SENSORS:
            ctrlModeStr = "A_BY_SENS";
            break;
        case CTRL_AUTO_BY_TIMER:
            ctrlModeStr = "A_BY_TIMER";
            break;
        default:
            ctrlModeStr = "UNKNOWN";
            break;
    }
    lcdDisplay.PrintMessage("Ctrl: " + ctrlModeStr, 0, 0);

    /** Display the hour in 24-hour format */
    lcdDisplay.PrintMessage("Hour: " + Hour, 0, 1);

    if( pbOkState) {
        /** If OK button is pressed, switch to option settings screen */
        retval = SCREEN_MODE_OPTION_SETTINGS;
    } else {
        /** If OK button is not pressed, remain on the main screen */
        retval = SCREEN_MODE_MAIN;
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
ScreenMode_t DisplayOptionSettingsScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, LCD_Display &lcdDisplay)
{
    /** Static variable to keep track of the selected menu index: 0 = Set Control, 1 = Set Hour */
    static uint8_t selectedIndex = 0;
    ScreenMode_t retval = SCREEN_MODE_OPTION_SETTINGS;

    /** Handle navigation */
    if (pbUpState && selectedIndex > 0) {
        selectedIndex = 0;
    }
    if (pbDownState && selectedIndex < 1) {
        selectedIndex = 1;
    }

    /** Display menu options with selector '>' */
    if (selectedIndex == 0) {
        lcdDisplay.PrintMessage(">Set AutoCtrl", 0, 0);
        lcdDisplay.PrintMessage(" Set Hour", 0, 1);
    } else {
        lcdDisplay.PrintMessage(" Set AutoCtrl", 0, 0);
        lcdDisplay.PrintMessage(">Set Hour", 0, 1);
    }

    /** Handle selection */
    if (pbOkState) {
        if (selectedIndex == 0) {
            retval = SCREEN_MODE_SET_CTRL_MODE;
        } else {
            retval = SCREEN_MODE_SET_DATE_TIME;
        }
        /** Reset selection for next entry */
        selectedIndex = 0;
    } else if (pbEscState) {
        retval = SCREEN_MODE_MAIN;
        /** Reset selection for next entry */
        selectedIndex = 0;
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
ScreenMode_t DisplaySetCtrlModeScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, CtrlModeSel_t &mode, LCD_Display &lcdDisplay) 
{
    /** Static variable to keep track of the selected menu index: 0 = Set Control, 1 = Set Hour */
    static uint8_t selectedIndex = 0;
    ScreenMode_t retval = SCREEN_MODE_SET_CTRL_MODE;

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
            retval = SCREEN_MODE_OPTION_SETTINGS;
            mode = CTRL_AUTO_BY_SENSORS;
        } else {
            retval = SCREEN_MODE_OPTION_SETTINGS;
            mode = CTRL_AUTO_BY_TIMER;
        }
        /** Reset selection for next entry */
        selectedIndex = 0;
    } else if (pbEscState) {
        retval = SCREEN_MODE_MAIN;
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
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplaySetDateTimeScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay)
{
    lcdDisplay.PrintMessage("SET_DATE_TIME", 0, 0);

    // Implement date and time setting logic here
    // For now, just return the same screen mode
    return SCREEN_MODE_SET_DATE_TIME;
}

/**
 * @brief Displays the pump 1 cycle setting screen.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplaySetPump1CycleScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay)
{
    lcdDisplay.PrintMessage("SET_PUMP1_CYCLE", 0, 0);

    // Implement pump 1 cycle setting logic here
    // For now, just return the same screen mode
    return SCREEN_MODE_SET_PUMP1_CYCLE;
}

/**
 * @brief Displays the pump 2 cycle setting screen.
 * @param pbOkState State of the OK push button.
 * @param pbEscState State of the ESC push button.
 * @param pbUpState State of the UP push button.
 * @param pbDownState State of the DOWN push button.
 * @param lcdDisplay Reference to the LCD display object.
 * @return The next screen mode based on user input.
 */
ScreenMode_t DisplaySetPump2CycleScreen(bool pbOkState, bool pbEscState, bool pbUpState, bool pbDownState, bool pbLeftState, bool pbRightState, LCD_Display &lcdDisplay)
{
    lcdDisplay.PrintMessage("SET_PUMP2_CYCLE", 0, 0);

    // Implement pump 2 cycle setting logic here
    // For now, just return the same screen mode
    return SCREEN_MODE_SET_PUMP2_CYCLE;
}