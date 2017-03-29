
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


