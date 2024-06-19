# Migration {#network_diffs}

The following sections show how to migrate projects from previous versions of the Middleware:

- \ref nw_diffs_project_migration describes how to migrate an MDK project from the previous
  Middleware to the latest version.
- \ref mig_nw_configuration describes the differences in the configuration files of the Network Component.
- \ref netDebugStdio explains how to use the legacy debug features.

## Project Migration {#nw_diffs_project_migration}

There is an easy migration path available for users of the Network Component v7. Simply open the project that is about
to be migrated in µVision. The IDE will show an error message that a software component is not available:

\image html mig_project_open.png "Error message opening a legacy project"

This message is shown because the previous variants **IPv4 Release**/**IPv4 Debug** have been removed.
Open the **Manage Run-Time Environment** window. It will show the same error message in a different way:

\image html mig_rte_open.png "Error message in the Run-Time Environment"

Select a New Component variant **MDK** to get rid of this validation message:

\image html mig_rte_choose_mdk.png "Manually resolve the error message"

### Update Configuration Files {#nw_diffs_update_configs}

After pressing OK, the Project window will look like this:

\image html mig_project_required_changes_new_items.png "Updated Project window"

Right-clicking each file will give you the update options:

\image html mig_config_update_merge.png "Update options for configuration files"

**Update Config File** will simply copy the new configuration file into your project containing the default settings.
The old file with your project's settings will be renamed to Net_Config.h.0000 for example. If you want to manually merge the
two files, you can open this backup file (located in the folder RTE\\Network) and compare it with the new file line by line.

A better way is to use the option **Update Config File and Launch Merge**. This option is available for µVision
users with version 5.15 and above. Refer to the µVision help \urlout{uv4_dg_mergetoolmnu} for details.

Not all configuration files require a merge, as many of them are compatible. The section \ref mig_nw_configuration describes
the differences in more detail.

### Configure Keil RTX {#nw_diffs_rtx_configuration}

If you use **RTX v5**, you do not need to change the **RTX settings**, because all resources are statically allocated.
Refer to the section \ref nw_resource_requirements for more information.

### Build and Run the Network Project {#nw_diffs_build_and_run}

Finally, after merging the configuration files, you should be able to compile the project and run it on your target hardware:

\image html mig_build_ok.png "Error-free build output"

## Network Configuration {#mig_nw_configuration}

The following screenshots show the previous settings on the left-hand side, while the new settings
can be seen on the right-hand side.

### Net_Config.c {#mig_nw_configuration_net_config_c}

The network core configuration file has been changed from `Net_Config.c` to `Net_Config.h`.

\image html diff_net_config_c.png "Old Net_Config.c vs. new Net_Config.h"

- **Variant** selection added:
  - IPv4 only
  - IPv4/IPv6 dual stack
- **net_sys_error()** handler function removed

### Net_Debug.c {#mig_nw_configuration_net_debug}

The network debug configuration file has been changed from `Net_Debug.c` to `Net_Debug.h`.

\image html diff_net_debug_c.png "Old Net_Debug.c vs. new Net_Debug.h"

- **Network Debug** enable switch added
- **Output Channel** selection added:
  - Event Recorder
  - STDIO
  - STDIO + Timestamp
- **Print Time Stamps** option integrated into the **Output Channel** option
- **Multicast Listener Discovery (MLD)** option added
- **net_debug_init()** function removed
- \c DBG_XXX prefix changed to \c NET_DEBUG_XXX
  
## STDIO Debug {#netDebugStdio}

STDIO Debug is a legacy debug variant that prints event information as ASCII messages to a standard IO port.
It is generally less feature-rich and slower than the debug with \ref net_evr "Event Recorder" and is not
recommended for new projects.

To enable STDIO debugging together with the Network Component, it is required to create an image
that generates event information. The necessary steps are:

1. \ref netDebugConfig "Select Debug mode": in the `Net_Config.h` enable the **Network Debug** for the component. 
2. In the RTE management dialog enable **CMSIS-Compiler:CORE** and **CMSIS-Compiler:STDOUT** components and
   select its **ITM** variant.
3. Configure the \ref netDebugLevels "Debug Levels".
4. Build the application code and download it to the target hardware.

### Module IDs

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
