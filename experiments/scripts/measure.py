import os
import datetime
import time
import numpy as NP
import cv2
import argparse
from pylepton import Lepton
import RPi.GPIO as gpio
import subprocess
import select


if __name__ == "__main__":
  gpio.setmode(gpio.BOARD)
  gpio.setup(35, gpio.OUT)
  gpio.output(35, gpio.HIGH)
  f = subprocess.Popen(['tail','-F','./data/temperaturehistory'],\
  stdout=subprocess.PIPE,stderr=subprocess.PIPE)
  p = select.poll()
  p.register(f.stdout)
  Ts=0;
  
  k=0
  delay=5
  target=open("measure","a+")
  run = True
  while run:
    try:
      with Lepton("/dev/spidev0.1") as l:
        data,_ = l.capture(retry_limit = 3)
      if l is not None:
        for line in data:
          l = len(line)
          if (l != 80):
            print("error: should be 80 columns, but we got {}".format(l))
        curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        fname = "{}".format(curtime)
        Ts=NP.true_divide(NP.amax(data[7:50]),100)-273;
        
		
    except:
      print("an error occurred. hardware restart...")
      gpio.output(35, gpio.HIGH)
      time.sleep(0.5)
      gpio.output(35, gpio.LOW)
      print("hardware restart completed")


    a=f.stdout.readline()
    ard=a.decode().split(',')
    Is=ard[6]
    dat=('%s, %s \n' %(Ts,Is))
    print(dat)
    target.write(dat)

        
