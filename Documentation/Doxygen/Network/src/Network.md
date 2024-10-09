# Overview {#mainpage}

The **Network Component** contains services, protocol sockets, and physical communication interfaces for creating IPv4
and IPv6 networking applications.

![Network Component Structure](Network.svg)

The various \ref netServices "services" provide program templates for common networking tasks:

- The \ref netHTTP_Func supports static and dynamic page content using \ref ws_script_langugage "scripting",
  \ref ws_ajax "AJAX" and \ref ws_soap "SOAP" technologies. Web resource files are accessed either via the File System (for
  component **Web Server**) or are directly stored in ROM (for component **Web Server Compact**). Using the
  \ref secure_communication "ARM mbed TLS" component, you can enable secure communication for both web servers using HTTPS.
- **FTP** \ref netFTPs_Func "server"/\ref netFTPc_Func "client" and **TFTP**
  \ref netTFTPs_Func "server"/\ref netTFTPc_Func "client" are used for file transfer. TFTP supports only file upload and
  download and is typically used for boot-loading remote devices in a closed LAN. It has very low memory requirements. FTP
  provides full file access including sub-directories, log-in and a set of manipulation commands.
- The \ref netTELNET_Func realizes a simple Command Line Interface (**CLI**) over an IP network.
- Devices are often monitored using the Simple Network Management Protocol (SNMP). The \ref netSNMP_Func helps to report
  vital information of a device to the network manager.
- The \ref netDNSc_Func resolves domain names like [www.arm.com](https://www.arm.com) to the respective IP address. It makes
  use of a freely configurable name server.
- The \ref netSNTPc_Func synchronizes clocks and enables a device to get an accurate time signal over the data network.
- The \ref netSMTPc_Func helps you to send out for example status updates or alerts via e-mail to freely configurable e-mail
  addresses. Using the \ref secure_communication "ARM mbed TLS" component, you can enable secure communication and send e-mails
  using publicly available services like Gmail, Yahoo, etc.

All services rely on a network \ref netSockets "socket" for communication. The Network Component supports these sockets:

- \ref netTCP_Func "TCP" is a connection-oriented, reliable full-duplex protocol. This means that packets delivered using TCP
  are error-checked and delivered in the right order.
- \ref netUDP_Func "UDP" is a transaction-oriented and stateless protocol. Data streaming or time-critical applications
  commonly use UDP. With UDP you can multicast a datagram to a group of devices.
- The \ref netBSD_Func API is a programming interface, which implements a standard Berkeley Socket communication interface
  (using TCP and UDP sockets for communication).

The **physical** \ref netInterfaces "interface" can be either Ethernet, WiFi or a serial connection.

- \ref netETH_Func is widely used for connecting local area networks (LAN). It can utilize different media as means of
  transmission.
- The **serial connection** supports two different protocols:
  - The \ref netSLIP_Func implements the Internet Protocol over a serial connection.
  - The \ref netPPP_Func connects two network devices directly and provides authentication.
- \ref netWiFi_Func is often used for wireless networking of local networks or to connect to the Internet of Things.

A **driver** provides the interface to the microcontroller peripherals or external components (for example an external PHY chip
or WiFi module).

- For Ethernet an **Ethernet MAC** and an **Ethernet PHY** is required.
- PPP or SLIP interfaces use a **USART** and a \ref netDS_Modem "Modem".
- For WiFi interface a **WiFi module** is required.

\note Usually, WiFi chips and modules have their own networking stack incorporated. This means that payload data is sent via a
serial interface to the WiFi chip/module and the Ethernet frames are assembled inside. If you intend to use the
MDK-Middleware Network component with a WiFi chip/module, make sure that its WiFi driver supports the
[bypass mode](https://arm-software.github.io/CMSIS_5/develop/Driver/html/group__wifi__bypass__gr.html)
(currently only available for the [Qualcomm QCA400x](https://www.keil.arm.com/packs/qca400x_wifi_driver-mdk-packs/versions/)).
This mode allows to send the Ethernet frames assembled by the Network component transparently through the WiFi chip/module.
Some devices may support transparent mode at a higher level, such as TCP - this will not work with the Network component.

For mass production, the \ref nw_runtime_config option can be used to assign individual network addresses to every device
that is produced. Unique identifiers are required for proper network operation.

An \ref RFC_list "list of implemented RFCs" is also available.

## Documentation Structure {#doc_structure}

This user's guide contains the following chapters:

- \ref rev_hist lists the changes made in each version of the Network Component.
- \ref create_app explains the necessary steps to develop a network project from scratch.
- \ref troubleshoot shows best practice when debugging general network/connection issues.
- \ref secure_communication shows how to use **mbed TLS** together with the Network Component to secure your communication
  channels. It also explains how to create your own SSL/TLS certificates.
- \ref examples is a good starting point for implementing your own networking device.
- The chapter \ref cyber_security explains how to test the TCP/IP stack with industry-standard tools against common
  vulnerabilities.
- \ref function_overview list provides a brief overview of all Network Component functions.
- \ref resource_requirements describes the resource requirements of the Network Component.
- \ref migration_guide section contains information how to migrate from previous versions to the Network Component v8.
- <a class=el href="./modules.html">API Reference</a> describes the functional interface of the Network Component in detail.
