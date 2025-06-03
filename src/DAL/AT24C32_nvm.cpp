#include "AT24C32_nvm.h"

/**
 * @brief Writes bytes to the I2C EEPROM (AT24C32).
 * This function writes a sequence of bytes to the I2C EEPROM starting from the specified address.
 * @param eeaddress The starting address in the EEPROM to write the data.
 * @param data A pointer to the data buffer to be written to the EEPROM.
 * @param length The number of bytes to write from the data buffer.
 */
void I2C_EEPROM_WriteBytes(uint16_t eeaddress, const uint8_t* data, uint16_t length) {
    while (length > 0) {
        Wire.beginTransmission(AT24C32_I2C_ADDR);
        Wire.write((int)(eeaddress >> 8));   // MSB
        Wire.write((int)(eeaddress & 0xFF)); // LSB
        uint8_t bytesThisPage = min(length, 32 - (eeaddress % 32)); // 32-byte page boundary
        for (uint8_t i = 0; i < bytesThisPage; i++) {
            Wire.write(data[i]);
        }
        Wire.endTransmission();
        delay(5); // Write cycle time
        eeaddress += bytesThisPage;
        data += bytesThisPage;
        length -= bytesThisPage;
    }
}

/**
 * @brief Reads bytes from the I2C EEPROM (AT24C32).
 * This function reads a sequence of bytes from the I2C EEPROM starting from the specified address.
 * @param eeaddress The starting address in the EEPROM to read the data.
 * @param data A pointer to the buffer where the read data will be stored.
 * @param length The number of bytes to read and store in the data buffer.
 */
void I2C_EEPROM_ReadBytes(uint16_t eeaddress, uint8_t* data, uint16_t length) {
    while (length > 0) {
        Wire.beginTransmission(AT24C32_I2C_ADDR);
        Wire.write((int)(eeaddress >> 8));   // MSB
        Wire.write((int)(eeaddress & 0xFF)); // LSB
        Wire.endTransmission();
        uint8_t bytesThisRead = min(length, 32);
        Wire.requestFrom(AT24C32_I2C_ADDR, bytesThisRead);
        for (uint8_t i = 0; i < bytesThisRead && Wire.available(); i++) {
            data[i] = Wire.read();
        }
        eeaddress += bytesThisRead;
        data += bytesThisRead;
        length -= bytesThisRead;
    }
}
