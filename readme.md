# Dual Activation Pump Alternator

## Overview

Pump Alternator Controller is an embedded project running on MCU ATMEGA328P designed to manage two pumps for filling a cistern from a well. The system alternates between pumps each time the cistern is emptied and refilled, ensuring balanced usage and protection against dry running. The project features a user interface with a I2C LCD and push buttons for mode selection, pump selection, and configuration. All configuration data  is stored in an external EEPROM for persistence.

## Features

- **Automatic Alternation:** Alternates between Pump 1 and Pump 2 each time the cistern is emptied and refilled.
- **Sensor-Based Control:** Operates pumps based on well and cistern level sensors.
- **Manual Mode:** Allows manual selection and activation of pumps (Pump 1, Pump 2, both, or none).
- **Timer-Based Alternation:** Each pump can be configured with a custom activation time (hours, minutes, seconds). Alternation only starts if both pump times are set.
- **User Interface:** 16x2 I2C LCD displays current mode, real-time clock, and settings menus.
- **Menu Navigation:** Push buttons for mode selection, pump selection, navigation (up, down, left, right), confirmation (OK), and escape (ESC).
- **Safe Operation:** Pumps are paused if the well is empty and resume when water is available.
- **RTC Configuration:** User can set the real-time clock (date and time) via the menu.
- **Pump Cycle Configuration:** User can set the activation time for each pump via the menu.
- **Debounced Inputs:** All digital inputs (buttons and sensors) are debounced in software.
- **Non-volatile Storage:** Pump cycle times are saved and loaded from an AT24C32 I2C EEPROM (address 0x57).

## Hardware Requirements

- ATMEGA328P
- 16x2 I2C LCD display 
- RTC I2C module DS3231
- AT24C32 I2C EEPROM module
- Two pumps (controlled via opto coupled digital outputs to relay outputs)
- Well and cistern level sensors (opto-coupled digital inputs)
- Push buttons for user interface (mode, pump select, up, down, left, right, OK, ESC)
- various electronics components (check Schematich diagram)

## How It Works

- **Automatic by Sensors:** When the cistern becomes empty, only one pump is activated (alternating each cycle). The pump runs until the cistern is full. If the well runs dry, the pump pauses and resumes when water is available.
- **Automatic by Timer:** When the cistern becomes empty, the selected pump runs for its configured cycle time, then alternates to the other pump for its configured time, repeating until the cistern is full. If the well runs dry, the pump pauses and resumes when water is available. Alternation only starts if both pump times are set.
- **Manual Mode:** The user can select which pump(s) to activate using the interface.
- **Menu System:** The LCD displays the current mode and time. The user can navigate to settings to change the control mode, set the system time, or configure pump cycle times.
- **EEPROM Handling:** On startup, pump cycle times are loaded from the AT24C32 EEPROM. If the EEPROM is uninitialized (all bytes are 0xFF), default values (0:0:0) are set and saved.

## Real-Time Clock (RTC) Usage

The system uses a DS3231 Real-Time Clock (RTC) module to keep accurate track of the current date and time, even when the controller is powered off. The RTC is used for:

- **Displaying the current date and time** on the LCD main screen.
- **Timer-based pump alternation:** The RTC provides precise timekeeping for activating each pump for its configured cycle duration in timer mode.
- **User configuration:** The user can set and adjust the date and time via the menu system using the push buttons.
- **Power loss recovery:** If the RTC loses power, it is automatically set to the compile time of the firmware on the next startup.


## Getting Started

1. **Wiring:** Connect all sensors, actuators, RTC, LCD, and the AT24C32 EEPROM as per the pin definitions and schematic.
2. **Build & Upload:** Use PlatformIO to build and upload the firmware to your Arduino Nano.
3. **Operation:** Use the push buttons to navigate the menu and select the desired mode. The LCD will provide feedback and status.

## Project Structure

- `src/` - Source code (main logic, hardware abstraction, user interface)
- `include/` - Header files
- `doc/` - Additional documentation and diagrams
- `platformio.ini` - PlatformIO project configuration

## Documentation and Diagrams

- Additional documentation and diagrams can be found in the [`doc/`](doc/) directory.

## License

This project is licensed under the MIT License.

The RTC is initialized during system startup. All time-based operations (such as pump alternation in timer mode) rely on the RTC for accuracy and persistence.

---