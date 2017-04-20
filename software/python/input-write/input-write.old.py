#!/bin/bash

from time import sleep
import numpy
import os

sequence = numpy.genfromtxt('input.csv',
                            delimiter=',',
                            dtype=[('v',float),('q',float),('f',float)])

def arduino_writeseq(device, writelist, delay):
    for item in writelist:
        #os.write(device, item.encode('utf-8'))
        os.write(device, 'v,{}\n'.format(item[0]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'f,{}\n'.format(item[2]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'q,{}\n'.format(item[1]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'v,{}\n'.format(item[0]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'f,{}\n'.format(item[2]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'q,{}\n'.format(item[1]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'v,{}\n'.format(item[0]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'f,{}\n'.format(item[2]).encode('utf-8'))
        sleep(0.1)
        os.write(device, 'q,{}\n'.format(item[1]).encode('utf-8'))
        sleep(0.1)

        print(item)
        sleep(delay-0.9)

ard = os.open("/dev/arduino",os.O_RDWR)

arduino_writeseq(ard,sequence,1.0)
