#!/usr/bin/env python

from OpenGL.GL import *
from OpenGL.GLU import *
import pygame
from pygame.locals import *
import math
import serial
import time
import matplotlib.pyplot as plt 
from drawnow import *

#ser = serial.Serial('/dev/tty.usbserial', 38400, timeout=1)
ser = serial.Serial('COM4', 19200, timeout=1)

ax = ay = az = 0.0
yaw_mode = False


toff=[]
poff=[]
to=0
gg=0
po=0
oco=''
press=0
temp=0
alt=0
reductor=0
temps=[]
pressures=[]
alts=[]
altsCalculated=[]
plt.ion() #To make interacive mode in plotting
cnt=0
fig = plt.figure()

def makeFig(): #Create a function that plot temp
    #plt.ylim(24,32)
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




    #plt.title("Temperaturas")
    #plt.ylim(78,85)
    #plt.grid(True)
    #ax1.ylabel("Temperatura")



    ax1.plot(temps,'ro-',label="Farenheit")
    ax1.legend(loc="upper left")

    ax2.plot(pressures,'g^-',label="Pascals")
    ax2.legend(loc="upper right")

    ax3.plot(alts,'b.-',label="Meters over sea level")
    ax3.legend(loc='upper left')

    plt2=plt.twinx()
    plt2.plot(altsCalculated,'c^-',label="meters relative")
    plt.ylim(bottom=0)
    plt2.legend(loc="upper right")


    #plt.show()

def calibrate():
    global poff
    global toff
    global to
    global po
    #toff=0
    #poff=0
    print 'Put satellite on ground for calibration'

    for i in range(1,21):
        while (ser.inWaiting() ==0):  #Runs only if there is data
            pass
        receivedString = ser.readline() #Read serial line from port
        dataAray = receivedString.split(',') #Separate data by comita
        if (len(dataAray) == 15):
            confirmacion0= dataAray[0].count('.')
            confirmacion1= dataAray[1].count('.')
            if(confirmacion0 and confirmacion1 == 1 ):
                temp= float (dataAray[0])
                pressu = float (dataAray[1])
                print 'Pressure for calibration',pressu ,'Pa'
                print 'temp for calibration', temp ,'F'
                poff.append(pressu)
                toff.append(temp)
                print i
            time.sleep(0.2)
    po=sum(poff)/len(poff)

    to=((sum(toff)/len(poff))-32)/1.8 + 273.15 #In K
    print 'Calibration done!' , po,' Pa , ',to,' K' 
    print 'Starting transmision'
    return po,to



def resize(width, height):
    if height==0:
        height=1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def drawText(position, textString):     
    font = pygame.font.SysFont ("Courier", 18, True)
    textSurface = font.render(textString, True, (255,255,255,255), (0,0,0,255))     
    textData = pygame.image.tostring(textSurface, "RGBA", True)     
    glRasterPos3d(*position)     
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

def draw():
    global rquad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
    r = 5
    h = 15
    n = float(20)

    circle_pts = []
    for i in range(int(n) + 1):
        angle = 2 * math.pi * (i/n)
        x = r * math.cos(angle)
        y = r * math.sin(angle)
        pt = (x, y)
        circle_pts.append(pt)

    glLoadIdentity()
    glTranslatef(0,0.0,-40.0)

    osd_text = "pitch: " + str("{0:.2f}".format(ay)) + ", roll: " + str("{0:.2f}".format(ax))

    if yaw_mode:
        osd_line = osd_text + ", yaw: " + str("{0:.2f}".format(az))
    else:
        osd_line = osd_text

    drawText((-7,-12, 4), osd_line)

    # the way I'm holding the IMU board, X and Y axis are switched 
    # with respect to the OpenGL coordinate system
    if yaw_mode:                             # experimental
        glRotatef(az, 0.0, 1.0, 0.0)  # Yaw,   rotate around y-axis
    else:
        glRotatef(0.0, 0.0, 1.0, 0.0)
    glRotatef(ay ,1.0,0.0,0.0)        # Pitch, rotate around x-axis
    glRotatef(-1*ax ,0.0,0.0,1.0)     # Roll,  rotate around z-axis


    

    glBegin(GL_TRIANGLE_FAN)#drawing the back circle
    glColor(1, 0, 0)
    glVertex(0, h/2.0, 0)
    for (x, z) in circle_pts:
        y = h/2.0
        glVertex(x, y, z)
    glEnd()

    glBegin(GL_TRIANGLE_FAN)#drawing the front circle
    glColor(0, 0, 1)
    glVertex(0, h/2.0, 0)
    for (x, z) in circle_pts:
        y = -h/2.0
        glVertex(x, y, z)
    glEnd()

    glBegin(GL_TRIANGLE_STRIP)#draw the tube
    glColor(0, 1, 0)
    for (x, z) in circle_pts:
        y = h/2.0
        glVertex(x, y, z)
        glVertex(x, -y, z)
    glEnd()
         
def read_data():
    global ax, ay, az
    global press
    global temp
    global alt
    global oco
    global gg
    aux=[]
    letras=[]
    letras1=[]
    proves=[]
    ax = ay = az = 0.0
    line_done = 0

    # request data by sending a dot
    #ser.write(b".") #* encode string to bytes
    #while not line_done:
    line = ser.readline() 
    angles = line.split(",")
    
    print angles
    if len(angles) == 15:    
    #     ax = float(angles[3])
    #     ay = float(angles[4])
    #     #az = float(angles[2])
    #     print angles
        for k in range (0,6):
            aux.append(angles[k].count('.'))
            letras.append(angles[k].replace('.',''))
            letras1.append(letras[k].replace('-',''))
            proves.append(letras1[k].isdigit())
        #print aux
        #print proves

        if( (all(prueba == 1 for prueba in aux)) and all(h == True for h in proves) ):
            f = open('fulldata.txt','a+') 
            f.write(line)
            f.close()
            r = open('fulldata1.txt','w') 
            r.write(line)
            r.close()



            temp = float(angles[0])
            #temps.append(temp)
            press= float(angles[1])
            alt= float (angles[2])
            #pressures.append(press)
            ax = float(angles[3])
            ay = float(angles[4])
            az = float(angles[5])
            #gg= float(angles[6])
            oco = angles[6].strip()
            oco= oco.replace('.','')
            #print oco
            #print oco.isdigit()

            if (oco.isdigit()==True):
                print oco
                g = open ('compas\Arduino_Compass\compas.txt','w')
                g.write(angles[6])
                g.close()
    return press,temp,alt
        
    #print angles

def main():
    global yaw_mode
    global cnt
    global reductor
    reductor=1+reductor

    video_flags = OPENGL|DOUBLEBUF
    calibrate()
    pygame.init()
    screen = pygame.display.set_mode((640,480), video_flags)
    pygame.display.set_caption("Press Esc to quit, z toggles yaw mode")
    resize(640,480)
    init()
    frames = 0
    ticks = pygame.time.get_ticks()

    #header = "Temperature,Pressure,Altitude,Roll,Pitch,Yaw"

    # with open('data.txt', 'a') as f:
    #     f.write(header + "\n")
    #     f.close

    while 1:
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            pygame.quit()  #* quit pygame properly
            break       
        if event.type == KEYDOWN and event.key == K_z:
            yaw_mode = not yaw_mode
            #ser.write(b"z")
        read_data()
        draw()
        pygame.display.flip()
        frames = frames+1

        if (frames>=30):
            temps.append(temp)
            pressures.append(press)
            alts.append(alt)
            altrelative=98.57*to*(math.log(po/press))
            altrelativeinmeters=altrelative*0.3048
            #print press
            altsCalculated.append(altrelativeinmeters)
            frames=0
            drawnow(makeFig)
            cnt=cnt+1
            #printdatata=[temp,press,alt,altrelativeinmeters]
            #','.join(printdatata)

            #h=open('printdata.txt','w')

            #h.write(printdatata)
            #h.close()

            if (cnt>100):
                temps.pop(0)
                pressures.pop(0)
            if (cnt>100):
                altsCalculated.pop(0)
                alts.pop(0)
            
        


    print ("fps:  %d" % ((frames*1000)/(pygame.time.get_ticks()-ticks)))
    ser.close()
    #estvel= 
    #print: 'Estimated vel:'

if __name__ == '__main__': main()

