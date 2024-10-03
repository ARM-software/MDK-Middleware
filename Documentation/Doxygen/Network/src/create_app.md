# Creating a Network Application {#create_app}

The steps to create a microcontroller application that uses TCP/IP communication are:

-# Select \ref nw_RTE_Software_Component_Selection "RTE Components" along with the Network Services that are
   required for your application.
-# \ref nw_Driver_Configuration "Enable the Network Driver and Controller" that is integrated in the microcontroller.
-# Configure the various \ref nw_Device_Configuration "Network Components".
-# Configure the \ref nw_system_resources "system resources" according to the network component's
   \ref resource_requirements.
-# Implement the \ref nw_User_Code_Implementation "User Code" using code templates that are provided for the Network
   Component.
-# \ref nw_debugging "Debug" your application using the built-in mechanisms of the Network Component.
-# Sometimes, it is necessary to change some network settings at \ref nw_runtime_config "runtime".

## RTE Component Selection {#nw_RTE_Software_Component_Selection}

Only a few steps are necessary to complete the RTE Component selection:

-# From the **Network** Component:
  - Select **Network:CORE** that provides the basic functionality required for TCP/IP communication.
  - Select your desired **Network:Interface**. For example, set **Network:Interface:Ethernet** to '1' for enabling
    Ethernet communication.
  - Select the desired Network Services. For example, select **Network:Service:Web Server Compact** to create a simple
    Web Server.
  - Select the desired Network Sockets. For example, select **Network:Socket:TCP** for TCP communication. Usually, you
    will also select **Network:Socket:UDP** for stateless communication.
-# From the **CMSIS Driver** Component:
  - If you do Ethernet communication, select appropriate **CMSIS Driver:Ethernet (API)** or
    **CMSIS Driver:Ethernet MAC (API)** *and* **CMSIS Driver:Ethernet PHY (API)** drivers suitable for your
    application. For PPP or SLIP communication simply select **CMSIS Driver:USART (API):USART**. If you use wireless
    communication, select appropriate **CMSIS Driver:WiFi (API)** driver that supports transparent mode.
-# From the **Device** Component:
  - Additional device specific drivers may be required according to the validation output.
-# From the **CMSIS** Component:
  - Select the **CMSIS:CORE** to provide the core interface to the processor.
  - Select the **CMSIS:RTOS2** that is required for the Networking Component.
-# If you need to \ref secure_communication "secure the communication", select from the **Security** Component:
  - **mbed TLS**

![RTE Component Selection Example](nw_rte_comp_selection.png)

## Network Driver and Controller Configuration {#nw_Driver_Configuration}

The Network Device Driver and the Network Controller of the microcontroller need to be correctly configured. This means:

- The \ref netETH_Func "Ethernet interface" or **UART** (for
  \ref netSLIP_Func "SLIP"/\ref netPPP_Func "PPP") is typically configured in the RTE_Device.h
  configuration file. While this file provides multiple options, it is typically sufficient to enable the Ethernet/UART
  interface. **Note**: Some microcontrollers may require settings that are related to a physical layer interface (PHY).
- The Network Controller of the microcontroller typically needs specific clock settings. Consult the user's guide of the
  microcontroller to understand the requirements. Alternatively, you may copy the setup of an
  \ref examples that is provided for various evaluation boards.

## Ethernet PHY Configuration {#nw_PHY_Configuration}

For Ethernet network communication, usually an external Ethernet PHY is required to interface the physical line to the
digital MAC of the microcontroller device. The MAC usually contains two buses:

-# Data bus: [MII, RMII,](https://en.wikipedia.org/wiki/Media-independent_interface) or Serial
   Network Interface (SNI)
-# Management bus: Serial Management Interface (**SMI**) (using MDC and MDIO)

SMI is used to access the PHY’s internal registers to read the state of the link (up/down), duplex mode, speed, and to
restart auto-negotiation etc. SMI is a serial bus, which allows to connect up to 32 devices. Devices on the bus are accessed
using a 5-bit device address. A default device address is hardware configurable by pin-strapping on the device (some pins are
sampled when a reset is asserted or at power-up).

The device’s internal weak pull-up or pull-down resistors define a default device address. This address can be changed by
connecting strong pull-up or pull-down resistors externally. In this case, the `ETH_PHY_ADDR` in the PHY driver needs to be
changed accordingly to be able to control the PHY and to communicate with it. In µVision use the **Options for Component**
dialog to override the default setting for `ETH_PHY_ADDR`:

![Ethernet PHY Configuration](eth_phy_config.png)

## Network Configuration {#nw_Device_Configuration}

All configuration files for the Network Component are listed in the Project window below the Component Class **Network**.

### Network Core {#nw_Network_Core}

![Net_Config.h Configuration File](net_config_h.png)

The configuration file Network Core `Net_Config.h` contains the setting for the library version to be used in the application.

The following **Variant** options are available:

- **IPv4 only** is a pure IPv4 implementation. This choice saves some system resources if no IPv6 networking is used in the application.
- **IPv4/IPv6 dual stack** implements both IPv4 and IPv6 internet protocols.

The **Local Host Name** specifies a name under which the network device can be accessed on a local area network (**LAN**).
This requires a \ref using_ethernet_interfaces_conf "NetBIOS Name Service" to be enabled in the configuration. This name is
very useful if you don't know the IP address that has been assigned to your device by the DHCP server.

The **Memory Pool Size** specifies the amount of RAM in bytes allocated for the memory pool. The buffers for the network
packets are allocated from this memory pool. Usually, the default value of 12000 bytes is sufficient.

Many \ref netServices "Network Services" are started by the Network Core automatically. If you disable
**Start System Services**, you need to enable/start them at runtime using the dedicated functions for that.

To change the default OS resource settings for the Network Core, use **Core Thread Stack Size**. The default value is 2048 bytes.
The default Core Thread priority is **osPriorityNormal**. You can change this priority by changing the `NET_THREAD_PRIORITY`
defined in this configuration file. This priority must be lower than the priorities of the network interface threads.

### Network Interface {#nw_Network_Interface}

The Network Interface configuration files **Net_Config_Interface_n.h** contain general IP address and other
settings. You also need to specify the hardware driver number that is to be used with the network interface. In case of
Ethernet for example, this is usually 0. If you are using a SLIP or PPP over a serial connection, you need to specify the
hardware driver number of the exact UART you wish to use. All settings for the different interfaces are described here:

- \ref using_ethernet_interfaces_conf "Net_Config_ETH_0.h"
- \ref using_wifi_interfaces_conf "Net_Config_WiFi_0.h"
- \ref using_ppp_interfaces_conf "Net_Config_PPP.h"
- \ref using_slip_interfaces_conf "Net_Config_SLIP.h"

### Network Socket {#nw_Network_Socket}

Usually, the needs of most applications are served by using the default settings for the sockets. Of course, there are
configuration files for all three socket types that are specified in

- \ref using_network_sockets_bsd_conf "Net_Config_BSD.h"
- \ref using_network_sockets_tcp_conf "Net_Config_TCP.h"
- \ref using_network_sockets_udp_conf "Net_Config_UDP.h"

### Network Service {#nw_Network_Service}

The configuration files for all the Network Services are explained in the respective section:

- \ref ws_configuration "Net_Config_HTTP_Server.h"
- \ref using_ftp_server_conf "Net_Config_FTP_Server.h"
- \ref using_ftpc_conf "Net_Config_FTP_Client.h"
- \ref using_tftp_server_conf "Net_Config_TFTP_Server.h"
- \ref using_tftpc_conf "Net_Config_TFTP_Client.h"
- \ref telnet_server_conf "Net_Config_Telnet_Server.h"
- \ref using_snmp_agent_conf "Net_Config_SNMP_Agent.h"
- \ref using_dns_client_conf "Net_Config_DNS_Client.h"
- \ref using_sntp_client_conf "Net_Config_SNTP_Client.h"
- \ref using_smtpc_conf "Net_Config_SMTP_Client.h"

## Security Configuration {#nw_security_config}

The mbed TLS component provides an API for \ref secure_communication "secure communication". When selecting this
software component, the mbedTLS_config.h configuration file is added to the project. The proper usage of this file is out
of the scope of this document. For further information, check the online documentation of
\urlout{mbedTLS}.

## System Resource Configuration {#nw_system_resources}

For proper operation, the Network Component requires some system configuration settings. The requirements are:

- Additional **main stack size** of **512 bytes**.
- Additional two **threads** when Ethernet Interface is enabled (`netCore_Thread` and `netETH_Thread`).
  - If **RTX v5** is used, no changes to the **RTX settings** are required as all resources are statically allocated.

For more information, check the Network Component's \ref resource_requirements section.

## User Code Implementation {#nw_User_Code_Implementation}

Before using the networking communications, the Network Core must be initialized with the function \ref netInitialize.
The function initializes the Network system resources and creates threads and other RTOS objects. The initialization is
usually executed from the **app_main** thread.

The initialization process is different depending on which network interface is used:

- **Ethernet interface**
```c
void app_main_thread (void *argument) {
  (void)argument;

  netInitialize ();

  osThreadExit();
}
```

- **WiFi interface**
```c
void app_main_thread (void *argument) {
  NET_WIFI_CONFIG wifi_config;
  (void)argument;

  netInitialize ();

  // Configure wireless parameters
  wifi_config.ssid       = "GuestAccess";
  wifi_config.password   = "guest";
  wifi_config.security   = netWiFi_SecurityWPA2;
  wifi_config.channel    = 0;
  wifi_config.wps_method = netWiFi_WPS_None;

  // Connect to wireless network
  netWiFi_Activate (0, &wifi_config);

  osThreadExit();
}
```

- **PPP interface**
```c
void app_main_thread (void *argument) {
  (void)argument;

  netInitialize ();

  // Connect to remote PPP server
  netPPP_Connect ("04213372", "Guest", "test")

  osThreadExit();
}
```

### User Code Templates

\urlout{uv4_ca_sourcefiles} files provide access to all functions that are required to communicate over the Network. The available
functions are explained in the [Reference](./modules.html) section of the Network
Component. These routines can be adapted to the needs of the microcontroller application, in case more functionality is
needed.

The following templates are available for the Network component:

|Template Name|User Functions|
|-------------|-------|
| DNS_Client.c | `dns_cbfunc` (Callback function for notification about DNS client events), `resolve_host` (DNS resolving process)|
| FTP_Client_UIF.c | \ref netFTPc_Process (Request parameters for FTP client session), \ref netFTPc_Notify (Notify the user application when FTP client operation ends)|
| FTP_Server_Access.c | \ref netFTPs_AcceptClient (Accept or deny connection from remote FTP client)|
| FTP_Server_Event.c | \ref netFTPs_Notify (Notify the user application about events in FTP server service)|
| FTP_Server_Multiuser.c | \ref netFTPs_CheckUsername (Check if an user account exists), \ref netFTPs_CheckPassword (Check user account password), \ref netFTPs_FileAccess (Check if remote user is allowed to access a file)|
| HTTP_Server_Access.c | \ref netHTTPs_AcceptClient (Accept or deny connection from remote HTTP client)|
| HTTP_Server_CGI.c | \ref netCGI_ProcessQuery (Process query string received by GET request), \ref netCGI_ProcessData (Process data received by POST request), \ref netCGI_Script (Generate dynamic web data from a script line)|
| HTTP_Server_Error.c | `net_http_error` (Define user-friendly HTTP error messages)|
| HTTP_Server_Multiuser.c | \ref netHTTPs_CheckAccount (Check if an user account exists), \ref netHTTPs_FileAccess (Check if remote user is allowed to access a file)|
| SMTP_Client_UIF.c | \ref netSMTPc_Process (Request parameters for SMTP client session), \ref netSMTPc_Notify (Notify the user application when SMTP client operation ends), \ref netSMTPc_AcceptAuthentication (Accept or deny authentication requested by SMTP server)|
| SNMP_Agent_MIB.c | `mib_table` (Defines MIB information data table), `register_mib_table` (Registers a MIB table in SNMP agent) |
| TCP_Socket_Client.c | `tcp_cb_client` (Notify the user application about TCP socket events), `send_data` (Connect to TCP server and send data)|
| TCP_Socket_Server.c | `tcp_cb_server` (Notify the user application about TCP socket events)|
| Telnet_Server_Access.c | \ref netTELNETs_AcceptClient (Accept or deny connection from remote Telnet client)|
| Telnet_Server_Multiuser.c | \ref netTELNETs_CheckUsername (Check if an user account exists), \ref netTELNETs_CheckPassword (Check user account password)|
| Telnet_Server_UIF.c | \ref netTELNETs_ProcessMessage (Request message for Telnet server session), \ref netTELNETs_ProcessCommand (Process a command and generate response)|
| TFTP_Client_UIF.c | `tftp_client_notify` (Notify the user application when TFTP client operation ends)|
| TFTP_Server_Access.c | \ref netTFTPs_AcceptClient (Accept or deny connection from remote TFTP client)|
| UDP_Socket.c | `udp_cb_func` (Notify the user application about UDP socket events), `send_udp_data` (Send UDP data to destination client)|

## Debugging {#nw_debugging}

Network Component can be easily configured to generate debug events and provide dynamic visibility to the component operation.

Network supports event annotations for the \urlout{Event-Recorder-About} and makes it very easy to analyze the internal operation
of the Network Component during application debugging. The old \ref netDebugStdio "STDIO" debug, which outputs event information
as ASCII messages on the standard output device, is also supported.

### Debug Configuration {#netDebugConfig}

The Network Component debug is configured using the `Net_Debug.h` file. The options are set in the file directly or using the
[Configuration Wizard](https://developer.arm.com/documentation/101407/0539/Utilities/Configuration-Wizard).

![Net_Debug.h Configuration File](net_debug_h.png)

The following settings are available:

- **Network Debug** enables or disables the debug mode.
- **Output Channel** selects where the diagnostic messages are to be sent:
  - \urlout{Event-Recorder-About} outputs event messages to event recorder.
  - \ref netDebugStdio "STDIO" outputs diagnostic messages on the standard output device.
  - \ref netDebugStdio "STDIO + Timestamp" additionally prints the time information with debug messages.

The debugging configuration settings are divided into groups that can be activated separately:

- **System** enables memory allocation and interface diagnostics.
- **IPv4** enables IP version 4 network layer diagnostics.
- **IPv6** enables IP version 6 network layer diagnostics.
- **Socket** enables socket layer diagnostics.
- **Service** enables service layer diagnostics.

#### Debug Levels {#netDebugLevels}

The system consists of several modules that output diagnostic messages. It is possible to configure the debug output
for each module separately. The **Debug level** for each module defines what kind of debug messages are output:

|Level |Description                                                            |
|------|-----------------------------------------------------------------------|
|Off   |The debug messages for the selected module are **disabled**.            |
|Errors|Only error messages are output. This mode is useful for error tracking.|
|All   |In this mode, **all** debug messages are output.                        |

#### Event filtering

The Network debug variant generates lots of debug events. This might have an impact on the operation of the Network library.
It is a good idea to disable debug for modules, which are not used, or at least to reduce the number of messages, that
are generated. Therefore, enable **Error** events for such modules and enable **All** debug events only for the modules you are
focused on.

### Event Recorder {#netEvrSupport}

\urlout{Event-Recorder-About} is a powerful tool that provides visibility to the dynamic execution of the program.
The Network Component generates a broad set of  \ref net_evr "Debug Events" for the Event Recorder and implements
required infrastructure to interface with it.

To use the Event Recorder together with Network Component, it is required to create an image with event generation support. The necessary steps are:

1. \ref netDebugConfig "Select Debug mode": in the `Net_Config.h` enable the **Network Debug** and select `Event Recorder` for **Output Channel**.
2. \urlout{Event-Recorder-Enable}: in the RTE management dialog enable the software component **CMSIS-View:Event Recorder**.
3. \urlout{Event-Recorder-Initialize}: in the application code, call the function **EventRecorderInitialize**.
4. \urlout{Event-Recorder-Configure}: if necessary, adjust default Event Recorder configuration.
5. Configure the \ref netDebugLevels "Debug Levels".
6. Build the application code, download it to the target hardware and start debug session.

Now, when the network services generate event information, it can be viewed in the \urlout{uv4-Event-Recorder}.

#### Event IDs

The network component uses the following event IDs:

| Component   | Event ID |
|-------------|----------|
| Net_SYS     | 0xC0     |
| Net_MEM     | 0xC1     |
| Net_ETH     | 0xC2     |
| Net_WiFi    | 0xDD     |
| Net_PPP     | 0xC3     |
| Net_SLIP    | 0xC4     |
| Net_LOOP    | 0xC5     |
| Net_IP4     | 0xC6     |
| Net_ICMP    | 0xC7     |
| Net_IGMP    | 0xC8     |
| Net_NBNS    | 0xC9     |
| Net_DHCP    | 0xCA     |
| Net_ARP     | 0xCB     |
| Net_IP6     | 0xCC     |
| Net_ICMP6   | 0xCD     |
| Net_DHCP6   | 0xCE     |
| Net_NDP     | 0xCF     |
| Net_MLD     | 0xDE     |
| Net_UDP     | 0xD0     |
| Net_TCP     | 0xD1     |
| Net_BSD     | 0xD2     |
| Net_HTTPs   | 0xD3     |
| Net_FTPs    | 0xD4     |
| Net_FTPc    | 0xD5     |
| Net_Teln    | 0xD6     |
| Net_TFTPs   | 0xD7     |
| Net_TFTPc   | 0xD8     |
| Net_SMTP    | 0xD9     |
| Net_DNS     | 0xDA     |
| Net_SNMP    | 0xDB     |
| Net_SNTP    | 0xDC     |

### STDIO Debug {#netDebugStdio}

STDIO Debug is a legacy debug variant that prints event information as ASCII messages to a standard IO port.
It is generally less feature-rich and slower than the debug with \ref netEvrSupport "Event Recorder" and is not
recommended for new projects.

To enable STDIO debugging together with the Network Component, it is required to create an image
that generates event information. The necessary steps are:

1. \ref netDebugConfig "Select Debug mode": in the `Net_Config.h` enable the **Network Debug** and select
   `STDIO + Timestamp` or `STDIO` for **Output Channel**.
2. In the RTE management dialog enable **CMSIS-Compiler:CORE** and **CMSIS-Compiler:STDOUT** components and
   select its `ITM` variant.
3. Configure the \ref netDebugLevels "Debug Levels".
4. Build the application code and download it to the target hardware.

#### Module IDs

The owner module of the displayed debug message is identified by the message prefix. The following system and application
modules are configurable for debugging:

|ID    |Module                            |Description|
|------|----------------------------------|-----------|
|SYS   |System Core                       |Handles Network system operation.|
|MEM   |Memory Management                 |Allocates and releases frame buffers.|
|ETH   |ETH Interface                     |Handles Ethernet link.|
|WIFI  |WiFi Interface                    |Handles wireless network link.|
|PPP   |PPP Interface                     |Handles serial line direct or modem connection PPP link.|
|SLIP  |SLIP Interface                    |Handles serial line direct or modem connection SLIP link.|
|LOOP  |Loopback Interface                |Handles localhost loopback interface.|
|IP4   |IPv4 Core                         |Processes the IP version 4 network layer.|
|ICMP  |Control Message (ICMP)            |Processes ICMP messages. Best known example is the ping.|
|ARP   |Address Resolution (ARP)          |Handles Ethernet MAC address resolution and caching.|
|IGMP  |Group Management (IGMP)           |Processes IGMP messages, Hosts groups and IP Multicasting.|
|NBNS  |NBNS Client                       |The NetBIOS Name Service maintains name access to your hardware.|
|DHCP  |DHCP Client                       |Handles automatic configuration of IP address, Net mask, Default Gateway, and Primary and Secondary DNS servers.|
|IP6   |IPv6 Core                         |Processes the IP version 6 network layer.|
|ICMP6 |Control Message (ICMP6)           |Processes ICMP version 6 messages. Best known example is the ping.|
|NDP   |Neighbor Discovery (NDP)          |Handles Neighbor Discovery MAC address resolution and caching.|
|MLD   |Multicast Listener Discovery (MLD)|Handles Neighbor Discovery MAC address resolution and caching.|
|DHCP6 |DHCP6 Client                      |Handles automatic configuration of IP address in IP version 6.|
|UDP   |UDP Socket                        |Processes UDP frames.|
|TCP   |TCP Socket                        |Processes TCP frames.|
|BSD   |BSD Socket                        |Processes TCP and UDP frames via standard BSD Sockets API.|
|HTTP  |HTTP Server                       |Delivers web pages on the request to web clients.|
|FTP   |FTP Server                        |Manages the files stored on the server and serves the file requests received from the clients.|
|FTPC  |FTP Client                        |Connects to FTP server to transfer files on the server, and to manage files stored on the server.|
|TELN  |Telnet Server                     |Allows remote clients to control the system using the command line interface.|
|TFTP  |TFTP Server                       |A simple service which allows you to send files to or read files from the server.|
|TFTPC |TFTP Client                       |Connects to TFTP server to send or receive files.|
|SMTP  |SMTP Client                       |Connects to SMTP server to send emails.|
|DNS   |DNS Client                        |Handles the resolution of the IP address from a host name.|
|SNMP  |SNMP Agent                        |Manages devices on IP network.|
|SNTP  |SNTP Client                       |Manages clock synchronization over the network.|

An example of the debug output is:

```txt
015.0 ETH:*** Processing frame ***
015.0 ETH: DstMAC 1E-30-6C-A2-45-5E
015.0 ETH: SrcMAC 00-11-43-A4-FE-40
015.0 ETH: Proto IP4, 66 bytes
015.0 IP4:*** Processing frame ***
015.0 IP4: SrcAddr 192.168.0.5
015.0 IP4: DstAddr 192.168.0.150
015.0 IP4: Proto TCP, Id=0x0622
015.0 IP4: Length 52 bytes
015.0 TCP:*** Processing frame ***
015.0 TCP: Ports  : Src=49232, Dst=80
015.0 TCP: Segment: Seq=0x31EDC88C, Ack=0x0
015.0 TCP: Control: [SYN] Win=8192, Cks=0x0C67
015.0 TCP: Mapped to Socket 1, state LISTEN
015.0 TCP:Process Options, 12 bytes
015.0 TCP: Opt-MSS: 1440
015.0 TCP: SendWin: 8192
015.0 TCP: RTO=4000ms (sa=0, sv=40)
015.0 TCP: Next state SYN_REC
```

In the above example, Ethernet, IP and TCP debug messages are enabled:

- Received Ethernet packets are processed by the Ethernet layer and a debug message containing Ethernet header information is
  printed out. Ethernet debug information contains source and destination MAC address, Ethernet frame length and Ethernet
  protocol type.
- The packet is then passed to the IP layer. IP layer prints out IP debug messages containing the IP header information such
  as source and destination IP address, frame length, protocol type etc.
- When the IP layer has processed the packet, the packet is passed to the upper TCP layer. TCP layer prints out TCP debug
  messages containing the TCP header information such as source and destination ports, sequence and acknowledge numbers,
  checksum value, frame length etc.

\note
- If the debug mode is used on a high traffic LAN, the system might block. Reduce the amount of printed debug messages
  in `Net_Debug.h` configuration or disable the debug mode completely.
- Printing debug messages blocks out the system task scheduler during the time when the message is being sent from the serial
  port. The incoming IP packets accumulate in the memory. This soon causes an **out of memory** error. Any further
  incoming packets are lost until some memory is released.
  
## Runtime Configuration {#nw_runtime_config}

It is often necessary to change the parameters and mode of operation of the network interface at startup or runtime. System
control functions allow reading and changing the settings of the network interface and the system (for example the
[hostname](https://en.wikipedia.org/wiki/Hostname)).

This is required for using the same application code for serial production of embedded devices. The **runtime configuration**
feature enables reading of configuration parameters from an EEPROM or SD Card and configuring the
network interface for each embedded device differently.

To control the network interface options, you can use:

- \ref netIF_GetOption function to retrieve the options
- \ref netIF_SetOption function to set the options
- \ref netIF_SetDefault function to set default interface for internet access

The options which can be changed are defined in the \ref netIF_Option enumerator. However, some interfaces do not support the
complete set of available options. For example, the PPP interface does not have a MAC address. If you try to modify an
unsupported option, an error is returned.

The [localhost](https://en.wikipedia.org/wiki/Localhost) name is used to access the embedded
system without knowledge of its IP address. \ref netSYS_GetHostName is used to retrieve the localhost name, whereas
\ref netSYS_SetHostName changes the localhost name.

The [DHCP](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol) client can be
disabled or enabled at runtime. When disabled, the user provided network parameters defined in the `Net_Config_ETH_0.h`
or `Net_Config_WiFi_0.h` configuration files are used. Switch the state of the DHCP client using \ref netDHCP_Disable and
\ref netDHCP_Enable.

\note Each embedded Ethernet device must have a unique **MAC address**, **IP address**, and **hostname**. This is very
important when multiple devices are connected to the same LAN. Otherwise, the system might not work (due to collisions or
disturbed network communications).

**Code Example**

```c
#include "rl_net.h"

/* The following values are read from EEPROM */
const char mac_addr[]  = { "00-01-02-32-3c-46" };
const char ip_addr[]   = { "192.168.0.150"     };
const char def_gw[]    = { "192.168.0.1"       };
const char net_mask[]  = { "255.255.255.0"     };
const char pri_dns[]   = { "194.25.2.129"      };
const char sec_dns[]   = { "194.25.2.130"      };
const char host_name[] = { "Keil_MCB"          };
bool DHCP_enabled      = false;

int main (void) {
  uint8_t buf[8];

  netInitialize ();

  /* Change host name */
  netSYS_SetHostName (host_name);

  /* Change MAC address */
  netMAC_aton (mac_addr, buf);
  netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionMAC_Address, buf, NET_ADDR_ETH_LEN);

  if (DHCP_enabled == false) {
    /* Static configuration mode */
    netDHCP_Disable (NET_IF_CLASS_ETH | 0);

    /* Change IP address */
    netIP_aton (ip_addr, NET_ADDR_IP4, buf);
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, buf, NET_ADDR_IP4_LEN);

    /* Change Network mask */
    netIP_aton (net_mask, NET_ADDR_IP4, buf);
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_SubnetMask, buf, NET_ADDR_IP4_LEN);

    /* Change Default Gateway address */
    netIP_aton (def_gw, NET_ADDR_IP4, buf);
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_DefaultGateway, buf, NET_ADDR_IP4_LEN);

    /* Change Primary DNS Server address */
    netIP_aton (pri_dns, NET_ADDR_IP4, buf);
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_PrimaryDNS, buf, NET_ADDR_IP4_LEN);

    /* Change Secondary DNS Server address */
    netIP_aton (sec_dns, NET_ADDR_IP4, buf);
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_SecondaryDNS, buf, NET_ADDR_IP4_LEN);
  }
}
```
