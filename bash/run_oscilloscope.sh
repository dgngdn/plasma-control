#!/bin/bash

function gettrace {
    python acquire_oscilloscope.py
    sleep 5
}

until gettrace; do
        echo "Oscilloscope logger crashed with exit code $?.  Respawning.." >&2
        sleep 5
done

#while true; do python scope2.py; sleep 5; done
