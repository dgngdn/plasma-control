## Building from Source

Currently the only supported compilers are GCC on linux and windows and clang on OS X.

Some dependencies must be satisfied for the correct installation of KiCad:

wxWidgets            >= 3.0.0           http://www.wxwidgets.org/

CMake                >= 2.8.4           http://www.cmake.org/

Boost C++ Libraries: >= 1.54            http://www.boost.org/
    files used by kicad are autmatically downloaded and patched if needed
    from boost site.

OpenGL               >= 2.1
  Linux:   Mesa 3D Graphics Library     http://www.mesa3d.org/
  Windows: built-in

Zlib Compression Library                http://www.zlib.net/

cairo                                   http://cairographics.org/

GLEW                                    http://glew.sourceforge.net/

libcurl                                 http://curl.haxx.se/

OpenMP               (optional)         http://openmp.org/

GLM                  >= 9.5.4           http://glm.g-truc.net/

pkg-config                              http://pkgconfig.freedesktop.org/

Doxygen              (optional)         http://www.stack.nl/~dimitri/doxygen/index.html

python               >= 2.6 (optional)  http://python.org/

wxPython             (optional)         http://www.wxpython.org/

SWIG                 >= 3.0 (optional)  http://swig.org/

libngspice           (optional)         http://ngspice.sourceforge.net/

OCE                  >= 0.16 (optional) https://github.com/tpaviot/oce

For Ubuntu 16.04:

sudo apt install libngspice-kicad libglm-dev liboce-foundation-dev liboce-modeling-dev swig doxygen pkg-config libcurl3 cmake libboost-dev zlibc libglu1-mesa-dev libglew-dev libcairo2-dev libwxgtk2.8-dev libwxgtk2.8-dbg

mkvirtualenv kicad
pip install wxpython

----

libngspice-kicad is in the nightly builds PPA, http://ppa.launchpad.net/js-reynaud/ppa-kicad

GLM is OpenGL Mathematics, https://github.com/g-truc/glm
https://launchpad.net/ubuntu/+source/glm
libglm-dev

OCE is Open CASCADE Community Edition, https://github.com/tpaviot/oce
C++ 3D modeling library
https://launchpad.net/ubuntu/+source/oce
liboce-foundation-dev

doxygen
http://www.stack.nl/~dimitri/doxygen/download.html
https://github.com/doxygen/doxygen

pkg-config
http://pkgconfig.freedesktop.org/

zlib
https://packages.ubuntu.com/source/trusty/zlib

OpenMP
http://openmp.org/
"You don't install OpenMP. OpenMP is a feature of the compiler. Check that the compiler you are using implements OpenMP, which is an API for parallel programming in C/C++/Fortran. Check with the OpenMP website, openmp.org for a list of compilers and platforms that support OpenMP programming."
via: https://askubuntu.com/questions/144352/how-can-i-install-openmp-in-ubuntu

GLEW
http://glew.sourceforge.net/
OpenGL Extension Wrangler Library
https://launchpad.net/ubuntu/+source/glew

cairo
http://cairographics.org/

wxWidgets
http://www.wxwidgets.org/

libcurl
https://askubuntu.com/questions/78183/installing-curl-h-library
apt-get install libcurl4-gnutls-dev
apt-get install libcurl4-openssl-dev
depending on whether you want to use gnutls or openssl for SSL.



----

## Ubuntu List PPAs

remember apt-cache search!

`grep ^ /etc/apt/sources.list /etc/apt/sources.list.d/*`

Looks like I'm using the kicad-4 PPA:

    /etc/apt/sources.list.d/js-reynaud-ubuntu-kicad-4-xenial.list:deb http://ppa.launchpad.net/js-reynaud/kicad-4/ubuntu xenial main
    /etc/apt/sources.list.d/js-reynaud-ubuntu-kicad-4-xenial.list:# deb-src http://ppa.launchpad.net/js-reynaud/kicad-4/ubuntu xenial main
    /etc/apt/sources.list.d/js-reynaud-ubuntu-kicad-4-xenial.list.save:deb http://ppa.launchpad.net/js-reynaud/kicad-4/ubuntu xenial main
    /etc/apt/sources.list.d/js-reynaud-ubuntu-kicad-4-xenial.list.save:# deb-src http://ppa.launchpad.net/js-reynaud/kicad-4/ubuntu xenial main

Stable:

sudo add-apt-repository --yes ppa:js-reynaud/kicad-4
sudo apt update
sudo apt install kicad


Nightly builds:

sudo add-apt-repository --yes ppa:js-reynaud/ppa-kicad
sudo apt update
sudo apt install kicad


----

https://askubuntu.com/questions/21460/how-can-i-install-firefox-4-nightlies-and-firefox-3-6-stable-on-the-same-system/22094

What you want to do is to pin firefox so it won't update from the PPA (but firefox-4.0 will).

Create the file /etc/apt/preferences.d/ubuntu-mozilla-daily-pin-400 and add this:

Package: *
Pin: release o=LP-PPA-ubuntu-mozilla-daily
Pin-Priority: 400
Now, the PPA will have lower priority than the official repositories, so firefox will always check from the official repositories, but firefox-4.0, which is only on the PPA, will be updated from it.

https://help.ubuntu.com/community/PinningHowto


/etc/apt/preferences.d/js-reynaud-ppa-kicad-4-pin-400

Verify the pinning priority:

brandon@bcurtis-desktop-01:~/repo/kicad$ sudo apt-cache policy kicad
kicad:
  Installed: 4.0.6+e0-6349~53~ubuntu16.04.1
  Candidate: 4.0.6+e0-6349~53~ubuntu16.04.1
  Version table:
 *** 4.0.6+e0-6349~53~ubuntu16.04.1 400
        400 http://ppa.launchpad.net/js-reynaud/kicad-4/ubuntu xenial/main amd64 Packages
        100 /var/lib/dpkg/status
     4.0.2+dfsg1-4 500
        500 http://us.archive.ubuntu.com/ubuntu xenial/universe amd64 Packages

Unfortunately they call it 'kicad' in both the stable and the nightly buid channels, so it's not possible to run both!

You have to build from source.
