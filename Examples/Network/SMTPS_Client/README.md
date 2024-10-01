SMTP Client Example
===========

This MDK-Middleware example project implements a **Secure SMTP client**. It shows you how to send e-mails from an embedded application.

For detailed description see [SMTPS Client Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/Network/SMTPS_Client_Example.html).

Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

To use this example, connect the evaluation board to a LAN with a router. The example will automatically configure the network parameters.

Note:  
For the example to work, you must change *SMTPS_Client.c*
- Change the content of the email information in the SMTP email
  descriptor structure (*NET_SMTP_MAIL mail*):
    - Email addresses: *From*, *To*, *Cc*, *Bcc*
    - Subject and the message (body) of the email
    - Optional attachments and encoding type

- Change the information about the SMTP server and the user credentials
  in the SMTP Mail Transfer Agent Descriptor structure (*NET_SMTP_MTA Server*):
    - Server name/IP
    - Server port
    - User name and password

- Provide an Internet network connection
