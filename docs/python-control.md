
## Process Control in Python

### py4sid

subspace identification for linear systems
https://github.com/mattjj/py4sid
https://github.com/mackelab/py4sid

def estimate_parameters_4sid(y,i,nhat=None,return_xs=False):


## data parameters
n, p = 16, 8
T = 30000

## generate a system and simulate from it
(A,B,C,D), (x,y) = rand_lds_and_data(T,n,p,eig_min=0.5,eig_max=1.0)

----

what are these variables?

n: 
p: 
T: 

lds = rand_lds(n,p)
data = rand_data(T,lds)

(A,B,C,D), (x,y) = rand_lds_and_data(T,n,p,eig_min=0.5,eig_max=1.0)

----

Can you briefly summarize what `test.py` is doing?

I'm familiar with state space representation of dynamical systems, but I'm getting tripped up because the ABCD matrices don't have the dimensions I think they should have.

n = # of states
m = # of inputs
p = # of outputs

A: [n x n]
B: [n x m]
C: [p x n]
D: [p x m]

But in `rand_lds()`:

```python
A = np.real(T.dot(np.diag(eigvals)).dot(np.linalg.inv(T)))  # [n x n] as expected
B = np.eye(n)                 # [n x n], but I would think it should be [n x m]
C = randn(p,n)               # [p x n] as expected
D = 0.1*np.eye(p)          # [p x p], but I would think it should be [p x m]
```

Can you help clarify?

### APMonitor

APMonitor, or "Advanced Process Monitor" is optimization software for mixed-integer and differential algebraic equations. It is coupled with large-scale solvers for linear, quadratic, nonlinear, and mixed integer programming (LP, QP, NLP, MILP, MINLP). Modes of operation include data reconciliation, moving horizon estimation, real-time optimization, dynamic simulation, and nonlinear predictive control with solution capabilities for high-index differential and algebraic (DAE) equations. It is available as a MATLAB toolbox, a Python module, a Julia package, or from a web browser interface.

they have a Python module: http://apmonitor.com/wiki/index.php/Main/PythonApp

APMonitor, or "Advanced Process Monitor" is optimization software for mixed-integer and differential algebraic equations. It is coupled with large-scale solvers for linear, quadratic, nonlinear, and mixed integer programming (LP, QP, NLP, MILP, MINLP). Modes of operation include data reconciliation, moving horizon estimation, real-time optimization, dynamic simulation, and nonlinear predictive control with solution capabilities for high-index differential and algebraic (DAE) equations. It is available as a MATLAB toolbox, a Python module, a Julia package, or from a web browser interface.

Google Group: https://groups.google.com/forum/#!forum/apmonitor

How is this thing actually licensed?

In December 2014, a BYU professor published an article in CACHE (Computer Aids in CHemical Engineering) about a process control lab that incorporated an Arduino for data acquisition and control of an electrical system with thermal dynamics.  This lab used Matlab for the analysis and control.

+ http://apmonitor.com/che436/index.php/Main/PhysicalLab

In Spring 2017, an updated version of the lab using Python was published on the website and GitHub:

+ http://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl
+ https://github.com/APMonitor/arduino

----

Introduction to Feedback Control using Design Studies
Randal W. Beard
Timothy W. McLain
http://controlbook.byu.edu/doku.php
http://controlbook.byu.edu/lib/exe/fetch.php?media=wiki:controlbook.pdf

"If I had given him
a transfer function and asked him to design a PID controller, or if I had given
him a state space model and asked him to design an observer and controller for
the system, he would not have had any difficulty. But he did not know how
to do an end-to-end design, that required developing models for the system,
including physical constraints. It was this experience that convinced me that
my current approach to teaching feedback control was inadequate"
