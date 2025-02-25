# SMTP Client Example

This MDK-Middleware example project implements an **SMTP client**. It shows you how to send e-mails from an embedded application.

For detailed description see [SMTP Client Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/Network/SMTP_Client_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

To use this example, connect the evaluation board to a LAN with a router. The example will automatically configure the network parameters.

For the example to work, you must change *SMTP_Client.c*

- Change the content of the email information in the SMTP email descriptor structure
   (*NET_SMTP_MAIL mail*):
  - Email addresses: *From*, *To*, *Cc*, *Bcc*
  - Subject and the message (body) of the email
  - Optional attachments and encoding type
- Change the information about the SMTP server and the user credentials in the
   SMTP Mail Transfer Agent Descriptor structure (*NET_SMTP_MTA Server*):
  - Server name/IP
  - Server port
  - User name and password
- Provide an Internet network connection
