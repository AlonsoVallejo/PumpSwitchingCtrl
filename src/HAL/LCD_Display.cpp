#include "LCD_Display.h"

/**
 * @brief Initializes the LCD display.
 * This function initializes the LCD display by calling the begin() method of the LiquidCrystal_I2C object,
 * sets the backlight, and clears the display.
 */
void LCD_Display::init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
}

/**
 * @brief Clears the LCD screen.
 * This function clears the LCD display by calling the clear() method of the LiquidCrystal_I2C object.
 */
void LCD_Display::clearScreen() {
    lcd.clear();
}

/**
 * @brief Prints a message to the LCD display.
 * @param message The message to be printed on the LCD.
 * @param col The column position (0-based index) where the message will be printed.
 * @param row The row position (0-based index) where the message will be printed.
 */
void LCD_Display::PrintMessage(const String &message, uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
    lcd.print(message);
}

/**
 * @brief Prints an integer value to the LCD display.
 * @param value The integer value to be printed on the LCD.
 * @param col The column position (0-based index) where the value will be printed.
 * @param row The row position (0-based index) where the value will be printed.
 */
void LCD_Display::PrintMessage(int value, uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
    lcd.print(value);
}

/**
 * @brief Sets the cursor position on the LCD display.
 * @param col The column position (0-based index).
 * @param row The row position (0-based index).
 */
void LCD_Display::setCursor(uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
}