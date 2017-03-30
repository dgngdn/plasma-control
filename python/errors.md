
## oscilloscope.py

Traceback (most recent call last):
  File "oscilloscope.py", line 127, in <module>
    data = read_from_channel(channel,preambles[str(channel)])
  File "oscilloscope.py", line 63, in read_from_channel
    ydata = instr.query_ascii_values(":WAVEFORM:DATA?",separator=wave_clean,container=np.array)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 455, in query_ascii_values
    block = self.read()
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 332, in read
    message = self.read_raw().decode(enco)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 306, in read_raw
    chunk, status = self.visalib.read(self.session, size)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/highlevel.py", line 253, in read
    return self.sessions[session].read(count)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/usb.py", line 102, in read
    usb.USBError)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/sessions.py", line 313, in _read
    current = reader()
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/usb.py", line 96, in <lambda>
    return self._read(lambda: self.interface.read(1),
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/protocols/usbtmc.py", line 311, in read
    response = BulkInMessage.from_bytes(resp)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/protocols/usbtmc.py", line 95, in from_bytes
    assert msgid == MsgID.dev_dep_msg_in
AssertionError


This bug report has been filed:

usbtmc assertion failure
https://github.com/hgrecco/pyvisa-py/issues/20

----

Rigol firmware
http://beyondmeasure.rigoltech.com/acton/fs/blocks/showLandingPage/a/1579/p/p-0019/t/page/fm/0

DSRemote, someone's remote visualiation software:
http://www.teuniz.net/DSRemote/
(no idea how it works)


----


http://www.eevblog.com/forum/testgear/rigol-usbtmcvisa-interface-is-really-terrible/

http://www.teuniz.net/DSRemote/
They note the kernel patch needed.

I had to change the MTU size to avoid the TCP/IP packets from being split making it too slow.

"This is what helped: Upgrading from 00.04.03.00.01 (2015-05-05) to 00.04.03.01.05 (2015-06-16). That made USBTMC finally working with my Linux machine!  *update*:  and for the record: all this is with the unmodified usbtmc.c kernel module shipping with the current kernel version 4.1."

"Just a note of caution, if you are thinking of upgrading to firmware 00.04.03.02.03:

I had been struggling to get USBTMC to work with my DS1000Z, then I upgraded to firmware 00.04.03.01.05 and all the problems went away :)  However, today I upgraded to 00.04.03.02.03 and the USBTMC is broken again :(  Simple commands still work but not fetching waveform data from memory. I am using Alex Forencich's Python USBTMC library."

response: "I guess there's something wrong with that python lib. Here it works fine with usbtmc and latest firmware."

----

Is there a way to set the timeout on pyvisa instrument objects?
  YES
  my_device.timeout = 25000 # 25 seconds
Is there a way to change the data chunk size?
  YES
  my_instrument.chunk_size = 102400 # 100kB


Timeouts are NOT working with usbtmc devices in pyvisa using the pyvisa-py backend:
  (with the timeout REMOVED it actually recovers itself after a minute sometimes!)

current time: 2017-03-28_191124.199255
saving /home/brandon/repos/bc-plasmacontrol/exp/scripts/data/oscilloscope/2017-03-28_191124.199255_chan1.csv
saving /home/brandon/repos/bc-plasmacontrol/exp/scripts/data/oscilloscope/2017-03-28_191124.199255_chan2.csv
saving /home/brandon/repos/bc-plasmacontrol/exp/scripts/data/oscilloscope/2017-03-28_191124.199255_chan3.csv
saving /home/brandon/repos/bc-plasmacontrol/exp/scripts/data/oscilloscope/2017-03-28_191124.199255_chan4.csv
DONE.
current time: 2017-03-28_191129.355014
^CTraceback (most recent call last):
  File "oscilloscope.py", line 143, in <module>
    data = read_from_channel(instr,channel,preambles[str(channel)])
  File "oscilloscope.py", line 75, in read_from_channel
    ydata = instr.query_ascii_values(":WAVEFORM:DATA?",separator=wave_clean,container=np.array)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 455, in query_ascii_values
    block = self.read()
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 332, in read
    message = self.read_raw().decode(enco)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa/resources/messagebased.py", line 306, in read_raw
    chunk, status = self.visalib.read(self.session, size)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/highlevel.py", line 253, in read
    return self.sessions[session].read(count)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/usb.py", line 102, in read
    usb.USBError)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/sessions.py", line 313, in _read
    current = reader()
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/usb.py", line 96, in <lambda>
    return self._read(lambda: self.interface.read(1),
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/protocols/usbtmc.py", line 309, in read
    resp = raw_read(recv_chunk)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/pyvisa-py/protocols/usbtmc.py", line 217, in read
    data = array_to_bytes(self.usb_recv_ep.read(size, self.timeout))
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/usb/core.py", line 402, in read
    return self.device.read(self, size_or_buffer, timeout)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/usb/core.py", line 988, in read
    self.__get_timeout(timeout))
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/usb/backend/libusb1.py", line 833, in bulk_read
    timeout)
  File "/home/brandon/dev/science/local/lib/python2.7/site-packages/usb/backend/libusb1.py", line 933, in __read
    timeout)
KeyboardInterrupt

and these print constantly while it's running:
[31120.258160] usb 1-1.4: usbfs: usb_submit_urb returned -121

----

when auto-loading the usbtmc driver on boot, the oscilloscope isn't added as `/dev/usbtmc0`

unplug and replug:

[31289.166825] usb 1-1.4: USB disconnect, device number 8
[31301.690148] usb 1-1.4: new high-speed USB device number 9 using dwc_otg
[31301.790565] usb 1-1.4: config 1 interface 0 altsetting 0 endpoint 0x81 has an invalid bInterval 80, changing to 10
[31301.790588] usb 1-1.4: config 1 interface 0 altsetting 0 bulk endpoint 0x82 has invalid maxpacket 64
[31301.790604] usb 1-1.4: config 1 interface 0 altsetting 0 bulk endpoint 0x3 has invalid maxpacket 64
[31301.791087] usb 1-1.4: New USB device found, idVendor=1ab1, idProduct=04ce
[31301.791103] usb 1-1.4: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[31301.791128] usb 1-1.4: Product: DS1000Z Series
[31301.791141] usb 1-1.4: Manufacturer: Rigol Technologies. 
[31301.791153] usb 1-1.4: SerialNumber: DS1ZA170603287

now `/dev/usbtmc0` appears!


