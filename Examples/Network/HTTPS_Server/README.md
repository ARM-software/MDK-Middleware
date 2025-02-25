# HTTPS Server Example

This MDK-Middleware example project implements a secure compact Web-Server on a device that can be accessed from a computer via network interface.

For detailed description see [HTTPS Server Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/HTTPS_Server_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To use this example, connect an evaluation board to a LAN with a router. You can also connect an evaluation board directly to a PC using a direct or crossover network cable.

To test the program, open a web browser on a PC connected to the same LAN, and enter the following address:

```http
https://my_host
```

Login with the following default credentials:

- user: admin
- password: \<none\>

> **Note for Windows users**
>
> - You must add the test CA certificate **ca.crt** to the Trusted Root Certificate store on your computer
    to avoid the *Your connection is not private* error in the Chrome or Microsoft Edge browser.
> - You need administrator rights for adding a certificate.
> - To install a **ca.crt** certificate on your computer, double-click on it and select
    **Install Certificate...**, then select **Local Machine** for the store location.
> - Select **Trusted Root Certification Authorities** as the location for the certificate.
