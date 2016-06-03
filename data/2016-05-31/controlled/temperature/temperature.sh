#!/bin/bash

function gettemp {
	stty -F /dev/arduino 57600 raw && sleep 1 && cat /dev/arduino >> temperaturehistory
	# still need to test this version for buffering problems...
	#unbuffer cat /dev/arduino | awk '{ "date +%Y-%m-%d_%T.%3N" | getline timestamp; close ("date +%Y-%m-%d_%T.%3N"); print timestamp,","$0; fflush(); }' >> temperaturehistory

	# this version still had buffering problems
	#unbuffer cat /dev/arduino | while read -r line; do echo "$(date '+%Y-%m-%d_%T.%3N'),$line" >> temperaturehistory; done

	#stty -F /dev/ttyACM0 57600 raw && sleep 1 && cat /dev/ttyACM0 >> temperaturehistory
        #$(date --iso-8601='s' | cut -d '-' -f 1-3 | sed 's/T0/_/')
}

until gettemp; do
	echo "Temperature logger crashed with exit code $?.  Respawning.." >&2
	sleep 1
done
