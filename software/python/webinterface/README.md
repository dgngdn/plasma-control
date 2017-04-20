# Remote Control Server

## Using `websocketd`

This is BY FAR the easiest way to start playing with websockets TODAY.

`websocketd` is created by Joe Walnes: https://github.com/joewalnes/websocketd/

For example, you can pipe the incoming Arduino serial data into a logfile:

`nice -20 cat /dev/ttyACM0 > current.csv &`

then use `websocketd` to serve the data that comes from tailing this logfile:

`websocketd --port=8080 tail -f -n 1000 current.csv`

The serving script can be pretty much anything that writes to STDOUT and reads from STDIN,
in any language of your choice. With a little Python non-blocking reading, I bet you could
even implement a simple chat server!

----

## Using Python `websocket` and `serial-aio`

You'll need to learn some asynchronous programming in Python, but this is a good starting
point for more complex servers.  The Python `websocket` module with async support: https://github.com/aaugustin/websockets

