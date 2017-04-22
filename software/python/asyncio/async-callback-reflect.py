#!/usr/bin/env python3.5

import asyncio
import serial.aio

class MyProtocol(asyncio.Protocol):
  
  def connection_made(self, transport):
    self._buffer = ""
    self._loop = asyncio.get_event_loop()
    self._transport = transport
    print("port opened\n", transport)
    transport.serial.rts = False
    
  
  def data_received(self, data):
    self._buffer += data.decode('ascii')
    if "\n" in self._buffer:
      print("data received: ", self._buffer)
      if self._buffer == "Hello World\n":
        print("acknowledged!")
        self._transport.write("yeah we got it\n".encode('utf8'))
      self._buffer = ""
   
  def connection_lost(self, exc):
    print("port closed")
    self.loop.stop()

loop = asyncio.get_event_loop()
coro = serial.aio.create_serial_connection(loop, MyProtocol, '/dev/ttyACM0', baudrate=9600)
loop.run_until_complete(coro)
try:
  loop.run_forever()
finally:
  loop.close()
