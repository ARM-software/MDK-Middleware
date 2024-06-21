This program is an example of an **SMTP client**. It shows you how to send e-mails
from an embedded application.

Detailed description is available on:  
<https://www.keil.com/pack/doc/mw/Network/html/_s_m_t_p__client__example.html>

To test this example, connect the evaluation board to a LAN with a router.
The example will automatically configure the network parameters.

Note:  
For the example to work, you must change *SMTP_Client.c*
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
