
#include <Wire.h>
#include "../libraries/BMP085-BMP180/BMP085.h"
#include "../libraries/L3G4200D/L3G4200D.h"
#include "../libraries/ADXL345/ADXL345.h"
#include "../libraries/KalmanFilter/KalmanFilter.h"
#include <Servo.h>

ADXL345 accelerometer;
BMP085 bmp;
L3G4200D gyroscope;


float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;


float accPitch = 0;
float accRoll = 0;

float kalPitch = 0;
float kalRoll = 0;


KalmanFilter kalmanX(0.001, 0.003, 0.03);
KalmanFilter kalmanY(0.001, 0.003, 0.03);


//double referencePressure;

//Servoo
Servo myservo;  // create servo object to control a servo

////
#define Magnetometer_mX0 0x03  
#define Magnetometer_mX1 0x04  
#define Magnetometer_mZ0 0x05  
#define Magnetometer_mZ1 0x06  
#define Magnetometer_mY0 0x07  
#define Magnetometer_mY1 0x08

int mX0, mX1, mX_out;
int mY0, mY1, mY_out;
int mZ0, mZ1, mZ_out;
float heading, headingDegrees, headingFiltered, declination;
float Xm,Ym,Zm;
#define Magnetometer 0x1E //I2C 7bit address of HMC5883</p><p>




void setup() 
{
  Serial.begin(19200);
  Wire.begin();
  Wire.beginTransmission(Magnetometer); 
  Wire.write(0x02); // Select mode register
  Wire.write(0x00); // Continuous measurement mode
  Wire.endTransmission();
  // Initialize BMP085 or BMP180 sensor
  //Serial.println("Initialize BMP085/BMP180 Serial");

  // Ultra high resolution: BMP085_ULTRA_HIGH_RES
  // (default) High resolution: BMP085_HIGH_RES
  // Standard: BMP085_STANDARD
  // Ultra low power: BMP085_ULTRA_LOW_POWER
  while(!bmp.begin(BMP085_STANDARD))
  {
    Serial.println("Could not find a valid BMP085 or BMP180 sensor, check wiring!");
    delay(500);
  }
   while(!gyroscope.begin(L3G4200D_SCALE_2000DPS, L3G4200D_DATARATE_400HZ_50))
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    delay(500);
  }

  // Enable or disable SOSS (Software oversampling)- Use with BMP085_ULTRA_HIGH_RES !
  // For applications where a low noise level is critical, averaging is recommended if the lower bandwidth is acceptable
  // Conversion time pressure: 76.5ms, RMS noise 0.02 hPA / 0.17 m
  // bmp.setSoftwareOversampling(0);

  // Get reference pressure for relative altitude
  //referencePressure = bmp.readPressure();

  //Check settings
  //checkSettings();

    while(!accelerometer.begin())
  {
    delay(500);
  }
 
  accelerometer.setRange(ADXL345_RANGE_2G);

  myservo.attach(9); 
  myservo.write(0);
}


void loop()
{

  Vector acc = accelerometer.readNormalize();
  Vector gyr = gyroscope.readNormalize();

  // Calculate Pitch, Roll and Yaw
 pitch = pitch + acc.YAxis * timeStep;
 roll = roll + acc.XAxis * timeStep;
 yaw = yaw + gyr.ZAxis * timeStep;

  // Calculate Pitch & Roll from accelerometer (deg)
  accPitch = -(atan2(acc.XAxis, sqrt(acc.YAxis*acc.YAxis + acc.ZAxis*acc.ZAxis))*180.0)/M_PI;
  accRoll  = (atan2(acc.YAxis, acc.ZAxis)*180.0)/M_PI;

  // Kalman filter
  kalPitch = kalmanY.update(accPitch, gyr.YAxis);
  kalRoll = kalmanX.update(accRoll, gyr.XAxis);

  
  // Read raw values
  //int rawTemp = bmp.readRawTemperature();
  //uint32_t rawPressure = bmp.readRawPressure();

  // Read true temperature & Pressure


//---- X-Axis
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mX1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mX0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mX0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mX1 = Wire.read();
  }//---- Y-Axis
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mY1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mY0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mY0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mY1 = Wire.read();
  }
  
  //---- Z-Axis
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mZ1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mZ0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mZ0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mZ1 = Wire.read();
  }
  
  //---- X-Axis
  mX1=mX1<<8;
  mX_out =mX0+mX1; // Raw data
  // From the datasheet: 0.92 mG/digit
  Xm = mX_out*0.00092; // Gauss unit
  //* Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.</p><p>  //---- Y-Axis
  mY1=mY1<<8;
  mY_out =mY0+mY1;
  Ym = mY_out*0.00092; //---- Z-Axis
  mZ1=mZ1<<8;
  mZ_out =mZ0+mZ1;
  Zm = mZ_out*0.00092;
  // ==============================
  //Calculating Heading
  heading = atan2(Ym, Xm);
 
  // Correcting the heading with the declination angle depending on your location
  // You can find your declination angle at: http://www.ngdc.noaa.gov/geomag-web/
  // At my location it's 4.2 degrees => 0.073 rad
  declination = 0.0781; 
  heading += declination;
  
  // Correcting when signs are reveresed
  if(heading <0) heading += 2*PI;  // Correcting due to the addition of the declination angle
  if(heading > 2*PI)heading -= 2*PI;  headingDegrees = heading * 180/PI; // The heading in Degrees unit</p><p>  // Smoothing the output angle / Low pass filter 
  headingFiltered = headingFiltered*0.85 + headingDegrees*0.15;  //Sending the heading value through the Serial Port to Processing IDE
  
  float realTemperature = bmp.readTemperature();
  float realPressure = bmp.readPressure();

  // Calculate altitude
  float absoluteAltitude = bmp.getAltitude(realPressure);
  //float relativeAltitude = bmp.getAltitude(realPressure, referencePressure);
  float tempF= realTemperature*1.8 + 32;

  if(tempF>87){
     myservo.write(180);
  }
  
  Serial.print(tempF);
  Serial.print(",");
  Serial.print(realPressure);
  Serial.print(",");
  Serial.print(absoluteAltitude);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.print(roll);
  Serial.print(",");
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(headingFiltered);
  Serial.print(",");
  Serial.print(kalPitch);
  Serial.print(",");
  Serial.print(kalRoll);
  Serial.print(",");
  Serial.print(acc.XAxis);
  Serial.print(",");
  Serial.print(acc.YAxis);
  Serial.print(",");
  Serial.print(acc.ZAxis);
  Serial.print(",");
  Serial.print(gyr.XAxis);
  Serial.print(",");
  Serial.print(gyr.YAxis);
  Serial.print(",");
  Serial.print(gyr.ZAxis);
  Serial.println("");

  delay(40);
}
