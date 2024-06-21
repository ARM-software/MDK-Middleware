This program is an example of a **BSD client**. It shows you how to control the LEDs
on the evaluation board of the server via the BSD socket.

To run this example, you must use the automatically assigned IPv6 address or set
the IPv4 address to match your local network.

To test this example, you also need a server application that you can connect to.
Download a BSD server and start it on another evaluation board connected to 
the same LAN. When BSD client is running, the LED diodes on both evaluation boards
should light up synchronously.

>The IPv4 address and MAC address in this example must be different from the IPv4
and MAC addresses on the BSD server.
