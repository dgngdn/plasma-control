
# Do not write bytecode to maintain clean directories
import sys
sys.dont_write_bytecode = True

# Imports required packages.
from casadi import *
import numpy as NP
import matplotlib.pyplot as plt
import scipy.io
from matplotlib.ticker import MaxNLocator
from scipy import linalg
from scipy import signal
from numpy import random
from scipy import io as sio
import matplotlib.pyplot as plt
import os
import datetime
import time
import cv2
import argparse
from pylepton import Lepton
import RPi.GPIO as gpio
gpio.setwarnings(False)
import subprocess
import select
import scipy.io as scio
import serial
import crcmod
import visa
sys.path.append('/home/brandon/repos/python-seabreeze')
import seabreeze.spectrometers as sb
import asyncio
import usbtmc
import socket
from scipy.interpolate import interp1d
# Import core code.
import core

#import model_v1 as jet
#import EKF_v1 as observer


crc8 = crcmod.predefined.mkCrcFun('crc-8-maxim')

##initialize oscilloscope
instr = usbtmc.Instrument(0x1ab1, 0x04ce)
instr.open()
while not (instr.timeout == 1 and instr.rigol_quirk == False):
    instr.timeout = 1
    instr.rigol_quirk = False
idg = ''
while not idg:
    try:
        idg = instr.ask("*IDN?")
    except Exception as e: # USBErrors
         print("{} in get_oscilloscope".format(e))
         time.sleep(0.4)
print("device info: {}".format(idg))
print("device timeout: {}".format(instr.timeout))
time.sleep(0.5)

## initialize spectrometer
devices = sb.list_devices()
#t_int=12000
t_int=12000*4
print("Available devices {}".format(devices))
spec = sb.Spectrometer(devices[0])
print("Using {}".format(devices[0]))
spec.integration_time_micros(t_int)
print("integratiopn time {} seconds.".format(t_int/1e6))
time.sleep(0.5)

class DummyFile(object):
    def write(self, x): pass

def nostdout(func):
    def wrapper(*args, **kwargs):
        save_stdout = sys.stdout
        sys.stdout = DummyFile()
        func(*args, **kwargs)
        sys.stdout = save_stdout
    return wrapper

def get_runopts():
  """g
  Gets the arguments provided to the interpreter at runtime
  """
  parser = argparse.ArgumentParser(description="runs MPC",
			  epilog="Example: python mpc_lin_test.py --quiet")
  #parser.add_argument("--quiet", help="silence the solver", action="store_true")
  parser.add_argument("--faket", help="use fake temperature data", action="store_true")
  parser.add_argument("--fakei", help="use fake intensity data", action="store_true")
  parser.add_argument("--timeout", type=float, help="timout (seconds) on oscilloscope operations",
                            default=0.4)
  parser.add_argument("--save_therm", help="save thermography photos", action="store_true")
  parser.add_argument("--dir", type=str, default="data",help="relative path to save the data")
  runopts = parser.parse_args()
  return runopts

##define input zero point
#U0 = NP.array([(8.0,16.0,1.2,40)], dtype=[('v','>f4'),('f','>f4'),('q','>f4'),('d','>f4')])
runopts = get_runopts()

SAVEDIR_therm = os.path.join(os.getcwd(),runopts.dir,"thermography") # path to the directory to save thermography files
if runopts.save_therm and not os.path.exists(SAVEDIR_therm):
  print("Creating directory: {}".format(SAVEDIR_therm))
  os.makedirs(SAVEDIR_therm)

def save_data(SAVEDIR,data,fname):
  print("saving {}.csv".format(os.path.join(SAVEDIR,fname)))
  np.savetxt(os.path.join(SAVEDIR,"{}.csv".format(fname)),data,delimiter=',',fmt='%.4e')

def send_inputs_direct(device,U):
  """
  Sends input values to the microcontroller to actuate them
  """
  Vn = U[0]
  Fn = U[1]
  Qn = U[2]
  Dn = U[3]
  Xn = U[4]
  Yn = U[5]
  input_string='echo "v,{:.2f}" > /dev/arduino && echo "f,{:.2f}" > /dev/arduino && echo "q,{:.2f}" > /dev/arduino'.format(Vn, Fn, Qn)
  #subprocess.run('echo -e "v,{:.2f}\nf,{:.2f}\nq,{:.2f}" > /dev/arduino'.format(U[:,0][0]+8, U[:,1][0]+16, U[:,2][0]+1.2), shell=True)
  device.reset_input_buffer()
  device.write("v,{:.2f}\n".format(Vn).encode('ascii'))
  time.sleep(0.200)
  device.write("f,{:.2f}\n".format(Fn).encode('ascii'))
  time.sleep(0.200)
  device.write("q,{:.2f}\n".format(Qn).encode('ascii'))
  time.sleep(0.200)
  device.write("d,{:.2f}\n".format(Dn).encode('ascii'))
  time.sleep(0.400)
  device.write("x,{:.2f}\n".format(Xn).encode('ascii'))
  time.sleep(0.200)
  device.write("y,{:.2f}\n".format(Yn).encode('ascii'))
  time.sleep(0.200)

  print("input values: {:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f}".format(Vn,Fn,Qn,Dn,Xn,Yn))

def send_inputs(device,U):
  """
  Sends input values to the microcontroller to actuate them
  """
  Vn = U[0]
  Fn = U[1]
  Qn = U[2]
  Dn = U[3]
  Xn = U[4]
  Yn = U[5]
  Pn = U[6]
  input_string='echo "v,{:.2f}" > /dev/arduino && echo "f,{:.2f}" > /dev/arduino && echo "q,{:.2f}" > /dev/arduino'.format(Vn, Fn, Qn)
  #subprocess.run('echo -e "v,{:.2f}\nf,{:.2f}\nq,{:.2f}" > /dev/arduino'.format(U[:,0][0]+8, U[:,1][0]+16, U[:,2][0]+1.2), shell=True)
  device.reset_input_buffer()

  subprocess.run('echo "" > /dev/arduino', shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "v,{:.2f}" > /dev/arduino'.format(Vn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "f,{:.2f}" > /dev/arduino'.format(Fn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "q,{:.2f}" > /dev/arduino'.format(Qn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "d,{:.2f}" > /dev/arduino'.format(Dn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "x,{:.2f}" > /dev/arduino'.format(Xn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "y,{:.2f}" > /dev/arduino'.format(Yn), shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "p,{:.2f}" > /dev/arduino'.format(Pn), shell=True)
  time.sleep(0.0500)

  print("input values: {:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f}".format(Vn,Fn,Qn,Dn,Xn,Yn,Pn))

def send_inputs_v_only(device,Vn):
  """
  Sends input values to the microcontroller to actuate them
  """
  Vn = U[0]
  #subprocess.run('echo -e "v,{:.2f}\nf,{:.2f}\nq,{:.2f}" > /dev/arduino'.format(U[:,0][0]+8, U[:,1][0]+16, U[:,2][0]+1.2), shell=True)
  device.reset_input_buffer()

  subprocess.run('echo "" > /dev/arduino', shell=True)
  time.sleep(0.0500)

  subprocess.run('echo "v,{:.2f}" > /dev/arduino'.format(Vn), shell=True)
  time.sleep(0.0500)

  print("input value: {:.2f}".format(Vn))

def is_valid(line):
  """
  Verify that the line is complete and correct
  """
  l = line.split(',')
  crc = int(l[-1])
  data = ','.join(l[:-1])
  return crc_check(data,crc)

def crc_check(data,crc):
  crc_from_data = crc8("{}\x00".format(data).encode('ascii'))
  print("crc:{} calculated: {} data: {}".format(crc,crc_from_data,data))
  return crc == crc_from_data


############################################ ASYNC DEFS ##################################################33

async def get_temp_a(runopts):
  """
  Gets treatment temperature with the Lepton thermal camera
  """
  if runopts.faket:
    return 24

  run = True
  while run:
    try:
      with Lepton("/dev/spidev0.1") as l:
        data,_ = l.capture(retry_limit = 3)
      if l is not None:
        ##print(data[10:80]);
        Ts = NP.amax(data[6:60,:,0]) / 100 - 273;
        #Ts=data[25,58,0]/100-273 #calibrated for the long jet
        #print(Ts_max, Ts)
        Tt = NP.amax(data[0:5,:,0]) / 100 - 273;
        mm= NP.where( data == NP.amax(data) )
        #mm=[26,58] ######################################## THIS NEEDS CALIBRATION #############
        Ts_lin=data[int(mm[0]),:,0] /100 - 273
        yy=Ts_lin-Ts_lin[0]
        #gg=interp1d(yy,range(80))
        #sig=gg(0.6*NP.amax(yy))-mm[1]
        #print('sig',sig)
        Ts2 = (Ts_lin[int(mm[1])+2]+Ts_lin[int(mm[1])-2])/2
        Ts3 = (Ts_lin[int(mm[1])+12]+Ts_lin[int(mm[1])-12])/2
        for line in data:
          l = len(line)
          if (l != 80):
            print("error: should be 80 columns, but we got {}".format(l))
          elif Ts > 150:
            print("Measured temperature is too high: {}".format(Ts))

        if runopts.save_therm:
            curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
            fname = "{}".format(curtime)
            save_data(SAVEDIR_therm,data[:,:,0]/100.0 - 273 ,fname)


        #curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        #fname = "{}".format(curtime)
        #Ts = NP.amax(data) / 100 - 273;
        #Ts = NP.true_divide(NP.amax(data[7:50]),100)-273;
        time.sleep(0.070)
        run = False
    except Exception as e:
      print(e)
      print("\nHardware error on the thermal camera. Lepton restarting...")
      gpio.output(35, gpio.HIGH)
      time.sleep(0.5)
      gpio.output(35, gpio.LOW)
      print("Lepton restart completed!\n\n")

  #print(Ts)
  return [Ts, Ts2, Ts3, Ts_lin[mm[1]-12:mm[1]+12], Tt, sig, data]

async def get_intensity_a(f,runopts):
  """
  Gets optical intensity from the microcontroller
  """
  if runopts.fakei:
    Is = 5
  else:
    run = True
    v_rms=0
    Is=0
    U=[0,0,0]
    while run:
      try:
        f.reset_input_buffer()
        f.readline()
        line = f.readline().decode('ascii')
        if is_valid(line):
          run = False
          Is = int(line.split(',')[6])
          v_rms = float(line.split(',')[7])
          V = float(line.split(',')[1])
          f = float(line.split(',')[2])
          q = float(line.split(',')[3])
          dsep=float(line.split(',')[4])
          T_emb=float(line.split(',')[8])
          I_emb=float(line.split(',')[9])
          x_pos=float(line.split(',')[10])
          y_pos=float(line.split(',')[11])
          print('xpos', x_pos)
        else:
          print("CRC8 failed. Invalid line!")
      #    run = False
      #    Is = 0
      #    v_rms = 0
      #    V = 0
      #    f = 0
      #    q = 0

        U=[V,f,q,dsep]
      except:
        pass

  #print(Is)
  return [Is,v_rms,U,x_pos,y_pos,dsep,T_emb]

def gpio_setup():
  gpio.setmode(gpio.BOARD)
  gpio.setup(35, gpio.OUT)
  gpio.output(35, gpio.HIGH)

async def get_oscilloscope_a(instr):

   # instr.write(":STOP")
    # Votlage measurement

    instr.write(":MEAS:SOUR MATH")
    P=float(instr.ask("MEAS:VAVG?"))

    if P>1e3:
        print('WARNING: Measured power is too large')
        time.sleep(0.8)
        instr.write(":MEAS:SOUR MATH")
        P=float(instr.ask("MEAS:VAVG?"))

   # instr.write(":RUN")
   # time.sleep(0.4)

    #print(P)

    return [P]

async def get_spec_a(spec):

    wv=spec.wavelengths()
    sp_int=spec.intensities()
    O777=max(sp_int[1200:1250])

    #print(O777)
    return O777


async def asynchronous_measure(f,instr,runopts):

        tasks=[asyncio.ensure_future(get_temp_a(runopts)),
              asyncio.ensure_future(get_intensity_a(f,runopts)),
              asyncio.ensure_future(get_oscilloscope_a(instr))]

        await asyncio.wait(tasks)
        return tasks


###########################################################

Ts_old=37
Ts2_old=32
Ts3_old=25

Pold=2
runopts = get_runopts()
gpio_setup()
f = serial.Serial('/dev/arduino', baudrate=38400,timeout=1)

if os.name == 'nt':
    ioloop = asyncio.ProactorEventLoop() # for subprocess' pipes on Windows
    asyncio.set_event_loop(ioloop)
else:
    ioloop = asyncio.get_event_loop()

print(instr)
a=ioloop.run_until_complete(asynchronous_measure(f,instr,runopts))

Temps=a[0].result()
Ts=Temps[0]
Ts2=Temps[1]
Ts3=Temps[2]
Ts_lin=Temps[3]
Tt=Temps[4]
sig=Temps[5]

Ard_out=a[1].result()
Is=Ard_out[0]
v_rms=Ard_out[1]
x_pos=Ard_out[3]
y_pos=Ard_out[4]
d_sep=Ard_out[5]

Osc_out=a[2].result()
Vrms=Osc_out[0]
P=Osc_out[0]

delta=3.0
#print(Ts)
#print(P)
msg="Temperature: {:.2f} Power: {:.2f}".format(Ts,P)
print('Measurment working...')
CEM=np.zeros(np.shape(Ts_lin)) #initialize CEM measurement

print(msg)
s=socket.socket(socket.SO_REUSEADDR,1)
host='pi3.dyn.berkeley.edu'
port=2223
s.bind((host,port))
s.listen(5)
print('\n')
print('Server is listening on port' ,port)
print('Waiting for connection...')

c, addr = s.accept()
c.settimeout(0.2)

print('Got connection from', addr)

u_ub=[10.,20,4.,100.]
u_lb=[6.,10.,1.,100.]

V=6.0 #initial applied voltage
Tset=40 #initial setpoint

t_el=0  #seconds sup. control timer
t_move=60.0 #seconds movement time
Delta_y=1.5 #mm
t_elps=0 #seconds movement timer
t_mel=0 #PI control timer
I1=0
Y_pos=0

############ initialize save document ################################
sv_fname=os.path.join(runopts.dir,"control_dat_{}".format(curtime)
save_file=open(sv_fname,'a+')
save_file.write('time,Tset,Ts,Ts2,Ts3,P,Freq/1000,V,F,Q,D,x_pos,y_pos,t1\n')

while True:
    try:
        try:
            data=c.recv(512).decode()
            data_str=data.split(',')
            data_flt=[float(i) for i in data_str]
            Tset=data_flt[0]
            t_el=data_flt[1]

            print('Optimal Reference Recieved!')
            print('Tref: {:6.2f}, t_el:{:6.2f}'.format(Tset,t_el))
        except:
            print('no data yet')

        t0=time.time()
        a=ioloop.run_until_complete(asynchronous_measure(f,instr,runopts))

        Temps=a[0].result()
        Ts=Temps[0]
        Ts2=Temps[1]
        Ts3=Temps[2]
        Ts_lin=Temps[3]
        Tt=Temps[4]
        sig=Temps[5]

        Ard_out=a[1].result()
        Is=Ard_out[0]
        v_rms=Ard_out[1]
        U_m=Ard_out[2]
        x_pos=Ard_out[3]
        y_pos=Ard_out[4]
        d_sep=Ard_out[5]
        T_emb=Ard_out[6]

        Osc_out=a[2].result()
        Vrms=Osc_out[0]

        P=Osc_out[0]

        print("Temperature: {:.2f} Power: {:.2f}".format(Ts,P))
        if abs(Ts)>90:
            Ts=Ts_old
            Ts2=Ts2_old
            Ts3=Ts3_old
            print('WARNING: Old value of Ts is used')
        else:
            Ts_old=Ts
            Ts2_old=Ts2
            Ts3_old=Ts3

        if abs(P)>10:
            P=Pold
            print('WARNING: Old value of Ts is used')
        else:
            Pold=P

        ########################## PI CONTROLS ################################
        Kp1=2.7
        Tp1=28.8
        lamb1=85.0

        Kc1=Tp1/(Kp1*lamb1)
        Ti1=100*Tp1
        e1=Tset-Ts

        u1= V+Kc1*(e1+I1/Ti1)

        if round(V,2)>=u_ub[0] and Tset>=Ts:
            I1 = I1
            V=u_ub[0]
        elif round(V,2)>=u_ub[0] and Tset<Ts:
            I1 = I1 + e1*t_mel
            V=u_ub[0]
        elif round(V,2)<=u_lb[0] and Tset<=Ts:
            I1 = I1
            V=u_lb[0]
        elif round(V,2)<=u_lb[0] and Tset>Ts:
            I1 = I1 + e1*t_mel
            V=u_lb[0]
        else:
            I1=I1+e1*t_mel
            V=round(u1,2)

        ########################################################################
        print("Sending inputs...")
        send_inputs(f,V)
        print("Inputs sent!")

        tm_el=time.time()-t0
        ##interpolate temperature to shift position
        x_gen=range(25)
        x_now=NP.array(range(25))-13+Y_pos

        Tshift=interp1d(x_now,Ts_lin,bounds_error=False,fill_value=min(Ts_lin))(x_gen)
        CEM=CEM+tm_el*(9.74e-14/60.0)*np.exp(np.multiply(0.6964,Tshift))
        msg='{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f} \n'.format(*CEM)
        print(msg)

        c.send(msg.encode())
        print('Measured outputs sent')

        save_file.write('{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f}\n'.format(time.time(),Tset,Ts,Ts2,Ts3,P,*U_m,x_pos,y_pos)

        if KeyboardInterrupt==1:
            sys.exit(1)

    except Exception as e:
        c.close()
        print('There was an error !')
        print(e)
        sys.exit(1)
