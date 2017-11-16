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
sys.path.append('/home/brandon/repos/python-seabreeze')
import seabreeze.spectrometers as sb
import asyncio
import async_measuere_try

crc8 = crcmod.predefined.mkCrcFun('crc-8-maxim')

##initialize oscilloscope
instr = usbtmc.Instrument(0x1ab1, 0x04ce)
instr.open()
while not (instr.timeout == 1 and instr.rigol_quirk == False):
    instr.timeout = 1
    instr.rigol_quirk = False
idg = ''
while not idg:
    try:
        idg = instr.ask("*IDN?")
    except Exception as e: # USBErrors
         print("{} in get_oscilloscope".format(e))
         time.sleep(0.4)
print("device info: {}".format(idg))
print("device timeout: {}".format(instr.timeout))
time.sleep(0.5)

## initialize spectrometer
devices = sb.list_devices()
#t_int=12000
t_int=12000*4
print("Available devices {}".format(devices))
spec = sb.Spectrometer(devices[0])
print("Using {}".format(devices[0])) 
spec.integration_time_micros(t_int)
print("integratiopn time {} seconds.".format(t_int/1e6))
time.sleep(0.5)


def get_runopts():
  """
  Gets the arguments provided to the interpreter at runtime
  """
  parser = argparse.ArgumentParser(description="runs MPC",
			  epilog="Example: python mpc_lin_test.py --quiet")
  #parser.add_argument("--quiet", help="silence the solver", action="store_true")
  parser.add_argument("--faket", help="use fake temperature data", action="store_true")
  parser.add_argument("--fakei", help="use fake intensity data", action="store_true")
  parser.add_argument("--timeout", type=float, help="timout (seconds) on oscilloscope operations",
                            default=0.4)
  runopts = parser.parse_args()
  return runopts

def gpio_setup():
  gpio.setmode(gpio.BOARD)
  gpio.setup(35, gpio.OUT)
  gpio.output(35, gpio.HIGH)





if __name__ == "__main__":
    t0=time.time()      
    runopts = get_runopts() 
    gpio_setup()
    f = serial.Serial('/dev/arduino', baudrate=38400,timeout=1)

    while True:
        [Vrms,Imax,Irms,P,Pp,Freq]=get_oscilloscope(instr)
        t1=time.time()
        print(P) 
      
        t1=time.time()-t0




