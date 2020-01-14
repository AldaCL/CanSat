#Test of performance of BMP280 vs BMP180 vs -Some other Sensors-
#Also Include here other sensor benchmark/comparsion.
#Only take values from serial ports, calculate height, pressure and temp. 

import serial


ser = serial.Serial('/dev/ttyACM0', 19200, timeout=1)  #Linux
#ser = serial.Serial('COM4', 19200, timeout=1) #Windows

dataFrameSize = 0  #Define number of elements to receive by Serial Port, this help to discard invalid/wrong values received, and to split values for his processing.

def readData():
    dataFrame = ser.readline()   #String/frame of data readed.
    dataArray = dataFrame.split(",")      #Splitting data in to an array by the "," between each value

    if len(dataArray) == dataFrameSize:
        print (dataArray)
