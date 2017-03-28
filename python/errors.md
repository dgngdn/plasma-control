
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

