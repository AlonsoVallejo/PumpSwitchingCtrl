#define ifndef AT24C32_NVM_H
#ifndef AT24C32_NVM_H

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

#define AT24C32_I2C_ADDR 0x57
#define AT24C32_START_ADDR 0x0000

void I2C_EEPROM_WriteBytes(uint16_t eeaddress, const uint8_t* data, uint16_t length);
void I2C_EEPROM_ReadBytes(uint16_t eeaddress, uint8_t* data, uint16_t length);

#endif