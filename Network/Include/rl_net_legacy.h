/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_net_legacy.h
 * Purpose: Network API (Legacy)
 *----------------------------------------------------------------------------*/

#ifndef __RL_NET_LEGACY_H
#define __RL_NET_LEGACY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "rl_net_ds.h"

#ifdef __cplusplus
extern "C"  {
#endif

// General definitions
#define ETH_ADDR_LEN                    NET_ADDR_ETH_LEN
#define IP4_ADDR_LEN                    NET_ADDR_IP4_LEN
#define IP6_ADDR_LEN                    NET_ADDR_IP6_LEN

// Network Interfaces
#define NETIF_ETH               0       // Network interface: Ethernet
#define NETIF_PPP               1       // Network interface: PPP
#define NETIF_SLIP              2       // Network interface: SLIP
#define NETIF_LOCAL             3       // Network interface: Localhost (loopback)
#define NETIF_NULL              4       // Network interface: Null (none)

// BSD Socket ioctl commands
#define FIO_DELAY_ACK           2       // Set DELAY_ACK mode for stream socket
#define FIO_KEEP_ALIVE          3       // Set KEEP_ALIVE mode for stream socket
#define FIO_FLOW_CTRL           4       // Set FLOW_CTRL mode for stream socket

//  ==== Network Core ====

// Functions
#define net_initialize()                netInitialize()
#define net_main()                      // Removed

//  ==== UDP Sockets ====

// Definitions
#define UDP_OPT_SEND_CHECKSUM           NET_UDP_CHECKSUM_SEND
#define UDP_OPT_VERIFY_CHECKSUM         NET_UDP_CHECKSUM_VERIFY

// Types
typedef uint32_t (*net_udp_cb_t)(int32_t socket, const uint8_t *ip_addr, uint16_t port, const uint8_t *buf, uint32_t len);

// Functions
#define udp_release_socket(soc)         netUDP_ReleaseSocket(soc)
#define udp_open(soc,port)              netUDP_Open(soc,port)
#define udp_close(soc)                  netUDP_Close(soc)
#define udp_multicast_ttl(soc,ttl)      netUDP_SetOption(soc,netUDP_OptionTTL,ttl)
#define udp_get_buf(size)               netUDP_GetBuffer(size)

extern int32_t udp_get_socket (uint8_t tos, uint8_t opt, net_udp_cb_t cb_func);
extern netStatus udp_send (int32_t socket, const uint8_t *ip_addr, uint16_t port, uint8_t *buf, uint32_t len);
extern net_udp_cb_t net_udp_cb_legacy[];

//  ==== TCP Sockets ====

// Definitions
#define TCP_TYPE_SERVER         0       // Removed
#define TCP_TYPE_CLIENT         0       // Removed
#define TCP_TYPE_CLIENT_SERVER  0       // Removed
#define TCP_TYPE_DELAY_ACK      0x01    // Socket Type: Delayed Acknowledge
#define TCP_TYPE_FLOW_CTRL      0x02    // Socket Type: Flow Control
#define TCP_TYPE_KEEP_ALIVE     0x04    // Socket Type: Keep Alive

// Enums
#define tcpEvent                        netTCP_Event
#define tcpEventConnect                 netTCP_EventConnect
#define tcpEventEstablished             netTCP_EventEstablished
#define tcpEventClosed                  netTCP_EventClosed
#define tcpEventAbort                   netTCP_EventAborted
#define tcpEventACK                     netTCP_EventACK
#define tcpEventData                    netTCP_EventData

#define tcpState                        netTCP_State
#define tcpStateUNUSED                  netTCP_StateUNUSED
#define tcpStateCLOSED                  netTCP_StateCLOSED
#define tcpStateLISTEN                  netTCP_StateLISTEN
#define tcpStateSYN_RECEIVED            netTCP_StateSYN_RECEIVED
#define tcpStateSYN_SENT                netTCP_StateSYN_SENT
#define tcpStateFIN_WAIT_1              netTCP_StateFIN_WAIT_1
#define tcpStateFIN_WAIT_2              netTCP_StateFIN_WAIT_2
#define tcpStateCLOSING                 netTCP_StateCLOSING
#define tcpStateLAST_ACK                netTCP_StateLAST_ACK
#define tcpStateTIME_WAIT               netTCP_StateTIME_WAIT
#define tcpStateESTABLISHED             netTCP_StateESTABLISHED

// Types
typedef uint32_t (*net_tcp_cb_t)(int32_t socket, tcpEvent event, const uint8_t *buf, uint32_t len);

// Functions
#define tcp_release_socket(soc)         netTCP_ReleaseSocket(soc)
#define tcp_listen(soc,port)            netTCP_Listen(soc,port)
#define tcp_get_buf(size)               netTCP_GetBuffer(size)
#define tcp_max_data_size(soc)          netTCP_GetMaxSegmentSize(soc)
#define tcp_check_send(soc)             netTCP_SendReady(soc)
#define tcp_send(soc,buf,len)           netTCP_Send(soc,buf,len)
#define tcp_close(soc)                  netTCP_Close(soc)
#define tcp_abort(soc)                  netTCP_Abort(soc)
#define tcp_reset_window(soc)           netTCP_ResetReceiveWindow(soc)

extern int32_t tcp_get_socket (uint8_t type, uint8_t tos, uint32_t tout, net_tcp_cb_t cb_func);
extern netStatus tcp_connect (int32_t socket, const uint8_t *ip_addr, uint16_t port, uint16_t local_port);
extern tcpState tcp_get_state (int32_t socket);
extern const char *tcp_ntoa (tcpState state);
extern net_tcp_cb_t net_tcp_cb_legacy[];

//  ==== Ethernet Interface ====

// Enums
#define arpCacheType                    netARP_CacheType
#define arpCacheFixedIP                 netARP_CacheFixedIP
#define arpCacheTemporaryIP             netARP_CacheTemporaryIP

// Ethernet Link Events
typedef enum {
  ethLinkDown,                          // Link down
  ethLinkUp_10MHalfDuplex,              // Link up (10 MBit half duplex)
  ethLinkUp_10MFullDuplex,              // Link up (10 MBit full duplex)
  ethLinkUp_100MHalfDuplex,             // Link up (100 MBit half duplex)
  ethLinkUp_100MFullDuplex,             // Link up (100 MBit full duplex)
  ethLinkUp_1GHalfDuplex,               // Link up (1 GBit half duplex)
  ethLinkUp_1GFullDuplex                // Link up (1 GBit full duplex)
} ethLinkEvent;

// DHCP Client Events
typedef enum {
  dhcpClientIPaddress     = 0,          // IP address change event
  dhcpClientNTPservers    = 42,         // NTP Servers option
  dhcpClientBootfileName  = 67          // Bootfile name option
} dhcpClientOption;

// Functions
#define arp_cache_ip(n,ip4,type)        netARP_CacheIP(n,ip4,type)
#define arp_cache_mac(n,mac)            netARP_CacheMAC(n,mac)
#define arp_get_ip(mac,ip4)             netARP_GetIP(0,mac,ip4)
#define arp_get_mac(ip4,mac)            netARP_GetMAC(0,ip4,mac)
#define igmp_join(n,ip4)                netIGMP_Join(n,ip4)
#define igmp_leave(n,ip4)               netIGMP_Leave(n,ip4)
#define dhcp_disable(n)                 netDHCP_Disable(n)

extern void eth_link_notify (uint32_t if_num, ethLinkEvent event);
extern void dhcp_client_notify (uint32_t if_num, dhcpClientOption opt, const uint8_t *val, uint32_t len);

//  ==== PPP Interface ====

#define ppp_listen(user,passw)          netPPP_Listen(user,passw)
#define ppp_connect(num,user,passw)     netPPP_Connect(num,user,passw)
#define ppp_close()                     netPPP_Close()
#define ppp_is_up()                     netPPP_LinkUp()

//  ==== SLIP Interface ====

#define slip_listen()                   netSLIP_Listen()
#define slip_connect(num)               netSLIP_Connect(num)
#define slip_close()                    netSLIP_Close()
#define slip_is_up()                    netSLIP_LinkUp()

//  ==== ICMP Ping ====

// Enums
#define icmpEvent                       netPing_Event
#define icmpEventSuccess                netPing_EventSuccess
#define icmpEventTimeout                netPing_EventTimeout

// Types
#define net_icmp_cb_t                   netPing_cb_t

// Functions
extern netStatus icmp_ping (const uint8_t *ip_addr, net_icmp_cb_t cb_func);

//  ==== DNS Client ====

// Enums
#define dnsClientEvent                  netDNSc_Event
#define dnsClientSuccess                netDNSc_EventSuccess
#define dnsClientTimeout                netDNSc_EventTimeout
#define dnsClientNotResolved            netDNSc_EventNotResolved
#define dnsClientError                  netDNSc_EventError

// Types
typedef void (*net_dns_client_cb_t)(dnsClientEvent event, const uint8_t *ip_addr);

// Functions
extern netStatus get_host_by_name (const char *name, net_dns_client_cb_t cb_func);

//  ==== FTP Server  ====

// Enums
#define ftpServerEvent                  netFTPs_Event
#define ftpServerLogin                  netFTPs_EventLogin
#define ftpServerLogout                 netFTPs_EventLogout
#define ftpServerLoginFailed            netFTPs_EventLoginFailed
#define ftpServerDownload               netFTPs_EventDownload
#define ftpServerUpload                 netFTPs_EventUpload
#define ftpServerDelete                 netFTPs_EventDelete
#define ftpServerRename                 netFTPs_EventRename
#define ftpServerMakeDirectory          netFTPs_EventMakeDirectory
#define ftpServerRemoveDirectory        netFTPs_EventRemoveDirectory
#define ftpServerOperationDenied        netFTPs_EventOperationDenied
#define ftpServerLocalFileError         netFTPs_EventLocalFileError
#define ftpServerFileError              netFTPs_EventFileError
#define ftpServerError                  netFTPs_EventError

// Functions
#define ftp_check_username(user)            netFTPs_CheckUsername(user)
#define ftp_check_password(user_id,passw)   netFTPs_CheckPassword(user_id,passw)
#define ftp_get_user_id()                   netFTPs_GetUserId()
#define ftp_server_notify(event)            netFTPs_Notify(event)

extern bool ftp_accept_client (const uint8_t *ip_addr, uint16_t port);
extern bool ftp_file_access (uint8_t user_id, const char *fname, uint8_t mode);

// FS interface
#define ftp_server_fopen(name,mode)         netFTPs_fopen(name,mode)
#define ftp_server_fclose(f)                netFTPs_fclose(f)
#define ftp_server_fread(f,buf,len)         netFTPs_fread(f,buf,len)
#define ftp_server_fwrite(f,buf,len)        netFTPs_fwrite(f,buf,len)
#define ftp_server_fdelete(name)            netFTPs_fdelete(name)
#define ftp_server_frename(old,new)         netFTPs_frename(old,new)
#define ftp_server_mkdir(path)              netFTPs_mkdir(path)
#define ftp_server_rmdir(path)              netFTPs_rmdir(path)
// ftp_server_ffind: Changed parameters and return value
#define ftp_server_ffind(mask,name,size,time,first) \
                                            netFTPs_ffind(mask,name,size,time,first)

//  ==== FTP Client ====

// Enums
#define ftpCommand                      netFTP_Command
#define ftpPUT                          netFTP_CommandPUT
#define ftpGET                          netFTP_CommandGET
#define ftpAPPEND                       netFTP_CommandAPPEND
#define ftpDELETE                       netFTP_CommandDELETE
#define ftpLIST                         netFTP_CommandLIST
#define ftpRENAME                       netFTP_CommandRENAME
#define ftpMKDIR                        netFTP_CommandMKDIR
#define ftpRMDIR                        netFTP_CommandRMDIR
#define ftpNLIST                        netFTP_CommandNLIST

#define ftpClientEvent                  netFTPc_Event
#define ftpClientSuccess                netFTPc_EventSuccess
#define ftpClientTimeout                netFTPc_EventTimeout
#define ftpClientLoginFailed            netFTPc_EventLoginFailed
#define ftpClientAccessDenied           netFTPc_EventAccessDenied
#define ftpClientFileNotFound           netFTPc_EventFileNotFound
#define ftpClientInvalidPath            netFTPc_EventInvalidDirectory
#define ftpClientLocalFileError         netFTPc_EventLocalFileError
#define ftpClientError                  netFTPc_EventError

#define ftpClientRequest                netFTPc_Request
#define ftpClientUsername               netFTPc_RequestUsername
#define ftpClientPassword               netFTPc_RequestPassword
#define ftpClientPath                   netFTPc_RequestDirectory
#define ftpClientFilename               netFTPc_RequestName      // changed to RequestName
#define ftpClientDirectory              netFTPc_RequestName      // changed to RequestName
#define ftpClientNewName                netFTPc_RequestNewName
#define ftpClientFilterMask             netFTPc_RequestListMask
#define ftpClientList                   netFTPc_RequestList
#define ftpClientLocalFile              netFTPc_RequestLocalFilename

// Functions
#define ftp_client_request(req,buf,len) netFTPc_Process(req,buf,len)
#define ftp_client_notify(event)        netFTPc_Notify(event)

extern netStatus ftp_client_connect (const uint8_t *ip_addr, uint16_t port, ftpCommand command);

// FS interface
#define ftp_client_fopen(name,mode)     netFTPc_fopen(name,mode)
#define ftp_client_fclose(f)            netFTPc_fclose(f)
#define ftp_client_fread(f,buf,len)     netFTPc_fread(f,buf,len)
#define ftp_client_fwrite(f,buf,len)    netFTPc_fwrite(f,buf,len)

//  ==== TFTP Server ====

// Functions
extern bool tftp_accept_client (const uint8_t *ip_addr, uint16_t port);

// FS interface
#define tftp_server_fopen(name,mode)    netTFTPs_fopen(name,mode)
#define tftp_server_fclose(f)           netTFTPs_fclose(f)
#define tftp_server_fread(f,buf,len)    netTFTPs_fread(f,buf,len)
#define tftp_server_fwrite(f,buf,len)   netTFTPs_fwrite(f,buf,len)

//  ==== TFTP Client ====

// Enums
#define tftpClientEvent                 netTFTPc_Event
#define tftpClientSuccess               netTFTPc_EventSuccess
#define tftpClientTimeout               netTFTPc_EventTimeout
#define tftpClientAccessDenied          netTFTPc_EventAccessDenied
#define tftpClientFileNotFound          netTFTPc_EventFileNotFound
#define tftpClientDiskFull              netTFTPc_EventDiskFull
// not defined                          netTFTPc_EventLocalFileError
#define tftpClientError                 netTFTPc_EventError

// Functions
extern netStatus tftp_client_put (const uint8_t *ip_addr, uint16_t port, const char *src, const char *dst);
extern netStatus tftp_client_get (const uint8_t *ip_addr, uint16_t port, const char *src, const char *dst);
extern void tftp_client_notify (tftpClientEvent event);

// FS interface
#define tftp_client_fopen(name,mode)    netTFTPc_fopen(name,mode)
#define tftp_client_fclose(f)           netTFTPc_fclose(f)
#define tftp_client_fread(f,buf,len)    netTFTPc_fread(f,buf,len)
#define tftp_client_fwrite(f,buf,len)   netTFTPc_fwrite(f,buf,len)

//  ==== Telnet Server ====

// Enums
#define telnetServerMessage             netTELNETs_Message
#define telnetServerWelcome             netTELNETs_MessageWelcome
#define telnetServerPrompt              netTELNETs_MessagePrompt
#define telnetServerLogin               netTELNETs_MessageLogin
#define telnetServerUsername            netTELNETs_MessageUsername
#define telnetServerPassword            netTELNETs_MessagePassword
#define telnetServerLoginFailed         netTELNETs_MessageLoginFailed
#define telnetServerLoginTimeout        netTELNETs_MessageLoginTimeout
#define telnetServerUnsolicitedMessage  netTELNETs_MessageUnsolicited

// Functions
#define telnet_server_set_delay(delay)          netTELNETs_RepeatCommand(delay)
#define telnet_server_get_session()             netTELNETs_GetSession()
#define telnet_check_command(cmd,user)          netTELNETs_CheckCommand(cmd,user)
#define telnet_server_message(msg,buf,len)      netTELNETs_ProcessMessage(msg,buf,len)
#define telnet_server_process(cmd,buf,len,pvar) netTELNETs_ProcessCommand(cmd,buf,len,pvar)
#define telnet_check_username(user)             netTELNETs_CheckUsername(user)
#define telnet_check_password(id,passw)         netTELNETs_CheckPassword(id,passw)
#define telnet_get_user_id()                    netTELNETs_GetUserId()

extern netStatus telnet_server_get_client (uint8_t *ip_addr, uint8_t *mac_addr);
extern bool telnet_accept_client (const uint8_t *ip_addr, uint16_t port);
// bool telnet_server_message_poll (int32_t session); // Removed, use netTELNETs_RequestMessage()

//  ==== HTTP Server ====

// Functions
#define http_get_env_var(env,ansi,mlen) netCGI_GetEnvVar(env,ansi,mlen)
#define http_server_get_session()       netHTTPs_GetSession()
#define http_server_get_lang()          netHTTPs_GetLanguage()
#define http_server_get_content_type()  netHTTPs_GetContentType()
#define cgi_process_query(qstr)         netCGI_ProcessQuery(qstr)
#define cgi_process_data(code,data,len) netCGI_ProcessData(code,data,len)
#define cgi_script(env,buf,buflen,pcgi) netCGI_Script(env,buf,buflen,pcgi)
#define cgi_content_type(file_ext)      netCGI_ContentType(file_ext)
#define cgx_content_type(v)             netCGX_ContentType(v)
#define http_encoding(v)                netCGI_Charset(v)
#define http_check_account(user,passw)  netHTTPs_CheckAccount(user,passw)
#define http_file_access(user_id,fname) netHTTPs_FileAccess(user_id,fname)
#define http_get_user_id()              netHTTPs_GetUserId()

extern bool http_accept_client (const uint8_t *ip_addr, uint16_t port);
extern netStatus http_server_get_client (uint8_t *ip_addr, uint8_t *mac_addr);

// FS interface
#define http_server_fopen(name)         netHTTPs_fopen(name)
#define http_server_fclose(f)           netHTTPs_fclose(f)
#define http_server_fread(f,buf,len)    netHTTPs_fread(f,buf,len)
#define http_server_fgets(f,buf,size)   netHTTPs_fgets(f,buf,size)
#define http_utc_time(h,m,s,d,mo,y)     net_http_time(h,m,s,d,mo,y)
// uint32_t http_server_ftime (const char *fname); // Changed to netHTTPs_fstat()
#define http_server_ftime(name,time,size) netHTTPs_fstat(name,time,size)

//  ==== SMTP Client ====

// Enums
#define smtpClientRequest               netSMTPc_Request
#define smtpClientUsername              netSMTPc_RequestUsername
#define smtpClientPassword              netSMTPc_RequestPassword
#define smtpClientSender                netSMTPc_RequestSender
#define smtpClientRecipient             netSMTPc_RequestRecipient
#define smtpClientSubject               netSMTPc_RequestSubject
#define smtpClientBody                  netSMTPc_RequestBody

#define smtpClientEvent                 netSMTPc_Event
#define smtpClientSuccess               netSMTPc_EventSuccess
#define smtpClientTimeout               netSMTPc_EventTimeout
#define smtpClientError                 netSMTPc_EventError

// Functions
#define smtp_client_request(req,buf,len,pv) netSMTPc_Process(req,buf,len,pv)
#define smtp_client_notify(event)           netSMTPc_Notify(event)

extern netStatus smtp_client_connect (const uint8_t *ip_addr, uint16_t port);
extern bool smtp_client_accept_authentication (const uint8_t *ip_addr);

//  ==== SNTP Client ====

// Types
typedef void (*net_sntp_client_cb_t)(uint32_t utc_time);

// Functions
extern netStatus sntp_get_time (const uint8_t *ip_addr, net_sntp_client_cb_t cb_func);

//  ==== SNMP Agent ====

// Definitions
#define MIB_INTEGER                     NET_SNMP_MIB_INTEGER
#define MIB_OCTET_STR                   NET_SNMP_MIB_OCTET_STR
#define MIB_OBJECT_ID                   NET_SNMP_MIB_OBJECT_ID
#define MIB_IP_ADDR                     NET_SNMP_MIB_IP_ADDR
#define MIB_COUNTER                     NET_SNMP_MIB_COUNTER
#define MIB_GAUGE                       NET_SNMP_MIB_GAUGE
#define MIB_TIME_TICKS                  NET_SNMP_MIB_TIME_TICKS
#define MIB_ATR_RO                      NET_SNMP_MIB_ATR_RO
#define MIB_OID_SIZE                    NET_SNMP_MIB_OID_SIZE
#define MIB_STR_SIZE                    NET_SNMP_MIB_STR_SIZE
#define MIB_READ                        NET_SNMP_MIB_READ
#define MIB_WRITE                       NET_SNMP_MIB_WRITE

// Macros
#define MIB_STR(s)                      NET_SNMP_MIB_STR(s)
#define MIB_INT(o)                      NET_SNMP_MIB_INT(o)
#define MIB_IP(ip)                      NET_SNMP_MIB_IP(ip)
#define MIB_OID0(f,s)                   NET_SNMP_MIB_OID0(f,s)
#define SNMP_MIB_SIZE(mib)              (sizeof(mib)) // use netSNMP_SetMIB_Table()

// Types
typedef struct snmp_mib {
  uint8_t  Type;                        // Object Type
  uint8_t  OidLen;                      // Object ID length
  uint8_t  Oid[MIB_OID_SIZE];           // Object ID value
  uint8_t  ValSz;                       // Size of a variable
  void     *Val;                        // Pointer to a variable
  void     (*cb_func)(int32_t mode);    // Write/Read event callback function
} const SNMP_MIB;

// Variables
#define snmp_SysUpTime                  netSNMP_sysUpTime

// Functions
#define snmp_set_community(com)         netSNMP_SetCommunity(com)

extern netStatus snmp_trap (const uint8_t *ip_addr, uint8_t generic, uint8_t specific, const uint16_t *obj_list);

// ==== Network Address Conversion ====

// Functions
#define mac_aton(str,mac)               netMAC_aton(str,mac)

extern const char *ip4_ntoa (const uint8_t *ip4_addr);
extern bool ip4_aton (const char *cp, uint8_t *ip4_addr);
extern const char *mac_ntoa (const uint8_t *mac_addr);

#ifdef __cplusplus
}
#endif

#endif /* __RL_NET_LEGACY_H */
