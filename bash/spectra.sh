#!/bin/bash

oopath=/home/brandon/oceanoptics/seabreeze-3.0.11/SeaBreeze/
export LD_LIBRARY_PATH=${oopath}lib
export CWD=$1
while true; do
  ${oopath}/sample-code/c/seabreeze-util --get-spectrum --integration-time-us 1000000 > $1/spectrum_$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
  sleep 1
done
