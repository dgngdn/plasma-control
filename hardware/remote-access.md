


This command will connect to the pi3:
  ssh -X pi3 -L 8890:localhost:8888

-X    X-forwarding
-L    local port forwarding

Jupyter on pi3 port 8888
  -L 8890:localhost:8888
      forwards pi3:8888 to connect computer's port 8890

the Jupyter server is started in ~/control/python
  so this is 'home' as far as the Jupyter server is concerned
  
----

ASIDE

make a note about how X-forwarding works and what it can allow you to do....

----

ASIDE

'pi3' is an SSH alias that is set up in the SSH configuration folder:
    .ssh/config
    
viewing this configuration file with `cat .ssh/config`
    Host pi3
      Hostname pi3.dyn.berkeley.edu
      Port 22
      User brandon
----

ASIDE

to start Jupyter notebook on a machine, run `jupyter notebook &`

if you want to start Jupyter notebook server so it won't stop when you break the ssh connection:

  `nohup jupyter notebook &`

----

the launch program is in ~/control/python/launch.ipynb

after startup, clear previous output with: Kernel > Restart and Clear

to load the functions defined in this notebook, run the first input codeblock with shift-enter
  after the code block is run, a number will appear in brackets to the left of the code block indicating the block run number (for determine block run order); if it's the first block to be run, a [1] will appear.
  
to start the experiment, run the start_experiment() function.  It does not currently take any arguments.  From the instant that start_experiment is run, the pi3 is logging data.  You can verify this by navigating into the data folder and seeing that current.csv is growing:

  tail -f current.csv

To make sure that sources of data other than the Arduino serial connection are being logged, you can navigate into the experiment's folder, enter the 'in' folder (for incoming data), and verify that the folders contained therein are growing with:

  du -sh *

----
ASIDE

When run_experiment() is running, it will also create ~/control/data/current.csv, which is a symlinked to the file where the Arduino serial output is currently being saved.  Feel free to follow changes to this file with `tail -f ~/control/data/current.csv` or otherwise connect other programs to it—it won't interfere with data collection.  Direct reading of the Arduino serial file object (/dev/arduino or /dev/ttyACM0 or whatever) WILL interfere with data collection, so you shouldn't do that.

----

run_experiment will request a bunch of input from the user, including experimental metadata (title, description of variables, &etc).  Once the required fields have been entered, the user can take additional notes which will be timestamped and added to the metadata file that is saved in the experiment's directory.

When you are done logging timestamped notes on the experiment, you can end the metadata logging by entering 'done' in the input field.

If an experiment has progressed successfully, run stop_experiment() to shut it down and compress the data for future analysis.  stop_experiment() will shut down logging, compress the data and move it to ~control/archive/, and finally delete the uncompressed data in ~/control/data.  After providing you a bunch of information that you can use to verify successful completion of the run, it will offer to delete the uncompressed data; to accept, type the randomly-generated string when prompted.  It will inform that deletion was successful.

You can then retrieve the compressed folder from ~/control/archive/<timestamp>.  It is a gzipped tarball.

You can either move the archive off of the pi3 manually in-person, or you can transfer it to another machine using `scp`:

    scp ./2016-12-05_16\:57\:48.tar.gz D105:/mnt/storage/data/

Once transferred, the archive can be decompressed for analysis with the usual tools:

    tar -xvf  ./2016-12-05_16\:57\:48.tar.gz
    
If an experiment FAILS and you just want to throw it away, run fail() instead of stop_experiment().  It stops and deletes everything without saving anything.

----

Now, for running an experiment with predefined inputs:

Navigate to ~/control/python/input-write

The important files in here are input-write.py and input.csv.

input-write.py reads the contents of input.csv and writes them to the Arduino.  This includes some failsafe stuff for validation of the data, so that any corruption in the input write procedure doesn't result in crazy inputs running on the Arduino.

You can copy your input file to ~control/python/input-write manually in-person on the pi3 or remotely with `scp` (or ftp or whatever).  Right now, the input file lines are written one at a time, once per second, to the Arduino.  The inputs are comma-separated, and the order is specified here:

  sequence = numpy.genfromtxt('input.csv',
                            delimiter=',',
                            dtype=[('v',float),('q',float),('f',float)])

The units are currently peak-to-peak voltage in kilovolts, flowrate in slm, and frequency in kHz. If an input outside of the applicable range is specified, the Arduino will currently go to the maximum or minimum value supported by the current setup (this could be changed such that invalid inputs are ignored).

To run input-write.py, navigate into the input-write folder and run:

    python input-write.py
  
after writing each input to the Arduino, this program will also write the input to stdout.  If you kill this sequence or if the sequence runs the entire way through the file and completes, the Arduino will just keep doing whatever it was doing in the last set of inputs... forever.

----

And finally, for manual control of the plasma:

There are a bunch of preprogrammed commands in the Arduino firmware for manually controlling its runtime settings.  These commands are defined in the manual_input() function in the Arduino firmware:

p       pause     pauses the progression (defined in firmware)
r       resume    resume the progression (defined in firmware)
b       begin     restarts the progression (defined in firmware)
d,#     delay     sets the timeout interval
v,#     set vp;tage
f,#     set frequency
q,#     set flowrate
s,s,#   sets PID setpoint
s,p,#   sets PID tuning - p
s,i,#   sets PID tuning - i
s,d,#   sets PID tuning - d
s,m,x   sets PID measured variable to x (v,f,q)
o,x     sets PID controlled variable to x (???)
c       toggles PID controller

For instance, to set v,f,q to zero:

echo "v,0" > /dev/arduino 
echo "f,0" > /dev/arduino 
echo "q,0" > /dev/arduino 


