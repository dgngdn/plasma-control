import numpy
import matplotlib
matplotlib.use("qt4agg", force=True)
from matplotlib import pyplot
from time import sleep
import sys
import os
#from time import time

if os.path.isfile("/home/brandon/control/data/current.csv"):
  DATAFILE = "/home/brandon/control/data/current.csv"
  #DATAFILE = "../../current.csv"
else:
  DATAFILE = "./current.csv"

# timestamps
timestamps = []                    # empty array to hold x-data
COLUMN_TIMESTAMP = 1               # index of the timestamp column
SECONDS_PER_TIMESTAMP_VAL = 0.001  # 0.001 for timesteps in milliseconds
VIEW_WINDOW = 300                  # seconds of data to view at once
LOOP_DELAY = 0.01                  # seconds between main loop iterations
timepoint_delay = 0                # timestamp values between timepoints
file_position = 0                  # current position in the data file
window_rightbound = VIEW_WINDOW    # set the window's initial right bound
starttime = 0
SKIPROWS = 20  ## rows to skip between plotted points
SKIPHEAD = 20  ## rows to skip at the head of the file

# data
TOTAL_COLUMNS = 14                        # total number of columns expected per datapoint
COLUMN_YDATA = [[7,8,12],[6],[9,10,11]]   # index of the y-data columns
YSCALING = { 6: (1), 7: (1), 8:(1), 9: (100/10.), 10: (100/24.), 11: (100/2.5), 12:(1) }
YMIN = [10,-10,-5]                         # y-min for plotting
YMAX = [60,300,110]                        # y-max for plotting
SUBTITLES = ['realtime temperature','realtime intensity','realtime inputs']
LINESTYLES = ['None', 'None', '-']
NUM_PLOTS = len(COLUMN_YDATA)              # number of subplots
NUM_YCOLS = [len(plty) for plty in COLUMN_YDATA] #number of y-data columns in each subplot
ydata = []
for subplot in range(NUM_PLOTS):
    ydata.append([])
    for ycol in range(NUM_YCOLS[subplot]):
        ydata[subplot].append([])

# create an animated plot
pyplot.ion()
fig1, ax = pyplot.subplots(1,NUM_PLOTS,figsize=(18,8),sharex=True)
#fig1.suptitle('realtime sensor data', fontsize='18', fontweight='bold')
#fig1.xlabel('time, seconds', fontsize='14', fontstyle='italic')
#fig1.ylabel('temperature, degC', fontsize='14', fontstyle='italic')
#fig1.axes().grid(True)

plotlines = []
for n, subplt in enumerate(ax):
    for ynum in range(NUM_YCOLS[n]):
        plotlines.append(subplt.plot(ydata[n][ynum],marker='o',markersize=2,linestyle=LINESTYLES[n],markeredgewidth=0.0)[0])
    subplt.set_xlim([0,VIEW_WINDOW])
    subplt.set_ylim([YMIN[n],YMAX[n]])
    subplt.set_title(SUBTITLES[n])
    pyplot.show(block=False)
#for ax in fig1.axes():
#  ax.grid(True)
#fig1.axes().grid(True)
fig1.tight_layout()

run = True

def get_time(timepoint):
    try:
        timeval = float(timepoint[COLUMN_TIMESTAMP])*SECONDS_PER_TIMESTAMP_VAL
        return timeval
    except:
        print("bad timepoint: " + str(timepoint[COLUMN_TIMESTAMP]))

def get_ydata(timepoint,ycol):
    try: return YSCALING[ycol]*float(timepoint[ycol])
    except: pass

def get_current_time(timestamps):
    #print(timestamps)
    return timestamps[-1]

def slide_view_window_incrementally(current_time):
    global window_rightbound
    if current_time > window_rightbound:
        window_rightbound = current_time + VIEW_WINDOW/4
        for subplt in ax:
            subplt.set_xlim([window_rightbound-VIEW_WINDOW,window_rightbound])

def slide_view_window_continuously(current_time):
    if current_time > VIEW_WINDOW:
        for subplt in ax:
            subplt.set_xlim([current_time-VIEW_WINDOW,current_time])

def stretch_view_window_continuously(current_time):
    for subplt in ax:
        subplt.set_xlim([0,current_time])

def stretch_view_window_incrementally(current_time):
    global window_rightbound
    global starttime
    if starttime == 0:
        starttime = current_time - (3./4)*VIEW_WINDOW
        window_rightbound = current_time + (1./4)*VIEW_WINDOW
        for subplt in ax:
            subplt.set_xlim([starttime,window_rightbound])
    #elif current_time > window_rightbound:
    #    window_rightbound = window_rightbound + (window_rightbound-starttime)/6
    #    for subplt in ax:
    #        subplt.set_xlim([starttime,window_rightbound])

def get_last_line():
    with open(DATAFILE, "rb") as f:
        f.readline()
        f.seek(-1024,2)              # Jump to one kilobyte before the end of the file (2)
        while f.read(1) != b"\n":    # Until EOL is found...
            f.seek(-2,1)             # ...jump back the read byte plus one more.
        lastline = f.readline()      # Read last line.
    return lastline.split(',')

def update_data(timepoint):
    if (len(timepoint) == TOTAL_COLUMNS):
        timestamps.append(get_time(timepoint))
        for yplot, splot in enumerate(ydata):
            for ycol, yseries in enumerate(splot):
                yseries.append(get_ydata(timepoint,COLUMN_YDATA[yplot][ycol]))

def update_plot(xseries,yseries):
    n = 0
    for yplot,splot in enumerate(yseries):
        for yline,ydata in enumerate(splot):
            plotlines[n].set_xdata(xseries)
            plotlines[n].set_ydata(ydata)
            n += 1
    current_time = get_current_time(timestamps)
    # slide the viewing frame along...
    #slide_view_window_incrementally(current_time)
    #slide_view_window_continuously(current_time)
    # ...or, stretch the viewing frame!
    #stretch_view_window_continuously(current_time)
    stretch_view_window_incrementally(current_time)
    fig1.canvas.draw()   # redraw the canvas
    fig1.canvas.flush_events() # http://bastibe.de/2013-05-30-speeding-up-matplotlib.html


def lookback_initial():
    global file_position

    with open(DATAFILE, "rb") as f:
        for i in range(SKIPHEAD):
            f.readline()
        try:
            timepoint_delay = -1*(get_time(f.readline().split(','))-get_time(f.readline().split(',')))
            print("Time delay between datapoints: {!s}".format(timepoint_delay))
            linesize = sys.getsizeof(f.readline())
            backbytes = int(linesize*VIEW_WINDOW*(1./timepoint_delay))
            print("{!s} bytes per line, {!s} lines per second...".format(linesize,(1./timepoint_delay)))
            f.seek(0,0)    # Jump to the end of the file (2)
            print("the file starts at byte {}".format(f.tell()))

            f.seek(0,2)    # Jump to the end of the file (2)
            print("the file is {} bytes long...".format(f.tell()))
            print("Attempting to jump back {!s} bytes...".format(backbytes))
            f.seek(-1*backbytes,2)      # Jump `backbytes` before the end of the file (2)
            print("We jumped back to {}".format(f.tell()))
        except:
            print("Can't look back that far, so we went to the beginning")
            f.seek(0,0)

        f.readline() # throw away the (probably) partial line you landed in the middle of
        #while f.read(1) != b"\n":   # Until EOL is found...
        #    f.seek(-2,1)            # ...jump back the read byte plus one more.

        while True:
            for i in range(0,SKIPROWS):
                f.readline()
            line = f.readline()
            if not line:
                break
            #timepoint = line.decode('utf8').split(',')
            timepoint = line.split(',')
            update_data(timepoint)
            file_position = f.tell() # advance the file position marker

def data_quality(line):
    datapoint = line.split(',')

    if not line:
        return False
    elif not (len(datapoint) == TOTAL_COLUMNS):
        return False
    elif not line.endswith('\n'):
        return False
    elif ('' in datapoint):
        return False
    else:
        return True

def lookback_update():
    global file_position
    with open(DATAFILE, "rb") as f:
        f.seek(file_position,0)
        while True:
            for i in range(0,SKIPROWS):
                f.readline()
            line = f.readline()
            if not line:
                break
            if data_quality(line):   # if the data is quality...
                timepoint = line.split(',')
                update_data(timepoint)
            file_position = f.tell()      # advance the file position marker

def main():
    lookback_initial()
    while run:
        lookback_update()
        update_plot(timestamps,ydata)
        sleep(LOOP_DELAY)

main()
