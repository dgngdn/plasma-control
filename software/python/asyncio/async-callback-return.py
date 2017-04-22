#!/usr/bin/env python3.5

import asyncio
import serial.aio

class BridgeServer:

  class MyProtocol(asyncio.Protocol):
  
    def __init__(self, bridge_server, state):
      self.bridge = bridge_server
      self.buffer = ""
      self.transport = None
      self.state = state

    def connection_made(self, transport):
      self.transport = transport
      self.bridge.connections[self.state] = self.transport
      print("port opened\n", transport)
      transport.serial.rts = False
    
    def data_received(self, data):
      self.buffer += data.decode('ascii')
      if "\n" in self.buffer:
        msg = self.buffer
        self.buffer = ""
        print("data received:", msg)
        if self.state == "input":
          self.bridge.send(msg)
     
    def connection_lost(self, exc):
      print("port closed")
      asyncio.get_event_loop().stop()
      
  def __init__(self, port_in, port_out, loop):
    self.loop = loop
    self.port_in = port_in
    self.port_out = port_out
    self.connections = {}
    self.server = asyncio.gather(serial.aio.create_serial_connection(loop, lambda: self.MyProtocol(self,"input"), self.port_in, baudrate=9600),
                                 serial.aio.create_serial_connection(loop, lambda: self.MyProtocol(self,"output"), self.port_out, baudrate=9600))
  
  def send(self, msg):
    self.connections["output"].write("yeah we got it\n".encode('utf8'))
    print("date sent:", msg)

loop = asyncio.get_event_loop()
bridge = BridgeServer("/dev/ttyACM0", "/dev/ttyACM1", loop)
loop.run_until_complete(bridge.server)
try:
  loop.run_forever()
finally:
  loop.close()
