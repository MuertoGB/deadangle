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
 * ./angle/angle.h
 * 
 */

#ifndef ANGLE_H
#define ANGLE_H

float readAngle();
void zeroAngle(void);
void calibratePro(void);
void calibrateAir(void);

#endif