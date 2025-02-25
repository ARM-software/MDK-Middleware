# HTTP Upload Example

This MDK-Middleware example project implements a web based interface to an SD card attached to a development board.

For detailed description see [HTTP Upload Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/HTTP_Upload_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To use this example, connect an evaluation board to a LAN with a router. You can also connect an evaluation board directly to a PC using a direct or crossover network cable.

To test the program, open a web browser on a PC connected to the same LAN, and enter the following address:

```http
http://my_host
```

Login with the following default credentials:

- user: admin
- password: \<none\>
