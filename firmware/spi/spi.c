/*
 * DeadAngle – MacBook Lid Angle Sensor Calibration Port for Pico SDK
 * 
 * Ported to the Pico SDK by MuertoGB.
 * Original project: Vladislav98759/Macbook-Lid-Angle-Sensor-Calibration-Tool
 * Original project has no license; this port is GPL v3 for MuertoGB's code.
 *
 * Copyright (C) 2025 MuertoGB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * ./spi/spi.c
 * 
 */

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <globals.h>
#include <angle/angle.h>
#include <spi/spi.h>
#include <gpio/gpio.h>

// set chip select low
void setCsLow()
{
    gpio_put(PIN_CS, nSetLow);
}

// set chip select high
void setCsHigh()
{
    gpio_put(PIN_CS, nSetHigh);
}

uint16_t spi_transfer16(uint16_t data)
{
    uint8_t tx[2] = {(data >> 8) & 0xFF, data & 0xFF};
    uint8_t rx[2] = {0, 0};
    spi_write_read_blocking(SPI_PORT, tx, rx, 2);
    return ((uint16_t)rx[0] << 8) | rx[1];
}

// store registers to non-volatile memory
void storeRegistersToNVM()
{
    setCsLow;
    spi_transfer16(0b110 << 13);
    setCsHigh;
    printf("All registers written to NVM\n");
}

// restore registers from non-volatile memory
void restoreRegistersFromNVM()
{
    setCsLow;
    spi_transfer16(0b101 << 13);
    setCsHigh;
    printf("All registers restored from NVM\n");
}

// helpers
static inline void writeRegisterCommand(int reg, uint16_t value) {
    setCsLow;
    spi_transfer16((0b100 << 13) | (reg << 8) | value);
    setCsHigh;
}

static inline uint16_t readRegisterCommand(int reg) {
    setCsLow;
    spi_transfer16((0b010 << 13) | (reg << 8)); // read command
    setCsHigh;

    sleep_ms(1);

    setCsLow;
    uint16_t v = spi_transfer16(0); // read register value
    setCsHigh;

    return v;
}

// read register
uint8_t readRegister(uint8_t reg)
{
    setCsLow;
    spi_transfer16((0b010 << 13) | (reg << 8));
    setCsHigh;
    sleep_ms(1);
    setCsLow;
    uint16_t value = spi_transfer16(0);
    setCsHigh;
    return value & 0xFF;
}

// write register
void writeRegister(int registerAddress, int registerValue)
{
    uint16_t v = (uint16_t)registerValue;

    // First attempt
    writeRegisterCommand(registerAddress, v);
    sleep_ms(1);
    uint16_t checkValue = readRegisterCommand(registerAddress);

    if ((checkValue & 0xFF) == v) {
        printf("Register %d write successful.\n", registerAddress);
        return;
    }

    printf("Write to register %d failed. Retrying...\n", registerAddress);

    // Retry once
    writeRegisterCommand(registerAddress, v);
    sleep_ms(1);
    checkValue = readRegisterCommand(registerAddress);

    if ((checkValue & 0xFF) == v) {
        printf("Register %d write successful after retry\n", registerAddress);
    } else {
        printf("Write to register %d failed again\n", registerAddress);
    }
}

// check flash
void checkFlash()
{
    uint16_t originalValue, checkValue;

    // --- Read register 0 ---
    setCsLow;
    spi_transfer16((0b010 << 13) | (0 << 8));
    setCsHigh;
    sleep_ms(1);
    setCsLow;
    originalValue = spi_transfer16(0);
    setCsHigh;

    // --- Attempt write ---
    writeRegister(0, (originalValue & 0xFF) + 1);
    sleep_ms(1);

    // --- Read register 0 again ---
    setCsLow;
    spi_transfer16((0b010 << 13) | (0 << 8));
    setCsHigh;
    sleep_ms(1);
    setCsLow;
    checkValue = spi_transfer16(0);
    setCsHigh;

    // --- Check if write succeeded ---
    if ((checkValue & 0xFF) == ((originalValue & 0xFF) + 1))
    {
        printf("Flash is unlocked (write succeeded)\n");

        if (nMode == 1)
        {
            calibratePro();
        }
        else if (nMode == 2)
        {
            calibrateAir();
        }
    }
    else if ((originalValue & 0xFF) != 0)
    {
        printf("Flash appears locked (write failed)\n");
        blinkLed(6, 100);
    }
    else
    {
        printf("Flash check failed (unexpected state)\n");
        blinkLed(3, 100);
    }
}