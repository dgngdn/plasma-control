#!/usr/bin/env python

import os
import numpy as np
import datetime
import time

device = os.open("/dev/usbtmc0",os.O_RDWR)
wait = 1
channels = 3

while True:
  data = []
  fbase = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
  
  try:  
    for channel in range(1,channels+1):
    
      command = ":WAVEFORM:SOURCE CHANNEL" + str(channel)
      os.write(device,command)
      os.write(device,":WAV:DATA?")
      rawdata = os.read(device,4000)
      cleandata = np.frombuffer(rawdata,'B')[11:-10:]
      
      os.write(device,"WAV:PREAMBLE?")
      preamble = os.read(device,4000).split(',')
      yincr = float(preamble[7])
      yorig = float(preamble[8])
      yref = float(preamble[9])
      
      if channel == 1:
        xincr = float(preamble[4])
        xorig = float(preamble[5])
        xref = float(preamble[6])
        
        x = xorig  
        xdata = []
        for measured in cleandata:
          xdata.append(x)
          x = x + xincr
        data.append(xdata)

      ### volts = (data[] - yorig - yref) * yincr
      data.append([(measured - yorig - yref) * yincr for measured in cleandata])
  
    #for channel in range(0,channels+1): #debug
    #  print(data[channel][:5]) #debug
  
    ### for saving all channels to the same csv file
    fname =  fbase
    dataset = []
    for point in range(0,len(data[0])):
      dataset.append([data[channel][point] for channel in range(0,channels+1)])
  
    np.savetxt(fname,dataset,delimiter=',')
  
    time.sleep(wait)
  except: time.sleep(wait);
