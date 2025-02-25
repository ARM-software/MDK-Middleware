# BSD Server Example

This MDK-Middleware example project implements a **BSD Server**. You can control the LEDs on the server board using requests from BSD clients.

For detailed description see [BSD Server Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/BSD_Server_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To run this example, you must use the automatically assigned IPv6 address or set the IPv4 address to match your local network, otherwise the server cannot be reached. This is because the requests for non-LAN hosts are forwarded to the default gateway.

Start the Windows application *LEDSwitch.exe* on your PC (part of Keil MDK uVision installation).

Enter the IP address and the port of the server and select the transmission protocol TCP or UDP. If you change the value of P2 in the Windows application, the LED diodes on the evaluation board should light up accordingly.

The example can be used with a BSD client running on a different evaluation board.
