## USBTMC Kernel Module

The USBTMC kernel module is available but does not come compiled for the Raspberry Pi.

Compile it yourself!

Then, load it with `sudo insmod usbtmc.ko`

Verify that `/dev/usbtmc0` appears when a USBTMC device is plugged in!


## Compiling the USBTMC kernel driver for Raspberry Pi

To talk with the oscilloscope and other USBTMC hardware, a USB Test & Measurement Class (USBTMC) 
kernel module ("driver") must be loaded on the Raspberry Pi.  This kernel module is not compiled by
default in the Raspberry Pi Ubuntu Mate image, so you must acquire the source and compile it.

**References**

+ [USBTMC Kernel Driver Documentation](http://www.keysight.com/upload/cmc_upload/All/usbtmc.html?&cc=US&lc=eng)
+ [What is USBTMC and How Can I Communicate with My USB Instrument?](http://digital.ni.com/public.nsf/allkb/32195E6866293D628625743C000C1BAE)
+ [How (recipe) to build only one kernel module?](https://askubuntu.com/questions/515407/how-recipe-to-build-only-one-kernel-module)
+ https://github.com/notro/rpi-source/wiki/Examples-on-how-to-build-various-modules
+ https://github.com/notro/rpi-source/wiki/Examples-on-how-to-build-various-modules#hello-world-example
+ https://github.com/notro/rpi-source/wiki
+ https://www.raspberrypi.org/forums/viewtopic.php?f=66&t=60784&p=469648
+ http://ziemann.web.cern.ch/ziemann/gadget/usbtmc/raspi-usbtmc.html
+ http://scruss.com/blog/2013/12/15/my-raspberry-pi-talks-to-my-oscilloscope/
+ http://bchavez.bitarmory.com/archive/2013/01/16/compiling-kernel-modules-for-raspberry-pi.aspx
+ https://www.raspberrypi.org/forums/viewtopic.php?f=66&t=60784
+ http://code.metager.de/source/xref/raspberrypi/linux/drivers/usb/class/usbtmc.c
+ http://scruss.com/blog/tag/raspberrypi/
+ http://www.eevblog.com/forum/reviews/using-a-raspberry-pi-with-linux-gpib-and-a-beiming-or-agilent-usb-gpib-adapter/25/
+ http://wiki.octave.org/Instrument_control_package#USBTMC
+ https://pypi.python.org/pypi/python-usbtmc
+ https://groups.google.com/forum/#!forum/python-ivi
+ https://github.com/python-ivi/python-usbtmc
+ http://alexforencich.com/wiki/en/python-usbtmc/readme


## Installing the Kernel Module to Load On Boot

+ https://askubuntu.com/questions/299676/how-to-install-3rd-party-module-so-that-it-is-loaded-on-boot

    Edit the /etc/modules file and add the name of the module (without the .ko extension) on its own line.
    On boot the kernel will try to load all the modules named in this file.
    Copy the module to a suitable folder in /lib/modules/`uname -r`/kernel/drivers
    Run `sudo depmod`
    Reboot
    Run `lsmod | grep <module-name>` to confirm that the module was loaded on boot


## how-to for usbtmc-python

http://scruss.com/blog/2013/12/15/my-raspberry-pi-talks-to-my-oscilloscope/

sudo nano /etc/udev/rules.d/usbtmc.rules
Bus 001 Device 011: ID 1ab1:04ce Rigol Technologies 
6833
1230

## Other Random Notes

<pre>
python-ivi/python-usbtmc


00:19:AF:34:3A:F4
00-19-AF-34-3A-F4

Welcome to Web of DS1000Z Series
Information About This Instrument:
Instrument Model:    DS1104Z
Manufacturer:    RIGOL TECHNOLOGIES
Serial Number:    DS1ZA164457681
Description:    rigollan
LXI Class:    LXI Core 2011
LXI Version:    1.4
Host Name:    rigollan.local
MAC Address:    00-19-AF-34-3A-F4
IP Address:    169.229.198.103
Firmware Revision:    00.04.02.SP3
VISA TCP/IP String:    TCPIP::169.229.198.103::INSTR
Auto-MDIX Capable:    NO
VISA USB Connect String:    USB0::0x1AB1::0x4CE::DS1ZA164457681::INSTR

----
http://stackoverflow.com/questions/31564583/usbtmc-in-python

I used this guide:
https://xdevs.com/guide/ni_gpib_rpi/

SUCCESS over VXI11 in Python!

>>> import vxi11
>>> instr = vxi11.Instrument("169.229.198.103")
>>> print(instr.ask("*IDN?"))
RIGOL TECHNOLOGIES,DS1104Z,DS1ZA164457681,00.04.02.SP3
</pre>

## How To: usbtmc-python

http://scruss.com/blog/2013/12/15/my-raspberry-pi-talks-to-my-oscilloscope/

    sudo nano /etc/udev/rules.d/usbtmc.rules
    Bus 001 Device 011: ID 1ab1:04ce Rigol Technologies 
    6833
    1230


