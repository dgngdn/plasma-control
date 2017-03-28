# Troubleshooting the Raspberry Pi3

## Always Boots Into Emergency Mode

The Pi was probably shut down incorrectly and it damaged the filesystem a little bit.

To fix via [RPi forums][emode]:

1. login as root in emergency mode
2. umount /dev/mmcblk0p2
3. e2fsck -f -y -v -C 0 /dev/mmcblk0p2
4. reboot

This was worked beautifully on 2017-03-27.

[emode]: https://www.raspberrypi.org/forums/viewtopic.php?f=56&t=124149

## USBTMC devices don't work

Linux has a kernel driver for USBTMC devices, but it does not come precompiled in the Ubuntu Mate RPi kernel.

You'll need to compile it yourself and configure it to load on boot.

This process is described in detail in the DRIVERS folder.
