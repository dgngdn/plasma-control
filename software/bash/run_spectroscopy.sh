#!/bin/bash

oopath="/home/brandon/software/oceanoptics/seabreeze-3.0.11/SeaBreeze"
export LD_LIBRARY_PATH=${oopath}/lib
export CWD=$(pwd)

function getspectrum {
    ${oopath}/sample-code/c/seabreeze-util --get-spectrum --integration-time-us 200000
}

function gettimestamp {
    date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/'
}

while true; do
    getspectrum > "$(gettimestamp)"
    #getspectrum > "$(pwd)/../in/spectra/$(gettimestamp)"
    sleep 1
done

#while true; do
#  ${oopath}/sample-code/c/seabreeze-util --get-spectrum --integration-time-us 1000000 > $(pwd)/spectrum_$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
#  sleep 1
#done
