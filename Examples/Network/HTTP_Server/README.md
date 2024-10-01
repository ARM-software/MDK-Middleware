HTTP Server Example
===========

This MDK-Middleware example implements a compact Web-Server on a device that can be accessed from a computer via network interface.

For detailed description see [FTP Server Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/Network/HTTP_Server_Example.html).

Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

To use this example, connect an evaluation board to a LAN with a router. You can also connect an evaluation board directly to a PC using a direct or crossover network cable.

To test the program, open a web browser on a PC connected to the same LAN, and enter the following address:

```
http://my_host  
```

Login with the following default credentials:

 - user: admin
 - password: \<none\>
