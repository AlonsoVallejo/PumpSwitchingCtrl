# Pump Alternator Controller

## Overview

Pump Alternator Controller is an embedded project running on MCU ATMEGA328P designed to manage two pumps for filling a cistern from a well. The system alternates between pumps each time the cistern is emptied and refilled, ensuring balanced usage and protection against dry running. The project features a user interface with a 16x2 I2C LCD and push buttons for mode selection, pump selection, and configuration.

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

## Hardware Requirements

- Arduino Nano (ATMEGA328P)
- 16x2 I2C LCD display (default address 0x27)
- Two pumps (controlled via digital outputs)
- Well and cistern level sensors (digital inputs)
- Push buttons for user interface (mode, pump select, up, down, left, right, OK, ESC)
- RTC module (DS3231 recommended)

## Pinout

- Check full schematic diagram in `doc/`

## How It Works

- **Automatic by Sensors:** When the cistern becomes empty, only one pump is activated (alternating each cycle). The pump runs until the cistern is full. If the well runs dry, the pump pauses and resumes when water is available.
- **Automatic by Timer:** When the cistern becomes empty, the selected pump runs for its configured cycle time, then alternates to the other pump for its configured time, repeating until the cistern is full. If the well runs dry, the pump pauses and resumes when water is available. Alternation only starts if both pump times are set.
- **Manual Mode:** The user can select which pump(s) to activate using the interface.
- **Menu System:** The LCD displays the current mode and time. The user can navigate to settings to change the control mode, set the system time, or configure pump cycle times.

## Getting Started

1. **Wiring:** Connect all sensors, actuators, RTC, and the LCD as per the pin definitions and schematic.
2. **Build & Upload:** Use PlatformIO to build and upload the firmware to your Arduino Nano.
3. **Operation:** Use the push buttons to navigate the menu and select the desired mode. The LCD will provide feedback and status.

## Project Structure

- `src/` - Source code (main logic, hardware abstraction, user interface)
- `include/` - Header files
- `doc/` - Additional documentation and diagrams
- `platformio.ini` - PlatformIO project configuration

## Documentation and Diagrams

- ![System Diagram](doc/pump_alternator_diagram.png)
- Additional documentation and diagrams can be found in the [`doc/`](doc/) directory.

## License

This project is licensed under the MIT License.

---