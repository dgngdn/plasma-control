#!/bin/bash

from time import sleep
import numpy
import os

repeatsend = 3
sequence = numpy.genfromtxt('input.csv',
                            delimiter=',',
                            dtype=[('v',float),('f',float),('q',float)])

def arduino_writeseq(device, writelist, delay):
    for item in writelist:
        setv = item['v']
        setf = item['f']
        setq = item['q']

        #os.write(device, item.encode('utf-8'))
        for i in range(0,repeatsend):
            os.write(device, 'v,{}\n'.format(setv).encode('utf-8'))
            sleep(0.1)
            os.write(device, 'f,{}\n'.format(setf).encode('utf-8'))
            sleep(0.1)
            os.write(device, 'q,{}\n'.format(setq).encode('utf-8'))
            sleep(0.1)

        #print(item)
        sleep(delay-0.3*repeatsend)

ard = os.open("/dev/arduino",os.O_RDWR)

arduino_writeseq(ard,sequence,1.0)
