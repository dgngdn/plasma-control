#!/usr/bin/env python

import asyncio
import websockets

async def hello(websocket, path):
    while True:
        msg = await websocket.recv()
        print("< {}".format(msg))
        greeting = "Hello {}!".format(msg)
        await websocket.send(greeting)
        print("> {}".format(greeting))

if __name__ == "__main__":
  start_server = websockets.serve(hello, 'localhost', 8765)

  asyncio.get_event_loop().run_until_complete(start_server)
  asyncio.get_event_loop().run_forever()
