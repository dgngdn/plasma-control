#!/bin/bash

ERROR=$(killall "thermograph" "spectra.sh" "oscilloscope.sh" "python scope.py" "temperature.sh" "cat" 2>&1)
if [ "$ERROR" = "" ]; then
    echo "Processes successfully killed!"
else
    echo "$ERROR"
fi
