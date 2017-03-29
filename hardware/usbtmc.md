



# What We're Using

## PyVISA-py

https://pyvisa-py.readthedocs.io/
https://github.com/hgrecco/pyvisa-py

"PyVISA-py is a backend for PyVISA. It implements most of the methods for Message Based communication (Serial/USB/GPIB/Ethernet) using Python and some well developed, easy to deploy and cross platform libraries."


## PyVISA

https://pyvisa.readthedocs.io/en/stable/

"PyVISA is a Python package that enables you to control all kinds of measurement devices independently of the interface (e.g. GPIB, RS232, USB, Ethernet)."





# Alternatives

## python-usbtmc

>>> import usbtmc
>>> i = usbtmc.Instrument(0x0957, 0x2007)
>>> i.ask('*IDN?')

It has some code to deal with the "Rigol quirk":

    RIGOL_QUIRK_PIDS = [0x04ce, 0x0588]


## usbtmc.c

http://lxr.free-electrons.com/source/drivers/usb/class/usbtmc.c


It also has some code to deal with the "Rigol quirk":

    125 struct usbtmc_ID_rigol_quirk {
    126         __u16 idVendor;
    127         __u16 idProduct;
    128 };
    129 
    130 static const struct usbtmc_ID_rigol_quirk usbtmc_id_quirk[] = {
    131         { 0x1ab1, 0x0588 },
    132         { 0x1ab1, 0x04b0 },
    133         { 0, 0 }
    134 };


## Rigol DS1000Z IVI Driver for Windows

http://www.rigol.eu/products/digital-oscilloscopes/ds1000z/







