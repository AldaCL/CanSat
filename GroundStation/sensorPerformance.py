#Test of performance of BMP280 vs BMP180 vs -Some other Sensors-
#Also Include here other sensor benchmark/comparsion.
#Only take values from serial ports, calculate height, pressure and temp. 

import serial
import matplotlib.pyplot as plt 
from drawnow import *


#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)  #Linux
ser = serial.Serial('COM4', 9600, timeout=1) #Windows

dataFrameSize = 3  #Define number of elements to receive by Serial Port, this help to discard invalid/wrong values received, and to split values for his processing.
dataArrayClean = []

temps_array = []
heights_array = []
press_array = []


plt.ion() #To make interacive mode in plotting
fig = plt.figure()



def readData():

    dataFrame = ser.readline()   #String/frame of data readed.
    dataArray = dataFrame.split(",")   #Splitting data in to an array by the "," between each value
    #dataArrayClean.append(dataArray[0].replace)
    nOfDecPoints = []
    filterDigits = []
    aux = dataArray

    

    if len(dataArray) == dataFrameSize+1:
        for k in range (0,dataFrameSize):
            nOfDecPoints.append(aux[k].count('.'))
            aux[k].replace('.','')
            aux[k].replace('-','')
            filterDigits.append(aux[k].isdigit())

            
        #print (dataArray)
        temp = float(dataArray[0])
        height = float(dataArray[1])
        press = float(dataArray[2])
        
        temps_array.append(temp)
        heights_array.append(height)
        press_array.append(press)

        #print (temp,height,press_array)

        if len(heights_array) >100:
            heights_array.pop(0)
            temps_array.pop(0)
            press_array.pop(0)
        
    else :
        print("Not data received")

def makeFig():
    ax1 = fig.add_subplot(221)
    ax1.set_xlabel('time')
    ax1.set_ylabel('F')
    ax1.set_title('Temperatures')

    ax2 = fig.add_subplot(222)
    ax2.set_xlabel('time')
    ax2.set_ylabel('Pa')
    ax2.set_title('Pressures')

    ax3 = fig.add_subplot(212)
    ax3.set_xlabel('time')
    ax3.set_ylabel('m')
    ax3.set_title('Heights')



    ax1.plot(temps_array,'ro-',label="Farenheit")
    ax1.legend(loc="upper left")

    ax2.plot(press_array,'g^-',label="Pascals")
    ax2.legend(loc="upper right")

    ax3.plot(heights_array,'b.-',label="Meters over sea level")
    ax3.legend(loc='upper left')

def main():
    while 1:
        readData()
        drawnow(makeFig)

if __name__ == '__main__': main()
