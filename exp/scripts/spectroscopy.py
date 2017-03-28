#!/bin/env python

"""Collects and saves data from the Ocean Optics spectrometer
"""

from __future__ import division
import subprocess
import os
import numpy as np
import matplotlib
import argparse
import datetime

integrate_micros = 200000
PEAKABSMAX = 65535
PEAKMAX = PEAKABSMAX / 4
PEAKMIN = PEAKABSMAX / 10

cwd = os.getcwd()
LIBPATH = os.path.join(cwd,"lib")
OOLIB = os.path.join(LIBPATH,"oceanoptics")

parser = argparse.ArgumentParser(description="collect data from Ocean Optics spectrophotometer")
parser.add_argument("--dir", type=str, default="data",
                        help="relative path to save the data")
parser.add_argument("--loop", help="repeatedly save oscilloscope data",
                        action="store_true")
opts = parser.parse_args()

SAVEDIR = os.path.join(os.getcwd(),opts.dir,"spectroscopy") # path to the directory to save files
if not os.path.exists(SAVEDIR):
    print("Creating directory: {}".format(SAVEDIR))
    os.makedirs(SAVEDIR)

def set_integrate(itime,peak):
  if peak > PEAKMAX:
    return int(itime * min(0.5,PEAKMAX/peak))
  elif peak < PEAKMIN:
    return int(itime * 1.1)
  else:
    return int(itime)

def print_stats(integrate_micros, peak):
  print("integration time: {} us; maxpeak: {}".format(integrate_micros,peak))

if __name__ == "__main__":
  run = True

  while run:
    p = subprocess.Popen([os.path.join(cwd,"spectroscopy-util"),"--get-spectrum","--integration-time-us",
                                          str(integrate_micros)],cwd=cwd,env={"LD_LIBRARY_PATH": OOLIB},
                                          stdout=subprocess.PIPE)
    p.wait()
    data,err = p.communicate()
    spectrum = np.array([e.split(',') for e in data.splitlines()],dtype=float)

    # save the spectrum to a csv file, timestamped with the integration time
    curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S_%f")
    fname = "{}-{}.csv".format(curtime,integrate_micros)
    np.savetxt(os.path.join(SAVEDIR,fname),spectrum,delimiter=',')

    # modify integration time as needed
    peak = np.max(spectrum[10:,1])
    print_stats(integrate_micros,peak)
    integrate_micros = set_integrate(integrate_micros,peak)
    run = opts.loop

