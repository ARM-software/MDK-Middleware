FTP Server Example
===========

This program is an example of an **FTP server**. It allows you to manage files on your SD card from your PC using an FTP client.

For detailed description see [FTP Server Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/Network/FTP_Server_Example.html).

Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

Connect an evaluation board to a LAN with a router. You can also connect an evaluation board directly to a PC using a direct or crossover network cable.

To access the server, use command console and enter: 

```
ftp://my_host
```

Login with the following default credentials:
 - user: admin
 - password: \<none\>
