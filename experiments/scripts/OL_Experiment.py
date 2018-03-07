#!/usr/bin/env python3.5

import sys
sys.dont_write_bytecode = True

import os
import datetime
import time
import numpy as NP
import cv2
import argparse
from pylepton import Lepton
import RPi.GPIO as gpio
gpio.setwarnings(False)
import subprocess
import select
import scipy.io as scio
from scipy import linalg
from casadi import *
# Import core code.
import core
import serial
#import asyncio
#import serial_asyncio
import crcmod
#import crcmod.predefinmaed
import usbtmc
import visa
sys.path.append('/home/brandon/repos/python-seabreeze')
import seabreeze.spectrometers as sb
import asyncio
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
t_int=200000 #integration time in microseconds

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
  """
  Gets the arguments provided to the interpreter at runtime
  """
  parser = argparse.ArgumentParser(description="runs MPC",
			  epilog="Example: python mpc_lin_test.py --quiet")
  #parser.add_argument("--quiet", help="silence the solver", action="store_true")
  parser.add_argument("--faket", help="use fake temperature data", action="store_true")
  parser.add_argument("--fakei", help="use fake intensity data", action="store_true")
  parser.add_argument("--save_therm", help="save thermography photos", action="store_true")
  parser.add_argument("--save_spec", help="save optical emission spectra", action="store_true")
  parser.add_argument("--save_osc", help="save oscilloscope waveforms", action="store_true")
  parser.add_argument("--timeout", type=float, help="timout (seconds) on oscilloscope operations",default=0.4)
  parser.add_argument("--Delta", type=float, help="time to hold each combo of inputs",default=150)
  parser.add_argument("--dir", type=str, default="data",help="relative path to save the data")
  parser.add_argument("--inputs", type=str, default="voltage_step_test.mat",help=".mat file containing the desired input sequence")
  runopts = parser.parse_args()
  return runopts

##define input zero point
U0 = NP.array([(8.0,16.0,1.2,40)], dtype=[('v','>f4'),('f','>f4'),('q','>f4'),('d','>f4')])

## configure save directory for thermography
runopts = get_runopts() 
SAVEDIR_therm = os.path.join(os.getcwd(),runopts.dir,"thermography") # path to the directory to save thermography files
SAVEDIR_spec = os.path.join(os.getcwd(),runopts.dir,"spectroscopy") # path to the directory to save thermography files
SAVEDIR_osc = os.path.join(os.getcwd(),runopts.dir,"oscilloscope") # path to the directory to save thermography files

if runopts.save_therm and not os.path.exists(SAVEDIR_therm):
  print("Creating directory: {}".format(SAVEDIR_therm))
  os.makedirs(SAVEDIR_therm)

if runopts.save_spec and not os.path.exists(SAVEDIR_spec):
  print("Creating directory: {}".format(SAVEDIR_spec))
  os.makedirs(SAVEDIR_spec)

def save_data(SAVEDIR,data,fname):
  print("saving {}.csv".format(os.path.join(SAVEDIR,fname)))
  np.savetxt(os.path.join(SAVEDIR,"{}.csv".format(fname)),data,delimiter=',',fmt='%.4e')

def send_inputs(device,U):
  """
  Sends input values to the microcontroller to actuate them
  """
  Vn = U[0]+U0['v'][0]
  Fn = U[1]+U0['f'][0]
  Qn = U[2]+U0['q'][0]
  #Dn = U[3]+U0['d'][0]
  input_string='echo "v,{:.2f}" > /dev/arduino && echo "f,{:.2f}" > /dev/arduino && echo "q,{:.2f}" > /dev/arduino'.format(Vn, Fn, Qn)
  #subprocess.run('echo -e "v,{:.2f}\nf,{:.2f}\nq,{:.2f}" > /dev/arduino'.format(U[:,0][0]+8, U[:,1][0]+16, U[:,2][0]+1.2), shell=True)
  device.reset_input_buffer()
  #device.write("v,{:.2f}\n".format(Vn).encode('ascii'))
  subprocess.run('echo "" > /dev/arduino', shell=True)
  time.sleep(0.200)
  subprocess.run('echo "v,{:.2f}" > /dev/arduino'.format(Vn), shell=True)
  time.sleep(0.200)
  #device.write("f,{:.2f}\n".format(Fn).encode('ascii'))
  subprocess.run('echo "f,{:.2f}" > /dev/arduino'.format(Fn), shell=True)
  time.sleep(0.200)
  #device.write("q,{:.2f}\n".format(Qn).encode('ascii'))
  subprocess.run('echo "q,{:.2f}" > /dev/arduino'.format(Qn), shell=True)
  #subprocess.call(input_string,  shell=True)
  #print("input: {}".format(input_string))
  #time.sleep(0.200)
  #subprocess.run('echo "d,{:.2f}" > /dev/arduino'.format(Dn), shell=True)
 # print("input values: {:.2f},{:.2f},{:.2f},{:.2f}".format(Vn,Fn,Qn,Dn))
  print("input values: {:.2f},{:.2f},{:.2f}".format(Vn,Fn,Qn))
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
        Tt = NP.amax(data[0:5,:,0]) / 100 - 273;
        mm= NP.where( data == NP.amax(data) )

        Ts_lin=data[int(mm[0]),:,0] /100 - 273
        Ts2 = (Ts_lin[int(mm[1])+2]+Ts_lin[int(mm[1])-2])/2
        Ts3 = (Ts_lin[int(mm[1])+6]+Ts_lin[int(mm[1])-6])/2
        for line in data:
          l = len(line)
          if (l != 80):
            print("error: should be 80 columns, but we got {}".format(l))
          elif Ts > 150:
            print("Measured temperature is too high: {}".format(Ts))
        
        if runopts.save_therm:
            curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
            fname = "{}".format(curtime)
            save_data(SAVEDIR_therm,data /100.0 - 273 ,fname)

        time.sleep(0.050)
        run = False

    except Exception as e:
      print(e)
      print("\nHardware error on the thermal camera. Lepton restarting...")
      gpio.output(35, gpio.HIGH)
      time.sleep(0.5)
      gpio.output(35, gpio.LOW)
      print("Lepton restart completed!\n\n")
  
  #print(Ts)
  return [Ts, Ts2, Ts3, Ts_lin, Tt, data]

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
    T_emb=0
    I_emb=0
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

  return [Is,v_rms,U,T_emb,I_emb]

def gpio_setup():
  gpio.setmode(gpio.BOARD)
  gpio.setup(35, gpio.OUT)
  gpio.output(35, gpio.HIGH)


async def get_oscilloscope_a(instr,runopts):

    #instr.write(":STOP")
    # Votlage measurement
    instr.write(":MEAS:SOUR CHAN1")
    Vrms=float(instr.ask("MEAS:ITEM? PVRMS"))
    Freq=float(instr.ask("MEAS:ITEM? FREQ"))
    #cycles on screen
    c_os=100*6*1e-6*Freq

    instr.write(":MEAS:SOUR CHAN2")
    Imax=float(instr.ask("MEAS:VMAX?"))*1000   
    Irms=float(instr.ask("MEAS:ITEM? PVRMS"))*1000   

    if Imax>1e3:
        print('WARNING: Measured current is too large')
        time.sleep(0.8)
        instr.write(":MEAS:SOUR CHAN2")
        Imax=float(instr.ask("MEAS:VMAX?"))*1000   
        Irms=float(instr.ask("MEAS:VRMS?"))*1000   
    instr.write(":MEAS:SOUR MATH")
    P=float(instr.ask("MEAS:VAVG?"))
    Pp=float(instr.ask("MEAS:ITEM? MPAR"))   


    if P>1e3:
        print('WARNING: Measured power is too large')
        time.sleep(0.8)
        instr.write(":MEAS:SOUR MATH")
        P=float(instr.ask("MEAS:VAVG?"))
        Pp=float(instr.ask("MEAS:ITEM? MPAR"))   
    
    if runopts.save_osc:
        instr.write(":WAV:MODE NORMAL")
        instr.write(":WAV:FORMAT ASCII")
        instr.write(":WAV:SOUR CHAN1")
        rawdata1 = instr.ask(":WAV:DATA?")
        instr.  write(":WAV:SOUR CHAN2")
        rawdata2 = instr.ask(":WAV:DATA?")
        data=[rawdata1,rawdata2]
        curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        fname = "{}".format(curtime)
        save_data(SAVEDIR_osc,data,fname)

    #print(P)
    time.sleep(0.4)
    return [Vrms,Imax,Irms,P,Pp,Freq]

async def get_spec_a(spec,runopts):

    wv=spec.wavelengths()
    sp_int=spec.intensities()
    O777=max(sp_int[1200:1250])
    O844=max(sp_int[1405:1455])
    N391=max(sp_int[126:146])
    if runopts.save_spec:
        data=[wv,sp_int]
        curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        fname = "{}".format(curtime)
        save_data(SAVEDIR_spec,data,fname)

    #print(O777)
    return [O777,O844,N391]

def syncronous_measure(f,instr,runopts):
    Ts=get_temp(runopts)
    get_intensity(f,runopts)
    get_oscilloscope(instr,runopts)
    get_spec(spec, runopts)
    return Ts


async def asynchronous_measure(f,instr,runopts):

        tasks=[asyncio.ensure_future(get_temp_a(runopts)),
              asyncio.ensure_future(get_intensity_a(f,runopts)),
              asyncio.ensure_future(get_oscilloscope_a(instr,runopts)),
              asyncio.ensure_future(get_spec_a(spec,runopts))]
        
        await asyncio.wait(tasks)
        return tasks

############################################ INITIALIZE ###########################################
curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
sv_fname=os.path.join(runopts.dir,"control_dat_{}".format(curtime))

save_file=open(sv_fname,'a+')
save_file.write('time,Ts,Ts2,Ts3,Tt,T_amb,Is,Vrms,v_rms,Imax,Irms,I_emb,P,Freq/1000,O777,O844,N391,V,F,Q,D,t1\n')
infile=runopts.inputs
#import input data
OL_opt=scio.loadmat(infile)
OL_in=OL_opt['u_opts']
delay = 10
Delta = runopts.Delta #how long each input combination is applied in s
osc_run=1;
u_opt = [0,0,0,0]
k=-1
t0=time.time()      
if __name__ == "__main__":
    
    runopts = get_runopts() 
    gpio_setup()
    f = serial.Serial('/dev/arduino', baudrate=38400,timeout=1)

############################################ MAIN LOOP ###########################################
    
    while True:
        #Ts=syncronous_measure(f,instr,runopts)
        #t1=time.time()
        #print('sync:',t1-t0)       
        #print(Ts)

        if os.name == 'nt':
            ioloop = asyncio.ProactorEventLoop() # for subprocess' pipes on Windows
            asyncio.set_event_loop(ioloop)
        else:
            ioloop = asyncio.get_event_loop()
          
############################################ MEASUREMENT ###########################################
            a=ioloop.run_until_complete(asynchronous_measure(f,instr,runopts))
           # Ts=tasks[0].result()
           # print('async:',t1-t0) 
        
          ##### UNPACKING TASK OUTS ########
            Temps=a[0].result()
            Ts=Temps[0]
            Ts2=Temps[1]
            Ts3=Temps[2]
            Tt=Temps[4]

            Ard_out=a[1].result() 
            print(Ard_out)  
            Is=Ard_out[0]
            v_rms=Ard_out[1]
            U=Ard_out[2]
            T_emb=Ard_out[3]
            I_emb=Ard_out[4]

            Osc_out=a[2].result()
            Vrms=Osc_out[0]
            Imax=Osc_out[1]
            Irms=Osc_out[2]
            P=Osc_out[3]
            Pp=Osc_out[4]
            Freq=Osc_out[5]  
                      
            spec_vals=a[3].result()
            O777=spec_vals[0]
            O844=spec_vals[1]
            N391=spec_vals[2]

        print("T(C): {:.2f}, I(a.u.): {:d}, Vrms(V): {:.2f}, Imax(mA): {:.2f}, Irms(mA): {:.2f}, Power(W):{:.2f},{:.2f}".format(Ts,Is,Vrms,Imax,Irms,P,Pp*Freq))

        t1=time.time()-t0

        try:
            save_file.write("{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f}, {:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f}\n".format(time.time(),Ts,Ts2,Ts3,Tt,T_emb,Is,Vrms,v_rms,Imax,Irms,I_emb,P,Freq/1000,O777,O844,N391,*U,t1))  ##X is never
            save_file.flush()   
        except:
            print('problem writing to file')
            pass      


############################################ ACTUATION ###########################################
        if t1<delay:  ## initial delay ####
            delay_flag=0
        else:
            delay_flag=1     


        if delay_flag==1:  ## actual actuation ####
            if t1<Delta:
                k=k
            else:
                k=k+1
                print("Next increment...")
                
                u_opt=OL_in[k,:]
         
                U = u_opt

                Ureal = U + NP.array([U0['v'],U0['f'],U0['q']])
                #print(Ureal.shape)
                print("sending inputs...")
                send_inputs(f,U)
                print("inputs sent!")
                t0=time.time()      


            ## print(tasks)

            ##ioloop.close()
            ##ioloop.close()

