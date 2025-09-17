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
 * ./main.c
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <pico/stdio_usb.h>
#include <globals.h>
#include <version.h>
#include <gpio/gpio.h>
#include <spi/spi.h>

int main()
{
    stdio_init_all();

    // init gpio
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, nSetHigh);

    gpio_init(MODE_LED);
    gpio_set_dir(MODE_LED, GPIO_OUT);
    gpio_put(MODE_LED, nSetLow);

    gpio_init(MODE_BUTTON);
    gpio_set_dir(MODE_BUTTON, GPIO_IN);
    gpio_pull_up(MODE_BUTTON);

    // init spi
    spi_init(SPI_PORT, 1000 * 1000); // 1 mhz
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

    while (1)
    {
        bool pressed = !gpio_get(MODE_BUTTON);
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (pressed && uiPressTime == 0 && !isLedBlinking)
        {
            uiPressTime = now;
            isButtonHeld = false;
        }

        // long press (activate)
        if (pressed && !isButtonHeld && !isLedBlinking && (now - uiPressTime > 1000))
        {
            isButtonHeld = true;

            if (nMode == 1 || nMode == 2)
            {
                checkFlash();
            }
            else if (nMode == 3)
            {
                isMonitoringAngle = true;
            }
        }

        // short press (Mode change)
        if (!pressed && uiPressTime != 0 && !isButtonHeld && !isLedBlinking)
        {
            // short press -> next mode
            nMode++;
            if (nMode > 3)
            {
                nMode = 1;
            }

            // blink LED according to mode
            isLedBlinking = true;
            for (int i = 0; i < nMode; i++)
            {
                gpio_put(MODE_LED, 1);
                sleep_ms(200);
                gpio_put(MODE_LED, 0);
                sleep_ms(200);
            }
            isLedBlinking = false;

            printf("%s | %s\n", PROG_NAME, PROG_VERSION);
            printf("Switched to mode %d.\n", nMode);

            // if short press occurs during mode 3 then stop monitoring
            if (isMonitoringAngle)
            {
                isMonitoringAngle = false;
            }

            uiPressTime = 0;
            isButtonHeld = false;
        }

        // continuous isMonitoringAngle for mode
        if (isMonitoringAngle && nMode == 3)
        {
            sensorPosition();
            sleep_ms(10);
        }

        // reset timing if released
        if (!pressed && uiPressTime != 0 && isButtonHeld)
        {
            uiPressTime = 0;
            isButtonHeld = false;
        }

        sleep_ms(10);
    }
}