#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define LCD_DISPLAY_I2C_ADDR 0x27 
#define LCD_DISPLAY_COLS 16       
#define LCD_DISPLAY_ROWS 2        

class LCD_Display {
private:
    LiquidCrystal_I2C lcd;
public:
    LCD_Display(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows)
        : lcd(lcdAddr, lcdCols, lcdRows) {}
    void init();
    void clearScreen();
    void PrintMessage(const String &message, uint8_t col = 0, uint8_t row = 0);
    void PrintMessage(int value, uint8_t col = 0, uint8_t row = 0);
    void setCursor(uint8_t col, uint8_t row);
};

#endif