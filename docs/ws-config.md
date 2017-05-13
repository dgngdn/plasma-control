

## Websocket Configuration

IP    169.229.198.50
Port  8765

reverse-SSH'd to:
IP    brandoncurtis.com
Port  10015


/usr/bin/autossh -M 0 -o ExitOnForwardFailure=yes -o ServerAliveInterval=30 -o ServerAliveCountMax=3 -o UserKnownHostsFile=/etc/ssh/ssh_known_hosts -NR 10015:localhost:8765 brandon@brandoncurtis.com -p 22 -i /home/brandon/.ssh/id_rsa -v



https://hashrocket.com/blog/posts/development-of-a-simple-command-line-websocket-client



