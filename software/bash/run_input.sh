#!/bin/bash

function write_inputs {
    python input-write.py
    sleep 5
}

write_inputs

#until gettrace; do
#        echo "Oscilloscope logger crashed with exit code $?.  Respawning.." >&2
#        sleep 5
#done
