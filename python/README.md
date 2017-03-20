## Python Scripts

### input-write.py

This Python3 script reads a .csv files and prints the lines, at a specified rate, to a serial devicefile.

In its current form:

+ writes the v, f, q messages separately with 100ms between them
+ takes an argument "repeatsend" and sends each line "repeatsend" times
+ the folder input-write/ contains the csv files that input-write.py will read

To do:

+ restructure the message format so that v,f,q are all set at once
+ add a check to verify that the desired settings have been applied
+ attach a CRC8 checksum and perform message verification on the microcontroller


### launch.ipynb

A hacked-together IPython Notebook that has served as the early graphical user interface
for this project.  This script calls the dozen other scripts that manage the startup and shutdown
of each piece of hardware.  It also prompts the user for experimental metadata, and provides simple
tools for changing the controller state, setting the control inputs, and managing (starting, archiving,
deleting) experiments.

The IPython notebook runs in the browser, but the user must SSH into the Raspberry Pi and forward
the appropriate local ports to access it.  Prematurely exiting the notebook without shutting down
the experiment can leave the system in an inconsistent state that must be corrected from the CLI.  
This notebook does not support realtime plotting, which was implemented instead using X-forwarding.

This IPython notebook is being supplanted by an asynchronous Python3 server that connects to a web
interface using the Websocket standard.  The web interface supports realtime plotting and system 
control with Javascript.


### run_oscilloscope.py

A simple Python script that manages data acquisition from the Rigol DS1074z oscilloscope.

Operation:
+ creates an oscilloscope Device object and a directory in which to save files.
+ requests data from the oscilloscope, one channel at a time
+ performs adjustments to convert the acquired data into appropriate units (which units?)
+ saves the data for all channels to a timestamped CSV file


### thermo.ipynb

An IPython notebook that converts the binary thermography files generated by the Lepton thermal camera
into nicely-colormapped PNG images.


### thermo.py

A Python script that converts the binary thermography files generated by the Lepton thermal camera
into a near-realtime thermal image.


### thermosight.py

A Python script that converts the binary thermography files generated by the Lepton thermal camera
into nicely-colormapped PNG images and saves them.


### thermo-work.py

A Python script that converts the binary thermography files generated by the Lepton thermal camera
into a plot.  Not 100% sure what this is for in relation to the other scripts.
