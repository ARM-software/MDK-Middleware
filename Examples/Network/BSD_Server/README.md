This program is an example of a BSD server. It is used to serve LED control
requests from BSD clients.

To run this example, you must use the automatically assigned IPv6 address or set
the IPv4 address to match your local network, otherwise the server cannot be reached.
This is because the requests for non-LAN hosts are forwarded to the default gateway.

To test this example, start the Windows application "LEDSwitch.exe" on your PC.
Enter the IP address and the port of the server and select the transmission protocol
TCP or UDP. If you change the value of P2 in the Windows application, the LED diodes
on the evaluation board should light up accordingly.

The same example can also be tested with a BSD client on a different evaluation board.
