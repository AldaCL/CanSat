#Test of performance of BMP280 vs BMP180 vs -Some other Sensors-
#Also Include here other sensor benchmark/comparsion.
#Only take values from serial ports, calculate height, pressure and temp. 
from PyQt5 import QtWidgets, QtCore, uic
from pyqtgraph.Qt import QtGui, QtCore
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




class GroundStationWindow(object):
    def __init__(self):

        # pyqtgraph stuff
        pg.setConfigOptions(antialias=True)
        self.traces = dict()
        self.app = QtGui.QApplication(sys.argv)
        self.win = pg.GraphicsWindow(title='Ground station')
        self.win.setWindowTitle('Telemetry System')

        self.x = list(range(100))  # 100 time points
        self.y = [0.0 for _ in range(100)]  # 100 data points
        self.y2 = [0.0 for _ in range(100)]  # 100 data points

        #self.win.setGeometry(5, 115, 1910, 1070)

        #wf_xlabels = [(0, '0'), (2048, '2048'), (4096, '4096')]
        #wf_xaxis = pg.AxisItem(orientation='bottom')
        #wf_xaxis.setTicks([wf_xlabels])

        #wf_ylabels = [(0, '0'), (127, '128'), (255, '255')]
        #wf_yaxis = pg.AxisItem(orientation='left')
        #wf_yaxis.setTicks([wf_ylabels])

        # sp_xlabels = [
        #     (np.log10(10), '10'), (np.log10(100), '100'),
        #     (np.log10(1000), '1000'), (np.log10(22050), '22050')
        # ]
        # sp_xaxis = pg.AxisItem(orientation='bottom')
        # sp_xaxis.setTicks([sp_xlabels])

        self.waveform = self.win.addPlot(
            title='TEMPERATURE', row=1, col=1,
        )
        self.spectrum = self.win.addPlot(
            title='HEIGHT', row=2, col=1,
        )


    def start(self):
        if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
            QtGui.QApplication.instance().exec_()

    # def set_plotdata(self,data_x, data_y):
    #             self.traces[name] = self.waveform.plot(pen='c', width=3)
    #         if name == 'spectrum':
    #             self.traces[name] = self.spectrum.plot(pen='m', width=3)
               

    def update(self):
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
                global height
                global press
                temp = float(self.dataArray[0])
                height = float(self.dataArray[1])
                press = float(self.dataArray[2])
        else :
            print("Not data received")



        self.x = self.x[1:]  # Remove the first y element.
        self.x.append(self.x[-1] + 1)  # Add a new value 1 higher than the last.

        self.y.append(temp)  # Add a new random value.
        self.y = self.y[1:]  # Remove the first 
        
        self.y2.append(height)  # Add a new random value.
        self.y = self.y[1:]  # Remove the first 

        self.plot(self.x, self.y)  # Update the data.
        self.plot(self.x, self.y2)  # Update the data.

    
    def animation(self):
        timer = QtCore.QTimer()
        timer.timeout.connect(self.update)
        timer.start(20)
        self.start()

if __name__ == '__main__':

    ground_station_app = GroundStationWindow()
    ground_station_app.animation()