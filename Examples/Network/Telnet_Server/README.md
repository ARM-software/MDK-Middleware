# Telnet Server Example

This MDK-Middleware example project implements a **Telnet Server**. It shows you how to set up a command line interface within an embedded application.

For detailed description see [Telnet Server Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/Telnet_Server_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To use this example, connect an evaluation board to a LAN with a router. You can also connect an evaluation board directly to a PC using a direct or crossover network cable.

To run the program, start a Telnet client (for example PuTTY) on your PC connected to the same LAN.

Use following parameters as default to establish a Telnet connection:

- Host name:  `my_host`
- Telnet port: 23
- user: admin
- password: \<none\>

Type *help* to see the available commands or *bye* to close the connection.
