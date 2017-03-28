#!/bin/env python

"""Collects and saves data from the Ocean Optics spectrometer
"""

from __future__ import division
import subprocess
import os
import numpy as np
import matplotlib

integrate_micros = 200000
PEAKABSMAX = 65535
PEAKMAX = PEAKABSMAX / 4
PEAKMIN = PEAKABSMAX / 10

cwd = os.getcwd()

LIBPATH = os.path.join(cwd,"lib")
OOLIB = os.path.join(CODEPATH,"oceanoptics")

def set_integrate(itime,peak):
  if peak > PEAKMAX:
    return int(itime * min(0.5,PEAKMAX/peak))
  elif peak < PEAKMIN:
    return int(itime * 1.1)
  else:
    return int(itime)

def print_stats(integrate_micros, peak):
  print("integration time: {} us; maxpeak: {}".format(integrate_micros,peak))

while True:
  p = subprocess.Popen([os.path.join(cwd,"spectroscopy-util"),"--get-spectrum","--integration-time-us",
                                        str(integrate_micros)],cwd=cwd,env={"LD_LIBRARY_PATH": LIBPATH},
                                        stdout=subprocess.PIPE)
  p.wait()
  data,err = p.communicate()
  spectrum = np.array([e.split(',') for e in data.splitlines()],dtype=float)
  peak = np.max(spectrum[10:,1])
  print_stats(integrate_micros,peak)
  integrate_micros = set_integrate(integrate_micros,peak)
