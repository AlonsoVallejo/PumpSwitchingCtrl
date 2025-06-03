# Pump Alternator Controller

## Overview

Pump Alternator Controller is an embedded project for Arduino Nano designed to manage two pumps for filling a cistern from a well. The system alternates between pumps each time the cistern is emptied and refilled, ensuring balanced usage and protection against dry running. The project features a user interface with an I2C LCD and push buttons for mode selection and settings.

## Features

- **Automatic Alternation:** Alternates between Pump 1 and Pump 2 each time the cistern is emptied and refilled.
- **Sensor-Based Control:** Operates pumps based on well and cistern level sensors.
- **Manual Mode:** Allows manual selection and activation of pumps.
- **User Interface:** 16x2 I2C LCD displays current mode, time, and settings menu.
- **Menu Navigation:** Push buttons for mode selection, pump selection, navigation, and confirmation.
- **Safe Operation:** Pumps are paused if the well is empty and resume when water is available.
- **Debounced Inputs:** All digital inputs are debounced for reliable operation.

## Hardware Requirements

- Check full schematic diagram in `doc/`

## Pinout

- Check full schematic diagram in `doc/`

## How It Works

- **Automatic by Sensors:** When the cistern becomes empty, only one pump is activated (alternating each cycle). The pump runs until the cistern is full. If the well runs dry, the pump pauses and resumes when water is available.
- **Manual Mode:** The user can select which pump(s) to activate using the interface.
- **Menu System:** The LCD displays the current mode and time. The user can navigate to settings to change the control mode or set the system time.

## Getting Started

1. **Wiring:** Connect all sensors, actuators, and the LCD as per the pin definitions.
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