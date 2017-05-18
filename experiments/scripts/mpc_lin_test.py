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
import scipy.io
from scipy import linalg
from casadi import *
# Import core code.
import core
import pyserial
#import asyncio
#import serial_asyncio
#import crcmod
import crcmod.predefined

crc8 = crcmod.predefined.mkCrcFun('crc-8-maxim')

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
  parser.add_argument("--quiet", help="silence the solver", action="store_true")
  parser.add_argument("--faket", help="use fake temperature data", action="store_true")
  parser.add_argument("--fakei", help="use fake intensity data", action="store_true")
  runopts = parser.parse_args()
  return runopts

def send_inputs(U):
  """
  Sends input values to the microcontroller to actuate them
  """
  input_string='echo "v,{:.2f}" > /dev/arduino && echo "f,{:.2f}" > /dev/arduino && echo "q,{:.2f}" > /dev/arduino'.format(U[:,0][0]+8, U[:,1][0]+16, U[:,2][0]+1.2)
  subprocess.call(input_string,  shell=True)
  print("input: {}".format(input_string))
  print("input values: {}".format(U+[8,16,1.2]))

def is_valid(line):
  """
  Verify that the line is complete and correct
  """
  l = line.split(',')
  crc = l[-1]
  data = l[:-1]
  return crc_check(data,crc)

def crc_check(data,crc):
    crc_from_data = crc8("{}\x00".format(data).encode('ascii'))
    return crc == crc_from_data:

def get_temp(runopts):
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
        for line in data:
          l = len(line)
          if (l != 80):
            print("error: should be 80 columns, but we got {}".format(l))
        curtime = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S.%f")
        fname = "{}".format(curtime)
        Ts = NP.true_divide(NP.amax(data[7:50]),100)-273;
        #time.sleep(0.5)
        run = False
    except:
      print("\n\nHardware error on the thermal camera. Lepton restarting...")
      gpio.output(35, gpio.HIGH)
      time.sleep(0.5)
      gpio.output(35, gpio.LOW)
      print("Lepton restart completed!\n\n")
  return Ts

def get_intensity(f,runopts):
  """
  Gets optical intensity from the microcontroller
  """
  if runopts.fake:
    Is = 5
  else:
    run = True
    while run:
      line = f.readline()
        if is_valid(line):
          run = False
      #a = f.stdout.readline()
    Is = int(line.decode().split(',')[6])
  return Is

def gpio_setup():
  gpio.setmode(gpio.BOARD)
  gpio.setup(35, gpio.OUT)
  gpio.output(35, gpio.HIGH)











save_file=open('control_dat','a+')
## SETUP THE MPC
#system dimension
nx = 7
nu = 3
ny = 2
nw = 2
nv = ny

# simulation time
Delta = 1     # sampling time, s

# parameters for MPC
N = 20           # prediction horizon

# declare casadi variable graph
x = MX.sym('x',nx)
u = MX.sym('u',nu)
w = MX.sym('w',nw)

# Define system matrices => from data driven subspace ID
A = NP.array([[  0,    1.0000   ,      0  ,       0  ,       0  ,       0  ,       0],
              [  0,         0 ,   1.0000   ,      0  ,       0 ,        0 ,        0],
              [  1.5496 ,  -4.4591,    3.9387  ,  0.1182 ,  -2.6865,   -1.4204,    4.0078],
              [  0  ,       0    ,     0     ,    0  ,  1.0000,         0   ,      0],
              [  0 ,        0     ,    0     ,    0  ,       0 ,   1.0000  ,       0],
              [  0 ,        0      ,   0      ,   0  ,       0 ,        0 ,   1.0000],
              [  -0.8813,    2.1836 ,  -1.3261 ,  -0.0731 ,   1.5081,    1.3487,   -1.7990]])

B = NP.array([[0.9099,    0.1478,   -4.4538],
              [1.1473   , 0.8071,  -4.4349],
              [ 1.5845 ,   0.8318,   -3.6681],
              [  5.9034,    3.3445,   32.8287],
              [ 1.4962,    0.2317 ,   3.1539],
              [-0.8336 ,  -0.3603 ,  -0.5211],
              [ 0.0463 ,   0.0354 ,   1.0197]])

C = NP.array([[   1,     0,     0,     0,     0,     0,     0],
              [   0,     0,     0,     1,     0,     0,     0]])

# steady state values
uss = [0.0, 0.0, 0.0]
xss = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# specify number of artificial disturbanes + matrices
nd = ny
Bd = NP.zeros((nx,nd))
Cd = NP.array([[1,0],[0,1]])

# controlled variable matrix, z=Hy (nz <= nu)
H = NP.array([[1.0, 0.0],[0.0, 1.0]])

# setpoint on controlled variables (in deviation variables)
ztar = NP.array([0.0, 0.0])

# get observer gain for augmented system [x ; d]
At = NP.zeros((nx+nd,nx+nd)); At[0:nx,0:nx] = A; At[0:nx,nx:] = Bd; At[nx:,nx:] = NP.eye(nd)
Bt = NP.zeros((nx+nd,nu)); Bt[0:nx,:] = B
Ct = NP.concatenate((C,Cd),axis=1)

Lt, Pt = core.dlqe(At,Ct,NP.eye(nx+nd),0.1*NP.eye(ny))

tarMat = NP.zeros((nx+ny,nx+nu)); tarMat[0:nx,0:nx] = NP.eye(nx)-A; tarMat[0:nx,nx:] = -B; tarMat[nx:,0:nx] = H.dot(C)

# mpc weights
# Qx = NP.array([[1.0/xss[0]**2, 0, 0], [0, 1.0/xss[1]**2, 0], [0, 0, 1.0/xss[2]**2]])
Qz = NP.array([[1.0,0],[0,0.5]])
Ru = 0.5*NP.array([[1.0,0,0],[0,1.0,0],[0,0,1.0]])
Qx = core.mtimes(C.T,H.T,Qz,H,C)

# stage cost (in deviation variables)
Lstage = core.mtimes(x.T,Qx,x) + core.mtimes(u.T,Ru,u)

# create casadi graph of deviation variables to dynamics and stage cost
MPC_dynamics = Function("MPC_dynamics", [x,u], [core.mtimes(A,x) + core.mtimes(B,u),Lstage])

# bounds on outputs
Ts_lb = -5;     Ts_ub = 5
Is_lb = -30;     Is_ub = 30

x_ub=[Ts_ub, inf, inf, Is_ub, inf, inf, inf]
x_lb=[Ts_lb, -inf, -inf, Is_lb, -inf, -inf, -inf]
# bounds on the control inputs
V_lb = -2;     V_ub = 1.5
f_lb = -3;     f_ub = 2
q_lb = -0.4;   q_ub = 0.6

u_lb = [V_lb, f_lb, q_lb]
u_ub = [V_ub, f_ub, q_ub]
u0 = [0,0,0] # initial guess for inputs
x0 = NP.array([0,0,0,0,0,0,0]) # initial states

# pick solver
# MySolver = "ipopt"
# MySolver = "worhp"
MySolver = "sqpmethod"

# number of monte carlo runs
Nmc = 1

# save figures (0=NO, 1=YES)
save_fig = 0

# export to matlab (0=NO, 1=YES)
export_to_matlab = 0
export_name = 'MPC_data_v3'

### Build instance of MPC problem
# start with an empty NLP
q = []
q0 = []
lbq = []
ubq = []
J = 0
g = []
lbg = []
ubg = []
# "lift" initial conditions
X0 = MX.sym('X0', nx)
q += [X0]
lbq += [0]*nx
ubq += [0]*nx
q0 += [0]*nx
# formulate the QP
Xk = X0
for k in range(N):
    # new NLP variable for the control
    Uk = MX.sym('U_' + str(k), nu)
    q   += [Uk]
    lbq += [u_lb[i]-uss[i] for i in range(nu)]
    ubq += [u_ub[i]-uss[i] for i in range(nu)]
    q0  += [0]*nu
    # next step dynamics and stage cost
    Fk, Lk = MPC_dynamics(Xk,Uk)
    Xk_end = Fk
    J = J + Lk
    # New NLP variable for state at end of interval
    Xk = MX.sym('X_' + str(k+1), nx)
    q   += [Xk]
    lbq += [x_lb[i]-xss[i] for i in range(nx)]
    ubq += [x_ub[i]-xss[i] for i in range(nx)]
    q0  += [0]*nx
    # Add equality constraint
    g   += [Xk_end-Xk]
    lbg += [0]*nx
    ubg += [0]*nx

# set solver options
opts = {}
if MySolver == "sqpmethod":
  opts["qpsol"] = "qpoases"
  opts["qpsol_options"] = {"printLevel":"none"}
  #opts["verbose_init"] = False
  #opts["print_header"] = False
  #opts["print_time"] = False
  #opts["print_iteration"] = False
# create NLP solver for MPC problem
prob = {'f':J, 'x':vertcat(*q), 'g':vertcat(*g)}
solver = nlpsol('solver', MySolver, prob, opts)

X = []
U = []
Y = []


if __name__ == "__main__":
  """
  Collect data from the system, feed it into an MPC algorithm,
  solve an optimization problem, and then feed the results
  back into the system
  """

  runopts = get_runopts()
  if runopts.quiet:
    ## silence the solver
    solver = nostdout(solver)
  gpio_setup()
  # shell calls - slow and somewhat brittle
  #f = subprocess.Popen(['tail','-f','./data/temperaturehistory'],\
  #        stdout = subprocess.PIPE,stderr=subprocess.PIPE)
  #p = select.poll()
  #p.register(f.stdout)
  # creating a good ol' serial object to read from when we want it
  f = serial.Serial('/dev/arduino', baudrate=9600)

  #initialize
  Ts = 0
  first_run = 0
  delay = 10
  k = 0
  Y0 = 0
  startMPC = 0
  Xhat = NP.zeros((nx,1))
  Uhat = NP.zeros((1,nu))
  Dhat = NP.zeros((nd,1))
  U = NP.zeros((1,nu))
  u_opt = [0,0,0]

  while True:
    Ts = get_temp(runopts)
    Is = get_intensity(f,runopts)
    print("temperature: {:.2f}, intensity: {:d}".format(Ts,Is))

    # k is the loop instance (incremented each loop)
    if k == delay:
        ## set Y0 as the initial state
        Y0 = [Ts,Is]
        startMPC = True
        print("starting mpc")
    elif k < delay:
        print("Don't start MPC yet...")
    else:
        print("MPC is running")

    # The actual MPC part
    if startMPC:
        start_time = time.time()
        if k > 100:
            ### change target
            ztar_k = ztar + NP.array([-4.0,0.0])
        else:
            ztar_k = ztar
	      # get measurement
        Y = NP.array([Ts-Y0[0], Is-Y0[1]])
        print(Y0)
        print(Y)
        # update predictor
        stack = NP.concatenate((Xhat,Dhat),axis=0)
        pred = At.dot(stack) + Bt.dot(U.T)
        err = Y - Ct.dot(pred).T
        update = pred + Lt.dot(err.T)
        Xhat = update[0:nx]
        Dhat = update[nx:]

        # target calculator
        btar = NP.concatenate((Bd.dot(Dhat).T,-H.dot(Cd).dot(Dhat).T+ztar_k),axis=1)
        tarSol = core.mtimes(linalg.pinv(tarMat),btar.T)

        Xtar = tarSol[0:nx] #target_state
        Utar = tarSol[nx:]  #target_input

        #  q0[0:nx] = Xhat[0:nx]-Xtar[0:nx]
		    #for i in range(0,len(q0)):
		    #	 q0[i] = Xhat[0][0]-Xtar[0][0]

        for i in range(nx):
            q0[i] = Xhat[i][0]-Xtar[i][0]
        lbq[0:nx] = q0[0:nx]
        ubq[0:nx] = q0[0:nx]

        # bounds
        u_lb_update = [u_lb[i]-Utar[i][0] for i in range(nu)]
        u_ub_update = [u_ub[i]-Utar[i][0] for i in range(nu)]
        # ONLY WORKS WHEN ALL STATES ARE MEASURED AND C=I
        x_lb_update = [x_lb[i]-Xtar[i][0] for i in range(nx)]
        x_ub_update = [x_ub[i]-Xtar[i][0] for i in range(nx)]

        for i in range(N):
            lbq[nx+i*(nx+nu):nx+nu+i*(nx+nu)] = u_lb_update
            ubq[nx+i*(nx+nu):nx+nu+i*(nx+nu)] = u_ub_update
            lbq[nx+nu+i*(nx+nu):2*nx+nu+i*(nx+nu)] = x_lb_update
            ubq[nx+nu+i*(nx+nu):2*nx+nu+i*(nx+nu)] = x_ub_update

        try:
            sol = solver(x0=q0, lbx=lbq, ubx=ubq, lbg=lbg, ubg=ubg)
            q_opt = sol['x'].full().flatten()
            u_opt = q_opt[nx:nx+nu]
            print("solved a thing!")
        except:
            u_opt = u_opt
            print("\n The solver could not converge! \n\n")

        U = u_opt + Utar.T
        Ureal = U + NP.array([8,16,1.2])
        #print(Ureal.shape)
        send_inputs(U)

        print("predictions: {}".format(Ct.dot(pred).T+Y0))
        #save_file.write("{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f}\n".format(Ts,Is,*Y,*X,*U))
        save_file.write("{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f},{:6.2f}\n".format(
                         time.time(),Ts,Is,*Y,*U.flatten()))  ##X is never referenced!
        print()
        save_file.flush()

        # figure out how long the loop took
        # if it's not time to run again, delay until it is
        end_time = time.time()
        time_el = end_time - start_time
        if time_el < 1:
            time.sleep(1 - time_el)
    ## increment the loop counter
    k = k + 1


