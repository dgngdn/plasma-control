#/usr/bin/env python

"""
Oscilloscope Interface

This script grabs data from the Rigol oscilloscope and saves to a file
"""

import numpy as np
from matplotlib import pyplot as plt
import datetime
import time
import os
import argparse
import usbtmc
import visa

TIMEOUT = 1.00 #0.35 without --align!
DATALENGTH = 15000
DATALENGTHPREAMBLE = 20
READWAIT = 0.15 # wait time between oscilloscope read and write, seconds
WAITREAD = 0.2 # wait time between oscope write() and read(), b/c ask() kills it
XUNIT = 's' # x-axis label
YUNIT = {1:'potential,volts',2:'potential,volts',3:'potential,volts',4:'potential,volts'} # y-units for each channel
C = 47.0e-9 #Farads, Lissajous Capacitance

def get_opts():
    parser = argparse.ArgumentParser(description="collects and logs data from the Rigol oscilloscope",
			    epilog="Example: python oscilloscope.py --chan 1 4 --loop")
    parser.add_argument("--chan", nargs='+', type=int, help="oscillocope channels to acquire",
                            action="store",dest="channels",required=True)
    parser.add_argument("--platform", default="usbtmc",
                            help="platform for connecting to oscilloscope")
    parser.add_argument("--plot", help="plot the acquired waveforms as they are collected",
                            action="store_true")
    parser.add_argument("--loop", help="continuously log data",
                            action="store_true")
    parser.add_argument("--align", help="guarantee all channels at same timepoint",
                            action="store_true")
    parser.add_argument("--timeout", type=float, help="timout (seconds) on oscilloscope operations",
                            default=0.4)
    parser.add_argument("--dir", type=str, default="data",
                            help="<optional> relative path to save the data")
    parser.add_argument("--onefile",
                            help="<optional> Put all the data in one file", action="store_true")
    parser.add_argument("--lissajous",
                            help="<optional> Calculate Lissajous Power", action="store_true")
    opts = parser.parse_args()
    print_opts(opts)
    return opts

def print_opts(opts):
    print("interfacing with: {}".format(opts.platform))
    print("acquiring channels: {}".format(opts.channels))
    if opts.plot:
        print("plotting waveforms")
    if opts.loop:
        print("continuously acquiring data")

def savedir_setup(directory):
    # path to the directory to save files
    savedir = os.path.join(os.getcwd(),directory,"oscilloscope")
    if not os.path.exists(savedir):
        print("Creating directory: {}".format(savedir))
        os.makedirs(savedir)
        print("Directory created!".format(savedir))
    return savedir

def PolyArea(x,y):
    return 0.5*np.abs(np.dot(x,np.roll(y,1))-np.dot(y,np.roll(x,1)))

def get_oscilloscope(opts):
    if opts.platform == 'visa':
        # create device manager object
        try:
            rm = visa.ResourceManager()
        except:
            rm = visa.ResourceManager('@py')
        # create instrument object
        # rm.list_resources()
        # chamber jet
        instr = rm.open_resource('USB0::0x1AB1::0x04CE::DS1ZA164457681::INSTR')
        # control jet
        #instr = rm.open_resource('USB0::0x1AB1::0x04CE::DS1ZA170603287::INSTR',
        #                            timeout=2000, chunk_size=102400)
        print("device info: {}".format(instr.query("*IDN?")))
        print("device timeout: {}".format(instr.timeout))
        print("device chunk size: {}".format(instr.chunk_size))
    else:
        instr = usbtmc.Instrument(0x1ab1, 0x04ce)
        instr.open()
        instr.rigol_quirk == True
        #while not (instr.timeout == opts.timeout and instr.rigol_quirk == False):
        while not (instr.timeout == opts.timeout):
            instr.timeout = opts.timeout
        #    instr.rigol_quirk = False
        id = ''
        while not id:
            try:
                id = instr.ask("*IDN?")
            except Exception as e: # USBError
                print("{} in get_oscilloscope".format(e))
                time.sleep(opts.timeout)
        print("device info: {}".format(id))
        print("device timeout: {}".format(instr.timeout))
    return instr

def prep_read(instr,platform,channel):
    """prepares the oscilloscope from reading a channel's data"""
    instr.write(":WAV:SOUR CHAN{}".format(channel))

def read_from_channel(instr,platform,preamble):
    """reads from specified oscilloscope channel;
       returns numpy array containing scaled (x,y) data"""
    """This is now set up as a recursive function!  If it errors, it just calls itself again until it works!"""
    ydata = []
    while len(ydata) < 1200:
        if platform == 'visa':
            ydata = instr.query_ascii_values(":WAV:DATA?",separator=wave_clean,container=np.array)
            xdata = generate_xdata(len(ydata),preamble)
            yscaled = ydata #wavscale(measured=ydata,pre=preamble)
            data = np.array(list(zip(xdata,yscaled)), dtype=[('x',float),('y',float)])
            return data

        else:
            rawdata = ''
            try:
                instr.write(":WAV:DATA?")
                time.sleep(WAITREAD)
                rawdata = instr.read()
                #print("Stuck on read_from_channel?")
                #print(len(rawdata))
            except Exception as e:
                #Tell you there's an error, then call this function again
                print("{} in read_from_channel".format(e))
                return read_from_channel(instr,platform,preamble)
            else:
                #If this function doesn't error, operate on the data and return it
                ydata = np.fromstring(rawdata[11:],dtype=float,sep=',')
                xdata = generate_xdata(len(ydata),preamble)
                yscaled = ydata #wavscale(measured=ydata,pre=preamble)
                data = np.array(list(zip(xdata,yscaled)), dtype=[('x',float),('y',float)])
                return data

def plot_data(data,ylabel,fname,savedir):
    print("plotting {}.png".format(fname))
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ax.plot(data['x'],data['y'],linestyle='none',marker='.',markersize=5)
    ax.set_title(fname)
    ax.set_ylabel(ylabel)
    ax.set_xlabel("time, {}".format(XUNIT))
    ax.grid(True)
    ax.get_xaxis().get_major_formatter().set_powerlimits((0, 0))
    fig.savefig(os.path.join(savedir,"{}.png".format(fname)),dpi=300)
    plt.show()
    plt.close(fig)

def save_data(data,fname,savedir):
    print("saving {}.csv".format(os.path.join(savedir,fname)))
    np.savetxt(os.path.join(savedir,"{}.csv".format(fname)),data,delimiter=',',fmt='%.5e')

def generate_xdata(points,pre):
    xincr = pre[4]
    xorig = pre[5]
    xref = pre[6]
    x = np.linspace(xorig,xincr*points,points)
    return x

#def scale_waveform(measured,pre):
#    yincr = pre[7]
#    yorig = pre[8]
#    yref = pre[9]
#    scaled = (measured - yorig - yref) * yincr
#    return measured

def preamble_clean(s):
    return filter(None, s.split('\n')[0].split(','))

def wave_clean(s):
    return filter(None, s[11:].split('\n')[0].split(','))

def instr_query(instrument, opts, msg):
    time.sleep(READWAIT)
    reply = ''
    while not reply:
        if opts.platform == "visa":
            reply = instrument.query(msg)
        else:
            try:
                reply = instrument.ask(msg)
            except Exception as e:
                print("{} in instr_query".format(e))
                time.sleep(opts.timeout)
    return reply

def instr_run(instrument, opts):
    setrun = False
    while not setrun:
        instrument.write(":RUN")
        status = instr_query(instrument, opts, ":TRIG:STAT?").strip()
        setrun = (status != "STOP")

def get_preambles(instrument, opts):
    """for each channel, grab the preamble containing the oscilloscope scaling information
    save to a dictionary for waveform scaling!"""
    preambles = {}
    for channel in opts.channels:
        instrument.write(":WAV:SOUR CHAN{}".format(channel))
        if (opts.platform == 'visa'):
            preamble = instrument.query_ascii_values(":WAV:PRE?",separator=preamble_clean)
        else:
            rawdata = ''
            while len(rawdata) < DATALENGTHPREAMBLE:
                #instr.write(":WAV:PRE?")
                try:
                    rawdata = instrument.ask(":WAV:PRE?")
                except:
                    print("{} in get_preambles".format(e))
                    time.sleep(READWAIT)
            preamble = np.fromstring(rawdata,dtype=float,sep=',')
        preambles[str(channel)] = preamble
    print(preambles['1'][4])
    print(type(preambles['1'][4]))
    return preambles

def get_waveforms(instrument, opts, preambles):
        print("acquiring waveforms!")
        savedir = savedir_setup(opts.dir)
        instrument.write(":STOP")
        curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        # read each channel and then save, plot (if desired)
#            if channel == 1:
#                instrument.timeout = opts.timeout * 1.5
#            else:
#                instrument.timeout = opts.timeout

        if opts.onefile: #saving all channels in one file
            channel_indexer = 0
            for channel in opts.channels:
                prep_read(instrument,opts.platform,channel)
                if channel_indexer == 0:#get first channel, keep both x (time) and y
                    data = ''
                    data = read_from_channel(instrument,opts.platform,preambles[str(channel)])
                    timedata = data['x']#split data into individual columns
                    ordinate = data['y']
                    data = np.column_stack((timedata,ordinate))#and now put them next to each other in a normal array that we can add more channels to
                    channel_indexer += 1
                else:#for other channels we just need to add the y data
                    temp_data = read_from_channel(instrument,opts.platform,preambles[str(channel)])#read the next channel
                    column_adder = temp_data['y'].reshape(len(temp_data['y']),1)#now data and column_adder are in same format
                    data = np.concatenate((data,column_adder),axis=1)#so we can can concatenate them and build the all-channel matrix
                print(channel)
            fname = "{}_allchan".format(curtime)
            save_data(data,fname,savedir)
            print("DONE.")

            if opts.lissajous:
                P_lis = lissajous(instrument,data,preambles)
                print(P_lis)

        else:#we're saving channels as separate files
            for channel in opts.channels:
                data = ''
                prep_read(instrument,opts.platform,channel)
                while len(data) == 0:
                    print("reading data...")
                    data = read_from_channel(instrument,opts.platform,preambles[str(channel)])
                    # if read_from_channel returns nothing, something went wrong
                    if len(data) == 0:
                        if opts.align:
                            print("ERROR, trashing reads...")
                            #instr_reset(instrument)
                            return
                        else:
                            # reset instrument
                            instr_reset(instrument)
                            # reset curtime, and then try again!
                            print("Resetting")
                            curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
                fname = "{}_chan{}".format(curtime,channel)
                if opts.plot:
                    ylabel = YUNIT[channel]
                    plot_data(data,ylabel,fname,savedir)
                save_data(data,fname,savedir)
                print("DONE.")



def instr_reset(instrument):
    instrument.write(":RUN")
    time.sleep(READWAIT)
    instrument.write(":STOP")

def lissajous(instrument,data,preambles):
    freq = float(instrument.ask(":MEAS:COUN:VAL?"))#Read the frequency from scope
    t_V = data[:,0]
    V = data[:,1]
    #I_in = data[:,2]
    Vcap = data[:,3]
    #I_out = Vcap - data[:,4]/56
    ts = preambles["1"][4]#Grabs time increment directly from the Channel 1 preamble
    ind = 1/freq/ts
    if ind < len(t_V):
        P_lis = C*freq*PolyArea(V[0:int(np.round(ind))-1],Vcap[0:int(np.round(ind))-1])
    else:
        P_lis = 0
    return P_lis

def capture_oscilloscope():
    opts = get_opts()
    instr = get_oscilloscope(opts)

    #wavscale = np.vectorize(scale_waveform, excluded=['pre'])
    instr.write(":WAV:MODE NORMAL")
    instr.write(":WAV:FORMAT ASCII")
    instr_run(instr, opts)
    run = True

    # we only get the preambles once, so don't mess with oscilloscope controls!
    preambles = get_preambles(instr, opts)

    #Our frequency will always come from Ch1, so just set this once
    instr.write("MEAS:COUN:SOUR CHAN1")
    time.sleep(1)#If we don't wait, the scope doesn't have time to finish and always thinks the frequency is 0

    # loop that grabs the waveform data
    while run:
        get_waveforms(instr,opts,preambles)
        instr_run(instr, opts)
        run = opts.loop

if __name__ == "__main__":
    capture_oscilloscope()
