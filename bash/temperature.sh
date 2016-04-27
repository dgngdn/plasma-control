#!/bin/bash

stty -F /dev/ttyACM0 57600 raw && cat /dev/ttyACM0 > ~/repos/bcduino/data/2016-04-26/temperature/temperature_$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
