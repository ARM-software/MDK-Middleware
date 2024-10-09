# Revision History {#rev_hist}

<table class="cmtable" summary="Revision History">
    <tr>
      <th>Version</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>V8.0.0</td>
      <td>
        - removed CORE component variants
        - changed source config files to headers
        - removed support for legacy Arm Compiler 5
        - removed support for CMSIS-RTOS version 1
        - removed legacy API interface version 6
        - corrected handling of a half-closed TCP socket, data delivery added
        - fixed DHCP client issue with relay agent causing "ServerAddressNotSelected" error
        - added function \ref netARP_AddCache to manually add an entry to the ARP cache table
        - added option for updating mbedTLS certificates at runtime
        - added IPv6 Multicasting with MLD, Multicast Listener Discovery Protocol
        - added BSD socket option SO_BINDTODEVICE to bind socket to a network interface
        - increased default Core Thread stack size (from 1024 to 2048)
      </td>
    </tr>
    <tr>
      <td>V7.19.0</td>
      <td>
        - using Event Recorder component from CMSIS-View pack (instead of deprecated ARM_Compiler pack)
        - fixed \ref netNBNS_Resolve function to return resolved IP address when TTL=0 in response message
        - fixed \ref netDNSc_GetHostByNameX function to return netTimeout instead of netError status on timeout
        - updated BSD socket API, type int changed to type int32_t
        - updated the default HTTP web page and error pages
        - fixed ARP issue that discarded ARP probe messages
        - fixed Neighbor Discovery issue that discarded NA messages if override flag not set
        - fixed possible allocation limit exceeded in debug version of SNMP agent
        - fixed possible NET_ERROR_MEM_FREE error when flooded with ARP requests
        - extended HTTP server functionality, added processing of query string in POST method
        - corrected BSD datagram socket to return BSD_EMSGSIZE error if message size is too large
        - fixed BSD socket datagram sending so that a large message is not split into smaller datagram messages
        - fixed possible buffer overflow in \ref netMAC_ntoa function
      </td>
    </tr>
    <tr>
      <td>V7.18.0</td>
      <td>
        - aligned with mbed TLS library version 3.1.0, SMTPS server verification is now mandatory
        - fixed ethernet issue not receiving VLAN-tagged frames
        - fixed DHCP client issue with relay agent causing "RelayAgentAddressInvalid" error
        - fixed un-initialize function to resume blocked threads if they are waiting for network resources
        - added functions \ref netNBNS_Resolve and \ref netNBNS_ClearCache in NBNS client
        - updated telnet server to allow three authorization attempts before closing connection
        - corrected md5 hash calculation in BE library versions
        - minor stability and performance improvements
        - improved code robustness
      </td>
    </tr>
    <tr>
      <td>V7.17.0</td>
      <td>
        - libraries built with Arm Compiler 6
        - updated FTP server to check the specified path in change directory command
        - improved FTP server file system interface for easier integration with a custom file system
        - improved Event Recorder debug, updated documentation for debug events
      </td>
    </tr>
    <tr>
      <td>V7.16.0</td>
      <td>
        - added support for HTTP cookies
        - fixed issue with \ref netHTTPs_GetSession function, which may have returned an invalid session number
        - upgraded BSD sockets to dual-stack sockets for IPv6
        - added function \ref netCGI_ProcessRequest to provide users with access to HTTP request haders
        - added option in SNMP agent to send NULL character in "OCTET STREAM" objects
        - fixed issue with \ref netSNMP_Trap when parameter "addr" is NULL
        - extended permission to change the root path in FTP server (until the user logs in)
        - corrected processing of truncated DNS response messages
        - added support for IPv6 fragmentation and reassembly
        - added configurable MTU parameter for network interfaces that support IPv6
      </td>
    </tr>
    <tr>
      <td>V7.15.0</td>
      <td>
        - added functions \ref netARP_ClearCache and \ref netNDP_ClearCache to clear the ARP or NDP cache at runtime
        - improved dynamic memory debugging, more detailed information in memory allocation and memory free debug events
        - improved DNS response validation to improve robustness and resilience against DNS cyber attacks
        - updated for use with the FuSa C Library, sprintf and sscanf are no longer used in the network library
        - fixed duplicate AutoIP addresses if the same firmware is deployed on multiple devices
        - fixed nonce caching issue in HTTP Digest authentication
        - corrected \ref select function to report a socket closed by a peer as readable
        - fixed an Ethernet connectivity issue when the ARP cache table is fully utilized
        - added functions in the user API that enable or disable echo response at runtime
        - fixed Ethernet driver handling so that the receive function is not called before the link-up
        - fixed possible NULL pointer dereference in a multi-interface configuration
        - fixed issue with HTTP Digest authentication with Apple Safari browser
      </td>
    </tr>
    <tr>
      <td>V7.14.0</td>
      <td>
        - fixed a problem in the \ref netUninitialize function that active BSD sockets remain blocked
          when the function is called by a low priority thread
        - fixed a problem with the HTTP server when uploading files from the Firefox browser
        - corrected OS layer timeouts for RTOS2, if the RTOS tick frequency is not 1000 Hz
        - fixed a build error when IPv6 was disabled in all LAN interfaces
        - fixed "no route found" error when sending broadcast frames
        - added \ref netUDP_OptionInterface option to change the default interface for sending broadcast frames
        - updated the \ref netFTPs_ffind function in the File System interface of the FTP server
        - added configuration option to prevent sending ping response (no echo reply)
        - added blocking functions \ref netARP_ProbeX and \ref netNDP_ProbeX for easy use
      </td>
    </tr>
    <tr>
      <td>V7.13.1</td>
      <td>
        - fixed an error when building projects with older network configurations that were not updated
      </td>
    </tr>
    <tr>
      <td>V7.13.0</td>
      <td>
        - added support for multiple LAN interfaces (Ethernet, WiFi) that can be used simultaneously
        - reworked network system viewer in debugger, added status view for multiple network interfaces
        - corrected IGMP checksum check error if the IGMP message is longer than 8 octets
        - updated HTTP server to also add custom HTTP headers to internally generated server error messages
        - corrected possible memory corruption in HTTP server, when a long HTTP URL request is received and Root Folder is used
        - added \ref netCGI_CustomHeader function to add a custom HTTP header to the web server response
        - corrected memory issue when BSD stream socket keeps sending data on half-closed connection
        - corrected DHCPv6 client issue to discard address offers, if preferred lifetime is equal to valid lifetime
        - corrected modem driver issue with modem initialization string length limited to 64 characters
        - added \ref netHTTPs_CalcHashHA1 function to calculate MD5 hash HA1 for HTTP Digest authentication
      </td>
    </tr>
    <tr>
      <td>V7.12.0</td>
      <td>
        - corrected possible memory corruption in CHAP authentication, when PPP interface used in client mode,
          and the server generates CHAP challenges of less than 16 bytes
        - added support for user accounts in HTTP server Digest authentication
        - added support for passwords stored as MD5 hash value HA1
        - added support for Digest access authentication in HTTP server
        - corrected memory issue when receiving fragmented broadcast message
        - corrected return code from BSD_EINVAL to BSD_ESOCK if BSD socket is not created
        - improved robustness in BSD sockets
        - corrected filtering for link-layer addressed UDP messages
          (datagrams with correct MAC address and invalid IP address are no longer received)
      </td>
    </tr>
    <tr>
      <td>V7.11.0</td>
      <td>
        - added support for the \ref netWiFi_Func "WiFi interface"
        - corrected Neighbor Discovery failure, when the requested global IPv6 address is on-link
        - updated lease renewal in the DHCP client also updates the default Gateway
        - corrected possible TCP socket retransmission failure in keep-alive mode for very short time limits,
          when the keep-alive timer expired before the retransmission timer
        - corrected CHAP authentication failure in the PPP interface, when used in client mode
        - optimized send_frame functions for network interfaces
        - updated IGMP multicast address filtering
        - improved and optimized network core processing
        - improved versioning, added library version to a map file (ie: "network_ip4_release_v7.10.6")
        - improved access to ethernet, serial and modem drivers
        - minor code cleanup and optimization
        - corrected problem in BSD socket timeouts, when the blocked receiving and blocked sending in the socket
          are simultaneously used from two threads
        - corrected problem, that the device is not accessible to IPv6 after changing the MAC address
        - added send frame buffering, if the MAC address in the IPv6 NDP cache is not resolved
          (the first call to the \ref netUDP_Send function does not fail anymore)
        - improved Event Recorder debug, updated documentation for debug events
      </td>
    </tr>
    <tr>
      <td>V7.10.0</td>
      <td>
        - corrected reduced performance in the loopback interface
        - added debug configuration for Event Recorder
        - added \ref sendmsg function to BSD sockets
        - corrected \ref netTCP_GetSocket and \ref netUDP_GetSocket functions to return netError code, if no free sockets
          are available, instead of calling net_sys_error handler
        - corrected potential failure to start network tick timer in RTX5 environment
        - added implicit binding to BSD sockets in \ref connect and \ref sendto functions
        - updated \ref bind function to support dynamic ports (bind to port 0)
        - added \ref recvmsg function to BSD sockets
        - added dynamic port range settings in TCP and UDP sockets
        - improved Ethernet debugging
        - added Virtual LAN support in Ethernet interface (VLAN)
        - updated Ethernet interface for reading the MAC address from the driver, if a unique MAC address is available in the driver
        - updated \ref netIF_SetOption and \ref netIF_GetOption functions to accept a possibly non-aligned buffer, when 16-bit
          or 32-bit option is passed in the buffer
      </td>
    </tr>
    <tr>
      <td>V7.9.0</td>
      <td>
        - added \ref select function to BSD sockets
        - added address conversion functions: \ref inet_addr, \ref inet_aton, \ref inet_ntoa, \ref inet_pton and \ref inet_ntop
          to BSD sockets
        - updated BSD sockets functionality and return codes to make them more consistent with standard BSD sockets
        - added blocking functions: \ref netSNTPc_GetTimeX, \ref netPing_EchoX and \ref netDNSc_GetHostByNameX for easy use
        - added SO_SNDTIMEO socket option in BSD sockets
        - added \ref getsockopt function to retrieve BSD socket options
        - corrected DHCP client problem by assigning an address, if the IP address has already been set by AutoIP
      </td>
    </tr>
    <tr>
      <td>V7.8.0</td>
      <td>
        - corrected quoted-printable encoding for the character '=' in \ref netSMTPc_SendMail function
        - documented possible return codes for Network API functions
        - changed return code \ref netError to \ref netFileError where appropriate in TFTP client
        - corrected \ref netSMTPc_SendMail functionality, if the requested attachment does not exist,
          the function fails with error code \ref netFileError, rather than send a dummy attachment
        - corrected a failure in \ref netSMTPc_SendMail function, when sending an email with a large attachment
        - added \ref netUninitialize function to uninitialize the Network component
      </td>
    </tr>
    <tr>
      <td>V7.7.0</td>
      <td>
        - added SMTPS Client for secure emails (SMTP over SSL/TLS) using mbed TLS library
        - corrected notifications in the Ethernet interface to send gratuitous ARP for each change of the IP address
        - corrected poor performance on the HTTP server, when opening large files
        - corrected thread lock issue in \ref accept function of BSD sockets, if called from a different thread in blocking mode
        - improved robustness required for Achilles level 1 certification
        - added transmit frame buffering, if MAC address in ARP cache is not resolved
          (first call to \ref netUDP_Send function does not fail anymore)
        - replaced deprecated "__declspec" compiler directives with "__attribute__" directives
        - updated SMTP client configuration for backward compatibility
        - some optimizations of Network Core and services
        - added \ref netSMTPc_SendMail function, which runs in blocking mode
        - added support for multiple email recipients (To, Cc, Bcc)
        - added support for email address friendly names in SMTP client
        - added configurable character encoding to support non ascii characters in SMTP client
        - added support for email attachments in SMTP client
      </td>
    </tr>
    <tr>
      <td>V7.5.0</td>
      <td>
        - added \ref netDHCP_SetOption function to change DHCP client options at runtime
          (for example DHCP Client-identifier option)
        - added support for IPv4 fragmentation and reassembly
        - added configurable MTU parameter for all network interfaces
        - extended \ref netIF_SetOption to change MTU parameter at runtime
        - added \ref netETH_SendRaw and \ref netETH_ReceiveRaw functions to support implementing ethernet protocols by users
        - added \ref netDNSc_ClearCache function to clear the DNS cache at runtime
      </td>
    </tr>
    <tr>
      <td>V7.4.0</td>
      <td>
        - corrected HTTPS Web server problem, upload files limited to small files
        - corrected problem of data loss in BSD stream sockets (TCP), which was caused by a traffic congestion
          on high-latency networks
        - corrected FCARM file converter processing of regular expressions in java script (js) files
        - corrected FCARM filename encoding to use slash '/' folder separator character
          (allows saving resource files in virtual subdirectories)
        - library made RTOS agnostic, OS calls are from external functions which are compile time available
        - added RTOS configuration files for CMSIS-RTOS and CMSIS-RTOS2
        - added \ref netIF_SetDefault function to dynamically change default interface at runtime
        - added \ref netHTTPs_SetRootPath, \ref netFTPs_SetRootPath and \ref netTFTPs_SetRootPath functions to
          change root folder at runtime
        - corrected PPP_ACCM option negotiation in PPP interface, which caused connectivity issues with some GSM modems
      </td>
    </tr>
    <tr>
      <td>V7.3.0</td>
      <td>
        - added event recording debug library variant
      </td>
    </tr>
    <tr>
      <td>V7.2.0</td>
      <td>
        - corrected DHCP client to accept DHCP OFFER message, if source IP of relay agent is 0.0.0.0
        - corrected return status in \ref netUDP_Send function to indicate an error, if the packet
          is dropped while sending UDP packets back to back
        - improved compatibility with ARM Compiler 6
        - expanded buffer size for the modem response string from 40 to 128 bytes
        - corrected infinite receive timeout in \ref recv function, when BSD stream socket is in server mode
        - simplified and optimized processing user command in Telnet server user interface
        - added decoding of URL-encoded file/path information in HTTP server
          (ie. space character in a resource filename)
        - optimized sending emails to generate less packets in SMTP client
        - corrected thread deadlock in \ref gethostbyname function, if \ref netDNSc_GetHostByName
          function is called at the same time
      </td>
    </tr>
    <tr>
      <td>V7.1.0</td>
      <td>
        - added HTTPS Server for secure communication (HTTP over SSL/TLS) using mbed TLS library
        - corrected link disconnect problem in \ref netPPP_Close and \ref netSLIP_Close functions
        - updated HTTP, FTP, Telnet and TFTP Server start/stop control functions to blocking mode
          (function is completed after the server is running/has stopped)
        - corrected problem of FTP Server not working with USB Host file system
        - corrected BSD receive functions to return BSD_ERROR_CLOSED, if the connection
          is unexpectedly closed in blocking mode
        - corrected serial communication problem on high-speed devices using PPP or SLIP interface
        - added osThreadYield in Network core main loop (cooperative multithreading)
        - added BSD socket event notification for mbed-client integration
        - added access to modem response string in modem driver
        - corrected low TCP performance problem when using only \ref netTCP_Send and no network services
      </td>
    </tr>
    <tr>
      <td>V7.0.0</td>
      <td>
        - corrected connectivity problem in PPP interface running in server mode
        - added support for relay agents in DHCP client (server on different subnet)
        - added SO_RCVTIMEO option in BSD socket interface to set blocking receive timeout
        - added support for AAAA-records in DNS client (resolves IPv6 address)
        - added DHCPv6 client for dynamic address configuration (stateless or stateful mode)
        - corrected minor bugs identified by static analysis of the source code
        - added support for content type "text/plain" in HTTP server POST request processing
        - corrected DHCP client lease renewal to update also DNS servers
        - added fractional seconds time information in SNTP client
        - added redirection of missing resources to a new location in HTTP server
        - added IP address conflict detection for IPv4 and IPv6
        - added Stateless Address Auto Configuration (SLAAC)
        - added IPv6 FTP extensions in FTP server and FTP client
        - added MIME type ".pdf" in HTTP server supported file types
        - added netCGI_ContentType() function for custom MIME types
        - added system configuration check in netInitialize(), signal NET_ERROR_CONFIG on error
        - added system created network Core and interface threads in a library
        - added mutex protection to network API functions, these functions are now thread-safe
        - added adaptive time polling in ETH thread when ETH driver is used in polling mode
        - updated dynamic port range in TCP and UDP sockets to use IANA recommendation (49152 - 65535)
        - updated to provide full path for FTP server user access control
        - corrected "frame too short" error in HTTP server when processing fragmented POST requests
        - corrected application build problem when using PPP interface
        - corrected bug in processing router advertisement message
        - corrected vulnerability to Spank Attack and Spoofed RST packets
        - corrected IPv6 receive address for loop-back interface
      </td>
    </tr>
    <tr>
      <td>V7.0.0 (beta)</td>
      <td>
        - Initial release of dual-stack (IPv4 and IPv6) version of Network Component
        - Incompatible API to version 6.x
        - Added \ref RFC_list
        - This Pack contains example projects for development boards: MCB1800, MCBSTM32F200, MCBSTM32F400
      </td>
    </tr>
 </table>
