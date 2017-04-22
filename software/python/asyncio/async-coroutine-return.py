#!/usr/bin/env python3.5

import asyncio
import serial_asyncio
import concurrent.futures

async def echo_server(loop):
  reader,writer = await serial_asyncio.open_serial_connection(url='/dev/ttyACM0', baudrate=9600)
  reader2,writer2 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM1', baudrate=9600)
  while True:
    # returns two sets of futures: (done, pending)
    done, pending = await asyncio.wait([gotmessage(reader, 'ar1'), 
                                        gotmessage(reader2, 'ar2')],
                                        return_when=concurrent.futures.FIRST_COMPLETED)
    for task in done:
      print(task.result())
    for task in pending:
      task.cancel()
    #print("from {}: {}".format(src,message))

  
async def gotmessage(reader,src):
  data = await reader.readline()
  message = data.decode()
  return message,src
  
  

loop = asyncio.get_event_loop()
loop.run_until_complete(echo_server(loop))
loop.run_forever()
