
## Preparing the Image

+ https://wiki.archlinux.org/index.php/Raspberry_Pi
+ https://wiki.archlinux.org/index.php/General_recommendations#System_administration





## Initial Access via SSH


### via graveslab-router

192.168.1.1

router_admin
GravesLab

we're good!

WAN 169.229.198.91
255.255.255.128
169.229.198.1
8.8.8.8
8.8.4.4

start IP 192.168.1.1

HTTP
10022

D105 keys are in there!
SSH successful with root.


D105	          192.168.1.147	BC:EE:7B:88:43:4D	1 day 00:00:00	
WIN-OAFOK856TMT	192.168.1.125	74:DA:38:5B:E6:40	1 day 00:00:00	
bcurtis-pi3	    192.168.1.127	B8:27:EB:01:C1:17	1 day 00:00:00	
alarm	          192.168.1.118	B8:27:EB:BE:36:E0	1 day 00:00:00	


## Users and Groups

passwd

usermod -l brandon alarm
usermod -d /home/brandon -m brandon
groupadd sudo
usermod -aG sudo brandon


## Package Manager Setup

update Arch mirror list
https://wiki.archlinux.org/index.php/mirrors

upgrade everything: pacman -Syyu

### Basic Packages

sudo pacman -S tmux autossh


### ssh

ssh-keygen

copy public keys to various places
copy public keys from various places to authorized_keys

set up autossh
configure sshd
nano /etc/ssh/sshd_config 


### virtualenvwrapper

https://wiki.archlinux.org/index.php/Python/Virtual_environment#virtualenvwrapper

export WORKON_HOME=~/.virtualenvs
source /usr/bin/virtualenvwrapper.sh

sudo pacman -S python-virtualenvwrapper

mkvirtualenv plasma

sudo pacman -S gcc gcc-fortran binutils
pip install ipython
pip install numpy
pip install matplotlib
pip install cv2 (nah, doing this from source!)


## spi and i2c devices

+ https://wiki.archlinux.org/index.php/udev
+ http://blog.chrysocome.net/2012/11/raspberry-pi-i2c.html (full walkthrough on rpi, 2012)

When you attach a device, you should be able to see it here: i2cdetect -y 1

Unless you want to write everything as root, you gotta set permissions after you set this up:

this solution worked for me (2017-04-25): https://www.raspberrypi.org/forums/viewtopic.php?p=238003

How the heck does udev uaccess work?

it works like this:
+ https://wiki.archlinux.org/index.php/Talk:Udev#Use_of_.27uaccess.27_instead_of_GROUP_and_MODE.3F
+ https://wiki.debian.org/USB/GadgetSetup
+ http://sigrok.org/bugzilla/show_bug.cgi?id=665

"libsigrok uses the obsolete group mechanism in udev rules: assigns device files to be R/W by group plugdev, which does not exist on Fedora. The correct action is to use systemd session mechanism that tags the devices as 'uaccess', which then creates ACLs enabling R/W for the current logged-in console user."

"You say that this 'uaccess' is the new generic way of doing things, but there's absolutely no documentation about this idea anywhere, not even from Fedora who are now apparently enforcing it. Which other distributions will it work on? How are we supposed to know?"

sudo udevadm control --reload
sudo udevadm trigger



TAG+="uaccess" only appears to give access to local users?  I'm just going to use an "i2c" group and MODE=0660.

udevadm monitor -u
udevadm test ...

---

trying something different!

sudo pacman -S i2c-tools

The following are all confirmed to work:

SUBSYSTEM=="i2c-dev", MODE="0666"
SUBSYSTEM=="i2c-dev", GROUP="i2c" MODE="0660"


### Scientific Computing Packages

General References
+ http://wyolum.com/numpyscipymatplotlib-on-raspberry-pi/
+ http://geoffboeing.com/2016/03/scientific-python-raspberry-pi/




#### OpenCV

sudo pacman -S base-devel make cmake pkg-config

remove everything that requires matplotlib locally?

install cv2:

https://github.com/opencv/opencv

git clone git@github.com:/opencv/opencv
git checkout 2.4.13.2

actually, this seems to indicate that OpenCV v3 provides the `cv2` Python module I need:
https://breakthrough.github.io/Installing-OpenCV/

git checkout 3.2.0

  281  mkdir build
  282  cd build/

----

https://bbs.archlinux.org/viewtopic.php?id=39935
"Arch does not have -dev packages, we ship headers right with the regular package."

cmake should let me know if something is not installed:

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..

(and yes it does! but it still completes even with all of the missing stuff)


#### OpenCV Dependencies

I found this list of dependencies:

sudo apt-get install 
cmake build-essential pkg-config 
libgtk2.0-dev libtbb-dev python-dev python-numpy python-scipy 
libjasper-dev libjpeg-dev libpng-dev libtiff-dev 

libavcodec-dev libavutil-dev libavformat-dev 
libswscale-dev libdc1394-22-dev libv4l-dev

translated to Arch:

sudo pacman -S ffmpeg

(accepted defaults for ffmpeg library providers)

pip install numpy
pip install scipy


#### OpenBLAS, Lapack

scipy needs lapack/blas - [AUR: openblas-lapack 0.2.19-1](https://aur.archlinux.org/packages/openblas-lapack/)

sudo pacman -S openblas-lapack

<FAIL> need to install from source!

+ git clone https://aur.archlinux.org/openblas-lapack.git
+ http://www.openblas.net/
+ https://github.com/xianyi/OpenBLAS/wiki/User-Manual
+ https://github.com/xianyi/OpenBLAS/wiki/Installation-Guide

git clone https://aur.archlinux.org/openblas-lapack.git

this is giving me an empty repo...

Actually this is how the AUR ("Arch User Repository") works!
+ https://arashmilani.com/post?id=85
+ https://wiki.archlinux.org/index.php/Arch_User_Repository#Installing_packages

"Run makepkg -si in the directory where the files are saved. This will download the code, resolve the dependencies with pacman, compile it, package it, and install the package."

Additional references in case I need them:
+ [https://verahill.blogspot.com/2013/02/336-compiling-atlas-netblas-lapack-and.html](Compiling ATLAS, netblas, lapack and openblas on Arch Linux)
+ [Arch Linux ARM: ATLAS BLAS&LAPACK Package](https://archlinuxarm.org/forum/viewtopic.php?f=4&t=6007)

"The Automatically Tuned Linear Algebra Software (ATLAS) provides access to optimised Basic Linear Algebra Subprograms (BLAS) and Linear Algebra PACKage (LAPACK ) functions. Optimisation of BLAS and LAPACK functions for specific CPUs can result in substantial reduction of the time it takes to complete calculations. Common applications making use of these libraries are programmes like NumPy, SAGE, R and the like."


#### Numpy and Scipy

+ https://github.com/scipy/scipy/releases
+ [https://stackoverflow.com/questions/23325222/correct-way-to-point-to-atlas-blas-lapack-libraries-for-numpy-build](Correct way to point to ATLAS/BLAS/LAPACK libraries for numpy build?)

Try running commands like this to get information about Atlast, Lapack, &etc:

>>> import numpy.distutils.system_info as si
>>> si.get_info('atlas')
{}
>>> si.get_info('lapack')
{'libraries': ['lapack', 'lapack'], 'library_dirs': ['/usr/lib'], 'language': 'f77'}
>>> si.get_info('blas')
{'libraries': ['cblas', 'blas'], 'library_dirs': ['/usr/lib'], 'language': 'c', 'define_macros': [('HAVE_CBLAS', None)]}

+ [SciPy: Building From Source on Linux](https://www.scipy.org/scipylib/building/linux.html)
+ https://github.com/scipy/scipy/blob/master/INSTALL.rst.txt

This is really struggling to install SciPy.

"Taking a break" from failing to install Cython and see if that helps.

A great guide that builds OpenCV from source in about an hour:
+ http://www.pyimagesearch.com/2016/04/18/install-guide-raspberry-pi-3-raspbian-jessie-opencv-3/















## RPi.GPIO for Python

+ https://pypi.python.org/pypi/RPi.GPIO
+ https://sourceforge.net/p/raspberry-gpio-python/wiki/BasicUsage/
+ https://sourceforge.net/p/raspberry-gpio-python/wiki/install/
+ https://learn.sparkfun.com/tutorials/raspberry-gpio/python-rpigpio-api
+ https://learn.sparkfun.com/tutorials/raspberry-gpio/python-rpigpio-example

Get this to work without root!
+ https://raspberrypi.stackexchange.com/questions/40105/access-gpio-pins-without-root-no-access-to-dev-mem-try-running-as-root/40106
  - Check that /dev/gpiomem has the correct permissions.
  - sudo adduser pi gpio




## BASH

history | cut -c 8-

