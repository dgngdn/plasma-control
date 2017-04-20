#!/usr/bin/env python3

import serial
arduino = serial.Serial("/dev/arduino-mega")

def getline():
  gotline = False
  while not gotline:
    data = arduino.readline().decode('utf-8')
    if len(data.split(',')) == 4 and data[-1] == '\n':
      gotline = True
  print(data)

if __name__ == '__main__':
  getline()

