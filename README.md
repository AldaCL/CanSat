# CanSat 🛰
## Ground Station System, CanSat On board Telemetry system with GY-80, Arduino and Xbee

## Table of Contents
1. [Ground Station](#gstation)
2. [On board telemetry system](#telemetry)
    * [BOM](#bom)
    * [Working Principle](#workingPrinciple)
    * [Wiring Diagram](#wiringDiagram)
3. [Mission results](#results)
4. [Installation and set-up](#install)
    * [Satellite](#satelliteinstall)
    * [Ground Station](#stationinstall)
5. [Task List](#task)




### Ground Station:  <a name="gstation"></a>



The system was developed pn python 2.7, implements **OpenGL** , **pygame** , **MatplotLib** and [Drawnow](https://github.com/stsievert/python-drawnow )

Firstly, the program reads 20 values from the sensor in order to calibrate it. 

![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/calibration.png)

The data sent by the sensor (Arduino / Xbee), make up an array of 15 values for different sensed magnitudes, in order, these are:
* Temperature (In Fahrenheit degrees)
* Pressure (Raw pressure, its converted to Pa)
* Altitude (Absolute OSL , with calibration we get height)
* Pitch, Roll, Yaw (Absolute magnitudes, its converted to degrees over calibration position)
* Heading Filtered 
* Kalman Pitch, Kalman Roll; Get after apply kalman filter. 
* Acceleration (X, Y ,Z), Got from Accelerometer
* Gyro (X, Y , Z), Got from Gyroscope. 

Feel free to ignore some magnitudes and use only those that are useful for your purposes< you only need to comment the lines on Arduino code and in python code; also change the size of array to receive. 

Once the sensor was calibrated, it start to receive the measurements, at a 19200 baud rate; (Also can be modified, in orden to adjust to your needs, but once again remember to change this value in Arduino code)
![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/ejec1.png)

A few moments after, the Matplotlib window come up, and shows the plot of data (We only take Temp, pressure, Altitude and height)

![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/can2.png)
![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/can3.png)

And the 3D model of CanSat.

![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/can1.png)
![alt text](https://github.com/AldaCL/CanSat/blob/master/Screenshots/can4.png)


We use the values of Pitch, roll and yaw to emulate the movement of the satellite; Axis were changed from original ones; because of the position of sensor into the CanSat. 


Arduino Script take libraries from [Korneliusz Jarzębski](https://github.com/jarzebski) repositories, just added to libraries dir to facilitate integration.

On ground, Xbee is connected to a Arduino UNO to provide Serial communication with the Laptop USB port. 
a Xbee Shield can be utilized, but due to the ease of getting an Arduino UNO, it was implemented with this. 

### On board telemetry system: <a name="telemetry"></a>

### BOM <a name="bom"></a>

The on telemetry system on satellite is conformed by:
- Arduino nano
- GY-80 Sensor
- Xbee S2C pro RF Module (working at 900 MHz)
- 9v battery 
- Mini 360 MP2307 (Step down converter)

### Working principle <a name="workingPrinciple"></a>

Arduino code reads values from sensor via I2C, then send it to XBee  Module connected. 

Xbee Module send data via RF to Xbee module in ground. (I strongly recommend research about Xbee protocol, configuration ,operation frequency, antenna gain and path losses effects because it define the maximum length of your communications system). 

Our configuration could reach a maximum distance of 600 m with LOS. At this distance any object in the path can cause loss of communication.

### Wiring diagram <a name="wiringDiagram"></a>

![alt text]()

### Mission results: <a name="results"></a>


### Installation : <a name="install"></a>

#### Cansat code and implementation  <a name="satelliteinstall"></a>

Open and compile **S2.ino** file with Arduino IDE, then upload it to Arduino NANO that will be placed inside satellite. Check wiring to sensors and Xbee.... (To be completed)

#### Ground Station environment and code  <a name="stationinstall"></a>

1. Clone this repo to your local dir, use `git clone https://github.com/AldaCL/CanSat.git` in terminal to clone via https (if you prefer, use ssh key and change the link.)

2. To get an isolated environment (And don't broke your python libraries system or generate conflicts with versions), I strongly recommend use venv to install dependencies and libraries, just follow: 
    1. Python 2.7 is needed, so check that you're using the correct **pip** in the case you have 2 versions of python (can be `pip`, `pip2`or `pip3` it depends on your python installation). I'll asume that `pip` corresponds to your pip version for python 2.7 
    2. `pip install virtualenv` 
    3. `python -m virtualenv cansatVenv`
    4. `source cansatVenv/bin/activate`
    5. `pip install -r requirements.txt`, Could be possible that you need to run this commands as SU.
    6. To got out of virtual environment just run `deactivate`, and to get in again run `source cansatVenv/bin/activate`

3. Now you can open **ultramasterv2.py** file, and edit file 13/14 to define the serial port in your system on which Arduino UNO is mounted. *dev/ttyACM0 for example*

4. Run `python ultramsterv2.py` and see the magic.  :rocket: 


### Task List  <a name="task"></a>

- [x] Telemetry implementation 
- [ ] BMP280 Sensor test for temperature, pressure and humidity
- [ ] 3D model optimization
- [ ] Mount circuits on PCB
- [ ] Landing system
 
