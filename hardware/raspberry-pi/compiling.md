https://github.com/Yadoms/yadoms/wiki/Cross-compile-for-raspberry-PI

sudo apt-get install git cmake libssl-dev libgnutls-dev libopencv-gpu-dev autoconf automake libtool curl make g++ unzip

nano ~/.bashrc

        export PATH=$PATH:$HOME/raspberry/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin

source ~/.bashrc

        arm-linux-gnueabihf-gcc --version

jumping to this guide:
http://hackaday.com/2016/02/03/code-craft-cross-compiling-for-the-raspberry-pi/

sudo apt-get install gdb-multiarch

had to make afew changes to the written stuff:

arm-linux-gnueabihf-g++ -O3 -g3 -Wall -c -fPIC -o "hello.o" "hello.cpp"


----

Let's try debtab to convert .deb to Arch package...

https://arashmilani.com/post?id=85

https://aur.archlinux.org/packages/debtap/

https://www.maketecheasier.com/install-deb-package-in-arch-linux/


Let's use this .deb, I guess:
http://packages.ubuntu.com/precise/python/python3-scipy
http://packages.ubuntu.com/xenial/python/python3-scipy
http://ports.ubuntu.com/ubuntu-ports/pool/universe/p/python-scipy/

http://ports.ubuntu.com/ubuntu-ports/pool/universe/p/python-scipy/python3-scipy_0.18.1-2_armhf.deb

okay.... debtap is not for ARM, and debtaparm is not ready for production.

----

oh fuck it, I'm just going to install python-scipy and figure out how to link it into the virtualenv

http://stackoverflow.com/questions/14571454/virtualenv-specifing-which-packages-to-use-system-wide-vs-local

$ ln -s /usr/lib/python2.7/dist-packages/PIL* $VIRTUAL_ENV/lib/python*/site-packages

