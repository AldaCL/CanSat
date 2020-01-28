#Test of performance of BMP280 vs BMP180 vs -Some other Sensors-
#Also Include here other sensor benchmark/comparsion.
#Only take values from serial ports, calculate height, pressure and temp. 
from PyQt5 import QtWidgets, QtCore, uic
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import sys  # We need sys so that we can pass argv to QApplication
import os
from random import randint


import serial
import matplotlib.pyplot as plt 
from drawnow import *



ser = serial.Serial('COM4', 9600, timeout=1) #Windows
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)  #Linux



class MainWindow(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.graphWidget = pg.PlotWidget()
        self.setCentralWidget(self.graphWidget)
        self.graphWidget.setTitle("Temperatures")
        self.graphWidget.setLabel('left', 'Temperature (C)', color='white', size=30)
        self.graphWidget.setLabel('bottom', 'time (s)', color='white', size=30)
        

        self.x = list(range(100))  # 100 time points
        self.y = [0.0 for _ in range(100)]  # 100 data points

        #self.graphWidget.setBackground('')

        pen = pg.mkPen(color="c")
        self.data_line =  self.graphWidget.plot(self.x, self.y, pen=pen)

         # ... init continued ...
        self.timer = QtCore.QTimer()
        self.timer.setInterval(25)
        self.timer.timeout.connect(self.update_plot_data)
        self.timer.start()




    def update_plot_data(self):
        
        self.dataFrameSize = 3  #Define number of elements to receive by Serial Port, this help to discard invalid/wrong values received, and to split values for his processing.
        self.dataArrayClean = []

        self.temps_array = []
        self.heights_array = []
        self.press_array = []

        self.dataFrame = ser.readline()   #String/frame of data readed.
        self.dataArray = self.dataFrame.split(",")   #Splitting data in to an array by the "," between each value
        #dataArrayClean.append(dataArray[0].replace)
        self.nOfDecPoints = []
        self.filterDigits = []
        self.aux = self.dataArray
        print self.dataArray

        if len(self.dataArray) == self.dataFrameSize+1:
            for k in range (0,self.dataFrameSize):
                self.nOfDecPoints.append(self.aux[k].count('.'))
                self.aux[k].replace('.','')
                self.aux[k].replace('-','')
                self.filterDigits.append(self.aux[k].isdigit())

                global temp 
                temp = float(self.dataArray[0])
                self.height = float(self.dataArray[1])
                self.press = float(self.dataArray[2])
        else :
            print("Not data received")



        self.x = self.x[1:]  # Remove the first y element.
        self.x.append(self.x[-1] + 1)  # Add a new value 1 higher than the last.

        self.y.append(temp)  # Add a new random value.
        self.y = self.y[1:]  # Remove the first 
        

        self.data_line.setData(self.x, self.y)  # Update the data.




def main():
        app = QtWidgets.QApplication(sys.argv)
        w = MainWindow()
        w.show()
        sys.exit(app.exec_())


if __name__ == '__main__': main()
