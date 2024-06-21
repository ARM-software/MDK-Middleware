This program is a **Secure Compact Web Server** example.

Detailed description is available on:  
<https://www.keil.com/pack/doc/MW/Network/html/_compact__secure__web__server__example.html>

Use this example to connect an evaluation board to a LAN with a router.
You can also connect an evaluation board directly to a PC using a direct or
crossover network cable.

To test this example, open your web browser and enter the address:  
> https://my_host
>
> Default user: admin  
> Default password: \<none\>

Note for Windows users:  
You must add the test CA certificate **ca.crt** to the Trusted Root Certificate store on your computer
to avoid the *Your connection is not private* error in the Chrome or Microsoft Edge browser.

To install a **ca.crt** certificate on your computer, double-click on it and select **Install Certificate...**,
then select **Local Machine** for the store location. You need administrator rights for this action.
Select **Trusted Root Certification Authorities** as the location for the certificate. 
 
