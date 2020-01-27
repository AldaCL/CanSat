#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
//#include "../libraries/L3G4200D/L3G4200D.h"
//#include "../libraries/ADXL345/ADXL345.h"

/* SPI port definition for BMP280 */
#define BMP_SCK  (13) 
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

//Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  Serial.begin(9600);
  //Serial.println(F("BMP280 test"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X2,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X8);     /* Filtering. */
                  //Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */
}

void loop() {
    //Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.print(",");
    //Serial.println(" *C");

    //Serial.print(F("Pressure = "));
    //Serial.print(bmp.readPressure());
    //Serial.println(" Pa");

    //Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.print(",");
    //Serial.print(',');
    //Serial.println(" m");
    //Serial.println();
    Serial.print(bmp.readPressure());
    Serial.print(",");
    Serial.println();
    delay(100);
}
