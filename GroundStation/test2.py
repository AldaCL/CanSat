from PyQt5 import QtWidgets, QtCore, uic
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import sys  # We need sys so that we can pass argv to QApplication
import os
from random import randint

pw = pg.plot(xVals, yVals, pen='r')  # plot x vs y in red
pw.plot(xVals, yVals2, pen='b')

win = pg.GraphicsWindow()  # Automatically generates grids with multiple items
win.addPlot(data1, row=0, col=0)
win.addPlot(data2, row=0, col=1)
win.addPlot(data3, row=1, col=0, colspan=2)

pg.show(imageData)  # ima