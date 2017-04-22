#!/usr/bin/env python3.5

import asyncio
import serial_asyncio
import concurrent.futures

async def echo_server(loop):
  reader1,writer1 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM0', baudrate=9600)
  reader2,writer2 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM1', baudrate=9600)
  reader3,writer3 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM2', baudrate=9600)
  reader4,writer4 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM3', baudrate=9600)
  reader5,writer5 = await serial_asyncio.open_serial_connection(url='/dev/ttyACM4', baudrate=9600)
  while True:
    # returns two sets of futures: (done, pending)
    done, pending = await asyncio.wait([gotmessage(reader1, 'ar1'), 
                                        gotmessage(reader2, 'ar2'),
                                        gotmessage(reader3, 'ar3'),
                                        gotmessage(reader4, 'ar4'),
                                        gotmessage(reader5, 'ar5')],
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
