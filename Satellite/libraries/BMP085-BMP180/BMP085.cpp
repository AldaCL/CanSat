/*
BMP085.cpp - Class file for the BMP085/BMP180 Barometric Pressure & Temperature Sensor Arduino Library.

Version: 1.0.2
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This library use integer equations from BMP085 Datasheet

This library use floating-point equations from the Weather Station Data Logger project
http://wmrx00.sourceforge.net/
http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <math.h>
#include <util/delay.h>

#include <BMP085.h>

bool BMP085::begin(bmp085_oss_t oversampling)
{
    Wire.begin();

    // Check BMP085 REG CHIP_ID
    if (fastRegister8(BMP085_REG_CHIP_ID) != 0x55)
    {
        return false;
    }

    readCalibrationData();
    calculatePolynomials();

    oss = oversampling;

    return true;
}


// Set oversampling value
void BMP085::setOversampling(bmp085_oss_t oversampling)
{
    oss = oversampling;
}

// Get oversampling value
bmp085_oss_t BMP085::getOversampling(void)
{
    return oss;
}

// Set software oversampling value
void BMP085::setSoftwareOversampling(bool softwareOversampling)
{
    soss = softwareOversampling;
}

// Get oversampling value
bool BMP085::getSoftwareOversampling(void)
{
    return soss;
}

// Polynomials for Floating Point Pressure Calculation
void BMP085::calculatePolynomials(void)
{
    double fc3,fc4,fb1;
    fc3 = 160.0 * pow(2,-15) * ac3;
    fc4 = pow(10,-3) * pow(2,-15) * ac4;
    fb1 = pow(160,2) * pow(2,-30) * b1;
    fc5 = (pow(2,-15) / 160) * ac5;
    fc6 = ac6;
    fmc = (pow(2,11) / pow(160,2)) * mc;
    fmd = md / 160.0;
    fx0 = ac1;
    fx1 = 160.0 * pow(2,-13) * ac2;
    fx2 = pow(160,2) * pow(2,-25) * b2;
    fy0 = fc4 * pow(2,15);
    fy1 = fc4 * fc3;
    fy2 = fc4 * fb1;
    fp0 = (3791.0 - 8.0) / 1600.0;
    fp1 = 1.0 - 7357.0 * pow(2,-20);
    fp2 = 3038.0 * 100.0 * pow(2,-36);
}

// Read calibration data (BMP085 datasheet, page 13, block 1)
void BMP085::readCalibrationData(void)
{
    ac1 = readRegister16(BMP085_REG_AC1);
    ac2 = readRegister16(BMP085_REG_AC2);
    ac3 = readRegister16(BMP085_REG_AC3);
    ac4 = readRegister16(BMP085_REG_AC4);
    ac5 = readRegister16(BMP085_REG_AC5);
    ac6 = readRegister16(BMP085_REG_AC6);
    b1 = readRegister16(BMP085_REG_B1);
    b2 = readRegister16(BMP085_REG_B2);
    mb = readRegister16(BMP085_REG_MB);
    mc = readRegister16(BMP085_REG_MC);
    md = readRegister16(BMP085_REG_MD);
}

// Read raw temperature (BMP085 datasheet, page 13, block 2)
uint16_t BMP085::readRawTemperature(void)
{
    writeRegister8(BMP085_REG_CONTROL, BMP085_CMD_MEASURE_TEMP);

    _delay_ms(5); // wait 4.5ms

    return readRegister16(BMP085_REG_DATA);
}

// Read temperature (BMP085 datasheet, page 13, block 4)
double BMP085::readTemperature(void)
{
    double T;
    int32_t UT = readRawTemperature();
    int32_t X1, X2;
    int32_t B5;

    X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) / pow(2,15);
    X2 = ((int32_t)mc * pow(2,11)) / (X1 + (int32_t)md);
    B5 = X1 + X2;
    T  = (B5+8) / pow(2,4);
    T /= 10;

    return T;
}

// Read Floating Point Temperature
double BMP085::readFloatTemperature(void)
{
    double tu, a;

    tu = readRawTemperature();
    a = fc5 * (tu - fc6);

    return a + (fmc / (a + fmd));
}

// Read raw pressure (BMP085 datasheet, page 13, block 3)
uint32_t BMP085::readRawPressure(bool rawRegister)
{
    uint32_t value;

    writeRegister8(BMP085_REG_CONTROL, BMP085_CMD_MEASURE_PRESSURE_0 + (oss << 6));

    if (oss == BMP085_ULTRA_LOW_POWER)
    {
	_delay_ms(5);
    } else
    if (oss == BMP085_STANDARD)
    {
	_delay_ms(8);
    } else
    if (oss == BMP085_HIGH_RES)
    {
	_delay_ms(14);
    } else
    {
	_delay_ms(26);
    }

    value = readRegister16(BMP085_REG_DATA);
    value <<= 8;
    value |= readRegister8(BMP085_REG_DATA + 2);

    if (rawRegister)
    {
	return value;
    }

    value >>= (8 - oss);

    return value;
}

// Read pressure (BMP085 datasheet, page 13, block 5)
uint32_t BMP085::readPressure(void)
{
    int32_t UT, UP;
    int32_t B3, B5, B6;
    uint32_t B4, B7;
    int32_t X1, X2, X3;
    int32_t P;

    // Read raw temperature & Pressure
    UT = readRawTemperature();

    if (soss)
    {
	UP = 0;

	for (int i = 0; i < 3; i++)
	{
	    UP += readRawPressure();
	}

	UP /= 3;
    } else
    {
	UP = readRawPressure();
    }

    // Do calc
    X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) / pow(2,15);
    X2 = ((int32_t)mc * pow(2,11)) / (X1 + (int32_t)md);
    B5 = X1 + X2;
    B6 = B5 - 4000;
    X1 = ((int32_t)b2 * ((B6 * B6) >> 12)) >> 11;
    X2 = ((int32_t)ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((int32_t)ac1 * 4 + X3) << oss) + 2) / 4;
    X1 = ((int32_t)ac3 * B6) >> 13;
    X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL >> oss);

    if (B7 < 0x80000000)
    {
	P = (B7 * 2) / B4;
    } else 
    {
	P = (B7 / B4) * 2;
    }

    X1 = (P >> 8) * (P >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * P) >> 16;
    P = P + ((X1 + X2 + (int32_t)3791) >> 4);

    return P;
}

// Read Floating Point Pressure
double BMP085::readFloatPressure(void)
{
    double pt, pu, s, x, y, z;
    uint32_t pr;
    uint8_t pr0, pr1, pr2;

    pt = readFloatTemperature();

    if (soss)
    {
	pr = 0;

	for (int i = 0; i < 3; i++)
	{
	    pr += readRawPressure(true);
	}

	pr /= 3;
    } else
    {
	pr = readRawPressure(true);
    }

    pr0 = pr & 0xFF;
    pr1 = (pr >> 8) & 0xFF;
    pr2 = (pr >> 16) & 0xFF;

    pu = (pr2 * 256.00) + pr1 + (pr0 / 256.00);

    s = pt - 25.0;
    x = (fx2 * pow(s,2)) + (fx1 * s) + fx0;
    y = (fy2 * pow(s,2)) + (fy1 * s) + fy0;
    z = (pu - x) / y;

    return ((fp2 * pow(z,2)) + (fp1 * z) + fp0) * 100;
}

// Calculate altitude from Pressure & Sea level pressure
double BMP085::getAltitude(double pressure, double seaLevelPressure)
{
    return (44330.0f * (1.0f - pow((double)pressure / (double)seaLevelPressure, 0.1902949f)));
}

// Calculate sea level from Pressure given on specific altitude
double BMP085::getSeaLevel(double pressure, double altitude)
{
    return ((double)pressure / pow(1.0f - ((double)altitude / 44330.0f), 5.255f));
}

// Get Version
uint16_t BMP085::getVersion()
{
    return readRegister16(BMP085_REG_VERSION);
}

// Write 8-bit to register
void BMP085::writeRegister8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(BMP085_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
	Wire.write(value);
    #else
	Wire.send(reg);
	Wire.send(value);
    #endif
    Wire.endTransmission();
}

// Fast read 8-bit from register
uint8_t BMP085::fastRegister8(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(BMP085_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
    #else
	Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.requestFrom(BMP085_ADDRESS, 1);
    #if ARDUINO >= 100
	value = Wire.read();
    #else
	value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// Read 8-bit from register
uint8_t BMP085::readRegister8(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(BMP085_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
    #else
	Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.requestFrom(BMP085_ADDRESS, 1);
    while(!Wire.available()) {};
    #if ARDUINO >= 100
	value = Wire.read();
    #else
	value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// Read 16-bit from register (oops MSB, LSB)
uint16_t BMP085::readRegister16(uint8_t reg)
{
    uint16_t value;
    Wire.beginTransmission(BMP085_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.requestFrom(BMP085_ADDRESS, 2);
    while(!Wire.available()) {};
    #if ARDUINO >= 100
        uint8_t vha = Wire.read();
        uint8_t vla = Wire.read();
    #else
        uint8_t vha = Wire.receive();
        uint8_t vla = Wire.receive();
    #endif;
    Wire.endTransmission();

    value = vha << 8 | vla;

    return value;
}
