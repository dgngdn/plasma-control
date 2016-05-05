#!/bin/bash

function gettemp {
	stty -F /dev/ttyACM0 57600 raw && sleep 1 && cat /dev/ttyACM0 >> temperaturehistory
        #$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
}

until gettemp; do
	echo "Temperature logger crashed with exit code $?.  Respawning.." >&2
	sleep 1
done
