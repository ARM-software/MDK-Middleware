# SNMP Agent Example

This MDK-Middleware example project implements an **SNMP Agent**. It shows how to use a Simple Network Management Protocol to control the embedded system.

For detailed description see [SNMP Agent Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/SNMP_Agent_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To use this example, connect an evaluation board to a LAN with a DHCP server (most LANs have one). This example automatically configures the network parameters via a DHCP protocol.

To test the program, run the Windows application *SNMPTest.exe* (part of Keil MDK uVision installation) connected to the same LAN.

Enter the IP address of your embedded SNMP_Agent and click on 'Connect'. Once the connection is established, you can change the LCD text on the target device, control the built-in LED diodes and monitor the status of the built-in push-buttons.
