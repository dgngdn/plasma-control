#!/usr/bin/env python3.5

import asyncio
import serial_asyncio

async def echo_server(loop):
  while True:
    reader,writer = await serial_asyncio.open_serial_connection(url='/dev/ttyACM0', baudrate=9600)
    writer.write("hello world!\n".encode())
    print("Sending message!")

    data = await reader.readline()
    message = data.decode()
    print("Received: {!r}".format(message))
    
    writer.write(data)
    await writer.drain()
    print("echoed!")

loop = asyncio.get_event_loop()
loop.run_until_complete(echo_server(loop))
loop.run_forever()
