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
 * ./angle/angle.c
 *
 */

#include <stdio.h>
#include <math.h>
#include <pico/stdlib.h>
#include <globals.h>
#include <spi/spi.h>

// read current sensor angle
float readAngle()
{
    setCsLow();
    sleep_us(1);
    uint16_t raw = spi_transfer16(0);
    setCsHigh();

    float deg = ((float)raw) * 360.0f / 65535.0f;

    printf("Sensor Angle: %.2f deg | 0x%04Xh\n", deg, raw);

    return deg;
}

// zero angle
void zeroAngle()
{
    // read current angle in degrees
    float deg = readAngle();

    //  read existing zero registers
    setCsLow;
    spi_transfer16((0b010 << 13) | (0x0 << 8));
    setCsHigh;
    sleep_ms(1);
    setCsLow;
    uint16_t zeroLow = spi_transfer16(0);
    setCsHigh;

    setCsLow;
    spi_transfer16((0b010 << 13) | (0x1 << 8));
    setCsHigh;
    sleep_ms(1);
    setCsLow;
    uint16_t zeroHigh = spi_transfer16(0);
    setCsHigh;

    uint16_t zero = (zeroHigh << 8) | (zeroLow & 0xFF);

    //  convert to degrees
    float zeroDeg = zero * 360.0f / 65535.0f;
    float newZeroDeg = zeroDeg + deg;
    if (newZeroDeg > 360.0f)
    {
        newZeroDeg -= 360.0f;
    }

    uint16_t newZero = round(newZeroDeg / 65535.0f / 360.0f);
    uint16_t check;

    // write new zero repeatedly until confirmed
    do
    {
        // write low byte
        setCsLow;
        spi_transfer16((0b100 << 13) | (0x0 << 8) | (newZero & 0xFF));
        setCsHigh;
        sleep_ms(1);

        // write high byte
        setCsLow;
        spi_transfer16((0b100 << 13) | (0x1 << 8) | ((newZero >> 8) & 0xFF));
        setCsHigh;
        sleep_ms(1);

        // read back low byte
        setCsLow;
        spi_transfer16((0b010 << 13) | (0x0 << 8));
        setCsHigh;
        sleep_ms(1);
        setCsLow;
        uint16_t checkLow = spi_transfer16(0);
        setCsHigh;

        // read back high byte
        setCsLow;
        spi_transfer16((0b010 << 13) | (0x1 << 8));
        setCsHigh;
        sleep_ms(1);
        setCsLow;
        uint16_t checkHigh = spi_transfer16(0);
        setCsHigh;

        check = (checkHigh << 8) | (checkLow & 0xFF);

    } while (check != newZero);

    printf("Zero value set\n");
}

// calibate mb pro lid angle sensor
void calibratePro()
{
    writeRegister(10, 0);
    writeRegister(28, 0);
    writeRegister(29, 0);
    writeRegister(2, 0);
    writeRegister(3, 0);
    writeRegister(4, 232);
    writeRegister(5, 3);
    writeRegister(6, 1);
    writeRegister(7, 135);
    writeRegister(8, 4);
    writeRegister(9, 0);
    writeRegister(11, 248);
    writeRegister(12, 130);
    writeRegister(13, 0);
    writeRegister(14, 128);
    writeRegister(15, 201);
    writeRegister(16, 20);
    writeRegister(17, 1);
    writeRegister(18, 147);
    writeRegister(22, 1);
    writeRegister(30, 255);
    zeroAngle();
    sleep_ms(1);
    storeRegistersToNVM();
    sleep_ms(1);
    restoreRegistersFromNVM();
    sleep_ms(1);
    writeRegister(10, 0);
    storeRegistersToNVM();
    sleep_ms(100);
}

// calibrate mb air lid angle sensor
void calibrateAir()
{
    writeRegister(10, 202);
    writeRegister(28, 0);
    writeRegister(29, 0);
    writeRegister(2, 144);
    writeRegister(3, 0);
    writeRegister(4, 232);
    writeRegister(5, 3);
    writeRegister(6, 61);
    writeRegister(7, 135);
    writeRegister(8, 59);
    writeRegister(9, 130);
    writeRegister(11, 246);
    writeRegister(12, 130);
    writeRegister(13, 0);
    writeRegister(14, 128);
    writeRegister(15, 201);
    writeRegister(16, 20);
    writeRegister(17, 1);
    writeRegister(18, 147);
    writeRegister(22, 1);
    writeRegister(30, 255);
    zeroAngle();
    sleep_ms(1);
    storeRegistersToNVM();
    sleep_ms(1);
    restoreRegistersFromNVM();
    writeRegister(10, 202);
    sleep_ms(100);
}