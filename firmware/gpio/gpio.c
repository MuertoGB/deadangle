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
 * ./gpio/gpio.c
 * 
 */

#include <pico/stdlib.h>
#include <globals.h>
#include <angle/angle.h>
#include <gpio/gpio.h>
#include <spi/spi.h>

// blink led
void blinkLed(int times, int millis)
{
    for (int i = 0; i < times; i++)
    {
        gpio_put(MODE_LED, nSetHigh);
        sleep_ms(millis);
        gpio_put(MODE_LED, nSetLow);
        sleep_ms(millis);
    }
}

// activate led based on sensor position
void sensorPosition()
{
    float deg = readAngle();

    if ((deg >= 130 && deg <= 360) || (deg >= 0 && deg <= 5))
    {
        gpio_put(MODE_LED, nSetLow);
    }
    else
    {
        gpio_put(MODE_LED, nSetHigh);
        sleep_ms(10);
    }
}

// gpio15 short press
void handleShortPress()
{
    nMode++;

    if (nMode > 3)
    {
        nMode = 1;
    }

    // blink LED according to mode
    for (int i = 0; i < nMode; i++)
    {
        gpio_put(MODE_LED, 0);
        sleep_ms(200);
        gpio_put(MODE_LED, 1);
        sleep_ms(200);
        gpio_put(MODE_LED, 0);
        sleep_ms(200);
    }
}

// gpio15 long press
void handleLongPress()
{
    gpio_put(MODE_LED, nSetLow);

    if (nMode == 1 || nMode == 2)
    {
        checkFlash();
    }
    else if (nMode == 3)
    {
        while (gpio_get(MODE_BUTTON))
        {
            sensorPosition();
            sleep_ms(10);
        }
    }
}