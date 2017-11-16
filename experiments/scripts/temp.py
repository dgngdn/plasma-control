import sys
sys.dont_write_bytecode = True

import os
import datetime
import time
import numpy as NP
import cv2
import argparse
from pylepton import Lepton
import RPi.GPIO as gpio
gpio.setwarnings(False)
import subprocess
import select
import scipy.io as scio
from scipy import linalg
from casadi import *
# Import core code.
import core
import serial
#import asyncio
#import serial_asyncio
import crcmod
#import crcmod.predefinmaed
import usbtmc
import visa
import asyncio
import matplotlib.pyplot as plt

def get_temp_a():
  """
  Gets treatment temperature with the Lepton thermal camera
  """
  run = True
  while run:
    try:
      with Lepton("/dev/spidev0.1") as l:
        data,_ = l.capture(retry_limit = 3)
      if l is not None:
        ##print(data[10:80]);
        Ts = NP.amax(data[6:60,:,0]) / 100 - 273;
        Tt = NP.amax(data[0:5,:,0]) / 100 - 273;
        mm= NP.where( data == NP.amax(data) )

        Ts_lin=data[int(mm[0]),:,0] /100 - 273
        Ts2 = (Ts_lin[int(mm[1])+2]+Ts_lin[int(mm[1])-2])/2
        Ts3 = (Ts_lin[int(mm[1])+6]+Ts_lin[int(mm[1])-6])/2
        for line in data:
          l = len(line)
          if (l != 80):
            print("error: should be 80 columns, but we got {}".format(l))
          elif Ts > 150:
            print("Measured temperature is too high: {}".format(Ts))
        #curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        #fname = "{}".format(curtime)
        #Ts = NP.amax(data) / 100 - 273;
        #Ts = NP.true_divide(NP.amax(data[7:50]),100)-273;
        time.sleep(0.050)
        run = False
    except:
      print("\nHardware error on the thermal camera. Lepton restarting...")
      gpio.output(35, gpio.HIGH)
      time.sleep(0.5)
      gpio.output(35, gpio.LOW)
      print("Lepton restart completed!\n\n")
  
  #print(Ts)
  return [Ts, Ts2, Ts3, Ts_lin, Tt, data]

[Ts, Ts2, Ts3, Ts_lin, Tt, data]=get_temp_a()
datt=[]
print(data.flatten().reshape(60,80))
print('surf')
print(Ts)
print(Ts2)
print(Ts3)
print('tube')
print(Tt)
x=linspace(6,60,60-6)
y=linspace(1,80,80)
X,Y = np.meshgrid(y,x)
plt.contourf(X,Y,data[6:60,:,0])
plt.plot(y,Ts_lin)
plt.show()
