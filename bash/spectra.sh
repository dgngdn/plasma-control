#!/bin/bash

cd ~/oceanoptics/seabreeze-3.0.11/SeaBreeze/
export LD_LIBRARY_PATH="$PWD/lib"
cd ~/oceanoptics/seabreeze-3.0.11/SeaBreeze/sample-code/c

while true; do
	./seabreeze-util --get-spectrum --integration-time-us 150000 > ~/repos/bcduino/data/2016-04-26/spectra/spectrum_$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
	sleep 1
done
