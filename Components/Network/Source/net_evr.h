/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_evr.h
 * Purpose: Network definitions for Event Recorder
 *----------------------------------------------------------------------------*/

#ifdef Network_Debug_EVR
#include <string.h>
#include "EventRecorder.h"

#ifndef __STATIC_INLINE
  #define __STATIC_INLINE   static __inline __attribute__((always_inline))
#endif

extern uint32_t net_dbg_buf[9];         ///< Buffer for complex debug structures

/// Union for accessing the debug parameters.
union evr_access {
  uint32_t u32[9];                      ///< 4-byte access
  uint16_t u16[18];                     ///< 2-byte access
  uint8_t  u8 [36];                     ///< 1-byte access
};
#define evr_buf             (*(union evr_access *)&net_dbg_buf)

// Local definition of NET_ADDR structure.
typedef struct evr_addr {
  int16_t  type;
  uint16_t port;
  uint8_t  addr[16];
} EVR_ADDR;

// Network components ----------------------------------------------------------
#define EvtNetSYS           0xC0        ///< System Core
#define EvtNetMEM           0xC1        ///< Memory Management
#define EvtNetETH           0xC2        ///< ETH Interface
#define EvtNetWiFi          0xDD        ///< WiFi Interface
#define EvtNetPPP           0xC3        ///< PPP Interface
#define EvtNetSLIP          0xC4        ///< SLIP Interface
#define EvtNetLOOP          0xC5        ///< Loopback Interface

#define EvtNetIP4           0xC6        ///< IPv4 Core
#define EvtNetICMP          0xC7        ///< ICMP Control
#define EvtNetIGMP          0xC8        ///< IGMP Group Management
#define EvtNetNBNS          0xC9        ///< NetBIOS Name Service Client
#define EvtNetDHCP          0xCA        ///< Dynamic Host Configuration Client
#define EvtNetARP           0xCB        ///< Ethernet Address Resolution

#define EvtNetIP6           0xCC        ///< IPv6 Core
#define EvtNetICMP6         0xCD        ///< ICMPv6 Control
#define EvtNetDHCP6         0xCE        ///< Dynamic Host Configuration Client for IPv6
#define EvtNetNDP           0xCF        ///< Neighbor Discovery for IPv6
#define EvtNetMLD           0xDE        ///< Multicast Listener Discovery for IPv6

#define EvtNetUDP           0xD0        ///< Socket UDP
#define EvtNetTCP           0xD1        ///< Socket TCP
#define EvtNetBSD           0xD2        ///< Socket BSD

#define EvtNetHTTPs         0xD3        ///< Web Server
#define EvtNetFTPs          0xD4        ///< File Transfer Server
#define EvtNetFTPc          0xD5        ///< File Transfer Client
#define EvtNetTeln          0xD6        ///< Telnet Server
#define EvtNetTFTPs         0xD7        ///< Trivial File Transfer Server
#define EvtNetTFTPc         0xD8        ///< Trivial File Transfer Client
#define EvtNetSMTP          0xD9        ///< Simple Mail Transfer Client
#define EvtNetDNS           0xDA        ///< Domain Name Service Client
#define EvtNetSNMP          0xDB        ///< Simple Network Management Agent
#define EvtNetSNTP          0xDC        ///< Simple Network Time Client

#define EvtNetStart         0xC0        ///< First Network component number
#define EvtNetEnd           0xDE        ///< Last Network component number
#endif


// NetSYS event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetSYS_InitSystem            EventID (EventLevelOp,    EvtNetSYS, 0)
#define EvtNetSYS_ThreadCreateFailed    EventID (EventLevelError, EvtNetSYS, 1)
#define EvtNetSYS_TimerCreateFailed     EventID (EventLevelError, EvtNetSYS, 2)
#define EvtNetSYS_InitComplete          EventID (EventLevelOp,    EvtNetSYS, 3)
#define EvtNetSYS_GetOption             EventID (EventLevelAPI,   EvtNetSYS, 4)
#define EvtNetSYS_SetOption             EventID (EventLevelAPI,   EvtNetSYS, 5)
#define EvtNetSYS_SetDefault            EventID (EventLevelAPI,   EvtNetSYS, 6)
#define EvtNetSYS_SetHostName           EventID (EventLevelAPI,   EvtNetSYS, 7)
#define EvtNetSYS_UninitSystem          EventID (EventLevelOp,    EvtNetSYS, 8)
#define EvtNetSYS_UninitComplete        EventID (EventLevelOp,    EvtNetSYS, 9)
#endif

/**
  \brief  Event on network initialize start (Op)
  \param  lib_version   encoded library version BCD (MMmmbbbb)
                         - MM:   Major
                         - mm:   minor
                         - bbbb: build
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_InitSystem(uint32_t lib_version) {
    uint16_t version = lib_version >> 16;
    uint16_t build   = lib_version & 0xFFFF;
    EventRecord2 (EvtNetSYS_InitSystem, version, build);
  }
#else
  #define EvrNetSYS_InitSystem(lib_version)
#endif

/**
  \brief  Event on failure to create network core thread (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_ThreadCreateFailed(void) {
    EventRecord2 (EvtNetSYS_ThreadCreateFailed, 0, 0);
  }
#else
  #define EvrNetSYS_ThreadCreateFailed()
#endif

/**
  \brief  Event on failure to create network interval timer (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_TimerCreateFailed(void) {
    EventRecord2 (EvtNetSYS_TimerCreateFailed, 0, 0);
  }
#else
  #define EvrNetSYS_TimerCreateFailed()
#endif

/**
  \brief  Event on network initialize complete (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_InitComplete(void) {
    EventRecord2 (EvtNetSYS_InitComplete, 0, 0);
  }
#else
  #define EvrNetSYS_InitComplete()
#endif

/**
  \brief  Event on set interface option (API)
  \param  if_id         network interface identifier
  \param  option        interface option to set
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_SetOption(uint16_t if_id, int32_t option) {
    EventRecord2 (EvtNetSYS_SetOption, if_id, (uint32_t)option);
  }
#else
  #define EvrNetSYS_SetOption(if_id, option)
#endif

/**
  \brief  Event on get interface option (API)
  \param  if_id         network interface identifier
  \param  option        interface option to get
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_GetOption(uint16_t if_id, int32_t option) {
    EventRecord2 (EvtNetSYS_GetOption, if_id, (uint32_t)option);
  }
#else
  #define EvrNetSYS_GetOption(if_id, option)
#endif

/**
  \brief  Event on set default interface (API)
  \param  if_id         network interface identifier
  \param  ip_version    internet protocol version to use
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_SetDefault(uint16_t if_id, int32_t ip_version) {
    EventRecord2 (EvtNetSYS_SetDefault, if_id, (uint32_t)ip_version);
  }
#else
  #define EvrNetSYS_SetDefault(if_id, ip_version)
#endif

/**
  \brief  Event on set local host name (API)
  \param  name          pointer to host name string
  \param  length        length of host name string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_SetHostName(const char *name, uint32_t length) {
    EventRecordData (EvtNetSYS_SetHostName, name, length);
  }
#else
  #define EvrNetSYS_SetHostName(name, length)
#endif

/**
  \brief  Event on network uninitialize start (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_UninitSystem(void) {
    EventRecord2 (EvtNetSYS_UninitSystem, 0, 0);
  }
#else
  #define EvrNetSYS_UninitSystem()
#endif

/**
  \brief  Event on network uninitialize complete (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSYS_UninitComplete(void) {
    EventRecord2 (EvtNetSYS_UninitComplete, 0, 0);
  }
#else
  #define EvrNetSYS_UninitComplete()
#endif


// NetMEM event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetMEM_InitMemory                EventID (EventLevelOp,    EvtNetMEM,  0)
#define EvtNetMEM_AllocMemory               EventID (EventLevelOp,    EvtNetMEM,  1)
#define EvtNetMEM_AllocLimitExceeded        EventID (EventLevelOp,    EvtNetMEM,  2)
#define EvtNetMEM_AllocOutOfMemory          EventID (EventLevelError, EvtNetMEM,  3)
#define EvtNetMEM_ShrinkMemory              EventID (EventLevelOp,    EvtNetMEM,  4)
#define EvtNetMEM_FreeMemory                EventID (EventLevelOp,    EvtNetMEM,  5)
#define EvtNetMEM_FreeInvalidBlock          EventID (EventLevelError, EvtNetMEM,  6)
#define EvtNetMEM_FreeLinkCorrupted         EventID (EventLevelError, EvtNetMEM,  7)
#define EvtNetMEM_UninitMemory              EventID (EventLevelOp,    EvtNetMEM,  8)
#endif

/**
  \brief  Event on initialize memory management (Op)
  \param  base          pointer to the the memory pool
  \param  size          size of the memory pool
  \param  limit0        usage limit 0 (limit for ethernet and BSD receive buffering)
  \param  limit1        usage limit 1 (limit for TCP send buffering)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_InitMemory(void *base, uint32_t size, uint32_t limit0, uint32_t limit1) {
    EventRecord4 (EvtNetMEM_InitMemory, (uint32_t)base, size, limit0, limit1);
  }
#else
  #define EvrNetMEM_InitMemory(base, size, limit0, limit1)
#endif

/**
  \brief  Event on request to allocate memory (Op)
  \param  mem           pointer to allocated memory block
  \param  size          requested memory size in bytes
  \param  used          used memory status in bytes
  \param  blocks        number of used blocks status
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_AllocMemory(void *mem, uint32_t size, uint32_t used, uint32_t blocks) {
    EventRecord4 (EvtNetMEM_AllocMemory, (uint32_t)mem, size, used, blocks);
  }
#else
  #define EvrNetMEM_AllocMemory(mem, size, used, blocks)
#endif

/**
  \brief  Event on memory allocation limit exceeded (Op)
  \param  size          requested memory size in bytes
  \param  used          used memory status in bytes
  \param  blocks        number of used blocks status
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_AllocLimitExceeded(uint32_t size, uint32_t used, uint32_t blocks) {
    EventRecord4 (EvtNetMEM_AllocLimitExceeded, size, used, blocks, 0);
  }
#else
  #define EvrNetMEM_AllocLimitExceeded(size, used, blocks)
#endif

/**
  \brief  Event on out of memory error (Error)
  \param  size          requested memory size in bytes
  \param  used          used memory status in bytes
  \param  blocks        number of used blocks status
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_AllocOutOfMemory(uint32_t size, uint32_t used, uint32_t blocks) {
    EventRecord4 (EvtNetMEM_AllocOutOfMemory, size, used, blocks, 0);
  }
#else
  #define EvrNetMEM_AllocOutOfMemory(size, used, blocks)
#endif

/**
  \brief  Event on shrink memory block request (Op)
  \param  mem           pointer to allocated memory block
  \param  new_size      new block size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_ShrinkMemory(void *mem, uint32_t new_size) {
    EventRecord2 (EvtNetMEM_ShrinkMemory, (uint32_t)mem, new_size);
  }
#else
  #define EvrNetMEM_ShrinkMemory(mem, new_size)
#endif

/**
  \brief  Event on request to free memory block (Op)
  \param  mem           pointer to the memory block
  \param  size          size of the memory block in bytes
  \param  used          used memory status in bytes
  \param  blocks        number of used blocks status
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_FreeMemory(void *mem, uint32_t size, uint32_t used, uint32_t blocks) {
    EventRecord4 (EvtNetMEM_FreeMemory, (uint32_t)mem, size, used, blocks);
  }
#else
  #define EvrNetMEM_FreeMemory(mem, size, used, blocks)
#endif

/**
  \brief  Event on attempt to free an invalid memory (Error)
  \param  mem           pointer to invalid memory block
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_FreeInvalidBlock(void *mem) {
    EventRecord2 (EvtNetMEM_FreeInvalidBlock, (uint32_t)mem, 0);
  }
#else
  #define EvrNetMEM_FreeInvalidBlock(mem)
#endif

/**
  \brief  Event on corrupted internal memory link (Error)
  \param  link          corrupted link pointer
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_FreeLinkCorrupted(void *link) {
    EventRecord2 (EvtNetMEM_FreeLinkCorrupted, (uint32_t)link, 0);
  }
#else
  #define EvrNetMEM_FreeLinkCorrupted(link)
#endif

/**
  \brief  Event on de-initialize memory management (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMEM_UninitMemory(void) {
    EventRecord2 (EvtNetMEM_UninitMemory, 0, 0);
  }
#else
  #define EvrNetMEM_UninitMemory()
#endif


// NetETH event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetETH_InitInterface             EventID (EventLevelOp,    EvtNetETH,  0)
#define EvtNetETH_MacAddressConfigError     EventID (EventLevelError, EvtNetETH,  1)
#define EvtNetETH_VlanConfigError           EventID (EventLevelError, EvtNetETH, 43)
#define EvtNetETH_VlanInitError             EventID (EventLevelError, EvtNetETH, 44)
#define EvtNetETH_PhyDriverConfigError      EventID (EventLevelError, EvtNetETH,  2)
#define EvtNetETH_PhyDriverInitError        EventID (EventLevelError, EvtNetETH,  3)
#define EvtNetETH_Ip4ConfigError            EventID (EventLevelError, EvtNetETH,  4)
#define EvtNetETH_Ip6ConfigError            EventID (EventLevelError, EvtNetETH,  5)
#define EvtNetETH_ThreadCreateFailed        EventID (EventLevelError, EvtNetETH,  6)
#define EvtNetETH_SemaphoreCreateFailed     EventID (EventLevelError, EvtNetETH, 48) // End
#define EvtNetETH_GetOptionInvalidParameter EventID (EventLevelError, EvtNetETH,  7)
#define EvtNetETH_SetOptionInvalidParameter EventID (EventLevelError, EvtNetETH,  8)
#define EvtNetETH_SetMacAddress             EventID (EventLevelOp,    EvtNetETH,  9)
#define EvtNetETH_SetVlanIdentifier         EventID (EventLevelOp,    EvtNetETH, 10)
#define EvtNetETH_SetIp4Address             EventID (EventLevelOp,    EvtNetETH, 11)
#define EvtNetETH_SetIp4SubnetMask          EventID (EventLevelOp,    EvtNetETH, 12)
#define EvtNetETH_SetIp4DefaultGateway      EventID (EventLevelOp,    EvtNetETH, 13)
#define EvtNetETH_SetIp4PrimaryDNS          EventID (EventLevelOp,    EvtNetETH, 14)
#define EvtNetETH_SetIp4SecondaryDNS        EventID (EventLevelOp,    EvtNetETH, 15)
#define EvtNetETH_SetIp4Mtu                 EventID (EventLevelOp,    EvtNetETH, 16)
#define EvtNetETH_SetIp6Address             EventID (EventLevelOp,    EvtNetETH, 17)
#define EvtNetETH_SetIp6DefaultGateway      EventID (EventLevelOp,    EvtNetETH, 18)
#define EvtNetETH_SetIp6PrimaryDNS          EventID (EventLevelOp,    EvtNetETH, 19)
#define EvtNetETH_SetIp6SecondaryDNS        EventID (EventLevelOp,    EvtNetETH, 20)
#define EvtNetETH_SetIp6PrefixLength        EventID (EventLevelOp,    EvtNetETH, 21)
#define EvtNetETH_SetIp6Mtu                 EventID (EventLevelOp,    EvtNetETH, 47)
#define EvtNetETH_SendFrame                 EventID (EventLevelOp,    EvtNetETH, 22)
#define EvtNetETH_LinkDownError             EventID (EventLevelError, EvtNetETH, 23)
#define EvtNetETH_SendDataTooLarge          EventID (EventLevelError, EvtNetETH, 24)
#define EvtNetETH_SendIp4Disabled           EventID (EventLevelError, EvtNetETH, 45)
#define EvtNetETH_Ip4LocalAddressUndefined  EventID (EventLevelError, EvtNetETH, 25)
#define EvtNetETH_Ip4MacAddressUnresolved   EventID (EventLevelError, EvtNetETH, 26)
#define EvtNetETH_EnqueueFrame              EventID (EventLevelOp,    EvtNetETH, 27)
#define EvtNetETH_SendIp6Disabled           EventID (EventLevelError, EvtNetETH, 46)
#define EvtNetETH_Ip6LocalAddressUndefined  EventID (EventLevelError, EvtNetETH, 28)
#define EvtNetETH_Ip6MacAddressUnresolved   EventID (EventLevelError, EvtNetETH, 29)
#define EvtNetETH_InvalidIpVersion          EventID (EventLevelError, EvtNetETH, 30)
#define EvtNetETH_ShowFrameHeader           EventID (EventLevelDetail,EvtNetETH, 31)
#define EvtNetETH_LinkDownStatus            EventID (EventLevelOp,    EvtNetETH, 32)
#define EvtNetETH_LinkUpStatus              EventID (EventLevelOp,    EvtNetETH, 33)
#define EvtNetETH_ReceiveFrame              EventID (EventLevelOp,    EvtNetETH, 34)
#define EvtNetETH_VlanInvalid               EventID (EventLevelOp,    EvtNetETH, 35)
#define EvtNetETH_Ip4Disabled               EventID (EventLevelOp,    EvtNetETH, 36)
#define EvtNetETH_Ip6Disabled               EventID (EventLevelOp,    EvtNetETH, 37)
#define EvtNetETH_ProtocolUnknown           EventID (EventLevelOp,    EvtNetETH, 38)
#define EvtNetETH_SendRawFrame              EventID (EventLevelAPI,   EvtNetETH, 39)
#define EvtNetETH_SendRawInvalidParameter   EventID (EventLevelError, EvtNetETH, 40)
#define EvtNetETH_OutputLowLevel            EventID (EventLevelOp,    EvtNetETH, 41)
#define EvtNetETH_UninitInterface           EventID (EventLevelOp,    EvtNetETH, 42)
#endif

/**
  \brief  Event on Ethernet interface initialize (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_InitInterface(uint8_t if_num) {
    EventRecord2 (EvtNetETH_InitInterface, if_num, 0);
  }
#else
  #define EvrNetETH_InitInterface(if_num)
#endif

/**
  \brief  Event on Ethernet MAC address configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_MacAddressConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_MacAddressConfigError, if_num, 0);
  }
#else
  #define EvrNetETH_MacAddressConfigError(if_num)
#endif

/**
  \brief  Event on Ethernet VLAN configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_VlanConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_VlanConfigError, if_num, 0);
  }
#else
  #define EvrNetETH_VlanConfigError(if_num)
#endif

/**
  \brief  Event on Ethernet VLAN initialization error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_VlanInitError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_VlanInitError, if_num, 0);
  }
#else
  #define EvrNetETH_VlanInitError(if_num)
#endif

/**
  \brief  Event on Ethernet PHY configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_PhyDriverConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_PhyDriverConfigError, if_num, 0);
  }
#else
  #define EvrNetETH_PhyDriverConfigError(if_num)
#endif

/**
  \brief  Event on Ethernet PHY initialization error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_PhyDriverInitError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_PhyDriverInitError, if_num, 0);
  }
#else
  #define EvrNetETH_PhyDriverInitError(if_num)
#endif

/**
  \brief  Event on Ethernet IPv4 configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip4ConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip4ConfigError, if_num, 0);
  }
#else
  #define EvrNetETH_Ip4ConfigError(if_num)
#endif

/**
  \brief  Event on Ethernet IPv6 configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip6ConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip6ConfigError, if_num, 0);
  }
#else
  #define EvrNetETH_Ip6ConfigError(if_num)
#endif

/**
  \brief  Event on Ethernet thread create failed (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_ThreadCreateFailed(uint8_t if_num) {
    EventRecord2 (EvtNetETH_ThreadCreateFailed, if_num, 0);
  }
#else
  #define EvrNetETH_ThreadCreateFailed(if_num)
#endif

/**
  \brief  Event on Ethernet semaphore create failed (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SemaphoreCreateFailed(uint8_t if_num) {
    EventRecord2 (EvtNetETH_SemaphoreCreateFailed, if_num, 0);
  }
#else
  #define EvrNetETH_SemaphoreCreateFailed(if_num)
#endif

/**
  \brief  Event on Ethernet get_option invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_GetOptionInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetETH_GetOptionInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetETH_GetOptionInvalidParameter(if_num)
#endif

/**
  \brief  Event on Ethernet set_option invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetOptionInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetETH_SetOptionInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetETH_SetOptionInvalidParameter(if_num)
#endif

/**
  \brief  Event on Ethernet set interface MAC address (Op)
  \param  if_num        interface number
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetMacAddress(uint8_t if_num, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_num;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetETH_SetMacAddress, &evr_buf, 8);
  }
#else
  #define EvrNetETH_SetMacAddress(if_num, mac_addr)
#endif

/**
  \brief  Event on Ethernet set VLAN identifier (Op)
  \param  if_num        interface number
  \param  vlan_id       VLAN identifier
                         - 0: VLAN tagging disabled
  \remark VLAN identifier is limited in the range from 1 to 4093.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetVlanIdentifier(uint8_t if_num, uint16_t vlan_id) {
    EventRecord2 (EvtNetETH_SetVlanIdentifier, if_num, vlan_id);
  }
#else
  #define EvrNetETH_SetVlanIdentifier(if_num, vlan_id)
#endif

/**
  \brief  Event on Ethernet set interface IPv4 address (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4Address(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetETH_SetIp4Address, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_SetIp4Address(if_num, ip4_addr)
#endif

/**
  \brief  Event on Ethernet set interface IPv4 subnet mask (Op)
  \param  if_num        interface number
  \param  net_mask      pointer to IPv4 subnet mask
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4SubnetMask(uint8_t if_num, const uint8_t *net_mask) {
    memcpy (&evr_buf.u32[0], net_mask, 4);
    EventRecord2 (EvtNetETH_SetIp4SubnetMask, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_SetIp4SubnetMask(if_num, net_mask)
#endif

/**
  \brief  Event on Ethernet set interface IPv4 default gateway (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4DefaultGateway(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetETH_SetIp4DefaultGateway, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_SetIp4DefaultGateway(if_num, ip4_addr)
#endif

/**
  \brief  Event on Ethernet set interface primary DNS server (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4PrimaryDNS(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetETH_SetIp4PrimaryDNS, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_SetIp4PrimaryDNS(if_num, ip4_addr)
#endif

/**
  \brief  Event on Ethernet set interface secondary DNS server (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4SecondaryDNS(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetETH_SetIp4SecondaryDNS, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_SetIp4SecondaryDNS(if_num, ip4_addr)
#endif

/**
  \brief  Event on Ethernet set interface MTU (Op)
  \param  if_num        interface number
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 576 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp4Mtu(uint8_t if_num, uint16_t mtu) {
    EventRecord2 (EvtNetETH_SetIp4Mtu, if_num, mtu);
  }
#else
  #define EvrNetETH_SetIp4Mtu(if_num, mtu)
#endif

/**
  \brief  Event on Ethernet set interface IPv6 static address (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6Address(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetETH_SetIp6Address, &evr_buf, 20);
  }
#else
  #define EvrNetETH_SetIp6Address(if_num, ip6_addr)
#endif

/**
  \brief  Event on Ethernet set interface IPv6 default gateway (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6DefaultGateway(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetETH_SetIp6DefaultGateway, &evr_buf, 20);
  }
#else
  #define EvrNetETH_SetIp6DefaultGateway(if_num, ip6_addr)
#endif

/**
  \brief  Event on Ethernet set interface primary DNS server (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6PrimaryDNS(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetETH_SetIp6PrimaryDNS, &evr_buf, 20);
  }
#else
  #define EvrNetETH_SetIp6PrimaryDNS(if_num, ip6_addr)
#endif

/**
  \brief  Event on Ethernet set interface secondary DNS server (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6SecondaryDNS(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetETH_SetIp6SecondaryDNS, &evr_buf, 20);
  }
#else
  #define EvrNetETH_SetIp6SecondaryDNS(if_num, ip6_addr)
#endif

/**
  \brief  Event on Ethernet set interface IPv6 subnet prefix length (Op)
  \param  if_num        interface number
  \param  pref_len      subnet prefix length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6PrefixLength(uint8_t if_num, uint8_t pref_len) {
    EventRecord2 (EvtNetETH_SetIp6PrefixLength, if_num, pref_len);
  }
#else
  #define EvrNetETH_SetIp6PrefixLength(if_num, pref_len)
#endif

/**
  \brief  Event on Ethernet set interface MTU (Op)
  \param  if_num        interface number
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 1280 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SetIp6Mtu(uint8_t if_num, uint16_t mtu) {
    EventRecord2 (EvtNetETH_SetIp6Mtu, if_num, mtu);
  }
#else
  #define EvrNetETH_SetIp6Mtu(if_num, mtu)
#endif

/**
  \brief  Event on Ethernet send frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendFrame(uint8_t if_num, uint32_t length, uint8_t ip_version) {
    uint32_t val2 = ((uint32_t)ip_version << 16) | length;
    EventRecord2 (EvtNetETH_SendFrame, if_num, val2);
  }
#else
  #define EvrNetETH_SendFrame(if_num, length, ip_version)
#endif

/**
  \brief  Event on Ethernet link down error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_LinkDownError(uint8_t if_num) {
    EventRecord2 (EvtNetETH_LinkDownError, if_num, 0);
  }
#else
  #define EvrNetETH_LinkDownError(if_num)
#endif

/**
  \brief  Event on Ethernet send payload too large error (Error)
  \param  if_num        interface number
  \param  length        data length in bytes
  \param  max_length    maximum length of data in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendDataTooLarge(uint8_t if_num, uint32_t length, uint32_t max_length) {
    uint32_t val2 = (max_length << 16) | length;
    EventRecord2 (EvtNetETH_SendDataTooLarge, if_num, val2);
  }
#else
  #define EvrNetETH_SendDataTooLarge(if_num, length, max_length)
#endif

/**
  \brief  Event on Ethernet send IPv4 disabled error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendIp4Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetETH_SendIp4Disabled, if_num, 0);
  }
#else
  #define EvrNetETH_SendIp4Disabled(if_num)
#endif

/**
  \brief  Event on Ethernet local IPv4 address undefined (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip4LocalAddressUndefined(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip4LocalAddressUndefined, if_num, 0);
  }
#else
  #define EvrNetETH_Ip4LocalAddressUndefined(if_num)
#endif

/**
  \brief  Event on Ethernet MAC address unresolved (Error)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip4MacAddressUnresolved(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetETH_Ip4MacAddressUnresolved, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetETH_Ip4MacAddressUnresolved(if_num, ip4_addr)
#endif

/**
  \brief  Event on enqueued Ethernet frame for unresolved MAC address (Op)
  \param  if_num        interface number
  \param  ca_entry      cache entry identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_EnqueueFrame(uint8_t if_num, uint8_t ca_entry, uint32_t length) {
    uint32_t val2 = ((uint32_t)ca_entry << 16) | length;
    EventRecord2 (EvtNetETH_EnqueueFrame, if_num, val2);
  }
#else
  #define EvrNetETH_EnqueueFrame(if_num, ca_entry, length)
#endif

/**
  \brief  Event on Ethernet send IPv6 disabled error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendIp6Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetETH_SendIp6Disabled, if_num, 0);
  }
#else
  #define EvrNetETH_SendIp6Disabled(if_num)
#endif

/**
  \brief  Event on Ethernet local IPv6 address undefined (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip6LocalAddressUndefined(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip6LocalAddressUndefined, if_num, 0);
  }
#else
  #define EvrNetETH_Ip6LocalAddressUndefined(if_num)
#endif

/**
  \brief  Event on Ethernet MAC address unresolved (Error)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip6MacAddressUnresolved(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetETH_Ip6MacAddressUnresolved, &evr_buf, 20);
  }
#else
  #define EvrNetETH_Ip6MacAddressUnresolved(if_num, ip6_addr)
#endif

/**
  \brief  Event on Ethernet invalid IP version (Error)
  \param  if_num        interface number
  \param  ip_version    invalid IP protocol version value
                        - 0: IPv4
                        - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_InvalidIpVersion(uint8_t if_num, uint8_t ip_version) {
    EventRecord2 (EvtNetETH_InvalidIpVersion, if_num, ip_version);
  }
#else
  #define EvrNetETH_InvalidIpVersion(if_num, ip_version)
#endif

/**
  \brief  Event on Ethernet display send/receive frame header (Detail)
  \param  eth_header    pointer to ethernet frame header of 14 bytes
                         - DstAddr  (6 bytes)
                         - SrcAddr  (6 bytes)
                         - Protocol (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_ShowFrameHeader(const void *eth_header) {
    EventRecordData (EvtNetETH_ShowFrameHeader, eth_header, 14);
  }
#else
  #define EvrNetETH_ShowFrameHeader(eth_header)
#endif

/**
  \brief  Event on Ethernet link down status (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_LinkDownStatus(uint8_t if_num) {
    EventRecord2 (EvtNetETH_LinkDownStatus, if_num, 0);
  }
#else
  #define EvrNetETH_LinkDownStatus(if_num)
#endif

/**
  \brief  Event on Ethernet link up status (Op)
  \param  if_num        interface number
  \param  link_info     link information:
                        - duplex (bit 2)
                        - speed  (bit 1,0)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_LinkUpStatus(uint8_t if_num, uint8_t link_info) {
    EventRecord2 (EvtNetETH_LinkUpStatus, if_num, link_info);
  }
#else
  #define EvrNetETH_LinkUpStatus(if_num, link_info)
#endif

/**
  \brief  Event on Ethernet receive frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_ReceiveFrame(uint8_t if_num, uint32_t length) {
    EventRecord2 (EvtNetETH_ReceiveFrame, if_num, length);
  }
#else
  #define EvrNetETH_ReceiveFrame(if_num, length)
#endif

/**
  \brief  Event on Ethernet receive VLAN invalid (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_VlanInvalid(uint8_t if_num) {
    EventRecord2 (EvtNetETH_VlanInvalid, if_num, 0);
  }
#else
  #define EvrNetETH_VlanInvalid(if_num)
#endif

/**
  \brief  Event on Ethernet receive IPv4 disabled (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip4Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip4Disabled, if_num, 0);
  }
#else
  #define EvrNetETH_Ip4Disabled(if_num)
#endif

/**
  \brief  Event on Ethernet receive IPv6 disabled (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_Ip6Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetETH_Ip6Disabled, if_num, 0);
  }
#else
  #define EvrNetETH_Ip6Disabled(if_num)
#endif

/**
  \brief  Event on Ethernet receive protocol unknown (Op)
  \param  if_num        interface number
  \param  protocol      unknown ethernet protocol type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_ProtocolUnknown(uint8_t if_num, uint16_t protocol) {
    EventRecord2 (EvtNetETH_ProtocolUnknown, if_num, protocol);
  }
#else
  #define EvrNetETH_ProtocolUnknown(if_num, protocol)
#endif

/**
  \brief  Event on Ethernet send raw frame (API)
  \param  if_num        interface number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendRawFrame(uint8_t if_num, uint32_t length) {
    EventRecord2 (EvtNetETH_SendRawFrame, if_num, length);
  }
#else
  #define EvrNetETH_SendRawFrame(if_num, length)
#endif

/**
  \brief  Event on Ethernet send raw frame invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_SendRawInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetETH_SendRawInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetETH_SendRawInvalidParameter(if_num)
#endif

/**
  \brief  Event on output Ethernet frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_OutputLowLevel(uint8_t if_num, uint32_t length) {
    EventRecord2 (EvtNetETH_OutputLowLevel, if_num, length);
  }
#else
  #define EvrNetETH_OutputLowLevel(if_num, length)
#endif

/**
  \brief  Event on Ethernet interface de-initialize (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetETH_UninitInterface(uint8_t if_num) {
    EventRecord2 (EvtNetETH_UninitInterface, if_num, 0);
  }
#else
  #define EvrNetETH_UninitInterface(if_num)
#endif


// NetWiFi event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetWiFi_InitInterface             EventID (EventLevelOp,    EvtNetWiFi,  0)
#define EvtNetWiFi_DriverInitFailed          EventID (EventLevelError, EvtNetWiFi,  1)
#define EvtNetWiFi_SetBypassModeFailed       EventID (EventLevelError, EvtNetWiFi,  2)
#define EvtNetWiFi_GetMacAddressFailed       EventID (EventLevelError, EvtNetWiFi,  3)
#define EvtNetWiFi_DriverMacAddress          EventID (EventLevelOp,    EvtNetWiFi,  4)
#define EvtNetWiFi_MacAddressConfigError     EventID (EventLevelError, EvtNetWiFi,  5)
#define EvtNetWiFi_SetMacAddressFailed       EventID (EventLevelError, EvtNetWiFi,  6)
#define EvtNetWiFi_Ip4ConfigError            EventID (EventLevelError, EvtNetWiFi,  7)
#define EvtNetWiFi_Ip6ConfigError            EventID (EventLevelError, EvtNetWiFi,  8)
#define EvtNetWiFi_ThreadCreateFailed        EventID (EventLevelError, EvtNetWiFi,  9)
#define EvtNetWiFi_SemaphoreCreateFailed     EventID (EventLevelError, EvtNetWiFi, 59) // End
#define EvtNetWiFi_GetOptionInvalidParameter EventID (EventLevelError, EvtNetWiFi, 10)
#define EvtNetWiFi_SetOptionInvalidParameter EventID (EventLevelError, EvtNetWiFi, 11)
#define EvtNetWiFi_SetMacAddress             EventID (EventLevelOp,    EvtNetWiFi, 12)
#define EvtNetWiFi_SetIp4Address             EventID (EventLevelOp,    EvtNetWiFi, 13)
#define EvtNetWiFi_SetIp4SubnetMask          EventID (EventLevelOp,    EvtNetWiFi, 14)
#define EvtNetWiFi_SetIp4DefaultGateway      EventID (EventLevelOp,    EvtNetWiFi, 15)
#define EvtNetWiFi_SetIp4PrimaryDNS          EventID (EventLevelOp,    EvtNetWiFi, 16)
#define EvtNetWiFi_SetIp4SecondaryDNS        EventID (EventLevelOp,    EvtNetWiFi, 17)
#define EvtNetWiFi_SetIp4Mtu                 EventID (EventLevelOp,    EvtNetWiFi, 18)
#define EvtNetWiFi_SetIp6Address             EventID (EventLevelOp,    EvtNetWiFi, 19)
#define EvtNetWiFi_SetIp6DefaultGateway      EventID (EventLevelOp,    EvtNetWiFi, 20)
#define EvtNetWiFi_SetIp6PrimaryDNS          EventID (EventLevelOp,    EvtNetWiFi, 21)
#define EvtNetWiFi_SetIp6SecondaryDNS        EventID (EventLevelOp,    EvtNetWiFi, 22)
#define EvtNetWiFi_SetIp6PrefixLength        EventID (EventLevelOp,    EvtNetWiFi, 23)
#define EvtNetWiFi_SetIp6Mtu                 EventID (EventLevelOp,    EvtNetWiFi, 58)
#define EvtNetWiFi_Scan                      EventID (EventLevelAPI,   EvtNetWiFi, 24)
#define EvtNetWiFi_ScanWrongMode             EventID (EventLevelError, EvtNetWiFi, 25)
#define EvtNetWiFi_ScanInvalidParameter      EventID (EventLevelError, EvtNetWiFi, 26)
#define EvtNetWiFi_ScanComplete              EventID (EventLevelOp,    EvtNetWiFi, 27)
#define EvtNetWiFi_GetOption                 EventID (EventLevelAPI,   EvtNetWiFi, 28)
#define EvtNetWiFi_SetOption                 EventID (EventLevelAPI,   EvtNetWiFi, 29)
#define EvtNetWiFi_Activate                  EventID (EventLevelAPI,   EvtNetWiFi, 30)
#define EvtNetWiFi_ActivateInvalidParameter  EventID (EventLevelError, EvtNetWiFi, 31)
#define EvtNetWiFi_Deactivate                EventID (EventLevelAPI,   EvtNetWiFi, 32)
#define EvtNetWiFi_DeactivateInvalidParam    EventID (EventLevelError, EvtNetWiFi, 33)
#define EvtNetWiFi_GetNetInfo                EventID (EventLevelAPI,   EvtNetWiFi, 34)
#define EvtNetWiFi_GetNetInfoWrongMode       EventID (EventLevelError, EvtNetWiFi, 35)
#define EvtNetWiFi_GetNetInfoInvalidParam    EventID (EventLevelError, EvtNetWiFi, 36)
#define EvtNetWiFi_SendFrame                 EventID (EventLevelOp,    EvtNetWiFi, 37)
#define EvtNetWiFi_NotConnected              EventID (EventLevelError, EvtNetWiFi, 38)
#define EvtNetWiFi_SendDataTooLarge          EventID (EventLevelError, EvtNetWiFi, 39)
#define EvtNetWiFi_SendIp4Disabled           EventID (EventLevelError, EvtNetWiFi, 56)
#define EvtNetWiFi_Ip4LocalAddressUndefined  EventID (EventLevelError, EvtNetWiFi, 40)
#define EvtNetWiFi_Ip4MacAddressUnresolved   EventID (EventLevelError, EvtNetWiFi, 41)
#define EvtNetWiFi_EnqueueFrame              EventID (EventLevelOp,    EvtNetWiFi, 42)
#define EvtNetWiFi_SendIp6Disabled           EventID (EventLevelError, EvtNetWiFi, 57)
#define EvtNetWiFi_Ip6LocalAddressUndefined  EventID (EventLevelError, EvtNetWiFi, 43)
#define EvtNetWiFi_Ip6MacAddressUnresolved   EventID (EventLevelError, EvtNetWiFi, 44)
#define EvtNetWiFi_InvalidIpVersion          EventID (EventLevelError, EvtNetWiFi, 45)
#define EvtNetWiFi_ShowFrameHeader           EventID (EventLevelDetail,EvtNetWiFi, 46)
#define EvtNetWiFi_LinkStateChange           EventID (EventLevelOp,    EvtNetWiFi, 47)
#define EvtNetWiFi_ReceiveFrame              EventID (EventLevelOp,    EvtNetWiFi, 48)
#define EvtNetWiFi_Ip4Disabled               EventID (EventLevelOp,    EvtNetWiFi, 49)
#define EvtNetWiFi_Ip6Disabled               EventID (EventLevelOp,    EvtNetWiFi, 50)
#define EvtNetWiFi_ProtocolUnknown           EventID (EventLevelOp,    EvtNetWiFi, 51)
#define EvtNetWiFi_OutputLowLevel            EventID (EventLevelOp,    EvtNetWiFi, 52)
#define EvtNetWiFi_TxQueueOverflow           EventID (EventLevelError, EvtNetWiFi, 53)
#define EvtNetWiFi_OutputNoMemory            EventID (EventLevelError, EvtNetWiFi, 54)
#define EvtNetWiFi_UninitInterface           EventID (EventLevelOp,    EvtNetWiFi, 55)
#endif

/**
  \brief  Event on WiFi interface initialize (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_InitInterface(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_InitInterface, if_num, 0);
  }
#else
  #define EvrNetWiFi_InitInterface(if_num)
#endif

/**
  \brief  Event on WiFi failed to initialize the driver (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_DriverInitFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_DriverInitFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_DriverInitFailed(if_num)
#endif

/**
  \brief  Event on WiFi failed to activate bypass mode (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetBypassModeFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SetBypassModeFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_SetBypassModeFailed(if_num)
#endif

/**
  \brief  Event on WiFi driver get MAC address failed (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetMacAddressFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_GetMacAddressFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_GetMacAddressFailed(if_num)
#endif

/**
  \brief  Event on WiFi driver provided MAC address (Op)
  \param  if_num        interface number
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_DriverMacAddress(uint8_t if_num, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_num;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetWiFi_DriverMacAddress, &evr_buf, 8);
  }
#else
  #define EvrNetWiFi_DriverMacAddress(if_num, mac_addr)
#endif

/**
  \brief  Event on WiFi MAC address configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_MacAddressConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_MacAddressConfigError, if_num, 0);
  }
#else
  #define EvrNetWiFi_MacAddressConfigError(if_num)
#endif

/**
  \brief  Event on WiFi driver failed to set MAC address (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetMacAddressFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SetMacAddressFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_SetMacAddressFailed(if_num)
#endif

/**
  \brief  Event on WiFi IPv4 configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip4ConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip4ConfigError, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip4ConfigError(if_num)
#endif

/**
  \brief  Event on WiFi IPv6 configuration error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip6ConfigError(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip6ConfigError, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip6ConfigError(if_num)
#endif

/**
  \brief  Event on WiFi thread create failed (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ThreadCreateFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_ThreadCreateFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_ThreadCreateFailed(if_num)
#endif

/**
  \brief  Event on WiFi semaphore create failed (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SemaphoreCreateFailed(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SemaphoreCreateFailed, if_num, 0);
  }
#else
  #define EvrNetWiFi_SemaphoreCreateFailed(if_num)
#endif

/**
  \brief  Event on WiFi get_option invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetOptionInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_GetOptionInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetWiFi_GetOptionInvalidParameter(if_num)
#endif

/**
  \brief  Event on WiFi set_option invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetOptionInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SetOptionInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetWiFi_SetOptionInvalidParameter(if_num)
#endif

/**
  \brief  Event on WiFi set interface MAC address (Op)
  \param  if_num        interface number
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetMacAddress(uint8_t if_num, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_num;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetWiFi_SetMacAddress, &evr_buf, 8);
  }
#else
  #define EvrNetWiFi_SetMacAddress(if_num, mac_addr)
#endif

/**
  \brief  Event on WiFi set interface IPv4 address (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4Address(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetWiFi_SetIp4Address, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_SetIp4Address(if_num, ip4_addr)
#endif

/**
  \brief  Event on WiFi set interface IPv4 subnet mask (Op)
  \param  if_num        interface number
  \param  net_mask      pointer to IPv4 subnet mask
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4SubnetMask(uint8_t if_num, const uint8_t *net_mask) {
    memcpy (&evr_buf.u32[0], net_mask, 4);
    EventRecord2 (EvtNetWiFi_SetIp4SubnetMask, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_SetIp4SubnetMask(if_num, net_mask)
#endif

/**
  \brief  Event on WiFi set interface IPv4 default gateway (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4DefaultGateway(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetWiFi_SetIp4DefaultGateway, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_SetIp4DefaultGateway(if_num, ip4_addr)
#endif

/**
  \brief  Event on WiFi set interface primary DNS server (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4PrimaryDNS(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetWiFi_SetIp4PrimaryDNS, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_SetIp4PrimaryDNS(if_num, ip4_addr)
#endif

/**
  \brief  Event on WiFi set interface secondary DNS server (Op)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4SecondaryDNS(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetWiFi_SetIp4SecondaryDNS, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_SetIp4SecondaryDNS(if_num, ip4_addr)
#endif

/**
  \brief  Event on WiFi set interface MTU (Op)
  \param  if_num        interface number
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 576 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp4Mtu(uint8_t if_num, uint16_t mtu) {
    EventRecord2 (EvtNetWiFi_SetIp4Mtu, if_num, mtu);
  }
#else
  #define EvrNetWiFi_SetIp4Mtu(if_num, mtu)
#endif

/**
  \brief  Event on WiFi set interface IPv6 static address (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6Address(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetWiFi_SetIp6Address, &evr_buf, 20);
  }
#else
  #define EvrNetWiFi_SetIp6Address(if_num, ip6_addr)
#endif

/**
  \brief  Event on WiFi set interface IPv6 default gateway (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6DefaultGateway(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetWiFi_SetIp6DefaultGateway, &evr_buf, 20);
  }
#else
  #define EvrNetWiFi_SetIp6DefaultGateway(if_num, ip6_addr)
#endif

/**
  \brief  Event on WiFi set interface primary DNS server (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6PrimaryDNS(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetWiFi_SetIp6PrimaryDNS, &evr_buf, 20);
  }
#else
  #define EvrNetWiFi_SetIp6PrimaryDNS(if_num, ip6_addr)
#endif

/**
  \brief  Event on WiFi set interface secondary DNS server (Op)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6SecondaryDNS(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetWiFi_SetIp6SecondaryDNS, &evr_buf, 20);
  }
#else
  #define EvrNetWiFi_SetIp6SecondaryDNS(if_num, ip6_addr)
#endif

/**
  \brief  Event on WiFi set interface IPv6 subnet prefix length (Op)
  \param  if_num        interface number
  \param  pref_len      subnet prefix length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6PrefixLength(uint8_t if_num, uint8_t pref_len) {
    EventRecord2 (EvtNetWiFi_SetIp6PrefixLength, if_num, pref_len);
  }
#else
  #define EvrNetWiFi_SetIp6PrefixLength(if_num, pref_len)
#endif

/**
  \brief  Event on WiFi set interface MTU (Op)
  \param  if_num        interface number
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 1280 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetIp6Mtu(uint8_t if_num, uint16_t mtu) {
    EventRecord2 (EvtNetWiFi_SetIp6Mtu, if_num, mtu);
  }
#else
  #define EvrNetWiFi_SetIp6Mtu(if_num, mtu)
#endif

/**
  \brief  Event on WiFi scan wireless networks (API)
  \param  if_num        interface number
  \param  max_num       maximum number of networks to scan
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Scan(uint8_t if_num, uint32_t max_num) {
    EventRecord2 (EvtNetWiFi_Scan, if_num, max_num);
  }
#else
  #define EvrNetWiFi_Scan(if_num, max_num)
#endif

/**
  \brief  Event on WiFi scan in wrong mode (Error)
  \param  if_num        interface number
  \param  mode          WiFi interface mode
                         - 0: Station
                         - 1: Access Point
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ScanWrongMode(uint8_t if_num, uint32_t mode) {
    EventRecord2 (EvtNetWiFi_ScanWrongMode, if_num, mode);
  }
#else
  #define EvrNetWiFi_ScanWrongMode(if_num, mode)
#endif

/**
  \brief  Event on WiFi scan invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ScanInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_ScanInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetWiFi_ScanInvalidParameter(if_num)
#endif

/**
  \brief  Event on WiFi scan wireless networks complete (Op)
  \param  if_num        interface number
  \param  num           number of wireless networks found
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ScanComplete(uint8_t if_num, uint32_t num) {
    EventRecord2 (EvtNetWiFi_ScanComplete, if_num, num);
  }
#else
  #define EvrNetWiFi_ScanComplete(if_num, num)
#endif

/**
  \brief  Event on WiFi get driver option (API)
  \param  if_num        interface number
  \param  opt           WiFi driver option to get
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetOption(uint8_t if_num, uint32_t opt) {
    EventRecord2 (EvtNetWiFi_GetOption, if_num, opt);
  }
#else
  #define EvrNetWiFi_GetOption(if_num, opt)
#endif

/**
  \brief  Event on WiFi set driver option (API)
  \param  if_num        interface number
  \param  opt           WiFi driver option to set
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SetOption(uint8_t if_num, uint32_t opt) {
    EventRecord2 (EvtNetWiFi_SetOption, if_num, opt);
  }
#else
  #define EvrNetWiFi_SetOption(if_num, opt)
#endif

/**
  \brief  Event on WiFi interface activate (API)
  \param  if_num        interface number
  \param  mode          WiFi interface mode
                         - 0: Station
                         - 1: Access Point
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Activate(uint8_t if_num, uint32_t mode) {
    EventRecord2 (EvtNetWiFi_Activate, if_num, mode);
  }
#else
  #define EvrNetWiFi_Activate(if_num, mode)
#endif

/**
  \brief  Event on WiFi activate invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ActivateInvalidParameter(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_ActivateInvalidParameter, if_num, 0);
  }
#else
  #define EvrNetWiFi_ActivateInvalidParameter(if_num)
#endif

/**
  \brief  Event on WiFi interface deactivate (API)
  \param  if_num        interface number
  \param  mode          WiFi interface mode
                         - 0: Station
                         - 1: Access Point
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Deactivate(uint8_t if_num, uint32_t mode) {
    EventRecord2 (EvtNetWiFi_Deactivate, if_num, mode);
  }
#else
  #define EvrNetWiFi_Deactivate(if_num, mode)
#endif

/**
  \brief  Event on WiFi deactivate invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_DeactivateInvalidParam(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_DeactivateInvalidParam, if_num, 0);
  }
#else
  #define EvrNetWiFi_DeactivateInvalidParam(if_num)
#endif

/**
  \brief  Event on WiFi get network information (API)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetNetInfo(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_GetNetInfo, if_num, 0);
  }
#else
  #define EvrNetWiFi_GetNetInfo(if_num)
#endif

/**
  \brief  Event on WiFi get network information in wrong mode (Error)
  \param  if_num        interface number
  \param  mode          WiFi interface mode
                         - 0: Station
                         - 1: Access Point
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetNetInfoWrongMode(uint8_t if_num, uint32_t mode) {
    EventRecord2 (EvtNetWiFi_GetNetInfoWrongMode, if_num, mode);
  }
#else
  #define EvrNetWiFi_GetNetInfoWrongMode(if_num, mode)
#endif

/**
  \brief  Event on WiFi get network information invalid parameter (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_GetNetInfoInvalidParam(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_GetNetInfoInvalidParam, if_num, 0);
  }
#else
  #define EvrNetWiFi_GetNetInfoInvalidParam(if_num)
#endif

/**
  \brief  Event on WiFi send frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SendFrame(uint8_t if_num, uint32_t length, uint8_t ip_version) {
    uint32_t val2 = ((uint32_t)ip_version << 16) | length;
    EventRecord2 (EvtNetWiFi_SendFrame, if_num, val2);
  }
#else
  #define EvrNetWiFi_SendFrame(if_num, length, ip_version)
#endif

/**
  \brief  Event on WiFi station not connected (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_NotConnected(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_NotConnected, if_num, 0);
  }
#else
  #define EvrNetWiFi_NotConnected(if_num)
#endif

/**
  \brief  Event on WiFi send payload too large error (Error)
  \param  if_num        interface number
  \param  length        data length in bytes
  \param  max_length    maximum length of data in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SendDataTooLarge(uint8_t if_num, uint32_t length, uint32_t max_length) {
    uint32_t val2 = (max_length << 16) | length;
    EventRecord2 (EvtNetWiFi_SendDataTooLarge, if_num, val2);
  }
#else
  #define EvrNetWiFi_SendDataTooLarge(if_num, length, max_length)
#endif

/**
  \brief  Event on WiFi send IPv4 disabled error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SendIp4Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SendIp4Disabled, if_num, 0);
  }
#else
  #define EvrNetWiFi_SendIp4Disabled(if_num)
#endif

/**
  \brief  Event on WiFi local IPv4 address undefined (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip4LocalAddressUndefined(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip4LocalAddressUndefined, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip4LocalAddressUndefined(if_num)
#endif

/**
  \brief  Event on WiFi MAC address unresolved (Error)
  \param  if_num        interface number
  \param  ip4_addr      pointer to IPv4 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip4MacAddressUnresolved(uint8_t if_num, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetWiFi_Ip4MacAddressUnresolved, if_num, evr_buf.u32[0]);
  }
#else
  #define EvrNetWiFi_Ip4MacAddressUnresolved(if_num, ip4_addr)
#endif

/**
  \brief  Event on enqueued WiFi frame for unresolved MAC address (Op)
  \param  if_num        interface number
  \param  ca_entry      cache entry identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_EnqueueFrame(uint8_t if_num, uint32_t ca_entry, uint32_t length) {
    uint32_t val2 = ((uint32_t)ca_entry << 16) | length;
    EventRecord2 (EvtNetWiFi_EnqueueFrame, if_num, val2);
  }
#else
  #define EvrNetWiFi_EnqueueFrame(if_num, ca_entry, length)
#endif

/**
  \brief  Event on WiFi send IPv6 disabled error (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_SendIp6Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_SendIp6Disabled, if_num, 0);
  }
#else
  #define EvrNetWiFi_SendIp6Disabled(if_num)
#endif

/**
  \brief  Event on WiFi local IPv6 address undefined (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip6LocalAddressUndefined(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip6LocalAddressUndefined, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip6LocalAddressUndefined(if_num)
#endif

/**
  \brief  Event on WiFi MAC address unresolved (Error)
  \param  if_num        interface number
  \param  ip6_addr      pointer to IPv6 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip6MacAddressUnresolved(uint8_t if_num, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_num;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetWiFi_Ip6MacAddressUnresolved, &evr_buf, 20);
  }
#else
  #define EvrNetWiFi_Ip6MacAddressUnresolved(if_num, ip6_addr)
#endif

/**
  \brief  Event on WiFi invalid IP version (Error)
  \param  if_num        interface number
  \param  ip_version    invalid IP protocol version value
                        - 0: IPv4
                        - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_InvalidIpVersion(uint8_t if_num, uint8_t ip_version) {
    EventRecord2 (EvtNetWiFi_InvalidIpVersion, if_num, ip_version);
  }
#else
  #define EvrNetWiFi_InvalidIpVersion(if_num, ip_version)
#endif

/**
  \brief  Event on WiFi display send/receive frame header (Detail)
  \param  eth_header    pointer to ethernet frame header of 14 bytes
                         - DstAddr  (6 bytes)
                         - SrcAddr  (6 bytes)
                         - Protocol (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ShowFrameHeader(const void *eth_header) {
    EventRecordData (EvtNetWiFi_ShowFrameHeader, eth_header, 14);
  }
#else
  #define EvrNetWiFi_ShowFrameHeader(eth_header)
#endif

/**
  \brief  Event on WiFi connection state change (Op)
  \param  if_num        interface number
  \param  state         wireless station connection state
                         - 0: disconnected
                         - 1: connected to Access Point
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_LinkStateChange(uint8_t if_num, uint32_t state) {
    EventRecord2 (EvtNetWiFi_LinkStateChange, if_num, state);
  }
#else
  #define EvrNetWiFi_LinkStateChange(if_num, state)
#endif

/**
  \brief  Event on WiFi receive frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ReceiveFrame(uint8_t if_num, uint32_t length) {
    EventRecord2 (EvtNetWiFi_ReceiveFrame, if_num, length);
  }
#else
  #define EvrNetWiFi_ReceiveFrame(if_num, length)
#endif

/**
  \brief  Event on WiFi receive IPv4 disabled (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip4Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip4Disabled, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip4Disabled(if_num)
#endif

/**
  \brief  Event on WiFi receive IPv6 disabled (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_Ip6Disabled(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_Ip6Disabled, if_num, 0);
  }
#else
  #define EvrNetWiFi_Ip6Disabled(if_num)
#endif

/**
  \brief  Event on WiFi receive protocol unknown (Op)
  \param  if_num        interface number
  \param  protocol      unknown ethernet protocol type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_ProtocolUnknown(uint8_t if_num, uint16_t protocol) {
    EventRecord2 (EvtNetWiFi_ProtocolUnknown, if_num, protocol);
  }
#else
  #define EvrNetWiFi_ProtocolUnknown(if_num, protocol)
#endif

/**
  \brief  Event on WiFi output frame (Op)
  \param  if_num        interface number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_OutputLowLevel(uint8_t if_num, uint32_t length) {
    EventRecord2 (EvtNetWiFi_OutputLowLevel, if_num, length);
  }
#else
  #define EvrNetWiFi_OutputLowLevel(if_num, length)
#endif

/**
  \brief  Event on WiFi output queue overflow (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_TxQueueOverflow(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_TxQueueOverflow, if_num, 0);
  }
#else
  #define EvrNetWiFi_TxQueueOverflow(if_num)
#endif

/**
  \brief  Event on WIFI output out of memory (Error)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_OutputNoMemory(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_OutputNoMemory, if_num, 0);
  }
#else
  #define EvrNetWiFi_OutputNoMemory(if_num)
#endif

/**
  \brief  Event on WiFi interface de-initialize (Op)
  \param  if_num        interface number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetWiFi_UninitInterface(uint8_t if_num) {
    EventRecord2 (EvtNetWiFi_UninitInterface, if_num, 0);
  }
#else
  #define EvrNetWiFi_UninitInterface(if_num)
#endif


// NetPPP event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
/* PPP-Core events */
#define EvtNetPPP_InitInterface             EventID (EventLevelOp,    EvtNetPPP,  0)
#define EvtNetPPP_ThreadCreateFailed        EventID (EventLevelError, EvtNetPPP,  1)
#define EvtNetPPP_SemaphoreCreateFailed     EventID (EventLevelError, EvtNetPPP,105) // End
#define EvtNetPPP_GetOptionInvalidParameter EventID (EventLevelError, EvtNetPPP,  2)
#define EvtNetPPP_SetOptionInvalidParameter EventID (EventLevelError, EvtNetPPP,  3)
#define EvtNetPPP_SetIp4Address             EventID (EventLevelOp,    EvtNetPPP,  4)
#define EvtNetPPP_SetIp4PrimaryDNS          EventID (EventLevelOp,    EvtNetPPP,  5)
#define EvtNetPPP_SetIp4SecondaryDNS        EventID (EventLevelOp,    EvtNetPPP,  6)
#define EvtNetPPP_SetIp4Mtu                 EventID (EventLevelOp,    EvtNetPPP,  7)
#define EvtNetPPP_ReceiveFrame              EventID (EventLevelOp,    EvtNetPPP,  8)
#define EvtNetPPP_FrameTooShort             EventID (EventLevelOp,    EvtNetPPP,  9)
#define EvtNetPPP_ChecksumFailed            EventID (EventLevelError, EvtNetPPP, 10)
#define EvtNetPPP_CtrlByteInvalid           EventID (EventLevelError, EvtNetPPP, 11)
#define EvtNetPPP_SendFrame                 EventID (EventLevelOp,    EvtNetPPP, 12)
#define EvtNetPPP_InvalidIpVersion          EventID (EventLevelError, EvtNetPPP, 13)
#define EvtNetPPP_NetworkLayerDown          EventID (EventLevelError, EvtNetPPP, 14)
#define EvtNetPPP_SendDataTooLarge          EventID (EventLevelError, EvtNetPPP, 15)
#define EvtNetPPP_NoMemoryError             EventID (EventLevelError, EvtNetPPP, 16)
#define EvtNetPPP_QueueAddTransmit          EventID (EventLevelOp,    EvtNetPPP, 17)
#define EvtNetPPP_RejectProtocol            EventID (EventLevelOp,    EvtNetPPP, 18)
#define EvtNetPPP_RejectCode                EventID (EventLevelOp,    EvtNetPPP, 19)
#define EvtNetPPP_Connect                   EventID (EventLevelAPI,   EvtNetPPP, 20)
#define EvtNetPPP_ConnectInvalidParameter   EventID (EventLevelError, EvtNetPPP, 21)
#define EvtNetPPP_ConnectWrongState         EventID (EventLevelError, EvtNetPPP, 22)
#define EvtNetPPP_Listen                    EventID (EventLevelAPI,   EvtNetPPP, 23)
#define EvtNetPPP_ListenInvalidParameter    EventID (EventLevelError, EvtNetPPP, 24)
#define EvtNetPPP_ListenWrongState          EventID (EventLevelError, EvtNetPPP, 25)
#define EvtNetPPP_Close                     EventID (EventLevelAPI,   EvtNetPPP, 26)
#define EvtNetPPP_ShowUsername              EventID (EventLevelOp,    EvtNetPPP, 27)
#define EvtNetPPP_ShowPassword              EventID (EventLevelOp,    EvtNetPPP, 28)
#define EvtNetPPP_ModemOffline              EventID (EventLevelOp,    EvtNetPPP, 29)
#define EvtNetPPP_DataLinkDown              EventID (EventLevelOp,    EvtNetPPP, 30)
#define EvtNetPPP_UninitInterface           EventID (EventLevelOp,    EvtNetPPP, 31)
/* PPP-LCP events */
#define EvtNetPPP_LcpInit                   EventID (EventLevelOp,    EvtNetPPP, 32)
#define EvtNetPPP_LcpSendConfigRequest      EventID (EventLevelOp,    EvtNetPPP, 33)
#define EvtNetPPP_LcpNoRetriesLeft          EventID (EventLevelError, EvtNetPPP, 34)
#define EvtNetPPP_LcpOptionCharMap          EventID (EventLevelDetail,EvtNetPPP, 35)
#define EvtNetPPP_LcpOptionPfc              EventID (EventLevelDetail,EvtNetPPP, 36)
#define EvtNetPPP_LcpOptionAcfc             EventID (EventLevelDetail,EvtNetPPP, 37)
#define EvtNetPPP_LcpOptionMagicNumber      EventID (EventLevelDetail,EvtNetPPP, 38)
#define EvtNetPPP_LcpOptionAuthPap          EventID (EventLevelDetail,EvtNetPPP, 39)
#define EvtNetPPP_LcpOptionAuthChapMd5      EventID (EventLevelDetail,EvtNetPPP, 40)
#define EvtNetPPP_LcpSendEchoRequest        EventID (EventLevelOp,    EvtNetPPP, 41)
#define EvtNetPPP_LcpSendTerminateRequest   EventID (EventLevelOp,    EvtNetPPP, 42)
#define EvtNetPPP_LcpSendFrame              EventID (EventLevelOp,    EvtNetPPP, 43)
#define EvtNetPPP_LcpReceiveFrame           EventID (EventLevelOp,    EvtNetPPP, 44)
#define EvtNetPPP_LcpDataLinkUp             EventID (EventLevelOp,    EvtNetPPP, 45)
#define EvtNetPPP_LcpOptionMru              EventID (EventLevelOp,    EvtNetPPP, 46)
#define EvtNetPPP_LcpOptionAuth             EventID (EventLevelOp,    EvtNetPPP, 47)
#define EvtNetPPP_LcpOptionAuthChapNotMd5   EventID (EventLevelOp,    EvtNetPPP, 48)
#define EvtNetPPP_LcpOptionUnknown          EventID (EventLevelOp,    EvtNetPPP, 49)
#define EvtNetPPP_LcpSendReject             EventID (EventLevelOp,    EvtNetPPP, 50)
#define EvtNetPPP_LcpSendNak                EventID (EventLevelOp,    EvtNetPPP, 51)
#define EvtNetPPP_LcpSendAck                EventID (EventLevelOp,    EvtNetPPP, 52)
#define EvtNetPPP_LcpWrongAckReceived       EventID (EventLevelOp,    EvtNetPPP, 53)
#define EvtNetPPP_LcpWrongNakReceived       EventID (EventLevelOp,    EvtNetPPP, 54)
#define EvtNetPPP_LcpConfigAuthFailed       EventID (EventLevelError, EvtNetPPP, 55)
#define EvtNetPPP_LcpWrongRejectReceived    EventID (EventLevelOp,    EvtNetPPP, 56)
#define EvtNetPPP_LcpEchoMagicNumber        EventID (EventLevelOp,    EvtNetPPP, 57)
#define EvtNetPPP_LcpWrongPeerMagicNumber   EventID (EventLevelOp,    EvtNetPPP, 58)
#define EvtNetPPP_LcpSendEchoReply          EventID (EventLevelOp,    EvtNetPPP, 59)
#define EvtNetPPP_LcpWrongEchoReplyReceived EventID (EventLevelOp,    EvtNetPPP, 60)
#define EvtNetPPP_LcpSendTerminateAck       EventID (EventLevelOp,    EvtNetPPP, 61)
#define EvtNetPPP_LcpUninit                 EventID (EventLevelOp,    EvtNetPPP, 62)
/* PPP-PAP events */
#define EvtNetPPP_PapInit                   EventID (EventLevelOp,    EvtNetPPP, 63)
#define EvtNetPPP_PapSendAuthRequest        EventID (EventLevelOp,    EvtNetPPP, 64)
#define EvtNetPPP_PapShowPassword           EventID (EventLevelOp,    EvtNetPPP, 65)
#define EvtNetPPP_PapSendFrame              EventID (EventLevelOp,    EvtNetPPP, 66)
#define EvtNetPPP_PapReceiveFrame           EventID (EventLevelOp,    EvtNetPPP, 67)
#define EvtNetPPP_PapLoginSuccess           EventID (EventLevelOp,    EvtNetPPP, 68)
#define EvtNetPPP_PapLoginFailed            EventID (EventLevelError, EvtNetPPP, 69)
#define EvtNetPPP_PapWrongAckReceived       EventID (EventLevelOp,    EvtNetPPP, 70)
#define EvtNetPPP_PapWrongNakReceived       EventID (EventLevelOp,    EvtNetPPP, 71)
#define EvtNetPPP_PapUninit                 EventID (EventLevelOp,    EvtNetPPP, 72)
/* PPP-CHAP events */
#define EvtNetPPP_ChapInit                  EventID (EventLevelOp,    EvtNetPPP, 73)
#define EvtNetPPP_ChapSendChallenge         EventID (EventLevelOp,    EvtNetPPP, 74)
#define EvtNetPPP_ChapSendFrame             EventID (EventLevelOp,    EvtNetPPP, 75)
#define EvtNetPPP_ChapReceiveFrame          EventID (EventLevelOp,    EvtNetPPP, 76)
#define EvtNetPPP_ChapWrongResponseReceived EventID (EventLevelOp,    EvtNetPPP, 77)
#define EvtNetPPP_ChapWrongSuccessReceived  EventID (EventLevelOp,    EvtNetPPP, 78)
#define EvtNetPPP_ChapWrongFailureReceived  EventID (EventLevelOp,    EvtNetPPP, 79)
#define EvtNetPPP_ChapLoginSuccess          EventID (EventLevelOp,    EvtNetPPP, 80)
#define EvtNetPPP_ChapLoginFailed           EventID (EventLevelError, EvtNetPPP, 81)
#define EvtNetPPP_ChapUninit                EventID (EventLevelOp,    EvtNetPPP, 82)
/* PPP-IPCP events */
#define EvtNetPPP_IpcpInit                  EventID (EventLevelOp,    EvtNetPPP, 83)
#define EvtNetPPP_IpcpIp4ConfigError        EventID (EventLevelError, EvtNetPPP, 84)
#define EvtNetPPP_IpcpSendConfigRequest     EventID (EventLevelOp,    EvtNetPPP, 85)
#define EvtNetPPP_IpcpOptionIpAddress       EventID (EventLevelDetail,EvtNetPPP, 86)
#define EvtNetPPP_IpcpOptionPrimaryDns      EventID (EventLevelDetail,EvtNetPPP, 87)
#define EvtNetPPP_IpcpOptionSecondaryDns    EventID (EventLevelDetail,EvtNetPPP, 88)
#define EvtNetPPP_IpcpSendFrame             EventID (EventLevelOp,    EvtNetPPP, 89)
#define EvtNetPPP_IpcpReceiveFrame          EventID (EventLevelOp,    EvtNetPPP, 90)
#define EvtNetPPP_IpcpOptionUnknown         EventID (EventLevelOp,    EvtNetPPP, 91)
#define EvtNetPPP_IpcpSendReject            EventID (EventLevelOp,    EvtNetPPP, 92)
#define EvtNetPPP_IpcpSendNak               EventID (EventLevelOp,    EvtNetPPP, 93)
#define EvtNetPPP_IpcpSendAck               EventID (EventLevelOp,    EvtNetPPP, 94)
#define EvtNetPPP_IpcpWrongAckReceived      EventID (EventLevelOp,    EvtNetPPP, 95)
#define EvtNetPPP_IpcpNetworkLayerUp        EventID (EventLevelOp,    EvtNetPPP, 96)
#define EvtNetPPP_IpcpWrongNakReceived      EventID (EventLevelOp,    EvtNetPPP, 97)
#define EvtNetPPP_IpcpWrongRejectReceived   EventID (EventLevelOp,    EvtNetPPP, 98)
#define EvtNetPPP_IpcpWrongSubnet           EventID (EventLevelOp,    EvtNetPPP, 99)
#define EvtNetPPP_IpcpPrimaryDnsRejected    EventID (EventLevelOp,    EvtNetPPP,100)
#define EvtNetPPP_IpcpSecondaryDnsRejected  EventID (EventLevelOp,    EvtNetPPP,101)
#define EvtNetPPP_IpcpIpAddressRejected     EventID (EventLevelError, EvtNetPPP,102)
#define EvtNetPPP_IpcpNotRequestedOption    EventID (EventLevelError, EvtNetPPP,103)
#define EvtNetPPP_IpcpUninit                EventID (EventLevelOp,    EvtNetPPP,104)
#endif

// PPP core event recorder functions -------------------------------------------

/**
  \brief  Event on PPP interface initialize (Op)
  \remark Point-to-Point Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_InitInterface(void) {
    EventRecord2 (EvtNetPPP_InitInterface, 0, 0);
  }
#else
  #define EvrNetPPP_InitInterface()
#endif

/**
  \brief  Event on PPP thread create failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ThreadCreateFailed(void) {
    EventRecord2 (EvtNetPPP_ThreadCreateFailed, 0, 0);
  }
#else
  #define EvrNetPPP_ThreadCreateFailed()
#endif

/**
  \brief  Event on PPP semaphore create failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SemaphoreCreateFailed(void) {
    EventRecord2 (EvtNetPPP_SemaphoreCreateFailed, 0, 0);
  }
#else
  #define EvrNetPPP_SemaphoreCreateFailed()
#endif

/**
  \brief  Event on PPP get_option invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_GetOptionInvalidParameter(void) {
    EventRecord2 (EvtNetPPP_GetOptionInvalidParameter, 0, 0);
  }
#else
  #define EvrNetPPP_GetOptionInvalidParameter()
#endif

/**
  \brief  Event on PPP set_option invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SetOptionInvalidParameter(void) {
    EventRecord2 (EvtNetPPP_SetOptionInvalidParameter, 0, 0);
  }
#else
  #define EvrNetPPP_SetOptionInvalidParameter()
#endif

/**
  \brief  Event on PPP set interface IPv4 address (Op)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SetIp4Address(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_SetIp4Address, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_SetIp4Address(ip4_addr)
#endif

/**
  \brief  Event on PPP set interface primary DNS server (Op)
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SetIp4PrimaryDNS(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_SetIp4PrimaryDNS, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_SetIp4PrimaryDNS(ip4_addr)
#endif

/**
  \brief  Event on PPP set interface secondary DNS server (Op)
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SetIp4SecondaryDNS(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_SetIp4SecondaryDNS, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_SetIp4SecondaryDNS(ip4_addr)
#endif

/**
  \brief  Event on PPP set interface MTU (Op)
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 576 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SetIp4Mtu(uint16_t mtu) {
    EventRecord2 (EvtNetPPP_SetIp4Mtu, mtu, 0);
  }
#else
  #define EvrNetPPP_SetIp4Mtu(mtu)
#endif

/**
  \brief  Event on PPP receive frame (Op)
  \param  protocol      PPP protocol type
                         - 0xC021: LCP
                         - 0xC023: PAP
                         - 0xC223: CHAP
                         - 0x8021: IPCP
                         - 0x0021: IP
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ReceiveFrame(uint16_t protocol, uint32_t length) {
    EventRecord2 (EvtNetPPP_ReceiveFrame, protocol, length);
  }
#else
  #define EvrNetPPP_ReceiveFrame(protocol, length)
#endif

/**
  \brief  Event on PPP receive frame too short (Op)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_FrameTooShort(uint32_t length, uint32_t min_length) {
    if (length > 1536) length = 0;
    EventRecord2 (EvtNetPPP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetPPP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on PPP frame checksum check failed (Error)
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChecksumFailed(uint32_t length) {
    EventRecord2 (EvtNetPPP_ChecksumFailed, length, 0);
  }
#else
  #define EvrNetPPP_ChecksumFailed(length)
#endif

/**
  \brief  Event on PPP frame control byte invalid (Error)
  \param  ctrl          received control byte
  \param  ctrl_valid    valid control byte
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_CtrlByteInvalid(uint8_t ctrl, uint8_t ctrl_valid) {
    EventRecord2 (EvtNetPPP_CtrlByteInvalid, ctrl, ctrl_valid);
  }
#else
  #define EvrNetPPP_CtrlByteInvalid(ctrl, ctrl_valid)
#endif

/**
  \brief  Event on PPP send frame (Op)
  \param  protocol      PPP protocol type
                         - 0xC021: LCP
                         - 0xC023: PAP
                         - 0xC223: CHAP
                         - 0x8021: IPCP
                         - 0x0021: IP
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SendFrame(uint16_t protocol, uint32_t length) {
    EventRecord2 (EvtNetPPP_SendFrame, protocol, length);
  }
#else
  #define EvrNetPPP_SendFrame(protocol, length)
#endif

/**
  \brief  Event on PPP invalid IP version (Error)
  \param  ip_version    invalid IP protocol version value
                        - 0: IPv4
                        - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_InvalidIpVersion(uint8_t ip_version) {
    EventRecord2 (EvtNetPPP_InvalidIpVersion, ip_version, 0);
  }
#else
  #define EvrNetPPP_InvalidIpVersion(ip_version)
#endif

/**
  \brief  Event on PPP network layer down (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_NetworkLayerDown(void) {
    EventRecord2 (EvtNetPPP_NetworkLayerDown, 0, 0);
  }
#else
  #define EvrNetPPP_NetworkLayerDown()
#endif

/**
  \brief  Event on PPP send payload too large (Error)
  \param  length        data length in bytes
  \param  max_length    maximum length of data in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_SendDataTooLarge(uint32_t length, uint32_t max_length) {
    EventRecord2 (EvtNetPPP_SendDataTooLarge, length, max_length);
  }
#else
  #define EvrNetPPP_SendDataTooLarge(length, max_length)
#endif

/**
  \brief  Event on PPP out of memory for send (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_NoMemoryError(void) {
    EventRecord2 (EvtNetPPP_NoMemoryError, 0, 0);
  }
#else
  #define EvrNetPPP_NoMemoryError()
#endif

/**
  \brief  Event on PPP add frame to transmit queue (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_QueueAddTransmit(void) {
    EventRecord2 (EvtNetPPP_QueueAddTransmit, 0, 0);
  }
#else
  #define EvrNetPPP_QueueAddTransmit()
#endif

/**
  \brief  Event on PPP reject unsupported protocol type (Op)
  \param  protocol      unsupported protocol type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_RejectProtocol(uint16_t protocol) {
    EventRecord2 (EvtNetPPP_RejectProtocol, protocol, 0);
  }
#else
  #define EvrNetPPP_RejectProtocol(protocol)
#endif

/**
  \brief  Event on PPP reject unsupported cp code (Op)
  \param  protocol      PPP protocol type
                         - 0xC021: LCP
                         - 0x8021: IPCP
  \param  code          unsupported control code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_RejectCode(uint16_t protocol, uint8_t code) {
    EventRecord2 (EvtNetPPP_RejectCode, protocol, code);
  }
#else
  #define EvrNetPPP_RejectCode(protocol, code)
#endif

/**
  \brief  Event on PPP start connecting (API)
  \param  dial_number   pointer to a dial number string
  \param  length        length of the dial number string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_Connect(const char *dial_number, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetPPP_Connect, dial_number, length);
  }
#else
  #define EvrNetPPP_Connect(dial_number, length)
#endif

/**
  \brief  Event on PPP connect invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ConnectInvalidParameter(void) {
    EventRecord2 (EvtNetPPP_ConnectInvalidParameter, 0, 0);
  }
#else
  #define EvrNetPPP_ConnectInvalidParameter()
#endif

/**
  \brief  Event on PPP connect in wrong state (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ConnectWrongState(void) {
    EventRecord2 (EvtNetPPP_ConnectWrongState, 0, 0);
  }
#else
  #define EvrNetPPP_ConnectWrongState()
#endif

/**
  \brief  Event on PPP start listening (API)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_Listen(void) {
    EventRecord2 (EvtNetPPP_Listen, 0, 0);
  }
#else
  #define EvrNetPPP_Listen()
#endif

/**
  \brief  Event on PPP listen invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ListenInvalidParameter(void) {
    EventRecord2 (EvtNetPPP_ListenInvalidParameter, 0, 0);
  }
#else
  #define EvrNetPPP_ListenInvalidParameter()
#endif

/**
  \brief  Event on PPP listen in wrong state (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ListenWrongState(void) {
    EventRecord2 (EvtNetPPP_ListenWrongState, 0, 0);
  }
#else
  #define EvrNetPPP_ListenWrongState()
#endif

/**
  \brief  Event on PPP close connection (API)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_Close(void) {
    EventRecord2 (EvtNetPPP_Close, 0, 0);
  }
#else
  #define EvrNetPPP_Close()
#endif

/**
  \brief  Event on PPP display username (Op)
  \param  username      pointer to a username string
  \param  length        length of the username string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ShowUsername(const char *username, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetPPP_ShowUsername, username, length);
  }
#else
  #define EvrNetPPP_ShowUsername(username, length)
#endif

/**
  \brief  Event on PPP display password (Op)
  \param  password      pointer to a password string
  \param  length        length of the password string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ShowPassword(const char *password, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetPPP_ShowPassword, password, length);
  }
#else
  #define EvrNetPPP_ShowPassword(password, length)
#endif

/**
  \brief  Event on PPP link change to modem offline (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ModemOffline(void) {
    EventRecord2 (EvtNetPPP_ModemOffline, 0, 0);
  }
#else
  #define EvrNetPPP_ModemOffline()
#endif

/**
  \brief  Event on PPP data-link layer down (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_DataLinkDown(void) {
    EventRecord2 (EvtNetPPP_DataLinkDown, 0, 0);
  }
#else
  #define EvrNetPPP_DataLinkDown()
#endif

/**
  \brief  Event on PPP interface de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_UninitInterface(void) {
    EventRecord2 (EvtNetPPP_UninitInterface, 0, 0);
  }
#else
  #define EvrNetPPP_UninitInterface()
#endif

// PPP-LCP event recorder functions --------------------------------------------

/**
  \brief  Event on PPP-LCP initialize (Op)
  \remark Point-to-Point Link Control Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpInit(void) {
    EventRecord2 (EvtNetPPP_LcpInit, 0, 0);
  }
#else
  #define EvrNetPPP_LcpInit()
#endif

/**
  \brief  Event on PPP-LCP send configuration request (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendConfigRequest(void) {
    EventRecord2 (EvtNetPPP_LcpSendConfigRequest, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendConfigRequest()
#endif

/**
  \brief  Event on PPP-LCP no retries left for LCP negotiation (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpNoRetriesLeft(void) {
    EventRecord2 (EvtNetPPP_LcpNoRetriesLeft, 0, 0);
  }
#else
  #define EvrNetPPP_LcpNoRetriesLeft()
#endif

/**
  \brief  Event on PPP-LCP option character map (Detail)
  \param  char_map      character bit-map for non printable characters
                         - bit 0: ascii character 0
                         - bit 1: ascii character 1
                         - bit 2: ascii character 2
                         -  ...
                         - bit 31: ascii character 31
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionCharMap(uint32_t char_map) {
    EventRecord2 (EvtNetPPP_LcpOptionCharMap, char_map, 0);
  }
#else
  #define EvrNetPPP_LcpOptionCharMap(char_map)
#endif

/**
  \brief  Event on PPP-LCP option PFC (Detail)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionPfc(void) {
    EventRecord2 (EvtNetPPP_LcpOptionPfc, 0, 0);
  }
#else
  #define EvrNetPPP_LcpOptionPfc()
#endif

/**
  \brief  Event on PPP-LCP option ACFC (Detail)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionAcfc(void) {
    EventRecord2 (EvtNetPPP_LcpOptionAcfc, 0, 0);
  }
#else
  #define EvrNetPPP_LcpOptionAcfc()
#endif

/**
  \brief  Event on PPP-LCP option magic number (Detail)
  \param  magic_number  magic random number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionMagicNumber(uint32_t magic_number) {
    EventRecord2 (EvtNetPPP_LcpOptionMagicNumber, magic_number, 0);
  }
#else
  #define EvrNetPPP_LcpOptionMagicNumber(magic_number)
#endif

/**
  \brief  Event on PPP-LCP option Authentication PAP (Detail)
  \remark Password Authentication Protocol - plain text
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionAuthPap(void) {
    EventRecord2 (EvtNetPPP_LcpOptionAuthPap, 0, 0);
  }
#else
  #define EvrNetPPP_LcpOptionAuthPap()
#endif

/**
  \brief  Event on PPP-LCP option Authentication CHAP (Detail)
  \remark Challenge Handshake Authentication Protocol with MD5
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionAuthChapMd5(void) {
    EventRecord2 (EvtNetPPP_LcpOptionAuthChapMd5, 0, 0);
  }
#else
  #define EvrNetPPP_LcpOptionAuthChapMd5()
#endif

/**
  \brief  Event on PPP-LCP send echo request (Op)
  \param  magic_number  magic random number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendEchoRequest(uint32_t magic_number) {
    EventRecord2 (EvtNetPPP_LcpSendEchoRequest, magic_number, 0);
  }
#else
  #define EvrNetPPP_LcpSendEchoRequest(magic_number)
#endif

/**
  \brief  Event on PPP-LCP send LCP terminate request (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendTerminateRequest(void) {
    EventRecord2 (EvtNetPPP_LcpSendTerminateRequest, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendTerminateRequest()
#endif

/**
  \brief  Event on PPP-LCP send frame (Op)
  \param  lcp_header    pointer to LCP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendFrame(const void *lcp_header) {
    EventRecordData (EvtNetPPP_LcpSendFrame, lcp_header, 4);
  }
#else
  #define EvrNetPPP_LcpSendFrame(lcp_header)
#endif

/**
  \brief  Event on PPP-LCP receive frame (Op)
  \param  lcp_header    pointer to LCP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpReceiveFrame(const void *lcp_header) {
    EventRecordData (EvtNetPPP_LcpReceiveFrame, lcp_header, 4);
  }
#else
  #define EvrNetPPP_LcpReceiveFrame(lcp_header)
#endif

/**
  \brief  Event on PPP-LCP data-link layer established (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpDataLinkUp(void) {
    EventRecord2 (EvtNetPPP_LcpDataLinkUp, 0, 0);
  }
#else
  #define EvrNetPPP_LcpDataLinkUp()
#endif

/**
  \brief  Event on PPP-LCP option MRU (Op)
  \param  mru_size      maximum receive unit size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionMru(uint16_t mru_size) {
    EventRecord2 (EvtNetPPP_LcpOptionMru, mru_size, 0);
  }
#else
  #define EvrNetPPP_LcpOptionMru(mru_size)
#endif

/**
  \brief  Event on PPP-LCP option authentication type (Op)
  \param  auth_type     authentication type code
                         - 0xC023: PAP
                         - 0xC223: CHAP
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionAuth(uint16_t auth_type) {
    EventRecord2 (EvtNetPPP_LcpOptionAuth, auth_type, 0);
  }
#else
  #define EvrNetPPP_LcpOptionAuth(auth_type)
#endif

/**
  \brief  Event on PPP-LCP option CHAP authentication not MD5 type (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionAuthChapNotMd5(void) {
    EventRecord2 (EvtNetPPP_LcpOptionAuthChapNotMd5, 0, 0);
  }
#else
  #define EvrNetPPP_LcpOptionAuthChapNotMd5()
#endif

/**
  \brief  Event on PPP-LCP unknown option received (Op)
  \param  lcp_option    unknown LCP option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpOptionUnknown(uint8_t lcp_option) {
    EventRecord2 (EvtNetPPP_LcpOptionUnknown, lcp_option, 0);
  }
#else
  #define EvrNetPPP_LcpOptionUnknown(lcp_option)
#endif

/**
  \brief  Event on PPP-LCP send reject (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendReject(void) {
    EventRecord2 (EvtNetPPP_LcpSendReject, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendReject()
#endif

/**
  \brief  Event on PPP-LCP send not acknowledge (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendNak(void) {
    EventRecord2 (EvtNetPPP_LcpSendNak, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendNak()
#endif

/**
  \brief  Event on PPP-LCP send acknowledge (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendAck(void) {
    EventRecord2 (EvtNetPPP_LcpSendAck, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendAck()
#endif

/**
  \brief  Event on PPP-LCP wrong acknowledge received (Op)
  \param  id            received LCP identifier
  \param  id_valid      valid LCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpWrongAckReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_LcpWrongAckReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_LcpWrongAckReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-LCP wrong not-acknowledge received (Op)
  \param  id            received LCP identifier
  \param  id_valid      valid LCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpWrongNakReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_LcpWrongNakReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_LcpWrongNakReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-LCP authentication type negotiation failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpConfigAuthFailed(void) {
    EventRecord2 (EvtNetPPP_LcpConfigAuthFailed, 0, 0);
  }
#else
  #define EvrNetPPP_LcpConfigAuthFailed()
#endif

/**
  \brief  Event on PPP-LCP wrong reject received (Op)
  \param  id            received LCP identifier
  \param  id_valid      valid LCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpWrongRejectReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_LcpWrongRejectReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_LcpWrongRejectReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-LCP echo magic number (Op)
  \param  magic_number  peer echo magic number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpEchoMagicNumber(uint32_t magic_number) {
    EventRecord2 (EvtNetPPP_LcpEchoMagicNumber, magic_number, 0);
  }
#else
  #define EvrNetPPP_LcpEchoMagicNumber(magic_number)
#endif

/**
  \brief  Event on PPP-LCP wrong peer magic number (Op)
  \param  magic         received peer magic number
  \param  magic_valid   valid peer magic number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpWrongPeerMagicNumber(uint32_t magic, uint32_t magic_valid) {
    EventRecord2 (EvtNetPPP_LcpWrongPeerMagicNumber, magic, magic_valid);
  }
#else
  #define EvrNetPPP_LcpWrongPeerMagicNumber(magic, magic_valid)
#endif

/**
  \brief  Event on PPP-LCP send echo reply (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendEchoReply(void) {
    EventRecord2 (EvtNetPPP_LcpSendEchoReply, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendEchoReply()
#endif

/**
  \brief  Event on PPP-LCP wrong echo reply received (Op)
  \param  id            received LCP identifier
  \param  id_valid      valid LCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpWrongEchoReplyReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_LcpWrongEchoReplyReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_LcpWrongEchoReplyReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-LCP send terminate acknowledge (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpSendTerminateAck(void) {
    EventRecord2 (EvtNetPPP_LcpSendTerminateAck, 0, 0);
  }
#else
  #define EvrNetPPP_LcpSendTerminateAck()
#endif

/**
  \brief  Event on PPP-LCP de-initialize (Op)
  \remark Point-to-Point Link Control Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_LcpUninit(void) {
    EventRecord2 (EvtNetPPP_LcpUninit, 0, 0);
  }
#else
  #define EvrNetPPP_LcpUninit()
#endif

// PPP-PAP event recorder functions --------------------------------------------

/**
  \brief  Event on PPP-PAP initialize (Op)
  \remark Password Authentication Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapInit(void) {
    EventRecord2 (EvtNetPPP_PapInit, 0, 0);
  }
#else
  #define EvrNetPPP_PapInit()
#endif

/**
  \brief  Event on PPP-PAP send authentication request (Op)
  \param  peer_id       peer identifier string (username)
  \param  length        length of the peer_id string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapSendAuthRequest(const char *peer_id, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetPPP_PapSendAuthRequest, peer_id, length);
  }
#else
  #define EvrNetPPP_PapSendAuthRequest(peer_id, length)
#endif

/**
  \brief  Event on PPP-PAP display password (Op)
  \param  password      authentication password
  \param  length        length of authentication password
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapShowPassword(const char *password, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetPPP_PapShowPassword, password, length);
  }
#else
  #define EvrNetPPP_PapShowPassword(password, length)
#endif

/**
  \brief  Event on PPP-PAP send frame (Op)
  \param  pap_header    pointer to PAP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapSendFrame(const void *pap_header) {
    EventRecordData (EvtNetPPP_PapSendFrame, pap_header, 4);
  }
#else
  #define EvrNetPPP_PapSendFrame(pap_header)
#endif

/**
  \brief  Event on PPP-PAP receive frame (Op)
  \param  pap_header    pointer to PAP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapReceiveFrame(const void *pap_header) {
    EventRecordData (EvtNetPPP_PapReceiveFrame, pap_header, 4);
  }
#else
  #define EvrNetPPP_PapReceiveFrame(pap_header)
#endif

/**
  \brief  Event on PPP-PAP login success (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapLoginSuccess(void) {
    EventRecord2 (EvtNetPPP_PapLoginSuccess, 0, 0);
  }
#else
  #define EvrNetPPP_PapLoginSuccess()
#endif

/**
  \brief  Event on PPP-PAP login failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapLoginFailed(void) {
    EventRecord2 (EvtNetPPP_PapLoginFailed, 0, 0);
  }
#else
  #define EvrNetPPP_PapLoginFailed()
#endif

/**
  \brief  Event on PPP-PAP wrong acknowledge received (Op)
  \param  id            received PAP identifier
  \param  id_valid      valid PAP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapWrongAckReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_PapWrongAckReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_PapWrongAckReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-PAP wrong not-acknowledge received (Op)
  \param  id            received PAP identifier
  \param  id_valid      valid PAP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapWrongNakReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_PapWrongNakReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_PapWrongNakReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-PAP de-initialize (Op)
  \remark Password Authentication Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_PapUninit(void) {
    EventRecord2 (EvtNetPPP_PapUninit, 0, 0);
  }
#else
  #define EvrNetPPP_PapUninit()
#endif

// PPP-CHAP event recorder functions -------------------------------------------

/**
  \brief  Event on PPP-CHAP initialize (Op)
  \remark Challenge-Handshake Authentication Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapInit(void) {
    EventRecord2 (EvtNetPPP_ChapInit, 0, 0);
  }
#else
  #define EvrNetPPP_ChapInit()
#endif

/**
  \brief  Event on PPP-CHAP send challenge (Op)
  \param  name          system identification string
  \param  length        length of the system identification string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapSendChallenge(const char *name, uint32_t length) {
    EventRecordData (EvtNetPPP_ChapSendChallenge, name, length);
  }
#else
  #define EvrNetPPP_ChapSendChallenge(name, length)
#endif

/**
  \brief  Event on PPP-CHAP send frame (Op)
  \param  chap_header   pointer to CHAP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapSendFrame(const void *chap_header) {
    EventRecordData (EvtNetPPP_ChapSendFrame, chap_header, 4);
  }
#else
  #define EvrNetPPP_ChapSendFrame(chap_header)
#endif

/**
  \brief  Event on PPP-CHAP receive frame (Op)
  \param  chap_header   pointer to CHAP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapReceiveFrame(const void *chap_header) {
    EventRecordData (EvtNetPPP_ChapReceiveFrame, chap_header, 4);
  }
#else
  #define EvrNetPPP_ChapReceiveFrame(chap_header)
#endif

/**
  \brief  Event on PPP-CHAP wrong response received (Op)
  \param  id            received CHAP identifier
  \param  id_valid      valid CHAP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapWrongResponseReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_ChapWrongResponseReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_ChapWrongResponseReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-CHAP wrong success received (Op)
  \param  id            received CHAP identifier
  \param  id_valid      valid CHAP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapWrongSuccessReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_ChapWrongSuccessReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_ChapWrongSuccessReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-CHAP wrong failure received (Op)
  \param  id            received CHAP identifier
  \param  id_valid      valid CHAP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapWrongFailureReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_ChapWrongFailureReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_ChapWrongFailureReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-CHAP login success (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapLoginSuccess(void) {
    EventRecord2 (EvtNetPPP_ChapLoginSuccess, 0, 0);
  }
#else
  #define EvrNetPPP_ChapLoginSuccess()
#endif

/**
  \brief  Event on PPP-CHAP login failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapLoginFailed(void) {
    EventRecord2 (EvtNetPPP_ChapLoginFailed, 0, 0);
  }
#else
  #define EvrNetPPP_ChapLoginFailed()
#endif

/**
  \brief  Event on PPP-CHAP de-initialize (Op)
  \remark Challenge-Handshake Authentication Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_ChapUninit(void) {
    EventRecord2 (EvtNetPPP_ChapUninit, 0, 0);
  }
#else
  #define EvrNetPPP_ChapUninit()
#endif

// PPP-IPCP event recorder functions -------------------------------------------

/**
  \brief  Event on PPP-IPCP initialize (Op)
  \remark PPP Internet Protocol Control Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpInit(void) {
    EventRecord2 (EvtNetPPP_IpcpInit, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpInit()
#endif

/**
  \brief  Event on PPP-IPCP interface IPv4 configuration error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpIp4ConfigError(void) {
    EventRecord2 (EvtNetPPP_IpcpIp4ConfigError, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpIp4ConfigError()
#endif

/**
  \brief  Event on PPP-IPCP send configuration request (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSendConfigRequest(void) {
    EventRecord2 (EvtNetPPP_IpcpSendConfigRequest, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpSendConfigRequest()
#endif

/**
  \brief  Event on PPP-IPCP option IP-address (Detail)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpOptionIpAddress(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_IpcpOptionIpAddress, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_IpcpOptionIpAddress(ip4_addr)
#endif

/**
  \brief  Event on PPP-IPCP option primary DNS server address (Detail)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpOptionPrimaryDns(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_IpcpOptionPrimaryDns, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_IpcpOptionPrimaryDns(ip4_addr)
#endif

/**
  \brief  Event on PPP-IPCP option secondary DNS server address (Detail)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpOptionSecondaryDns(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_IpcpOptionSecondaryDns, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_IpcpOptionSecondaryDns(ip4_addr)
#endif

/**
  \brief  Event on PPP-IPCP send frame (Op)
  \param  ipcp_header   pointer to IPCP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSendFrame(const void *ipcp_header) {
    EventRecordData (EvtNetPPP_IpcpSendFrame, ipcp_header, 4);
  }
#else
  #define EvrNetPPP_IpcpSendFrame(ipcp_header)
#endif

/**
  \brief  Event on PPP-IPCP receive frame (Op)
  \param  ipcp_header   pointer to IPCP frame header of 4 bytes
                         - Code (1 byte)
                         - Id   (1 byte)
                         - Len  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpReceiveFrame(const void *ipcp_header) {
    EventRecordData (EvtNetPPP_IpcpReceiveFrame, ipcp_header, 4);
  }
#else
  #define EvrNetPPP_IpcpReceiveFrame(ipcp_header)
#endif

/**
  \brief  Event on PPP-IPCP unknown option received (Op)
  \param  ipcp_option   unknown IPCP option
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpOptionUnknown(uint8_t ipcp_option) {
    EventRecord2 (EvtNetPPP_IpcpOptionUnknown, ipcp_option, 0);
  }
#else
  #define EvrNetPPP_IpcpOptionUnknown(ipcp_option)
#endif

/**
  \brief  Event on PPP-IPCP send reject (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSendReject(void) {
    EventRecord2 (EvtNetPPP_IpcpSendReject, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpSendReject()
#endif

/**
  \brief  Event on PPP-IPCP send not acknowledge (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSendNak(void) {
    EventRecord2 (EvtNetPPP_IpcpSendNak, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpSendNak()
#endif

/**
  \brief  Event on PPP-IPCP send acknowledge (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSendAck(void) {
    EventRecord2 (EvtNetPPP_IpcpSendAck, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpSendAck()
#endif

/**
  \brief  Event on PPP-IPCP wrong acknowledge received (Op)
  \param  id            received IPCP identifier
  \param  id_valid      valid IPCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpWrongAckReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_IpcpWrongAckReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_IpcpWrongAckReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-IPCP network-layer established (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpNetworkLayerUp(void) {
    EventRecord2 (EvtNetPPP_IpcpNetworkLayerUp, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpNetworkLayerUp()
#endif

/**
  \brief  Event on PPP-IPCP wrong not-acknowledge received (Op)
  \param  id            received IPCP identifier
  \param  id_valid      valid IPCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpWrongNakReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_IpcpWrongNakReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_IpcpWrongNakReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-IPCP wrong reject received (Op)
  \param  id            received IPCP identifier
  \param  id_valid      valid IPCP identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpWrongRejectReceived(uint8_t id, uint8_t id_valid) {
    EventRecord2 (EvtNetPPP_IpcpWrongRejectReceived, id, id_valid);
  }
#else
  #define EvrNetPPP_IpcpWrongRejectReceived(id, id_valid)
#endif

/**
  \brief  Event on PPP-IPCP advertised IP-address not in subnet (Op)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpWrongSubnet(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetPPP_IpcpWrongSubnet, ip4_addr, 4);
  }
#else
  #define EvrNetPPP_IpcpWrongSubnet(ip4_addr)
#endif

/**
  \brief  Event on PPP-IPCP primary DNS server option rejected (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpPrimaryDnsRejected(void) {
    EventRecord2 (EvtNetPPP_IpcpPrimaryDnsRejected, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpPrimaryDnsRejected()
#endif

/**
  \brief  Event on PPP-IPCP secondary DNS server option rejected (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpSecondaryDnsRejected(void) {
    EventRecord2 (EvtNetPPP_IpcpSecondaryDnsRejected, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpSecondaryDnsRejected()
#endif

/**
  \brief  Event on PPP-IPCP IP-address option rejected (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpIpAddressRejected(void) {
    EventRecord2 (EvtNetPPP_IpcpIpAddressRejected, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpIpAddressRejected()
#endif

/**
  \brief  Event on PPP-IPCP not requested option rejected (Error)
  \param  ipcp_option   rejected option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpNotRequestedOption(uint8_t ipcp_option) {
    EventRecord2 (EvtNetPPP_IpcpNotRequestedOption, ipcp_option, 0);
  }
#else
  #define EvrNetPPP_IpcpNotRequestedOption(ipcp_option)
#endif

/**
  \brief  Event on PPP-IPCP de-initialize (Op)
  \remark PPP Internet Protocol Control Protocol
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetPPP_IpcpUninit(void) {
    EventRecord2 (EvtNetPPP_IpcpUninit, 0, 0);
  }
#else
  #define EvrNetPPP_IpcpUninit()
#endif


// NetSLIP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetSLIP_InitInterface            EventID (EventLevelOp,    EvtNetSLIP,  0)
#define EvtNetSLIP_Ip4ConfigError           EventID (EventLevelError, EvtNetSLIP,  1)
#define EvtNetSLIP_ThreadCreateFailed       EventID (EventLevelError, EvtNetSLIP,  2)
#define EvtNetSLIP_SemaphoreCreateFailed    EventID (EventLevelError, EvtNetSLIP, 23) // End
#define EvtNetSLIP_GetOptionInvalidParameter EventID (EventLevelError,EvtNetSLIP,  3)
#define EvtNetSLIP_SetOptionInvalidParameter EventID (EventLevelError,EvtNetSLIP,  4)
#define EvtNetSLIP_SetIp4Address            EventID (EventLevelOp,    EvtNetSLIP,  5)
#define EvtNetSLIP_SetIp4PrimaryDNS         EventID (EventLevelOp,    EvtNetSLIP,  6)
#define EvtNetSLIP_SetIp4SecondaryDNS       EventID (EventLevelOp,    EvtNetSLIP,  7)
#define EvtNetSLIP_SetIp4Mtu                EventID (EventLevelOp,    EvtNetSLIP,  8)
#define EvtNetSLIP_Connect                  EventID (EventLevelAPI,   EvtNetSLIP,  9)
#define EvtNetSLIP_ConnectInvalidParameter  EventID (EventLevelError, EvtNetSLIP, 10)
#define EvtNetSLIP_ConnectWrongState        EventID (EventLevelError, EvtNetSLIP, 11)
#define EvtNetSLIP_Listen                   EventID (EventLevelAPI,   EvtNetSLIP, 12)
#define EvtNetSLIP_ListenWrongState         EventID (EventLevelError, EvtNetSLIP, 13)
#define EvtNetSLIP_Close                    EventID (EventLevelAPI,   EvtNetSLIP, 14)
#define EvtNetSLIP_SendFrame                EventID (EventLevelOp,    EvtNetSLIP, 15)
#define EvtNetSLIP_InvalidIpVersion         EventID (EventLevelError, EvtNetSLIP, 16)
#define EvtNetSLIP_SendDataTooLarge         EventID (EventLevelError, EvtNetSLIP, 17)
#define EvtNetSLIP_NoMemoryError            EventID (EventLevelError, EvtNetSLIP, 18)
#define EvtNetSLIP_QueueAddTransmit         EventID (EventLevelOp,    EvtNetSLIP, 19)
#define EvtNetSLIP_ModemOffline             EventID (EventLevelOp,    EvtNetSLIP, 20)
#define EvtNetSLIP_ReceiveFrame             EventID (EventLevelOp,    EvtNetSLIP, 21)
#define EvtNetSLIP_UninitInterface          EventID (EventLevelOp,    EvtNetSLIP, 22)
#endif

/**
  \brief  Event on SLIP interface initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_InitInterface(void) {
    EventRecord2 (EvtNetSLIP_InitInterface, 0, 0);
  }
#else
  #define EvrNetSLIP_InitInterface()
#endif

/**
  \brief  Event on SLIP IPv4 configuration error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_Ip4ConfigError(void) {
    EventRecord2 (EvtNetSLIP_Ip4ConfigError, 0, 0);
  }
#else
  #define EvrNetSLIP_Ip4ConfigError()
#endif

/**
  \brief  Event on SLIP thread create failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ThreadCreateFailed(void) {
    EventRecord2 (EvtNetSLIP_ThreadCreateFailed, 0, 0);
  }
#else
  #define EvrNetSLIP_ThreadCreateFailed()
#endif

/**
  \brief  Event on SLIP semaphore create failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SemaphoreCreateFailed(void) {
    EventRecord2 (EvtNetSLIP_SemaphoreCreateFailed, 0, 0);
  }
#else
  #define EvrNetSLIP_SemaphoreCreateFailed()
#endif

/**
  \brief  Event on SLIP get_option invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_GetOptionInvalidParameter(void) {
    EventRecord2 (EvtNetSLIP_GetOptionInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSLIP_GetOptionInvalidParameter()
#endif

/**
  \brief  Event on SLIP set_option invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SetOptionInvalidParameter(void) {
    EventRecord2 (EvtNetSLIP_SetOptionInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSLIP_SetOptionInvalidParameter()
#endif

/**
  \brief  Event on SLIP set interface IPv4 address (Op)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SetIp4Address(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSLIP_SetIp4Address, ip4_addr, 4);
  }
#else
  #define EvrNetSLIP_SetIp4Address(ip4_addr)
#endif

/**
  \brief  Event on SLIP set interface primary DNS server (Op)
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SetIp4PrimaryDNS(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSLIP_SetIp4PrimaryDNS, ip4_addr, 4);
  }
#else
  #define EvrNetSLIP_SetIp4PrimaryDNS(ip4_addr)
#endif

/**
  \brief  Event on SLIP set interface secondary DNS server (Op)
  \param  ip4_addr      pointer to IPv4 address of a DNS server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SetIp4SecondaryDNS(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSLIP_SetIp4SecondaryDNS, ip4_addr, 4);
  }
#else
  #define EvrNetSLIP_SetIp4SecondaryDNS(ip4_addr)
#endif

/**
  \brief  Event on SLIP set interface MTU (Op)
  \param  mtu           maximum transmission unit
                         - 0: fragmentation disabled
  \remark MTU is limited in the range from 296 to 1500 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SetIp4Mtu(uint16_t mtu) {
    EventRecord2 (EvtNetSLIP_SetIp4Mtu, mtu, 0);
  }
#else
  #define EvrNetSLIP_SetIp4Mtu(mtu)
#endif

/**
  \brief  Event on SLIP start connecting (API)
  \param  dial_number   pointer to a number to dial string
  \param  length        length of the dial_number string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_Connect(const char *dial_number, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetSLIP_Connect, dial_number, length);
  }
#else
  #define EvrNetSLIP_Connect(dial_number, length)
#endif

/**
  \brief  Event on SLIP connect invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ConnectInvalidParameter(void) {
    EventRecord2 (EvtNetSLIP_ConnectInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSLIP_ConnectInvalidParameter()
#endif

/**
  \brief  Event on SLIP connect in wrong state (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ConnectWrongState(void) {
    EventRecord2 (EvtNetSLIP_ConnectWrongState, 0, 0);
  }
#else
  #define EvrNetSLIP_ConnectWrongState()
#endif

/**
  \brief  Event on SLIP start listening (API)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_Listen(void) {
    EventRecord2 (EvtNetSLIP_Listen, 0, 0);
  }
#else
  #define EvrNetSLIP_Listen()
#endif

/**
  \brief  Event on SLIP listen in wrong state (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ListenWrongState(void) {
    EventRecord2 (EvtNetSLIP_ListenWrongState, 0, 0);
  }
#else
  #define EvrNetSLIP_ListenWrongState()
#endif

/**
  \brief  Event on SLIP close connection (API)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_Close(void) {
    EventRecord2 (EvtNetSLIP_Close, 0, 0);
  }
#else
  #define EvrNetSLIP_Close()
#endif

/**
  \brief  Event on SLIP send frame (Op)
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SendFrame(uint32_t length, uint8_t ip_version) {
    EventRecord2 (EvtNetSLIP_SendFrame, length, ip_version);
  }
#else
  #define EvrNetSLIP_SendFrame(length, ip_version)
#endif

/**
  \brief  Event on SLIP invalid IP version (Error)
  \param  ip_version    invalid IP protocol version value
                        - 0: IPv4
                        - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_InvalidIpVersion(uint8_t ip_version) {
    EventRecord2 (EvtNetSLIP_InvalidIpVersion, ip_version, 0);
  }
#else
  #define EvrNetSLIP_InvalidIpVersion(ip_version)
#endif

/**
  \brief  Event on SLIP send payload too large (Error)
  \param  length        data length in bytes
  \param  max_length    maximum length of data in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_SendDataTooLarge(uint32_t length, uint32_t max_length) {
    EventRecord2 (EvtNetSLIP_SendDataTooLarge, length, max_length);
  }
#else
  #define EvrNetSLIP_SendDataTooLarge(length, max_length)
#endif

/**
  \brief  Event on SLIP out of memory for send (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_NoMemoryError(void) {
    EventRecord2 (EvtNetSLIP_NoMemoryError, 0, 0);
  }
#else
  #define EvrNetSLIP_NoMemoryError()
#endif

/**
  \brief  Event on SLIP add frame to transmit queue (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_QueueAddTransmit(void) {
    EventRecord2 (EvtNetSLIP_QueueAddTransmit, 0, 0);
  }
#else
  #define EvrNetSLIP_QueueAddTransmit()
#endif

/**
  \brief  Event on SLIP link change to modem offline (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ModemOffline(void) {
    EventRecord2 (EvtNetSLIP_ModemOffline, 0, 0);
  }
#else
  #define EvrNetSLIP_ModemOffline()
#endif

/**
  \brief  Event on SLIP receive frame (Op)
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_ReceiveFrame(uint32_t length) {
    EventRecord2 (EvtNetSLIP_ReceiveFrame, length, 0);
  }
#else
  #define EvrNetSLIP_ReceiveFrame(length)
#endif

/**
  \brief  Event on SLIP interface de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSLIP_UninitInterface(void) {
    EventRecord2 (EvtNetSLIP_UninitInterface, 0, 0);
  }
#else
  #define EvrNetSLIP_UninitInterface()
#endif


// NetLOOP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetLOOP_InitInterface            EventID (EventLevelOp,    EvtNetLOOP, 0)
#define EvtNetLOOP_SendFrame                EventID (EventLevelOp,    EvtNetLOOP, 1)
#define EvtNetLOOP_NoMemoryError            EventID (EventLevelError, EvtNetLOOP, 2)
#define EvtNetLOOP_ReceiveFrame             EventID (EventLevelOp,    EvtNetLOOP, 3)
#define EvtNetLOOP_UninitInterface          EventID (EventLevelOp,    EvtNetLOOP, 4)
#endif

/**
  \brief  Event on Loopback interface initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetLOOP_InitInterface(void) {
    EventRecord2 (EvtNetLOOP_InitInterface, 0, 0);
  }
#else
  #define EvrNetLOOP_InitInterface()
#endif

/**
  \brief  Event on Loopback send frame (Op)
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetLOOP_SendFrame(uint32_t length, uint8_t ip_version) {
    EventRecord2 (EvtNetLOOP_SendFrame, length, ip_version);
  }
#else
  #define EvrNetLOOP_SendFrame(length, ip_version)
#endif

/**
  \brief  Event on Loopback out of memory for send (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetLOOP_NoMemoryError(void) {
    EventRecord2 (EvtNetLOOP_NoMemoryError, 0, 0);
  }
#else
  #define EvrNetLOOP_NoMemoryError()
#endif

/**
  \brief  Event on Loopback receive frame (Op)
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetLOOP_ReceiveFrame(uint32_t length, uint8_t ip_version) {
    EventRecord2 (EvtNetLOOP_ReceiveFrame, length, ip_version);
  }
#else
  #define EvrNetLOOP_ReceiveFrame(length, ip_version)
#endif

/**
  \brief  Event on Loopback interface de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetLOOP_UninitInterface(void) {
    EventRecord2 (EvtNetLOOP_UninitInterface, 0, 0);
  }
#else
  #define EvrNetLOOP_UninitInterface()
#endif


// NetIP4 event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetIP4_InitCore                  EventID (EventLevelOp,    EvtNetIP4,  0)
#define EvtNetIP4_ReceiveFrame              EventID (EventLevelOp,    EvtNetIP4,  1)
#define EvtNetIP4_FrameTooShort             EventID (EventLevelError, EvtNetIP4,  2)
#define EvtNetIP4_InvalidIpVersion          EventID (EventLevelError, EvtNetIP4,  3)
#define EvtNetIP4_ShowFrameHeader           EventID (EventLevelDetail,EvtNetIP4,  4)
#define EvtNetIP4_SourceIpAddressInvalid    EventID (EventLevelError, EvtNetIP4,  5)
#define EvtNetIP4_WrongMulticastProtocol    EventID (EventLevelError, EvtNetIP4,  6)
#define EvtNetIP4_WrongBroadcastProtocol    EventID (EventLevelError, EvtNetIP4,  7)
#define EvtNetIP4_WrongDestinationAddress   EventID (EventLevelOp,    EvtNetIP4,  8)
#define EvtNetIP4_ChecksumFailed            EventID (EventLevelError, EvtNetIP4,  9)
#define EvtNetIP4_FragmentDfFlagSet         EventID (EventLevelError, EvtNetIP4, 10)
#define EvtNetIP4_FragmentationDisabled     EventID (EventLevelError, EvtNetIP4, 11)
#define EvtNetIP4_SendFrame                 EventID (EventLevelOp,    EvtNetIP4, 12)
#define EvtNetIP4_ProtocolUnknown           EventID (EventLevelError, EvtNetIP4, 13)
#define EvtNetIP4_DestinationAddressNull    EventID (EventLevelError, EvtNetIP4, 14)
#define EvtNetIP4_SetDefaultInterface       EventID (EventLevelOp,    EvtNetIP4, 15)
#define EvtNetIP4_UninitCore                EventID (EventLevelOp,    EvtNetIP4, 16)
#endif

/**
  \brief  Event on IP4 core initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_InitCore(uint16_t if_id) {
    EventRecord2 (EvtNetIP4_InitCore, if_id, 0);
  }
#else
  #define EvrNetIP4_InitCore(if_id)
#endif

/**
  \brief  Event on IP4 receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetIP4_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetIP4_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on IP4 receive frame too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetIP4_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetIP4_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on IP4 invalid protocol version (Error)
  \param  if_id         network interface identifier
  \param  ip_version    IP protocol version value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_InvalidIpVersion(uint16_t if_id, uint8_t ip_version) {
    EventRecord2 (EvtNetIP4_InvalidIpVersion, if_id, ip_version);
  }
#else
  #define EvrNetIP4_InvalidIpVersion(if_id, ip_version)
#endif

/**
  \brief  Event on IP4 display frame header (Detail)
  \param  ip4_header    pointer to IPv4 frame header of 20 bytes
                         - VerHlen  (1 byte)
                         - Tos      (1 byte)
                         - Len      (2 bytes)
                         - Id       (2 bytes)
                         - FragOffs (2 bytes)
                         - Ttl      (1 byte)
                         - Proto    (1 byte)
                         - Chksum   (2 bytes)
                         - SrcAddr  (4 bytes)
                         - DstAddr  (4 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_ShowFrameHeader(const void *ip4_header) {
    EventRecordData (EvtNetIP4_ShowFrameHeader, ip4_header, 20);
  }
#else
  #define EvrNetIP4_ShowFrameHeader(ip4_header)
#endif

/**
  \brief  Event on IP4 source IP address invalid (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_SourceIpAddressInvalid(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIP4_SourceIpAddressInvalid, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIP4_SourceIpAddressInvalid(if_id, ip4_addr)
#endif

/**
  \brief  Event on IP4 wrong multicast protocol type (Error)
  \param  if_id         network interface identifier
  \param  ip4_protocol  wrong protocol type value
  \remark Only UDP and IGMP multicast frames are allowed.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_WrongMulticastProtocol(uint16_t if_id, uint8_t ip4_protocol) {
    EventRecord2 (EvtNetIP4_WrongMulticastProtocol, if_id, ip4_protocol);
  }
#else
  #define EvrNetIP4_WrongMulticastProtocol(if_id, ip4_protocol)
#endif

/**
  \brief  Event on IP4 wrong broadcast protocol type (Error)
  \param  if_id         network interface identifier
  \param  ip4_protocol  wrong protocol type value
  \remark Only UDP subnet broadcast frames are allowed.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_WrongBroadcastProtocol(uint16_t if_id, uint8_t ip4_protocol) {
    EventRecord2 (EvtNetIP4_WrongBroadcastProtocol, if_id, ip4_protocol);
  }
#else
  #define EvrNetIP4_WrongBroadcastProtocol(if_id, ip4_protocol)
#endif

/**
  \brief  Event on IP4 wrong destination address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_WrongDestinationAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIP4_WrongDestinationAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIP4_WrongDestinationAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on IP4 header checksum check failed (Error)
  \param  if_id         network interface identifier
  \param  length        header length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_ChecksumFailed(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetIP4_ChecksumFailed, if_id, length);
  }
#else
  #define EvrNetIP4_ChecksumFailed(if_id, length)
#endif

/**
  \brief  Event on IP4 receive fragmented frame with DF-flag set (Error)
  \param  if_id         network interface identifier
  \param  frag          IP flags and fragment offset
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_FragmentDfFlagSet(uint16_t if_id, uint16_t frag) {
    EventRecord2 (EvtNetIP4_FragmentDfFlagSet, if_id, frag);
  }
#else
  #define EvrNetIP4_FragmentDfFlagSet(if_id, frag)
#endif

/**
  \brief  Event on IP4 receive fragmented frame (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_FragmentationDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetIP4_FragmentationDisabled, if_id, 0);
  }
#else
  #define EvrNetIP4_FragmentationDisabled(if_id)
#endif

/**
  \brief  Event on IP4 send frame (Op)
  \param  if_id         network interface identifier
  \param  ip4_protocol  IPv4 protocol type
                         - 1:  ICMP
                         - 2:  IGMP
                         - 6:  TCP
                         - 17: UDP
  \param  length        frame length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_SendFrame(uint16_t if_id, uint8_t ip4_protocol, uint32_t length) {
    uint32_t val2 = (length << 16) | ip4_protocol;
    EventRecord2 (EvtNetIP4_SendFrame, if_id, val2);
  }
#else
  #define EvrNetIP4_SendFrame(if_id, ip4_protocol, length)
#endif

/**
  \brief  Event on IP4 unknown protocol type in sending frame (Error)
  \param  if_id         network interface identifier
  \param  ip4_protocol  unknown IPv4 protocol type value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_ProtocolUnknown(uint16_t if_id, uint8_t ip4_protocol) {
    EventRecord2 (EvtNetIP4_ProtocolUnknown, if_id, ip4_protocol);
  }
#else
  #define EvrNetIP4_ProtocolUnknown(if_id, ip4_protocol)
#endif

/**
  \brief  Event on IP4 destination address not provided (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_DestinationAddressNull(uint16_t if_id) {
    EventRecord2 (EvtNetIP4_DestinationAddressNull, if_id, 0);
  }
#else
  #define EvrNetIP4_DestinationAddressNull(if_id)
#endif

/**
  \brief  Event on IP4 set default interface for internet access (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_SetDefaultInterface(uint16_t if_id) {
    EventRecord2 (EvtNetIP4_SetDefaultInterface, if_id, 0);
  }
#else
  #define EvrNetIP4_SetDefaultInterface(if_id)
#endif

/**
  \brief  Event on IP4 core de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP4_UninitCore(void) {
    EventRecord2 (EvtNetIP4_UninitCore, 0, 0);
  }
#else
  #define EvrNetIP4_UninitCore()
#endif


// NetICMP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
/* ICMP-Core events */
#define EvtNetICMP_ReceiveFrame             EventID (EventLevelOp,    EvtNetICMP,  0)
#define EvtNetICMP_ShowFrameHeader          EventID (EventLevelDetail,EvtNetICMP, 24)
#define EvtNetICMP_FrameTooShort            EventID (EventLevelError, EvtNetICMP,  1)
#define EvtNetICMP_ChecksumFailed           EventID (EventLevelError, EvtNetICMP,  2)
#define EvtNetICMP_EchoRequestWrongCode     EventID (EventLevelError, EvtNetICMP,  3)
#define EvtNetICMP_EchoRequestReceived      EventID (EventLevelOp,    EvtNetICMP,  4)
#define EvtNetICMP_EchoReplyDisabled        EventID (EventLevelOp,    EvtNetICMP, 25) // End
#define EvtNetICMP_SendEchoReply            EventID (EventLevelOp,    EvtNetICMP,  5)
#define EvtNetICMP_EchoReplyReceived        EventID (EventLevelOp,    EvtNetICMP,  6)
#define EvtNetICMP_EchoReplyWrongState      EventID (EventLevelError, EvtNetICMP,  7)
#define EvtNetICMP_EchoReplyWrongCode       EventID (EventLevelError, EvtNetICMP,  8)
#define EvtNetICMP_EchoReplyWrongIpAddress  EventID (EventLevelError, EvtNetICMP,  9)
#define EvtNetICMP_EchoReplyWrongId         EventID (EventLevelError, EvtNetICMP, 10)
#define EvtNetICMP_EchoReplyWrongPayload    EventID (EventLevelError, EvtNetICMP, 11)
#define EvtNetICMP_MessageTypeUnknown       EventID (EventLevelOp,    EvtNetICMP, 12)
#define EvtNetICMP_SendEchoRequest          EventID (EventLevelOp,    EvtNetICMP, 13)
/* ICMP-Ping client events */
#define EvtNetICMP_PingInit                 EventID (EventLevelOp,    EvtNetICMP, 14)
#define EvtNetICMP_PingEcho                 EventID (EventLevelAPI,   EvtNetICMP, 15)
#define EvtNetICMP_PingTargetNotValid       EventID (EventLevelError, EvtNetICMP, 16)
#define EvtNetICMP_PingDnsError             EventID (EventLevelError, EvtNetICMP, 17)
#define EvtNetICMP_PingInvalidParameter     EventID (EventLevelError, EvtNetICMP, 18)
#define EvtNetICMP_PingClientBusy           EventID (EventLevelError, EvtNetICMP, 19)
#define EvtNetICMP_PingSendRequest          EventID (EventLevelOp,    EvtNetICMP, 20)
#define EvtNetICMP_PingRetransmitRequest    EventID (EventLevelOp,    EvtNetICMP, 21)
#define EvtNetICMP_PingTimeout              EventID (EventLevelOp,    EvtNetICMP, 22)
#define EvtNetICMP_PingUninit               EventID (EventLevelOp,    EvtNetICMP, 23)
#endif

/**
  \brief  Event on ICMP receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetICMP_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetICMP_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on ICMP display send/receive frame header (Detail)
  \param  icmp_header   pointer to ICMP frame header of 8 bytes
                         - Type    (1 byte)
                         - Code    (1 byte)
                         - Chksum  (2 bytes)
                         - EchoId  (2 bytes)
                         - EchoSeq (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_ShowFrameHeader(const void *icmp_header) {
    EventRecordData (EvtNetICMP_ShowFrameHeader, icmp_header, 8);
  }
#else
  #define EvrNetICMP_ShowFrameHeader(icmp_header)
#endif

/**
  \brief  Event on ICMP receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetICMP_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetICMP_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on ICMP frame checksum check failed (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_ChecksumFailed(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetICMP_ChecksumFailed, if_id, length);
  }
#else
  #define EvrNetICMP_ChecksumFailed(if_id, length)
#endif

/**
  \brief  Event on ICMP receive wrong code in echo request (Error)
  \param  if_id         network interface identifier
  \param  code          received ICMP code
  \param  code_valid    valid ICMP code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoRequestWrongCode(uint16_t if_id, uint8_t code, uint8_t code_valid) {
    uint32_t val2 = ((uint32_t)code_valid << 16) | code;
    EventRecord2 (EvtNetICMP_EchoRequestWrongCode, if_id, val2);
  }
#else
  #define EvrNetICMP_EchoRequestWrongCode(if_id, code, code_valid)
#endif

/**
  \brief  Event on ICMP receive echo request (Op)
  \param  if_id         network interface identifier
  \param  data_length   length of the payload data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoRequestReceived(uint16_t if_id, uint32_t data_length) {
    EventRecord2 (EvtNetICMP_EchoRequestReceived, if_id, data_length);
  }
#else
  #define EvrNetICMP_EchoRequestReceived(if_id, data_length)
#endif

/**
  \brief  Event on ICMP sending echo reply disabled (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_EchoReplyDisabled, if_id, 0);
  }
#else
  #define EvrNetICMP_EchoReplyDisabled(if_id)
#endif

/**
  \brief  Event on ICMP send echo reply (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_SendEchoReply(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_SendEchoReply, if_id, 0);
  }
#else
  #define EvrNetICMP_SendEchoReply(if_id)
#endif

/**
  \brief  Event on ICMP receive echo reply (Op)
  \param  if_id         network interface identifier
  \param  data_length   length of the payload data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyReceived(uint16_t if_id, uint32_t data_length) {
    EventRecord2 (EvtNetICMP_EchoReplyReceived, if_id, data_length);
  }
#else
  #define EvrNetICMP_EchoReplyReceived(if_id, data_length)
#endif

/**
  \brief  Event on ICMP receive echo reply in wrong state (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyWrongState(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_EchoReplyWrongState, if_id, 0);
  }
#else
  #define EvrNetICMP_EchoReplyWrongState(if_id)
#endif

/**
  \brief  Event on ICMP receive wrong code in echo reply (Error)
  \param  if_id         network interface identifier
  \param  code          received ICMP code
  \param  code_valid    valid ICMP code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyWrongCode(uint16_t if_id, uint8_t code, uint8_t code_valid) {
    uint32_t val2 = ((uint32_t)code_valid << 16) | code;
    EventRecord2 (EvtNetICMP_EchoReplyWrongCode, if_id, val2);
  }
#else
  #define EvrNetICMP_EchoReplyWrongCode(if_id, code, code_valid)
#endif

/**
  \brief  Event on ICMP wrong source IPv4 address in echo reply (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyWrongIpAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetICMP_EchoReplyWrongIpAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetICMP_EchoReplyWrongIpAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on ICMP wrong echo identifier in echo reply (Error)
  \param  if_id         network interface identifier
  \param  eid           received echo identifier
  \param  eid_valid     valid echo identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyWrongId(uint16_t if_id, uint16_t eid, uint16_t eid_valid) {
    uint32_t val2 = ((uint32_t)eid_valid << 16) | eid;
    EventRecord2 (EvtNetICMP_EchoReplyWrongId, if_id, val2);
  }
#else
  #define EvrNetICMP_EchoReplyWrongId(if_id, eid, eid_valid)
#endif

/**
  \brief  Event on ICMP invalid payload data in echo reply (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_EchoReplyWrongPayload(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_EchoReplyWrongPayload, if_id, 0);
  }
#else
  #define EvrNetICMP_EchoReplyWrongPayload(if_id)
#endif

/**
  \brief  Event on ICMP message type unknown (Op)
  \param  if_id         network interface identifier
  \param  icmp_type     wrong ICMP message type value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_MessageTypeUnknown(uint16_t if_id, uint8_t icmp_type) {
    EventRecord2 (EvtNetICMP_MessageTypeUnknown, if_id, icmp_type);
  }
#else
  #define EvrNetICMP_MessageTypeUnknown(if_id, icmp_type)
#endif

/**
  \brief  Event on ICMP send echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_SendEchoRequest(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_SendEchoRequest, if_id, 0);
  }
#else
  #define EvrNetICMP_SendEchoRequest(if_id)
#endif

// ICMP ping client event recorder functions -----------------------------------

/**
  \brief  Event on Ping client initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingInit(void) {
    EventRecord2 (EvtNetICMP_PingInit, 0, 0);
  }
#else
  #define EvrNetICMP_PingInit()
#endif

/**
  \brief  Event on Ping echo request (API)
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingEcho(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetICMP_PingEcho, ip4_addr, 4);
  }
#else
  #define EvrNetICMP_PingEcho(ip4_addr)
#endif

/**
  \brief  Event on Ping target name not valid (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingTargetNotValid(void) {
    EventRecord2 (EvtNetICMP_PingTargetNotValid, 0, 0);
  }
#else
  #define EvrNetICMP_PingTargetNotValid()
#endif

/**
  \brief  Event on Ping host name resolver error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingDnsError(void) {
    EventRecord2 (EvtNetICMP_PingDnsError, 0, 0);
  }
#else
  #define EvrNetICMP_PingDnsError()
#endif

/**
  \brief  Event on Ping invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingInvalidParameter(void) {
    EventRecord2 (EvtNetICMP_PingInvalidParameter, 0, 0);
  }
#else
  #define EvrNetICMP_PingInvalidParameter()
#endif

/**
  \brief  Event on Ping client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingClientBusy(void) {
    EventRecord2 (EvtNetICMP_PingClientBusy, 0, 0);
  }
#else
  #define EvrNetICMP_PingClientBusy()
#endif

/**
  \brief  Event on Ping send echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingSendRequest(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_PingSendRequest, if_id, 0);
  }
#else
  #define EvrNetICMP_PingSendRequest(if_id)
#endif

/**
  \brief  Event on Ping retransmit echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingRetransmitRequest(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_PingRetransmitRequest, if_id, 0);
  }
#else
  #define EvrNetICMP_PingRetransmitRequest(if_id)
#endif

/**
  \brief  Event on Ping client timeout (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingTimeout(uint16_t if_id) {
    EventRecord2 (EvtNetICMP_PingTimeout, if_id, 0);
  }
#else
  #define EvrNetICMP_PingTimeout(if_id)
#endif

/**
  \brief  Event on Ping client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP_PingUninit(void) {
    EventRecord2 (EvtNetICMP_PingUninit, 0, 0);
  }
#else
  #define EvrNetICMP_PingUninit()
#endif


// NetIGMP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetIGMP_InitManager              EventID (EventLevelOp,    EvtNetIGMP,  0)
#define EvtNetIGMP_Join                     EventID (EventLevelAPI,   EvtNetIGMP,  1)
#define EvtNetIGMP_AlreadyInGroup           EventID (EventLevelOp,    EvtNetIGMP,  2)
#define EvtNetIGMP_NoFreeEntries            EventID (EventLevelError, EvtNetIGMP,  3)
#define EvtNetIGMP_SendReport               EventID (EventLevelOp,    EvtNetIGMP,  4)
#define EvtNetIGMP_Leave                    EventID (EventLevelAPI,   EvtNetIGMP,  5)
#define EvtNetIGMP_NotInGroup               EventID (EventLevelError, EvtNetIGMP,  6)
#define EvtNetIGMP_SendLeave                EventID (EventLevelOp,    EvtNetIGMP,  7)
#define EvtNetIGMP_ReceiveFrame             EventID (EventLevelOp,    EvtNetIGMP,  8)
#define EvtNetIGMP_FrameTooShort            EventID (EventLevelError, EvtNetIGMP,  9)
#define EvtNetIGMP_ChecksumFailed           EventID (EventLevelError, EvtNetIGMP, 10)
#define EvtNetIGMP_GroupSpecificQuery       EventID (EventLevelOp,    EvtNetIGMP, 11)
#define EvtNetIGMP_DestAddressWrong         EventID (EventLevelError, EvtNetIGMP, 12)
#define EvtNetIGMP_DelayedReportScheduled   EventID (EventLevelOp,    EvtNetIGMP, 13)
#define EvtNetIGMP_GeneralQuery             EventID (EventLevelOp,    EvtNetIGMP, 14)
#define EvtNetIGMP_StartModeIGMPv1          EventID (EventLevelOp,    EvtNetIGMP, 15)
#define EvtNetIGMP_MaxTimeForReport         EventID (EventLevelOp,    EvtNetIGMP, 16)
#define EvtNetIGMP_GroupReportsScheduled    EventID (EventLevelOp,    EvtNetIGMP, 17)
#define EvtNetIGMP_NoReportScheduled        EventID (EventLevelOp,    EvtNetIGMP, 23) // End
#define EvtNetIGMP_ReportReceived           EventID (EventLevelOp,    EvtNetIGMP, 18)
#define EvtNetIGMP_OwnReportCanceled        EventID (EventLevelOp,    EvtNetIGMP, 19)
#define EvtNetIGMP_StartModeIGMPv2          EventID (EventLevelOp,    EvtNetIGMP, 20)
#define EvtNetIGMP_SendDelayedReport        EventID (EventLevelOp,    EvtNetIGMP, 21)
#define EvtNetIGMP_UninitManager            EventID (EventLevelOp,    EvtNetIGMP, 22)
#endif

/**
  \brief  Event on IGMP manager initialize (Op)
  \param  if_id         network interface identifier
  \param  num_entries   number of entries available in membership table
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_InitManager(uint16_t if_id, uint32_t num_entries) {
    EventRecord2 (EvtNetIGMP_InitManager, if_id, num_entries);
  }
#else
  #define EvrNetIGMP_InitManager(if_id, num_entries)
#endif

/**
  \brief  Event on IGMP join group membership (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_Join(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_Join, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_Join(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP join already a member (Op)
  \param  if_id         network interface identifier
  \param  group_id      group membership identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_AlreadyInGroup(uint16_t if_id, uint8_t group_id) {
    EventRecord2 (EvtNetIGMP_AlreadyInGroup, if_id, group_id);
  }
#else
  #define EvrNetIGMP_AlreadyInGroup(if_id, group_id)
#endif

/**
  \brief  Event on IGMP join no free entries available (Error)
  \param  if_id         network interface identifier
  \param  num_used      number of used membership table entries
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_NoFreeEntries(uint16_t if_id, uint8_t num_used) {
    EventRecord2 (EvtNetIGMP_NoFreeEntries, if_id, num_used);
  }
#else
  #define EvrNetIGMP_NoFreeEntries(if_id, num_used)
#endif

/**
  \brief  Event on IGMP send report message (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_SendReport(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_SendReport, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_SendReport(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP leave group membership (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_Leave(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_Leave, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_Leave(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP leave not a member (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_NotInGroup(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_NotInGroup, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_NotInGroup(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP send leave message (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_SendLeave(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_SendLeave, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_SendLeave(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP receive frame (Op)
  \param  if_id         network interface identifier
  \param  message_type  type of received message
  \param  length        length of received message
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_ReceiveFrame(uint16_t if_id, uint8_t message_type, uint32_t length) {
    uint32_t val1 = ((uint32_t)message_type << 16) | if_id;
    EventRecord2 (EvtNetIGMP_ReceiveFrame, val1, length);
  }
#else
  #define EvrNetIGMP_ReceiveFrame(if_id, message_type, length)
#endif

/**
  \brief  Event on IGMP receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetIGMP_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetIGMP_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on IGMP frame checksum check failed (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_ChecksumFailed(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetIGMP_ChecksumFailed, if_id, length);
  }
#else
  #define EvrNetIGMP_ChecksumFailed(if_id, length)
#endif

/**
  \brief  Event on IGMPv2 group specific query (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_GroupSpecificQuery(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_GroupSpecificQuery, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_GroupSpecificQuery(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMPv2 destination address wrong (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_DestAddressWrong(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_DestAddressWrong, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_DestAddressWrong(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMPv2 report message delayed (Op)
  \param  if_id         network interface identifier
  \param  max_time      maximum delay time in 100 ms ticks
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_DelayedReportScheduled(uint16_t if_id, uint8_t max_time) {
    EventRecord2 (EvtNetIGMP_DelayedReportScheduled, if_id, max_time);
  }
#else
  #define EvrNetIGMP_DelayedReportScheduled(if_id, max_time)
#endif

/**
  \brief  Event on IGMPv1 general query (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_GeneralQuery(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_GeneralQuery, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_GeneralQuery(if_id, ip4_addr)
#endif

/**
  \brief  Event on start IGMPv1 mode (Op)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_StartModeIGMPv1(uint16_t if_id) {
    EventRecord2 (EvtNetIGMP_StartModeIGMPv1, if_id, 0);
  }
#else
  #define EvrNetIGMP_StartModeIGMPv1(if_id)
#endif

/**
  \brief  Event on IGMPv2 maximum time for report messages (Op)
  \param  if_id         network interface identifier
  \param  max_time      maximum delay time in 100 ms ticks
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_MaxTimeForReport(uint16_t if_id, uint8_t max_time) {
    EventRecord2 (EvtNetIGMP_MaxTimeForReport, if_id, max_time);
  }
#else
  #define EvrNetIGMP_MaxTimeForReport(if_id, max_time)
#endif

/**
  \brief  Event on IGMP scheduled reports for all active groups (Op)
  \param  if_id         network interface identifier
  \param  n_reports     number of scheduled reports
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_GroupReportsScheduled(uint16_t if_id, uint32_t n_reports) {
    EventRecord2 (EvtNetIGMP_GroupReportsScheduled, if_id, n_reports);
  }
#else
  #define EvrNetIGMP_GroupReportsScheduled(if_id, n_reports)
#endif

/**
  \brief  Event on IGMP no report scheduled as no active group (Op)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_NoReportScheduled(uint16_t if_id) {
    EventRecord2 (EvtNetIGMP_NoReportScheduled, if_id, 0);
  }
#else
  #define EvrNetIGMP_NoReportScheduled(if_id)
#endif

/**
  \brief  Event on IGMP received report message (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_ReportReceived(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_ReportReceived, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_ReportReceived(if_id, ip4_addr)
#endif

/**
  \brief  Event on own report canceled (Op)
  \param  if_id         network interface identifier
  \param  group_id      group membership identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_OwnReportCanceled(uint16_t if_id, uint8_t group_id) {
    EventRecord2 (EvtNetIGMP_OwnReportCanceled, if_id, group_id);
  }
#else
  #define EvrNetIGMP_OwnReportCanceled(if_id, group_id)
#endif

/**
  \brief  Event on start IGMPv2 mode (Op)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_StartModeIGMPv2(uint16_t if_id) {
    EventRecord2 (EvtNetIGMP_StartModeIGMPv2, if_id, 0);
  }
#else
  #define EvrNetIGMP_StartModeIGMPv2(if_id)
#endif

/**
  \brief  Event on IGMP send delayed report message (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_SendDelayedReport(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetIGMP_SendDelayedReport, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetIGMP_SendDelayedReport(if_id, ip4_addr)
#endif

/**
  \brief  Event on IGMP manager de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIGMP_UninitManager(uint16_t if_id) {
    EventRecord2 (EvtNetIGMP_UninitManager, if_id, 0);
  }
#else
  #define EvrNetIGMP_UninitManager(if_id)
#endif


// NetNBNS event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetNBNS_InitService              EventID (EventLevelOp,    EvtNetNBNS,  0)
#define EvtNetNBNS_GetSocketFailed          EventID (EventLevelError, EvtNetNBNS,  1)
#define EvtNetNBNS_ReceiveFrame             EventID (EventLevelOp,    EvtNetNBNS,  2)
#define EvtNetNBNS_WrongRemotePort          EventID (EventLevelError, EvtNetNBNS,  3)
#define EvtNetNBNS_FrameTooShort            EventID (EventLevelError, EvtNetNBNS,  4)
#define EvtNetNBNS_NetBiosDisabled          EventID (EventLevelOp,    EvtNetNBNS,  5)
#define EvtNetNBNS_NameQueryRequest         EventID (EventLevelOp,    EvtNetNBNS,  6)
#define EvtNetNBNS_QueryFromAddress         EventID (EventLevelDetail,EvtNetNBNS,  7)
#define EvtNetNBNS_NameQueryResponse        EventID (EventLevelOp,    EvtNetNBNS,  8)
#define EvtNetNBNS_WrongTransactionId       EventID (EventLevelError, EvtNetNBNS,  9)
#define EvtNetNBNS_ResolveInvalidParameter  EventID (EventLevelError, EvtNetNBNS, 10)
#define EvtNetNBNS_ResolveNetBiosDisabled   EventID (EventLevelError, EvtNetNBNS, 11)
#define EvtNetNBNS_ResolveClientBusy        EventID (EventLevelError, EvtNetNBNS, 12)
#define EvtNetNBNS_Resolve                  EventID (EventLevelAPI,   EvtNetNBNS, 13)
#define EvtNetNBNS_ResolvedFromCache        EventID (EventLevelOp,    EvtNetNBNS, 14)
#define EvtNetNBNS_ResolveTimeoutExpired    EventID (EventLevelError, EvtNetNBNS, 15)
#define EvtNetNBNS_ResolvedAddress          EventID (EventLevelOp,    EvtNetNBNS, 16)
#define EvtNetNBNS_ClearCacheInvalidParam   EventID (EventLevelError, EvtNetNBNS, 17)
#define EvtNetNBNS_ClrCacheNetBiosDisabled  EventID (EventLevelError, EvtNetNBNS, 18)
#define EvtNetNBNS_ClearCacheClientBusy     EventID (EventLevelError, EvtNetNBNS, 19)
#define EvtNetNBNS_ClearCache               EventID (EventLevelAPI,   EvtNetNBNS, 20)
#define EvtNetNBNS_ResolveRetransmit        EventID (EventLevelOp,    EvtNetNBNS, 21)
#define EvtNetNBNS_UninitService            EventID (EventLevelOp,    EvtNetNBNS, 22)
#endif

/**
  \brief  Event on NBNS name service initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_InitService(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_InitService, if_id, 0);
  }
#else
  #define EvrNetNBNS_InitService(if_id)
#endif

/**
  \brief  Event on NBNS failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_GetSocketFailed(void) {
    EventRecord2 (EvtNetNBNS_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetNBNS_GetSocketFailed()
#endif

/**
  \brief  Event on NBNS receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetNBNS_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetNBNS_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on NBNS wrong remote UDP port (Error)
  \param  if_id         network interface identifier
  \param  port          received remote port number
  \param  port_valid    valid remote port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_WrongRemotePort(uint16_t if_id, uint16_t port, uint16_t port_valid) {
    uint32_t val2 = ((uint32_t)port_valid << 16) | port;
    EventRecord2 (EvtNetNBNS_WrongRemotePort, if_id, val2);
  }
#else
  #define EvrNetNBNS_WrongRemotePort(if_id, port, port_valid)
#endif

/**
  \brief  Event on NBNS receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetNBNS_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetNBNS_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on NBNS receive frame for NetBIOS disabled (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_NetBiosDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_NetBiosDisabled, if_id, 0);
  }
#else
  #define EvrNetNBNS_NetBiosDisabled(if_id)
#endif

/**
  \brief  Event on NBNS name query request received (Op)
  \param  q_name        pointer to a question name string
  \param  length        length of the question name string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_NameQueryRequest(const char *q_name, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetNBNS_NameQueryRequest, q_name, length);
  }
#else
  #define EvrNetNBNS_NameQueryRequest(q_name, length)
#endif

/**
  \brief  Event on NBNS name query from IP-address (Detail)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of a host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_QueryFromAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetNBNS_QueryFromAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetNBNS_QueryFromAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on NBNS response to a name query (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to local IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_NameQueryResponse(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetNBNS_NameQueryResponse, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetNBNS_NameQueryResponse(if_id, ip4_addr)
#endif

/**
  \brief  Event on NBNS client wrong transaction identifier (TID) received (Error)
  \param  if_id         network interface identifier
  \param  tid           received transaction identifier
  \param  tid_valid     valid transaction identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_WrongTransactionId(uint16_t if_id, uint16_t tid, uint16_t tid_valid) {
    uint32_t val2 = ((uint32_t)tid_valid << 16) | tid;
    EventRecord2 (EvtNetNBNS_WrongTransactionId, if_id, val2);
  }
#else
  #define EvrNetNBNS_WrongTransactionId(if_id, tid, tid_valid)
#endif

/**
  \brief  Event on NBNS resolve request with invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolveInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ResolveInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetNBNS_ResolveInvalidParameter(if_id)
#endif

/**
  \brief  Event on NBNS resolve request with NetBIOS disabled (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolveNetBiosDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ResolveNetBiosDisabled, if_id, 0);
  }
#else
  #define EvrNetNBNS_ResolveNetBiosDisabled(if_id)
#endif

/**
  \brief  Event on NBNS resolve request with client busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolveClientBusy(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ResolveClientBusy, if_id, 0);
  }
#else
  #define EvrNetNBNS_ResolveClientBusy(if_id)
#endif

/**
  \brief  Event on NBNS resolve host requested (API)
  \param  host_name     name of the host to resolve
  \param  length        length of the host_name string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_Resolve(const char *host_name, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetNBNS_Resolve, host_name, length);
  }
#else
  #define EvrNetNBNS_Resolve(host_name, length)
#endif

/**
  \brief  Event on NBNS address resolved from cache (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolvedFromCache(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetNBNS_ResolvedFromCache, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetNBNS_ResolvedFromCache(if_id, ip4_addr)
#endif

/**
  \brief  Event on NBNS resolve request timeout expired (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolveTimeoutExpired(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ResolveTimeoutExpired, if_id, 0);
  }
#else
  #define EvrNetNBNS_ResolveTimeoutExpired(if_id)
#endif

/**
  \brief  Event on NBNS address successfully resolved (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolvedAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetNBNS_ResolvedAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetNBNS_ResolvedAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on NBNS clear cache request with invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ClearCacheInvalidParam(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ClearCacheInvalidParam, if_id, 0);
  }
#else
  #define EvrNetNBNS_ClearCacheInvalidParam(if_id)
#endif

/**
  \brief  Event on NBNS clear cache request with NetBIOS disabled (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ClrCacheNetBiosDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ClrCacheNetBiosDisabled, if_id, 0);
  }
#else
  #define EvrNetNBNS_ClrCacheNetBiosDisabled(if_id)
#endif

/**
  \brief  Event on NBNS clear cache request with client busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ClearCacheClientBusy(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ClearCacheClientBusy, if_id, 0);
  }
#else
  #define EvrNetNBNS_ClearCacheClientBusy(if_id)
#endif

/**
  \brief  Event on NBNS clear cache requested (API)
  \param  if_id         network interface identifier
  \param  deleted       number of entries deleted from cache
  \param  available     number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ClearCache(uint16_t if_id, uint32_t deleted, uint32_t available) {
    uint32_t val2 = (available << 16) | deleted;
    EventRecord2 (EvtNetNBNS_ClearCache, if_id, val2);
  }
#else
  #define EvrNetNBNS_ClearCache(if_id, deleted, available)
#endif

/**
  \brief  Event on NBNS resolve address retransmit (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_ResolveRetransmit(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_ResolveRetransmit, if_id, 0);
  }
#else
  #define EvrNetNBNS_ResolveRetransmit(if_id)
#endif

/**
  \brief  Event on NBNS name service de-initialize (Op)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNBNS_UninitService(uint16_t if_id) {
    EventRecord2 (EvtNetNBNS_UninitService, if_id, 0);
  }
#else
  #define EvrNetNBNS_UninitService(if_id)
#endif


// NetDHCP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetDHCP_InitClient               EventID (EventLevelOp,    EvtNetDHCP,  0)
#define EvtNetDHCP_GetSocketFailed          EventID (EventLevelError, EvtNetDHCP,  1)
#define EvtNetDHCP_StartClient              EventID (EventLevelOp,    EvtNetDHCP,  2)
#define EvtNetDHCP_StopClient               EventID (EventLevelOp,    EvtNetDHCP,  3)
#define EvtNetDHCP_ClientState              EventID (EventLevelOp,    EvtNetDHCP,  4)
#define EvtNetDHCP_NextState                EventID (EventLevelOp,    EvtNetDHCP,  5)
#define EvtNetDHCP_StateRetransmit          EventID (EventLevelOp,    EvtNetDHCP,  6)
#define EvtNetDHCP_ChangeStateOnTimeout     EventID (EventLevelOp,    EvtNetDHCP,  7)
#define EvtNetDHCP_AutoIpAddressProbe       EventID (EventLevelOp,    EvtNetDHCP,  8)
#define EvtNetDHCP_AutoIpSuccess            EventID (EventLevelOp,    EvtNetDHCP,  9)
#define EvtNetDHCP_T2Expired                EventID (EventLevelOp,    EvtNetDHCP, 10)
#define EvtNetDHCP_LeaseExpired             EventID (EventLevelError, EvtNetDHCP, 11)
#define EvtNetDHCP_SendDhcpMessage          EventID (EventLevelOp,    EvtNetDHCP, 12)
#define EvtNetDHCP_ReceiveFrame             EventID (EventLevelOp,    EvtNetDHCP, 13)
#define EvtNetDHCP_WrongServerPort          EventID (EventLevelError, EvtNetDHCP, 14)
#define EvtNetDHCP_FrameTooShort            EventID (EventLevelError, EvtNetDHCP, 15)
#define EvtNetDHCP_MisformedReply           EventID (EventLevelError, EvtNetDHCP, 16)
#define EvtNetDHCP_WrongTransactionId       EventID (EventLevelError, EvtNetDHCP, 17)
#define EvtNetDHCP_WrongClientHwAddress     EventID (EventLevelError, EvtNetDHCP, 18)
#define EvtNetDHCP_WrongMagicCookie         EventID (EventLevelError, EvtNetDHCP, 19)
#define EvtNetDHCP_InvalidMessageType       EventID (EventLevelOp,    EvtNetDHCP, 20)
#define EvtNetDHCP_ViewMessage              EventID (EventLevelOp,    EvtNetDHCP, 21)
#define EvtNetDHCP_FileOverloadOptions      EventID (EventLevelDetail,EvtNetDHCP, 22)
#define EvtNetDHCP_SnameOverloadOptions     EventID (EventLevelDetail,EvtNetDHCP, 23)
#define EvtNetDHCP_MissingServerId          EventID (EventLevelOp,    EvtNetDHCP, 24)
#define EvtNetDHCP_ForwardedMessage         EventID (EventLevelOp,    EvtNetDHCP, 25)
#define EvtNetDHCP_OfferedAddressInvalid    EventID (EventLevelError, EvtNetDHCP, 26)
#define EvtNetDHCP_ViewRelayAgentAddress    EventID (EventLevelDetail,EvtNetDHCP, 27)
#define EvtNetDHCP_ViewOfferedAddress       EventID (EventLevelOp,    EvtNetDHCP, 28)
#define EvtNetDHCP_ServerAddressNotSelected EventID (EventLevelOp,    EvtNetDHCP, 29)
#define EvtNetDHCP_AssignedAddrNotRequested EventID (EventLevelOp,    EvtNetDHCP, 30)
#define EvtNetDHCP_ViewAssignedAddress      EventID (EventLevelOp,    EvtNetDHCP, 31)
#define EvtNetDHCP_ViewServerId             EventID (EventLevelOp,    EvtNetDHCP, 32)
#define EvtNetDHCP_ViewNetMask              EventID (EventLevelOp,    EvtNetDHCP, 33)
#define EvtNetDHCP_ViewGatewayAddress       EventID (EventLevelOp,    EvtNetDHCP, 34)
#define EvtNetDHCP_ViewDnsServers           EventID (EventLevelOp,    EvtNetDHCP, 35)
#define EvtNetDHCP_ViewLeaseTime            EventID (EventLevelOp,    EvtNetDHCP, 36)
#define EvtNetDHCP_ViewTimeT1               EventID (EventLevelOp,    EvtNetDHCP, 37)
#define EvtNetDHCP_ViewTimeT2               EventID (EventLevelOp,    EvtNetDHCP, 38)
#define EvtNetDHCP_ViewBootfileName         EventID (EventLevelOp,    EvtNetDHCP, 39)
#define EvtNetDHCP_ViewNtpServerList        EventID (EventLevelOp,    EvtNetDHCP, 40)
#define EvtNetDHCP_SetOptionInvalidParam    EventID (EventLevelError, EvtNetDHCP, 41)
#define EvtNetDHCP_SetOptionInvalidState    EventID (EventLevelError, EvtNetDHCP, 42)
#define EvtNetDHCP_SetOptionClientIdDefault EventID (EventLevelAPI,   EvtNetDHCP, 43)
#define EvtNetDHCP_SetOptionClientId        EventID (EventLevelAPI,   EvtNetDHCP, 44)
#define EvtNetDHCP_UninitClient             EventID (EventLevelOp,    EvtNetDHCP, 45)
#endif

/**
  \brief  Event on DHCP client initialize (Op)
  \param  if_id         network interface identifier
  \param  bootfile      Bootfile name option enabled
  \param  ntp_servers   NTP servers list option enabled
  \param  vcid          Vendor Class Identifier option enabled
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_InitClient(uint16_t if_id, uint8_t bootfile,
                                             uint8_t ntp_servers, uint8_t vcid) {
    uint32_t val2 = (uint32_t)((bootfile << 2) | (ntp_servers << 1) | vcid);
    EventRecord2 (EvtNetDHCP_InitClient, if_id, val2);
  }
#else
  #define EvrNetDHCP_InitClient(if_id, bootfile, ntp_servers, vcid)
#endif

/**
  \brief  Event on DHCP failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_GetSocketFailed(void) {
    EventRecord2 (EvtNetDHCP_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetDHCP_GetSocketFailed()
#endif

/**
  \brief  Event on DHCP client start (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_StartClient(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP_StartClient, if_id, 0);
  }
#else
  #define EvrNetDHCP_StartClient(if_id)
#endif

/**
  \brief  Event on DHCP client stop (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_StopClient(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP_StopClient, if_id, 0);
  }
#else
  #define EvrNetDHCP_StopClient(if_id)
#endif

/**
  \brief  Event on DHCP indication of client state (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ClientState(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_ClientState, if_id, state);
  }
#else
  #define EvrNetDHCP_ClientState(if_id, state)
#endif

/**
  \brief  Event on DHCP transition to next state (Op)
  \param  if_id         network interface identifier
  \param  state         next state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_NextState(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_NextState, if_id, state);
  }
#else
  #define EvrNetDHCP_NextState(if_id, state)
#endif

/**
  \brief  Event on DHCP retransmit in current state (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_StateRetransmit(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_StateRetransmit, if_id, state);
  }
#else
  #define EvrNetDHCP_StateRetransmit(if_id, state)
#endif

/**
  \brief  Event on DHCP state timeout and transit to next state (Op)
  \param  if_id         network interface identifier
  \param  current       current state
  \param  next          next state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ChangeStateOnTimeout(uint16_t if_id, uint8_t current, uint8_t next) {
    uint32_t val2 = ((uint32_t)next << 16) | current;
    EventRecord2 (EvtNetDHCP_ChangeStateOnTimeout, if_id, val2);
  }
#else
  #define EvrNetDHCP_ChangeStateOnTimeout(if_id, current, next)
#endif

/**
  \brief  Event on DHCP address probe for AutoIP mode (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_AutoIpAddressProbe(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_AutoIpAddressProbe, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_AutoIpAddressProbe(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP assign AutoIP address success (Op)
  \param  if_id         network interface identifier
  \param  auto_lm       pointer to assigned AutoIP address and network mask
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_AutoIpSuccess(uint16_t if_id, const uint8_t *auto_lm) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], auto_lm, 8);
    EventRecordData (EvtNetDHCP_AutoIpSuccess, &evr_buf, 12);
  }
#else
  #define EvrNetDHCP_AutoIpSuccess(if_id, auto_lm)
#endif

/**
  \brief  Event on DHCP timer T2 expired (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_T2Expired(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_T2Expired, if_id, state);
  }
#else
  #define EvrNetDHCP_T2Expired(if_id, state)
#endif

/**
  \brief  Event on DHCP address lease time expired (Error)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_LeaseExpired(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_LeaseExpired, if_id, state);
  }
#else
  #define EvrNetDHCP_LeaseExpired(if_id, state)
#endif

/**
  \brief  Event on DHCP send message (Op)
  \param  if_id         network interface identifier
  \param  msg_type      message type
  \param  broadcast     broadcast the message
                         - 0: no
                         - 1: yes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SendDhcpMessage(uint16_t if_id, uint8_t msg_type, uint8_t broadcast) {
    uint32_t val2 = ((uint32_t)broadcast << 16) | msg_type;
    EventRecord2 (EvtNetDHCP_SendDhcpMessage, if_id, val2);
  }
#else
  #define EvrNetDHCP_SendDhcpMessage(if_id, msg_type, broadcast)
#endif

/**
  \brief  Event on DHCP receive frame (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address of the server
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ReceiveFrame(uint16_t if_id, const uint8_t *ip4_addr, uint32_t length) {
    uint32_t val1 = (length << 16) | if_id;
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ReceiveFrame, val1, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ReceiveFrame(if_id, ip4_addr, length)
#endif

/**
  \brief  Event on DHCP wrong server UDP port (Error)
  \param  if_id         network interface identifier
  \param  port          received server port number
  \param  port_valid    valid server port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_WrongServerPort(uint16_t if_id, uint16_t port, uint16_t port_valid) {
    uint32_t val2 = ((uint32_t)port_valid << 16) | port;
    EventRecord2 (EvtNetDHCP_WrongServerPort, if_id, val2);
  }
#else
  #define EvrNetDHCP_WrongServerPort(if_id, port, port_valid)
#endif

/**
  \brief  Event on DHCP receive frame too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetDHCP_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetDHCP_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on DHCP receive misformed reply (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_MisformedReply(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP_MisformedReply, if_id, 0);
  }
#else
  #define EvrNetDHCP_MisformedReply(if_id)
#endif

/**
  \brief  Event on DHCP receive wrong transaction identifier (Error)
  \param  if_id         network interface identifier
  \param  xid           received transaction identifier
  \param  xid_valid     valid transaction identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_WrongTransactionId(uint16_t if_id, uint32_t xid, uint32_t xid_valid) {
    EventRecord4 (EvtNetDHCP_WrongTransactionId, if_id, xid, xid_valid, 0);
  }
#else
  #define EvrNetDHCP_WrongTransactionId(if_id, xid, xid_valid)
#endif

/**
  \brief  Event on DHCP receive wrong Client Hardware Address (Error)
  \param  if_id         network interface identifier
  \param  chaddr        pointer to received chaddr address (MAC)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_WrongClientHwAddress(uint16_t if_id, const uint8_t *chaddr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], chaddr, 6);
    EventRecordData (EvtNetDHCP_WrongClientHwAddress, &evr_buf, 8);
  }
#else
  #define EvrNetDHCP_WrongClientHwAddress(if_id, chaddr)
#endif

/**
  \brief  Event on DHCP receive wrong BOOTP Magic Cookie (Error)
  \param  if_id         network interface identifier
  \param  cookie        received magic cookie
  \param  cookie_valid  valid magic cookie
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_WrongMagicCookie(uint16_t if_id, uint32_t cookie, uint32_t cookie_valid) {
    EventRecord4 (EvtNetDHCP_WrongMagicCookie, if_id, cookie, cookie_valid, 0);
  }
#else
  #define EvrNetDHCP_WrongMagicCookie(if_id, cookie, cookie_valid)
#endif

/**
  \brief  Event on DHCP receive invalid message type (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \param  msg_valid     valid message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_InvalidMessageType(uint16_t if_id, uint8_t msg_type, uint8_t msg_valid) {
    uint32_t val2 = ((uint32_t)msg_valid << 16) | msg_type;
    EventRecord2 (EvtNetDHCP_InvalidMessageType, if_id, val2);
  }
#else
  #define EvrNetDHCP_InvalidMessageType(if_id, msg_type, msg_valid)
#endif

/**
  \brief  Event on DHCP display message type (Op)
  \param  if_id         network interface identifier
  \param  msg_type      message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewMessage(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP_ViewMessage, if_id, msg_type);
  }
#else
  #define EvrNetDHCP_ViewMessage(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP receive options overloaded to "File" field (Detail)
  \param  if_id         network interface identifier
  \param  msg_type      message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_FileOverloadOptions(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP_FileOverloadOptions, if_id, msg_type);
  }
#else
  #define EvrNetDHCP_FileOverloadOptions(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP receive options overloaded to "Sname" field (Detail)
  \param  if_id         network interface identifier
  \param  msg_type      message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SnameOverloadOptions(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP_SnameOverloadOptions, if_id, msg_type);
  }
#else
  #define EvrNetDHCP_SnameOverloadOptions(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP receive message with no Server ID (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_MissingServerId(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP_MissingServerId, if_id, msg_type);
  }
#else
  #define EvrNetDHCP_MissingServerId(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP receive forwarded message (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ForwardedMessage(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP_ForwardedMessage, if_id, msg_type);
  }
#else
  #define EvrNetDHCP_ForwardedMessage(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP receive message offering an invalid IP address (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_OfferedAddressInvalid(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_OfferedAddressInvalid, if_id,  evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_OfferedAddressInvalid(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display relay agent IP address (Detail)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewRelayAgentAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ViewRelayAgentAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewRelayAgentAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display offered IP address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewOfferedAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ViewOfferedAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewOfferedAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP receive message from not selected server (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ServerAddressNotSelected(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ServerAddressNotSelected, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ServerAddressNotSelected(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP receive wrong assigned IP address in DHCP_ACK (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_AssignedAddrNotRequested(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_AssignedAddrNotRequested, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_AssignedAddrNotRequested(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display assigned IP address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewAssignedAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ViewAssignedAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewAssignedAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display Server Identifier (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewServerId(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ViewServerId, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewServerId(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display Network Mask (Op)
  \param  if_id         network interface identifier
  \param  net_mask      pointer to network mask
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewNetMask(uint16_t if_id, const uint8_t *net_mask) {
    memcpy (&evr_buf.u32[0], net_mask, 4);
    EventRecord2 (EvtNetDHCP_ViewNetMask, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewNetMask(if_id, net_mask)
#endif

/**
  \brief  Event on DHCP display gateway IPv4 address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IPv4 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewGatewayAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetDHCP_ViewGatewayAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetDHCP_ViewGatewayAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on DHCP display DNS server addresses (Op)
  \param  if_id         network interface identifier
  \param  dns_list      pointer to DNS server address list
  \remark Displays primary and secondary DNS server address.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewDnsServers(uint16_t if_id, const uint8_t *dns_list) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], dns_list, 8);
    EventRecordData (EvtNetDHCP_ViewDnsServers, &evr_buf, 12);
  }
#else
  #define EvrNetDHCP_ViewDnsServers(if_id, dns_list)
#endif

/**
  \brief  Event on DHCP display address lease time (Op)
  \param  if_id         network interface identifier
  \param  lease_time    lease time in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewLeaseTime(uint16_t if_id, uint32_t lease_time) {
    EventRecord2 (EvtNetDHCP_ViewLeaseTime, if_id, lease_time);
  }
#else
  #define EvrNetDHCP_ViewLeaseTime(if_id, lease_time)
#endif

/**
  \brief  Event on DHCP display renewal time T1 (Op)
  \param  if_id         network interface identifier
  \param  t1_time       t1 time in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewTimeT1(uint16_t if_id, uint32_t t1_time) {
    EventRecord2 (EvtNetDHCP_ViewTimeT1, if_id, t1_time);
  }
#else
  #define EvrNetDHCP_ViewTimeT1(if_id, t1_time)
#endif

/**
  \brief  Event on DHCP display rebinding time T2 (Op)
  \param  if_id         network interface identifier
  \param  t2_time       t2 time in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewTimeT2(uint16_t if_id, uint32_t t2_time) {
    EventRecord2 (EvtNetDHCP_ViewTimeT2, if_id, t2_time);
  }
#else
  #define EvrNetDHCP_ViewTimeT2(if_id, t2_time)
#endif

/**
  \brief  Event on DHCP display Bootfile Name (Op)
  \param  bootfile      bootfile name string
  \param  length        length of the bootfile name string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewBootfileName(const uint8_t *bootfile, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData (EvtNetDHCP_ViewBootfileName, bootfile, length);
  }
#else
  #define EvrNetDHCP_ViewBootfileName(bootfile, length)
#endif

/**
  \brief  Event on DHCP display list of NTP server addresses (Op)
  \param  if_id         network interface identifier
  \param  ntp_list      pointer to an array of NTP server IP addresses
  \param  length        length of an array in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_ViewNtpServerList(uint16_t if_id, const uint8_t *ntp_list, uint32_t length) {
    evr_buf.u16[0] = if_id;
    evr_buf.u16[1] = (length > 12 ? 12 : length) & 0xFFFF;
    memcpy (&evr_buf.u16[2], ntp_list, evr_buf.u16[1]);
    EventRecordData (EvtNetDHCP_ViewNtpServerList, &evr_buf, evr_buf.u16[1]);
  }
#else
  #define EvrNetDHCP_ViewNtpServerList(if_id, ntp_list, length)
#endif

/**
  \brief  Event on DHCP set_option invalid parameter (Error)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SetOptionInvalidParam(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP_SetOptionInvalidParam, if_id, 0);
  }
#else
  #define EvrNetDHCP_SetOptionInvalidParam(if_id)
#endif

/**
  \brief  Event on DHCP set_option invalid client state (Error)
  \param  if_id         network interface identifier
  \param  state         DHCP client state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SetOptionInvalidState(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP_SetOptionInvalidState, if_id, state);
  }
#else
  #define EvrNetDHCP_SetOptionInvalidState(if_id, state)
#endif

/**
  \brief  Event on DHCP set_option with default Client-identifier (API)
  \param  if_id         network interface identifier
  \param  type          hardware type
                         - 0:   Generic,
                         - 1:   Ethernet,
                         - 255: IANA.
  \remark Default DHCP Client-identifier is ethernet MAC address.
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SetOptionClientIdDefault(uint16_t if_id, uint8_t type) {
    EventRecord2 (EvtNetDHCP_SetOptionClientIdDefault, if_id, type);
  }
#else
  #define EvrNetDHCP_SetOptionClientIdDefault(if_id, type)
#endif

/**
  \brief  Event on DHCP set_option with user Client-identifier (API)
  \param  if_id         network interface identifier
  \param  type          hardware type
                         - 0:   Generic,
                         - 1:   Ethernet,
                         - 255: IANA.
  \param  length        length of DHCP option including hardware type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_SetOptionClientId(uint16_t if_id, uint8_t type, uint32_t length) {
    uint32_t val2 = ((uint32_t)type << 16) | length;
    EventRecord2 (EvtNetDHCP_SetOptionClientId, if_id, val2);
  }
#else
  #define EvrNetDHCP_SetOptionClientId(if_id, type, length)
#endif

/**
  \brief  Event on DHCP client de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP_UninitClient(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP_UninitClient, if_id, 0);
  }
#else
  #define EvrNetDHCP_UninitClient(if_id)
#endif


// NetARP event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetARP_InitCache                 EventID (EventLevelOp,    EvtNetARP,  0)
#define EvtNetARP_ReceiveFrame              EventID (EventLevelOp,    EvtNetARP,  1)
#define EvtNetARP_FrameTooShort             EventID (EventLevelError, EvtNetARP,  2)
#define EvtNetARP_FrameCorrupted            EventID (EventLevelError, EvtNetARP,  3)
#define EvtNetARP_ShowFrameHeader           EventID (EventLevelDetail,EvtNetARP,  4)
#define EvtNetARP_SenderAddressInvalid      EventID (EventLevelOp,    EvtNetARP,  5)
#define EvtNetARP_CacheEntryUpdate          EventID (EventLevelOp,    EvtNetARP,  6)
#define EvtNetARP_ProbeResponseReceived     EventID (EventLevelOp,    EvtNetARP,  7)
#define EvtNetARP_WrongIpAddress            EventID (EventLevelOp,    EvtNetARP,  8)
#define EvtNetARP_WrongMacAddress           EventID (EventLevelOp,    EvtNetARP,  9)
#define EvtNetARP_OpcodeUnknown             EventID (EventLevelOp,    EvtNetARP, 10)
#define EvtNetARP_SendReply                 EventID (EventLevelOp,    EvtNetARP, 11)
#define EvtNetARP_CacheEntryRefreshed       EventID (EventLevelOp,    EvtNetARP, 12)
#define EvtNetARP_WrongResponse             EventID (EventLevelError, EvtNetARP, 13)
#define EvtNetARP_SendRequest               EventID (EventLevelOp,    EvtNetARP, 14)
#define EvtNetARP_CacheAllocFailed          EventID (EventLevelError, EvtNetARP, 15)
#define EvtNetARP_CacheIpRefreshed          EventID (EventLevelOp,    EvtNetARP, 16)
#define EvtNetARP_CacheAdd                  EventID (EventLevelOp,    EvtNetARP, 17)
#define EvtNetARP_GatewayUnknown            EventID (EventLevelError, EvtNetARP, 18)
#define EvtNetARP_CacheEntryAdded           EventID (EventLevelOp,    EvtNetARP, 19)
#define EvtNetARP_CacheEarly                EventID (EventLevelOp,    EvtNetARP, 20)
#define EvtNetARP_CacheFind                 EventID (EventLevelOp,    EvtNetARP, 21)
#define EvtNetARP_UsingGateway              EventID (EventLevelOp,    EvtNetARP, 22)
#define EvtNetARP_UnresolvedMacAddress      EventID (EventLevelError, EvtNetARP, 23)
#define EvtNetARP_EntryFound                EventID (EventLevelOp,    EvtNetARP, 24)
#define EvtNetARP_CacheIp                   EventID (EventLevelAPI,   EvtNetARP, 25)
#define EvtNetARP_CacheIpInvalidParameter   EventID (EventLevelError, EvtNetARP, 26)
#define EvtNetARP_CacheMac                  EventID (EventLevelAPI,   EvtNetARP, 27)
#define EvtNetARP_CacheMacInvalidParameter  EventID (EventLevelError, EvtNetARP, 28)
#define EvtNetARP_UnresolvedIpAddress       EventID (EventLevelError, EvtNetARP, 29)
#define EvtNetARP_GetIp                     EventID (EventLevelAPI,   EvtNetARP, 30)
#define EvtNetARP_GetIpInvalidParameter     EventID (EventLevelError, EvtNetARP, 31)
#define EvtNetARP_GetIpEntryNotFound        EventID (EventLevelOp,    EvtNetARP, 32)
#define EvtNetARP_GetMac                    EventID (EventLevelAPI,   EvtNetARP, 33)
#define EvtNetARP_GetMacInvalidParameter    EventID (EventLevelError, EvtNetARP, 34)
#define EvtNetARP_GetMacEntryNotFound       EventID (EventLevelOp,    EvtNetARP, 35)
#define EvtNetARP_Probe                     EventID (EventLevelAPI,   EvtNetARP, 36)
#define EvtNetARP_ProbeInvalidParameter     EventID (EventLevelError, EvtNetARP, 37)
#define EvtNetARP_ProbeBusy                 EventID (EventLevelError, EvtNetARP, 38)
#define EvtNetARP_ProbeRetransmit           EventID (EventLevelOp,    EvtNetARP, 39)
#define EvtNetARP_ProbeTimeout              EventID (EventLevelOp,    EvtNetARP, 40)
#define EvtNetARP_AddCache                  EventID (EventLevelAPI,   EvtNetARP, 49)
#define EvtNetARP_AddCacheInvalidParam      EventID (EventLevelError, EvtNetARP, 50)
#define EvtNetARP_AddCacheInvalidIpAddress  EventID (EventLevelError, EvtNetARP, 51)
#define EvtNetARP_AddCacheInvalidMacAddress EventID (EventLevelError, EvtNetARP, 52) // End
#define EvtNetARP_ClearCache                EventID (EventLevelAPI,   EvtNetARP, 46)
#define EvtNetARP_ClearCacheInvalidParam    EventID (EventLevelError, EvtNetARP, 47)
#define EvtNetARP_ClearCacheClientBusy      EventID (EventLevelError, EvtNetARP, 48)
#define EvtNetARP_CacheEntryTimeout         EventID (EventLevelOp,    EvtNetARP, 41)
#define EvtNetARP_EntryReleased             EventID (EventLevelOp,    EvtNetARP, 42)
#define EvtNetARP_ResolveEntry              EventID (EventLevelOp,    EvtNetARP, 43)
#define EvtNetARP_RefreshEntry              EventID (EventLevelOp,    EvtNetARP, 44)
#define EvtNetARP_UninitCache               EventID (EventLevelOp,    EvtNetARP, 45)
#endif

/**
  \brief  Event on ARP cache initialize (Op)
  \param  if_id         network interface identifier
  \param  num_entries   number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_InitCache(uint16_t if_id, uint32_t num_entries) {
    EventRecord2 (EvtNetARP_InitCache, if_id, num_entries);
  }
#else
  #define EvrNetARP_InitCache(if_id, num_entries)
#endif

/**
  \brief  Event on ARP receive frame (Op)
  \param  if_id         network interface identifier
  \param  opcode        operation code of received message
                         - 0x0001: ARP Request
                         - 0x0002: ARP Reply
                         - 0x0003: Reverse ARP Request
                         - 0x0004: Reverse ARP Reply
                         - 0x0008: Inverse ARP Request
                         - 0x0009: Inverse ARP Response
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ReceiveFrame(uint16_t if_id, uint16_t opcode, uint32_t length) {
    uint32_t val2 = ((uint32_t)opcode << 16) | length;
    EventRecord2 (EvtNetARP_ReceiveFrame, if_id, val2);
  }
#else
  #define EvrNetARP_ReceiveFrame(if_id, opcode, length)
#endif

/**
  \brief  Event on ARP receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetARP_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetARP_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on ARP received frame corrupted (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_FrameCorrupted(uint16_t if_id) {
    EventRecord2 (EvtNetARP_FrameCorrupted, if_id, 0);
  }
#else
  #define EvrNetARP_FrameCorrupted(if_id)
#endif

/**
  \brief  Event on ARP display send/receive frame header (Detail)
  \param  arp_header    pointer to ARP frame header of 28 bytes
                         - HwType       (2 bytes)
                         - ProtType     (2 bytes)
                         - HwLen        (1 byte)
                         - ProtLen      (1 byte)
                         - OpCode       (2 bytes)
                         - SendHwAddr   (6 bytes)
                         - SendIpAddr   (4 bytes)
                         - TargHwAddr   (6 bytes)
                         - TargIpAddr   (4 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ShowFrameHeader(const void *arp_header) {
    EventRecordData (EvtNetARP_ShowFrameHeader, arp_header, 28);
  }
#else
  #define EvrNetARP_ShowFrameHeader(arp_header)
#endif

/**
  \brief  Event on ARP invalid IP or MAC address of sender received (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_SenderAddressInvalid(uint16_t if_id) {
    EventRecord2 (EvtNetARP_SenderAddressInvalid, if_id, 0);
  }
#else
  #define EvrNetARP_SenderAddressInvalid(if_id)
#endif

/**
  \brief  Event on ARP cache entry update (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheEntryUpdate(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_CacheEntryUpdate, if_id, entry);
  }
#else
  #define EvrNetARP_CacheEntryUpdate(if_id, entry)
#endif

/**
  \brief  Event on ARP probe response received (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ProbeResponseReceived(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_ProbeResponseReceived, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_ProbeResponseReceived(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP wrong destination IP address received (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_WrongIpAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_WrongIpAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_WrongIpAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP wrong destination MAC address received (Op)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_WrongMacAddress(uint16_t if_id, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetARP_WrongMacAddress, &evr_buf, 8);
  }
#else
  #define EvrNetARP_WrongMacAddress(if_id, mac_addr)
#endif

/**
  \brief  Event on ARP unknown opcode received (Op)
  \param  if_id         network interface identifier
  \param  opcode        unknown operation code value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_OpcodeUnknown(uint16_t if_id, uint16_t opcode) {
    EventRecord2 (EvtNetARP_OpcodeUnknown, if_id, opcode);
  }
#else
  #define EvrNetARP_OpcodeUnknown(if_id, opcode)
#endif

/**
  \brief  Event on ARP send reply message (Op)
  \param  if_id         network interface identifier
  \param  opcode        operation code of sending message
                         - 0x0002: ARP Reply
                         - 0x0009: Inverse ARP Response
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_SendReply(uint16_t if_id, uint16_t opcode) {
    EventRecord2 (EvtNetARP_SendReply, if_id, opcode);
  }
#else
  #define EvrNetARP_SendReply(if_id, opcode)
#endif

/**
  \brief  Event on ARP cache entry refreshed (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheEntryRefreshed(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_CacheEntryRefreshed, if_id, entry);
  }
#else
  #define EvrNetARP_CacheEntryRefreshed(if_id, entry)
#endif

/**
  \brief  Event on ARP wrong response received (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_WrongResponse(uint16_t if_id) {
    EventRecord2 (EvtNetARP_WrongResponse, if_id, 0);
  }
#else
  #define EvrNetARP_WrongResponse(if_id)
#endif

/**
  \brief  Event on ARP send request message (Op)
  \param  if_id         network interface identifier
  \param  opcode        operation code of sending message
                         - 0x0001: ARP Request
                         - 0x0008: Inverse ARP Request
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_SendRequest(uint16_t if_id, uint16_t opcode) {
    EventRecord2 (EvtNetARP_SendRequest, if_id, opcode);
  }
#else
  #define EvrNetARP_SendRequest(if_id, opcode)
#endif

/**
  \brief  Event on ARP allocate free cache entry failed (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheAllocFailed(uint16_t if_id) {
    EventRecord2 (EvtNetARP_CacheAllocFailed, if_id, 0);
  }
#else
  #define EvrNetARP_CacheAllocFailed(if_id)
#endif

/**
  \brief  Event on ARP refresh cached IP address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheIpRefreshed(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_CacheIpRefreshed, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_CacheIpRefreshed(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP add host address to ARP cache (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheAdd(uint16_t if_id, const uint8_t *ip4_addr, const uint8_t *mac_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_CacheAdd, &evr_buf, 14);
  }
#else
  #define EvrNetARP_CacheAdd(if_id, ip4_addr, mac_addr)
#endif

/**
  \brief  Event on ARP cache add gateway unknown (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GatewayUnknown(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_GatewayUnknown, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_GatewayUnknown(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP cache entry added (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheEntryAdded(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_CacheEntryAdded, if_id, entry);
  }
#else
  #define EvrNetARP_CacheEntryAdded(if_id, entry)
#endif

/**
  \brief  Event on ARP early cache address of the sender (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
  \param  mac_addr      pointer to MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheEarly(uint16_t if_id, const uint8_t *ip4_addr, const uint8_t *mac_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_CacheEarly, &evr_buf, 14);
  }
#else
  #define EvrNetARP_CacheEarly(if_id, ip4_addr, mac_addr)
#endif

/**
  \brief  Event on ARP find IP address in cache (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheFind(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_CacheFind, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_CacheFind(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP use gateway to access external address (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_UsingGateway(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_UsingGateway, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_UsingGateway(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP unresolved MAC address in cache (Error)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to requested IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_UnresolvedMacAddress(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_UnresolvedMacAddress, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_UnresolvedMacAddress(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP cache entry found (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_EntryFound(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_EntryFound, if_id, entry);
  }
#else
  #define EvrNetARP_EntryFound(if_id, entry)
#endif

/**
  \brief  Event on netARP_CacheIP execute (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to requested IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheIp(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_CacheIp, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_CacheIp(if_id, ip4_addr)
#endif

/**
  \brief  Event on netARP_CacheIP invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheIpInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetARP_CacheIpInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetARP_CacheIpInvalidParameter(if_id)
#endif

/**
  \brief  Event on netARP_CacheMAC execute (API)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to requested MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheMac(uint16_t if_id, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetARP_CacheMac, &evr_buf, 8);
  }
#else
  #define EvrNetARP_CacheMac(if_id, mac_addr)
#endif

/**
  \brief  Event on netARP_CacheMAC invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheMacInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetARP_CacheMacInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetARP_CacheMacInvalidParameter(if_id)
#endif

/**
  \brief  Event on ARP unresolved IP address in cache (Op)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to MAC cache address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_UnresolvedIpAddress(uint16_t if_id, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetARP_UnresolvedIpAddress, &evr_buf, 8);
  }
#else
  #define EvrNetARP_UnresolvedIpAddress(if_id, mac_addr)
#endif

/**
  \brief  Event on netARP_GetIP resolving success (API)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to requested MAC address
  \param  ip4_addr      pointer to resolved IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetIp(uint16_t if_id, const uint8_t *mac_addr, const uint8_t *ip4_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_GetIp, &evr_buf, 14);
  }
#else
  #define EvrNetARP_GetIp(if_id, mac_addr, ip4_addr)
#endif

/**
  \brief  Event on netARP_GetIP invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetIpInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetARP_GetIpInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetARP_GetIpInvalidParameter(if_id)
#endif

/**
  \brief  Event on netARP_GetIP entry with MAC address not found in cache (Op)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to requested MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetIpEntryNotFound(uint16_t if_id, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetARP_GetIpEntryNotFound, &evr_buf, 8);
  }
#else
  #define EvrNetARP_GetIpEntryNotFound(if_id, mac_addr)
#endif

/**
  \brief  Event on netARP_GetMAC resolving success (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to requested IP address
  \param  mac_addr      pointer to resolved MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetMac(uint16_t if_id, const uint8_t *ip4_addr, const uint8_t *mac_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_GetMac, &evr_buf, 14);
  }
#else
  #define EvrNetARP_GetMac(if_id, ip4_addr, mac_addr)
#endif

/**
  \brief  Event on netARP_GetMAC invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetMacInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetARP_GetMacInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetARP_GetMacInvalidParameter(if_id)
#endif

/**
  \brief  Event on netARP_GetMAC entry with IP address not found in cache (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to requested IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_GetMacEntryNotFound(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_GetMacEntryNotFound, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_GetMacEntryNotFound(if_id, ip4_addr)
#endif

/**
  \brief  Event on netARP_Probe execute (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to requested IP address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_Probe(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_Probe, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_Probe(if_id, ip4_addr)
#endif

/**
  \brief  Event on netARP_Probe invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ProbeInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetARP_ProbeInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetARP_ProbeInvalidParameter(if_id)
#endif

/**
  \brief  Event on ARP probe process busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ProbeBusy(uint16_t if_id) {
    EventRecord2 (EvtNetARP_ProbeBusy, if_id, 0);
  }
#else
  #define EvrNetARP_ProbeBusy(if_id)
#endif

/**
  \brief  Event on ARP probe retransmit (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address of probed host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ProbeRetransmit(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_ProbeRetransmit, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_ProbeRetransmit(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP probe timeout (Op)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address of probed host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ProbeTimeout(uint16_t if_id, const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetARP_ProbeTimeout, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetARP_ProbeTimeout(if_id, ip4_addr)
#endif

/**
  \brief  Event on ARP add static host address to ARP cache (API)
  \param  if_id         network interface identifier
  \param  ip4_addr      pointer to IP address of the host
  \param  mac_addr      pointer to MAC address of the host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_AddCache(uint16_t if_id, const uint8_t *ip4_addr, const uint8_t *mac_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_AddCache, &evr_buf, 14);
  }
#else
  #define EvrNetARP_AddCache(if_id, ip4_addr, mac_addr)
#endif

/**
  \brief  Event on netARP_AddCache invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_AddCacheInvalidParam(uint16_t if_id) {
    EventRecord2 (EvtNetARP_AddCacheInvalidParam, if_id, 0);
  }
#else
  #define EvrNetARP_AddCacheInvalidParam(if_id)
#endif

/**
  \brief  Event on netARP_AddCache invalid IP address error (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_AddCacheInvalidIpAddress(uint16_t if_id) {
    EventRecord2 (EvtNetARP_AddCacheInvalidIpAddress, if_id, 0);
  }
#else
  #define EvrNetARP_AddCacheInvalidIpAddress(if_id)
#endif

/**
  \brief  Event on netARP_AddCache invalid MAC address error (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_AddCacheInvalidMacAddress(uint16_t if_id) {
    EventRecord2 (EvtNetARP_AddCacheInvalidMacAddress, if_id, 0);
  }
#else
  #define EvrNetARP_AddCacheInvalidMacAddress(if_id)
#endif

/**
  \brief  Event on ARP clear cache request (API)
  \param  if_id         network interface identifier
  \param  deleted       number of entries deleted from cache
  \param  available     number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ClearCache(uint16_t if_id, uint32_t deleted, uint32_t available) {
    uint32_t val2 = (available << 16) | deleted;
    EventRecord2 (EvtNetARP_ClearCache, if_id, val2);
  }
#else
  #define EvrNetARP_ClearCache(if_id, deleted, available)
#endif

/**
  \brief  Event on netARP_ClearCache invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ClearCacheInvalidParam(uint16_t if_id) {
    EventRecord2 (EvtNetARP_ClearCacheInvalidParam, if_id, 0);
  }
#else
  #define EvrNetARP_ClearCacheInvalidParam(if_id)
#endif

/**
  \brief  Event on ARP clear cache client busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ClearCacheClientBusy(uint16_t if_id) {
    EventRecord2 (EvtNetARP_ClearCacheClientBusy, if_id, 0);
  }
#else
  #define EvrNetARP_ClearCacheClientBusy(if_id)
#endif

/**
  \brief  Event on ARP cache entry timeout (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
  \param  ip4_addr      pointer to IP address
  \param  mac_addr      pointer to MAC address
  \param  type          cache entry type
                        - 0: Fixed IP
                        - 1: Temporary IP
                        - 2: In-use temporary IP
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_CacheEntryTimeout(uint16_t if_id, uint32_t entry,
                    const uint8_t *ip4_addr, const uint8_t *mac_addr, uint32_t type) {
    evr_buf.u16[0] = if_id;
    evr_buf.u8[2]  = entry & 0xFF;
    evr_buf.u8[3]  = type  & 0xFF;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    memcpy (&evr_buf.u8[8], mac_addr, 6);
    EventRecordData (EvtNetARP_CacheEntryTimeout, &evr_buf, 14);
  }
#else
  #define EvrNetARP_CacheEntryTimeout(if_id, entry, type, ip4_addr, mac_addr)
#endif

/**
  \brief  Event on ARP cache entry released (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_EntryReleased(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_EntryReleased, if_id, entry);
  }
#else
  #define EvrNetARP_EntryReleased(if_id, entry)
#endif

/**
  \brief  Event on resolve ARP cache entry (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_ResolveEntry(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_ResolveEntry, if_id, entry);
  }
#else
  #define EvrNetARP_ResolveEntry(if_id, entry)
#endif

/**
  \brief  Event on refresh ARP cache entry (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_RefreshEntry(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetARP_RefreshEntry, if_id, entry);
  }
#else
  #define EvrNetARP_RefreshEntry(if_id, entry)
#endif

/**
  \brief  Event on ARP cache de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetARP_UninitCache(uint16_t if_id) {
    EventRecord2 (EvtNetARP_UninitCache, if_id, 0);
  }
#else
  #define EvrNetARP_UninitCache(if_id)
#endif


// NetIP6 event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetIP6_InitCore                  EventID (EventLevelOp,    EvtNetIP6,  0)
#define EvtNetIP6_ReceiveFrame              EventID (EventLevelOp,    EvtNetIP6,  1)
#define EvtNetIP6_FrameTooShort             EventID (EventLevelError, EvtNetIP6,  2)
#define EvtNetIP6_InvalidIpVersion          EventID (EventLevelError, EvtNetIP6,  3)
#define EvtNetIP6_ShowFrameHeader           EventID (EventLevelDetail,EvtNetIP6,  4)
#define EvtNetIP6_WrongDestinationAddress   EventID (EventLevelOp,    EvtNetIP6,  5)
#define EvtNetIP6_InterfaceNotSupported     EventID (EventLevelError, EvtNetIP6,  6)
#define EvtNetIP6_WrongMulticastProtocol    EventID (EventLevelError, EvtNetIP6,  7)
#define EvtNetIP6_FragmentationDisabled     EventID (EventLevelError, EvtNetIP6, 14) // End
#define EvtNetIP6_SendFrame                 EventID (EventLevelOp,    EvtNetIP6,  8)
#define EvtNetIP6_ProtocolUnknown           EventID (EventLevelError, EvtNetIP6,  9)
#define EvtNetIP6_SourceAddressNull         EventID (EventLevelError, EvtNetIP6, 10)
#define EvtNetIP6_DestinationAddressNull    EventID (EventLevelError, EvtNetIP6, 11)
#define EvtNetIP6_SetDefaultInterface       EventID (EventLevelOp,    EvtNetIP6, 12)
#define EvtNetIP6_UninitCore                EventID (EventLevelOp,    EvtNetIP6, 13)
#endif

/**
  \brief  Event on IP6 core initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_InitCore(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_InitCore, if_id, 0);
  }
#else
  #define EvrNetIP6_InitCore(if_id)
#endif

/**
  \brief  Event on IP6 receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetIP6_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetIP6_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on IP6 receive frame too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetIP6_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetIP6_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on IP6 invalid protocol version (Error)
  \param  if_id         network interface identifier
  \param  ip_version    IPv6 protocol version value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_InvalidIpVersion(uint16_t if_id, uint8_t ip_version) {
    EventRecord2 (EvtNetIP6_InvalidIpVersion, if_id, ip_version);
  }
#else
  #define EvrNetIP6_InvalidIpVersion(if_id, ip_version)
#endif

/**
  \brief  Event on IP6 display frame header (Detail)
  \param  ip6_header    pointer to IPv6 frame header of 40 bytes
                         - VerClass  (1 byte)
                         - ClassFlow (1 byte)
                         - Flow      (2 bytes)
                         - Len       (2 bytes)
                         - NextHdr   (1 byte)
                         - HopLim    (1 byte)
                         - SrcAddr   (16 bytes)
                         - DstAddr   (16 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_ShowFrameHeader(const void *ip6_header) {
    EventRecordData (EvtNetIP6_ShowFrameHeader, ip6_header, 40);
  }
#else
  #define EvrNetIP6_ShowFrameHeader(ip6_header)
#endif

/**
  \brief  Event on IP6 wrong destination address (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_WrongDestinationAddress(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetIP6_WrongDestinationAddress, &evr_buf, 20);
  }
#else
  #define EvrNetIP6_WrongDestinationAddress(if_id, ip6_addr)
#endif

/**
  \brief  Event on IP6 network interface not supported (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_InterfaceNotSupported(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_InterfaceNotSupported, if_id, 0);
  }
#else
  #define EvrNetIP6_InterfaceNotSupported(if_id)
#endif

/**
  \brief  Event on IP6 wrong multicast protocol type (Error)
  \param  if_id         network interface identifier
  \param  ip6_protocol  received protocol type
  \remark Only UDP multicast frames are allowed.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_WrongMulticastProtocol(uint16_t if_id, uint8_t ip6_protocol) {
    EventRecord2 (EvtNetIP6_WrongMulticastProtocol, if_id, ip6_protocol);
  }
#else
  #define EvrNetIP6_WrongMulticastProtocol(if_id, ip6_protocol)
#endif

/**
  \brief  Event on IP6 receive fragmented frame (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_FragmentationDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_FragmentationDisabled, if_id, 0);
  }
#else
  #define EvrNetIP6_FragmentationDisabled(if_id)
#endif

/**
  \brief  Event on IP6 send frame (Op)
  \param  if_id         network interface identifier
  \param  ip6_protocol  IPv6 protocol type
                         - 6:  TCP
                         - 17: UDP
                         - 58: ICMP6
  \param  length        frame length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_SendFrame(uint16_t if_id, uint8_t ip6_protocol, uint32_t length) {
    uint32_t val2 = (length << 16) | ip6_protocol;
    EventRecord2 (EvtNetIP6_SendFrame, if_id, val2);
  }
#else
  #define EvrNetIP6_SendFrame(if_id, ip6_protocol, length)
#endif

/**
  \brief  Event on IP6 unknown protocol type in sending frame (Error)
  \param  if_id         network interface identifier
  \param  ip6_protocol  unknown IPv6 protocol type value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_ProtocolUnknown(uint16_t if_id, uint8_t ip6_protocol) {
    EventRecord2 (EvtNetIP6_ProtocolUnknown, if_id, ip6_protocol);
  }
#else
  #define EvrNetIP6_ProtocolUnknown(if_id, ip6_protocol)
#endif

/**
  \brief  Event on IP6 source address not provided (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_SourceAddressNull(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_SourceAddressNull, if_id, 0);
  }
#else
  #define EvrNetIP6_SourceAddressNull(if_id)
#endif

/**
  \brief  Event on IP6 destination address not provided (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_DestinationAddressNull(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_DestinationAddressNull, if_id, 0);
  }
#else
  #define EvrNetIP6_DestinationAddressNull(if_id)
#endif

/**
  \brief  Event on IP6 set default interface for internet access (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_SetDefaultInterface(uint16_t if_id) {
    EventRecord2 (EvtNetIP6_SetDefaultInterface, if_id, 0);
  }
#else
  #define EvrNetIP6_SetDefaultInterface(if_id)
#endif

/**
  \brief  Event on IP6 core de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetIP6_UninitCore(void) {
    EventRecord2 (EvtNetIP6_UninitCore, 0, 0);
  }
#else
  #define EvrNetIP6_UninitCore()
#endif


// NetICMP6 event identifiers --------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetICMP6_ReceiveFrame            EventID (EventLevelOp,    EvtNetICMP6,  0)
#define EvtNetICMP6_ShowFrameHeader         EventID (EventLevelDetail,EvtNetICMP6, 25)
#define EvtNetICMP6_FrameTooShort           EventID (EventLevelError, EvtNetICMP6,  1)
#define EvtNetICMP6_ChecksumFailed          EventID (EventLevelError, EvtNetICMP6,  2)
#define EvtNetICMP6_EchoRequestWrongCode    EventID (EventLevelError, EvtNetICMP6,  3)
#define EvtNetICMP6_EchoRequestReceived     EventID (EventLevelOp,    EvtNetICMP6,  4)
#define EvtNetICMP6_EchoReplyDisabled       EventID (EventLevelOp,    EvtNetICMP6, 26) // End
#define EvtNetICMP6_SendEchoReply           EventID (EventLevelOp,    EvtNetICMP6,  5)
#define EvtNetICMP6_EchoReplyReceived       EventID (EventLevelOp,    EvtNetICMP6,  6)
#define EvtNetICMP6_EchoReplyWrongState     EventID (EventLevelError, EvtNetICMP6,  7)
#define EvtNetICMP6_EchoReplyWrongCode      EventID (EventLevelError, EvtNetICMP6,  8)
#define EvtNetICMP6_EchoReplyWrongIpAddress EventID (EventLevelError, EvtNetICMP6,  9)
#define EvtNetICMP6_EchoReplyWrongId        EventID (EventLevelError, EvtNetICMP6, 10)
#define EvtNetICMP6_EchoReplyWrongPayload   EventID (EventLevelError, EvtNetICMP6, 11)
#define EvtNetICMP6_MessageDiscarded        EventID (EventLevelOp,    EvtNetICMP6, 12)
#define EvtNetICMP6_WrongHopLimit           EventID (EventLevelError, EvtNetICMP6, 13)
#define EvtNetICMP6_WrongCode               EventID (EventLevelError, EvtNetICMP6, 14)
#define EvtNetICMP6_MessageTypeUnknown      EventID (EventLevelOp,    EvtNetICMP6, 15)
#define EvtNetICMP6_SendFrame               EventID (EventLevelOp,    EvtNetICMP6, 16)
/* ICMP6-Ping client events */
#define EvtNetICMP6_PingEcho                EventID (EventLevelAPI,   EvtNetICMP6, 17)
#define EvtNetICMP6_PingTargetNotValid      EventID (EventLevelError, EvtNetICMP6, 18)
#define EvtNetICMP6_PingDnsError            EventID (EventLevelError, EvtNetICMP6, 19)
#define EvtNetICMP6_PingInvalidParameter    EventID (EventLevelError, EvtNetICMP6, 20)
#define EvtNetICMP6_PingClientBusy          EventID (EventLevelError, EvtNetICMP6, 21)
#define EvtNetICMP6_PingSendRequest         EventID (EventLevelOp,    EvtNetICMP6, 22)
#define EvtNetICMP6_PingRetransmitRequest   EventID (EventLevelOp,    EvtNetICMP6, 23)
#define EvtNetICMP6_PingTimeout             EventID (EventLevelOp,    EvtNetICMP6, 24)
#endif

/**
  \brief  Event on ICMP6 receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetICMP6_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetICMP6_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on ICMP6 display send/receive frame header (Detail)
  \param  icmp6_header  pointer to ICMP6 frame header of 8 bytes
                         - Type    (1 byte)
                         - Code    (1 byte)
                         - Chksum  (2 bytes)
                         - EchoId  (2 bytes)
                         - EchoSeq (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_ShowFrameHeader(const void *icmp6_header) {
    EventRecordData (EvtNetICMP6_ShowFrameHeader, icmp6_header, 8);
  }
#else
  #define EvrNetICMP6_ShowFrameHeader(icmp6_header)
#endif

/**
  \brief  Event on ICMP6 receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetICMP6_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetICMP6_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on ICMP6 frame checksum check failed (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_ChecksumFailed(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetICMP6_ChecksumFailed, if_id, length);
  }
#else
  #define EvrNetICMP6_ChecksumFailed(if_id, length)
#endif

/**
  \brief  Event on ICMP6 wrong code in echo request received (Error)
  \param  if_id         network interface identifier
  \param  code          received ICMPv6 code
  \param  code_valid    valid ICMPv6 code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoRequestWrongCode(uint16_t if_id, uint8_t code, uint8_t code_valid) {
    uint32_t val2 = ((uint32_t)code_valid) | code;
    EventRecord2 (EvtNetICMP6_EchoRequestWrongCode, if_id, val2);
  }
#else
  #define EvrNetICMP6_EchoRequestWrongCode(if_id, code, code_valid)
#endif

/**
  \brief  Event on ICMP6 echo request received (Op)
  \param  if_id         network interface identifier
  \param  data_length   length of the payload data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoRequestReceived(uint16_t if_id, uint32_t data_length) {
    EventRecord2 (EvtNetICMP6_EchoRequestReceived, if_id, data_length);
  }
#else
  #define EvrNetICMP6_EchoRequestReceived(if_id, data_length)
#endif

/**
  \brief  Event on ICMP6 sending echo reply disabled (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyDisabled(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_EchoReplyDisabled, if_id, 0);
  }
#else
  #define EvrNetICMP6_EchoReplyDisabled(if_id)
#endif

/**
  \brief  Event on ICMP6 send echo reply (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_SendEchoReply(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_SendEchoReply, if_id, 0);
  }
#else
  #define EvrNetICMP6_SendEchoReply(if_id)
#endif

/**
  \brief  Event on ICMP6 echo reply received (Op)
  \param  if_id         network interface identifier
  \param  data_length   length of the payload data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyReceived(uint16_t if_id, uint32_t data_length) {
    EventRecord2 (EvtNetICMP6_EchoReplyReceived, if_id, data_length);
  }
#else
  #define EvrNetICMP6_EchoReplyReceived(if_id, data_length)
#endif

/**
  \brief  Event on ICMP6 echo reply received in wrong ping process state (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyWrongState(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_EchoReplyWrongState, if_id, 0);
  }
#else
  #define EvrNetICMP6_EchoReplyWrongState(if_id)
#endif

/**
  \brief  Event on ICMP6 wrong code in echo reply received (Error)
  \param  if_id         network interface identifier
  \param  code          received ICMPv6 code
  \param  code_valid    valid ICMPv6 code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyWrongCode(uint16_t if_id, uint8_t code, uint8_t code_valid) {
    uint32_t val2 = ((uint32_t)code_valid << 16) | code;
    EventRecord2 (EvtNetICMP6_EchoReplyWrongCode, if_id, val2);
  }
#else
  #define EvrNetICMP6_EchoReplyWrongCode(if_id, code, code_valid)
#endif

/**
  \brief  Event on ICMP6 wrong source IPv6 address of echo reply (Error)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyWrongIpAddress(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetICMP6_EchoReplyWrongIpAddress, &evr_buf, 20);
  }
#else
  #define EvrNetICMP6_EchoReplyWrongIpAddress(if_id, ip6_addr)
#endif

/**
  \brief  Event on ICMP6 wrong echo identifier value in echo reply (Error)
  \param  if_id         network interface identifier
  \param  eid           received echo identifier
  \param  eid_valid     valid echo identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyWrongId(uint16_t if_id, uint16_t eid, uint16_t eid_valid) {
    uint32_t val2 = ((uint32_t)eid_valid << 16) | eid;
    EventRecord2 (EvtNetICMP6_EchoReplyWrongId, if_id, val2);
  }
#else
  #define EvrNetICMP6_EchoReplyWrongId(if_id, eid, eid_valid)
#endif

/**
  \brief  Event on ICMP6 echo reply payload data invalid (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_EchoReplyWrongPayload(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_EchoReplyWrongPayload, if_id, 0);
  }
#else
  #define EvrNetICMP6_EchoReplyWrongPayload(if_id)
#endif

/**
  \brief  Event on ICMP6 router solicitation message discarded (Op)
  \param  if_id         network interface identifier
  \param  type          ICMPv6 message type
  \remark Hosts must silently discard RS messages [RFC4861 page 38]
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_MessageDiscarded(uint16_t if_id, uint8_t type) {
    EventRecord2 (EvtNetICMP6_MessageDiscarded, if_id, type);
  }
#else
  #define EvrNetICMP6_MessageDiscarded(if_id, type)
#endif

/**
  \brief  Event on ICMP6 wrong hop limit in discover message (Error)
  \param  if_id         network interface identifier
  \param  hop_limit     received hop limit
  \param  hop_valid     valid hop limit
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_WrongHopLimit(uint16_t if_id, uint8_t hop_limit, uint8_t hop_valid) {
    uint32_t val2 = ((uint32_t)hop_valid << 16) | hop_limit;
    EventRecord2 (EvtNetICMP6_WrongHopLimit, if_id, val2);
  }
#else
  #define EvrNetICMP6_WrongHopLimit(if_id, hop_limit, hop_valid)
#endif

/**
  \brief  Event on ICMP6 wrong code in discover message (Error)
  \param  if_id         network interface identifier
  \param  code          received ICMPv6 code
  \param  code_valid    valid ICMPv6 code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_WrongCode(uint16_t if_id, uint8_t code, uint8_t code_valid) {
    uint32_t val2 = ((uint32_t)code_valid << 16) | code;
    EventRecord2 (EvtNetICMP6_WrongCode, if_id, val2);
  }
#else
  #define EvrNetICMP6_WrongCode(if_id, code, code_valid)
#endif

/**
  \brief  Event on ICMP6 message type unknown (Op)
  \param  if_id         network interface identifier
  \param  type          ICMP6 message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_MessageTypeUnknown(uint16_t if_id, uint8_t type) {
    EventRecord2 (EvtNetICMP6_MessageTypeUnknown, if_id, type);
  }
#else
  #define EvrNetICMP6_MessageTypeUnknown(if_id, type)
#endif

/**
  \brief  Event on ICMP6 send frame (Op)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_SendFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetICMP6_SendFrame, if_id, length);
  }
#else
  #define EvrNetICMP6_SendFrame(if_id, length)
#endif

/**
  \brief  Event on ICMP6 ping echo request (API)
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingEcho(const uint8_t *ip6_addr) {
    EventRecordData (EvtNetICMP6_PingEcho, ip6_addr, 16);
  }
#else
  #define EvrNetICMP6_PingEcho(ip6_addr)
#endif

/**
  \brief  Event on ICMP6 ping target name not valid (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingTargetNotValid(void) {
    EventRecord2 (EvtNetICMP6_PingTargetNotValid, 0, 0);
  }
#else
  #define EvrNetICMP6_PingTargetNotValid()
#endif

/**
  \brief  Event on ICMP6 ping host name resolver error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingDnsError(void) {
    EventRecord2 (EvtNetICMP6_PingDnsError, 0, 0);
  }
#else
  #define EvrNetICMP6_PingDnsError()
#endif

/**
  \brief  Event on ICMP6 ping invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingInvalidParameter(void) {
    EventRecord2 (EvtNetICMP6_PingInvalidParameter, 0, 0);
  }
#else
  #define EvrNetICMP6_PingInvalidParameter()
#endif

/**
  \brief  Event on ICMP6 ping client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingClientBusy(void) {
    EventRecord2 (EvtNetICMP6_PingClientBusy, 0, 0);
  }
#else
  #define EvrNetICMP6_PingClientBusy()
#endif

/**
  \brief  Event on ICMP6 send echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingSendRequest(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_PingSendRequest, if_id, 0);
  }
#else
  #define EvrNetICMP6_PingSendRequest(if_id)
#endif

/**
  \brief  Event on ICMP6 retransmit echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingRetransmitRequest(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_PingRetransmitRequest, if_id, 0);
  }
#else
  #define EvrNetICMP6_PingRetransmitRequest(if_id)
#endif

/**
  \brief  Event on ICMP6 timeout on send echo request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetICMP6_PingTimeout(uint16_t if_id) {
    EventRecord2 (EvtNetICMP6_PingTimeout, if_id, 0);
  }
#else
  #define EvrNetICMP6_PingTimeout(if_id)
#endif


// NetDHCP6 event identifiers --------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetDHCP6_InitClient              EventID (EventLevelOp,    EvtNetDHCP6,  0)
#define EvtNetDHCP6_GetSocketFailed         EventID (EventLevelError, EvtNetDHCP6,  1)
#define EvtNetDHCP6_StartClient             EventID (EventLevelOp,    EvtNetDHCP6,  2)
#define EvtNetDHCP6_StopClient              EventID (EventLevelOp,    EvtNetDHCP6,  3)
#define EvtNetDHCP6_ClientState             EventID (EventLevelOp,    EvtNetDHCP6,  4)
#define EvtNetDHCP6_NextState               EventID (EventLevelOp,    EvtNetDHCP6,  5)
#define EvtNetDHCP6_NextStateDelay          EventID (EventLevelOp,    EvtNetDHCP6,  6)
#define EvtNetDHCP6_ChangeStateLinkDown     EventID (EventLevelOp,    EvtNetDHCP6,  7)
#define EvtNetDHCP6_NoAddressAvailable      EventID (EventLevelOp,    EvtNetDHCP6,  8)
#define EvtNetDHCP6_StateRetransmit         EventID (EventLevelOp,    EvtNetDHCP6,  9)
#define EvtNetDHCP6_StateTimeout            EventID (EventLevelOp,    EvtNetDHCP6, 10)
#define EvtNetDHCP6_ChangeStateOnTimeout    EventID (EventLevelOp,    EvtNetDHCP6, 11)
#define EvtNetDHCP6_T1Expired               EventID (EventLevelOp,    EvtNetDHCP6, 12)
#define EvtNetDHCP6_T2Expired               EventID (EventLevelOp,    EvtNetDHCP6, 13)
#define EvtNetDHCP6_SendDhcpMessage         EventID (EventLevelOp,    EvtNetDHCP6, 14)
#define EvtNetDHCP6_ReceiveFrame            EventID (EventLevelOp,    EvtNetDHCP6, 15)
#define EvtNetDHCP6_FrameTooShort           EventID (EventLevelError, EvtNetDHCP6, 16)
#define EvtNetDHCP6_WrongTransactionId      EventID (EventLevelError, EvtNetDHCP6, 17)
#define EvtNetDHCP6_MessageTypeUnknown      EventID (EventLevelError, EvtNetDHCP6, 18)
#define EvtNetDHCP6_ViewMessage             EventID (EventLevelOp,    EvtNetDHCP6, 19)
#define EvtNetDHCP6_InvalidMessageType      EventID (EventLevelOp,    EvtNetDHCP6, 20)
#define EvtNetDHCP6_WrongClientId           EventID (EventLevelOp,    EvtNetDHCP6, 21)
#define EvtNetDHCP6_ServerNotChosen         EventID (EventLevelOp,    EvtNetDHCP6, 22)
#define EvtNetDHCP6_StatusCode              EventID (EventLevelOp,    EvtNetDHCP6, 23)
#define EvtNetDHCP6_WrongServerId           EventID (EventLevelOp,    EvtNetDHCP6, 24)
#define EvtNetDHCP6_ViewDnsServers          EventID (EventLevelOp,    EvtNetDHCP6, 25)
#define EvtNetDHCP6_MissingServerId         EventID (EventLevelOp,    EvtNetDHCP6, 26)
#define EvtNetDHCP6_WrongIaid               EventID (EventLevelOp,    EvtNetDHCP6, 27)
#define EvtNetDHCP6_ViewIanaOffer           EventID (EventLevelOp,    EvtNetDHCP6, 28)
#define EvtNetDHCP6_OfferTimerCheckFailed   EventID (EventLevelError, EvtNetDHCP6, 29)
#define EvtNetDHCP6_UninitClient            EventID (EventLevelOp,    EvtNetDHCP6, 30)
#endif

/**
  \brief  Event on DHCP6 client initialize (Op)
  \param  if_id         network interface identifier
  \param  vclass        Vendor Class option enabled
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_InitClient(uint16_t if_id, uint8_t vclass) {
    EventRecord2 (EvtNetDHCP6_InitClient, if_id, vclass);
  }
#else
  #define EvrNetDHCP6_InitClient(if_id, vclass)
#endif

/**
  \brief  Event on DHCP6 failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_GetSocketFailed(void) {
    EventRecord2 (EvtNetDHCP6_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetDHCP6_GetSocketFailed()
#endif

/**
  \brief  Event on DHCP6 client start (Op)
  \param  if_id         network interface identifier
  \param  mode          client mode
                         - 0: Stateless mode
                         - 1: Stateful mode
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_StartClient(uint16_t if_id, uint32_t mode) {
    EventRecord2 (EvtNetDHCP6_StartClient, if_id, mode);
  }
#else
  #define EvrNetDHCP6_StartClient(if_id, mode)
#endif

/**
  \brief  Event on DHCP6 client stop (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_StopClient(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP6_StopClient, if_id, 0);
  }
#else
  #define EvrNetDHCP6_StopClient(if_id)
#endif

/**
  \brief  Event on DHCP6 indication of client state (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ClientState(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_ClientState, if_id, state);
  }
#else
  #define EvrNetDHCP6_ClientState(if_id, state)
#endif

/**
  \brief  Event on DHCP6 transition to next state (Op)
  \param  if_id         network interface identifier
  \param  state         next state

 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_NextState(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_NextState, if_id, state);
  }
#else
  #define EvrNetDHCP6_NextState(if_id, state)
#endif

/**
  \brief  Event on DHCP6 transition to next state with random delay (Op)
  \param  if_id         network interface identifier
  \param  state         next state
  \param  delay         start delay (in 100ms ticks)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_NextStateDelay(uint16_t if_id, uint8_t state, uint16_t delay) {
    uint32_t val2 = ((uint32_t)delay << 16) | state;
    EventRecord2 (EvtNetDHCP6_NextStateDelay, if_id, val2);
  }
#else
  #define EvrNetDHCP6_NextStateDelay(if_id, state, delay)
#endif

/**
  \brief  Event on DHCP6 change state on link-down (Op)
  \param  if_id         network interface identifier
  \param  state         next state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ChangeStateLinkDown(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_ChangeStateLinkDown, if_id, state);
  }
#else
  #define EvrNetDHCP6_ChangeStateLinkDown(if_id, state)
#endif

/**
  \brief  Event on DHCP6 receive status no address available (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_NoAddressAvailable(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_NoAddressAvailable, if_id, state);
  }
#else
  #define EvrNetDHCP6_NoAddressAvailable(if_id, state)
#endif

/**
  \brief  Event on DHCP6 retransmit request in current state (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_StateRetransmit(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_StateRetransmit, if_id, state);
  }
#else
  #define EvrNetDHCP6_StateRetransmit(if_id, state)
#endif

/**
  \brief  Event on DHCP6 timeout in current state (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_StateTimeout(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_StateTimeout, if_id, state);
  }
#else
  #define EvrNetDHCP6_StateTimeout(if_id, state)
#endif

/**
  \brief  Event on DHCP6 state timeout and transit to next state (Op)
  \param  if_id         network interface identifier
  \param  current       current state
  \param  next          next state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ChangeStateOnTimeout(uint16_t if_id, uint8_t current, uint8_t next) {
    uint32_t val2 = ((uint32_t)next << 16) | current;
    EventRecord2 (EvtNetDHCP6_ChangeStateOnTimeout, if_id, val2);
  }
#else
  #define EvrNetDHCP6_ChangeStateOnTimeout(if_id, current, next)
#endif

/**
  \brief  Event on DHCP6 timer T1 expired (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_T1Expired(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_T1Expired, if_id, state);
  }
#else
  #define EvrNetDHCP6_T1Expired(if_id, state)
#endif

/**
  \brief  Event on DHCP6 timer T2 expired (Op)
  \param  if_id         network interface identifier
  \param  state         current state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_T2Expired(uint16_t if_id, uint8_t state) {
    EventRecord2 (EvtNetDHCP6_T2Expired, if_id, state);
  }
#else
  #define EvrNetDHCP6_T2Expired(if_id, state)
#endif

/**
  \brief  Event on DHCP6 send message (Op)
  \param  if_id         network interface identifier
  \param  msg_type      message type
  \param  xid           transaction identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_SendDhcpMessage(uint16_t if_id, uint8_t msg_type, uint32_t xid) {
    uint32_t val1 = ((uint32_t)msg_type << 16) | if_id;
    EventRecord2 (EvtNetDHCP6_SendDhcpMessage, val1, xid);
  }
#else
  #define EvrNetDHCP6_SendDhcpMessage(if_id, msg_type, xid)
#endif

/**
  \brief  Event on DHCP6 receive frame (Op)
  \param  if_id         network interface identifier
  \param  length        message length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ReceiveFrame(uint16_t if_id, uint32_t length) {
    EventRecord2 (EvtNetDHCP6_ReceiveFrame, if_id, length);
  }
#else
  #define EvrNetDHCP6_ReceiveFrame(if_id, length)
#endif

/**
  \brief  Event on DHCP6 receive frame too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetDHCP6_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetDHCP6_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on DHCP6 receive wrong transaction identifier (Error)
  \param  if_id         network interface identifier
  \param  xid           received transaction identifier
  \param  xid_valid     valid transaction identifier
  \remark The size of transaction identifier (XID) is 3 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_WrongTransactionId(uint16_t if_id, uint32_t xid, uint32_t xid_valid) {
    EventRecord4 (EvtNetDHCP6_WrongTransactionId, if_id, xid, xid_valid, 0);
  }
#else
  #define EvrNetDHCP6_WrongTransactionId(if_id, xid, xid_valid)
#endif

/**
  \brief  Event on DHCP6 receive unknown message type (Error)
  \param  if_id         network interface identifier
  \param  msg_type      message type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_MessageTypeUnknown(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP6_MessageTypeUnknown, if_id, msg_type);
  }
#else
  #define EvrNetDHCP6_MessageTypeUnknown(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP6 display message information (Op)
  \param  if_id         network interface identifier
  \param  msg_type      message type
  \param  xid           transaction identifier
  \remark The size of transaction identifier (XID) is 3 bytes.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ViewMessage(uint16_t if_id, uint8_t msg_type, uint32_t xid) {
    uint32_t val1 = ((uint32_t)msg_type << 16) | if_id;
    EventRecord2 (EvtNetDHCP6_ViewMessage, val1, xid);
  }
#else
  #define EvrNetDHCP6_ViewMessage(if_id, msg_type, xid)
#endif

/**
  \brief  Event on DHCP6 receive invalid message type (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \param  msg_valid     valid message type
  \remark Silently discard other message types.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_InvalidMessageType(uint16_t if_id, uint8_t msg_type, uint8_t msg_valid) {
    uint32_t val2 = ((uint32_t)msg_valid << 16) | msg_type;
    EventRecord2 (EvtNetDHCP6_InvalidMessageType, if_id, val2);
  }
#else
  #define EvrNetDHCP6_InvalidMessageType(if_id, msg_type, msg_valid)
#endif

/**
  \brief  Event on DHCP6 receive wrong client identifier (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \remark Silently discard the message.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_WrongClientId(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP6_WrongClientId, if_id, msg_type);
  }
#else
  #define EvrNetDHCP6_WrongClientId(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP6 server not chosen in DISCOVER, REBIND state (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \remark Silently discard the message.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ServerNotChosen(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP6_ServerNotChosen, if_id, msg_type);
  }
#else
  #define EvrNetDHCP6_ServerNotChosen(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP6 receive option status code (Op)
  \param  if_id         network interface identifier
  \param  status        status code
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_StatusCode(uint16_t if_id, uint16_t status) {
    EventRecord2 (EvtNetDHCP6_StatusCode, if_id, status);
  }
#else
  #define EvrNetDHCP6_StatusCode(if_id, status)
#endif

/**
  \brief  Event on DHCP6 receive wrong server identifier (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \remark Silently discard the message.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_WrongServerId(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP6_WrongServerId, if_id, msg_type);
  }
#else
  #define EvrNetDHCP6_WrongServerId(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP6 display DNS server addresses (Op)
  \param  if_id         network interface identifier
  \param  dns_list      pointer to DNS server address list
  \remark Displays primary and secondary DNS server address.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ViewDnsServers(uint16_t if_id, const uint8_t *dns_list) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], dns_list, 32);
    EventRecordData (EvtNetDHCP6_ViewDnsServers, &evr_buf, 36);
  }
#else
  #define EvrNetDHCP6_ViewDnsServers(if_id, dns_list)
#endif

/**
  \brief  Event on DHCP6 missing server identifier option (Op)
  \param  if_id         network interface identifier
  \param  msg_type      received message type
  \remark Silently discard the message.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_MissingServerId(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetDHCP6_MissingServerId, if_id, msg_type);
  }
#else
  #define EvrNetDHCP6_MissingServerId(if_id, msg_type)
#endif

/**
  \brief  Event on DHCP6 receive wrong IAID (Op)
  \param  if_id         network interface identifier
  \param  iaid          received IAID value
  \param  iaid_valid    valid IAID value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_WrongIaid(uint16_t if_id, uint32_t iaid, uint32_t iaid_valid) {
    EventRecord4 (EvtNetDHCP6_WrongIaid, if_id, iaid, iaid_valid, 0);
  }
#else
  #define EvrNetDHCP6_WrongIaid(if_id, iaid, iaid_valid)
#endif

/**
  \brief  Event on DHCP6 display IANA offer (Op)
  \param  iana_offer    pointer to IANA structure of 32 bytes
                         - T1        (4 bytes)
                         - T2        (4 bytes)
                         - Addr     (16 bytes)
                         - PrefTime  (4 bytes)
                         - ValidTime (4 bytes)
  \remark IANA: Identity Association for non-temporary Address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_ViewIanaOffer(const void *iana_offer) {
    EventRecordData (EvtNetDHCP6_ViewIanaOffer, iana_offer, 32);
  }
#else
  #define EvrNetDHCP6_ViewIanaOffer(iana_offer)
#endif

/**
  \brief  Event on DHCP6 offer address timer check failed (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_OfferTimerCheckFailed(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP6_OfferTimerCheckFailed, if_id, 0);
  }
#else
  #define EvrNetDHCP6_OfferTimerCheckFailed(if_id)
#endif

/**
  \brief  Event on DHCP6 client de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDHCP6_UninitClient(uint16_t if_id) {
    EventRecord2 (EvtNetDHCP6_UninitClient, if_id, 0);
  }
#else
  #define EvrNetDHCP6_UninitClient(if_id)
#endif


// NetNDP event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetNDP_InitCache                 EventID (EventLevelOp,    EvtNetNDP,  0)
#define EvtNetNDP_FrameCheckFailed          EventID (EventLevelError, EvtNetNDP,  1)
#define EvtNetNDP_AddressNotLinkLocal       EventID (EventLevelOp,    EvtNetNDP,  2)
#define EvtNetNDP_RouterAdvertisement       EventID (EventLevelOp,    EvtNetNDP,  3)
#define EvtNetNDP_OptionMtu                 EventID (EventLevelOp,    EvtNetNDP,  4)
#define EvtNetNDP_OptionSourceMacAddress    EventID (EventLevelOp,    EvtNetNDP,  5)
#define EvtNetNDP_OptionPrefixInfo          EventID (EventLevelOp,    EvtNetNDP,  6)
#define EvtNetNDP_PrefixInfoNotValid        EventID (EventLevelOp,    EvtNetNDP,  7)
#define EvtNetNDP_WrongTargetAddress        EventID (EventLevelOp,    EvtNetNDP,  8)
#define EvtNetNDP_NeighborSolicitation      EventID (EventLevelOp,    EvtNetNDP,  9)
#define EvtNetNDP_ProbeRequest              EventID (EventLevelOp,    EvtNetNDP, 10)
#define EvtNetNDP_NeighborAdvertisement     EventID (EventLevelOp,    EvtNetNDP, 11)
#define EvtNetNDP_ProbeResponse             EventID (EventLevelOp,    EvtNetNDP, 12)
#define EvtNetNDP_WrongDestinationAddress   EventID (EventLevelOp,    EvtNetNDP, 13)
#define EvtNetNDP_SendReply                 EventID (EventLevelOp,    EvtNetNDP, 14)
#define EvtNetNDP_ViewFlags                 EventID (EventLevelOp,    EvtNetNDP, 15)
#define EvtNetNDP_SendRequest               EventID (EventLevelOp,    EvtNetNDP, 16)
#define EvtNetNDP_ViewTargetAddress         EventID (EventLevelOp,    EvtNetNDP, 17)
#define EvtNetNDP_OptionTargetMacAddress    EventID (EventLevelOp,    EvtNetNDP, 18)
#define EvtNetNDP_CacheEntryUpdate          EventID (EventLevelOp,    EvtNetNDP, 19)
#define EvtNetNDP_WrongResponse             EventID (EventLevelError, EvtNetNDP, 20)
#define EvtNetNDP_MakeLinkLocalAddress      EventID (EventLevelOp,    EvtNetNDP, 21)
#define EvtNetNDP_MakeTempAddress           EventID (EventLevelOp,    EvtNetNDP, 22)
#define EvtNetNDP_CacheIpRefreshed          EventID (EventLevelOp,    EvtNetNDP, 23)
#define EvtNetNDP_CacheAdd                  EventID (EventLevelOp,    EvtNetNDP, 24)
#define EvtNetNDP_RouterListFull            EventID (EventLevelOp,    EvtNetNDP, 25)
#define EvtNetNDP_CacheAllocFailed          EventID (EventLevelError, EvtNetNDP, 26)
#define EvtNetNDP_CacheEntryAdded           EventID (EventLevelOp,    EvtNetNDP, 27)
#define EvtNetNDP_CacheEntryDeleted         EventID (EventLevelOp,    EvtNetNDP, 28)
#define EvtNetNDP_CacheEntryRefreshed       EventID (EventLevelOp,    EvtNetNDP, 29)
#define EvtNetNDP_CacheFind                 EventID (EventLevelOp,    EvtNetNDP, 30)
#define EvtNetNDP_GatewayUnknown            EventID (EventLevelError, EvtNetNDP, 31)
#define EvtNetNDP_UsingGateway              EventID (EventLevelOp,    EvtNetNDP, 32)
#define EvtNetNDP_UnresolvedMacAddress      EventID (EventLevelError, EvtNetNDP, 33)
#define EvtNetNDP_EntryFound                EventID (EventLevelOp,    EvtNetNDP, 34)
#define EvtNetNDP_CacheIp                   EventID (EventLevelAPI,   EvtNetNDP, 35)
#define EvtNetNDP_CacheIpInvalidParameter   EventID (EventLevelError, EvtNetNDP, 36)
#define EvtNetNDP_GetIp                     EventID (EventLevelAPI,   EvtNetNDP, 37)
#define EvtNetNDP_GetIpInvalidParameter     EventID (EventLevelError, EvtNetNDP, 38)
#define EvtNetNDP_GetIpEntryNotFound        EventID (EventLevelOp,    EvtNetNDP, 39)
#define EvtNetNDP_GetMac                    EventID (EventLevelAPI,   EvtNetNDP, 40)
#define EvtNetNDP_GetMacInvalidParameter    EventID (EventLevelError, EvtNetNDP, 41)
#define EvtNetNDP_GetMacEntryNotFound       EventID (EventLevelOp,    EvtNetNDP, 42)
#define EvtNetNDP_Probe                     EventID (EventLevelAPI,   EvtNetNDP, 43)
#define EvtNetNDP_ProbeInvalidParameter     EventID (EventLevelError, EvtNetNDP, 44)
#define EvtNetNDP_ProbeBusy                 EventID (EventLevelError, EvtNetNDP, 45)
#define EvtNetNDP_ProbeRetransmit           EventID (EventLevelOp,    EvtNetNDP, 46)
#define EvtNetNDP_ProbeTimeout              EventID (EventLevelOp,    EvtNetNDP, 47)
#define EvtNetNDP_ClearCache                EventID (EventLevelAPI,   EvtNetNDP, 53)
#define EvtNetNDP_ClearCacheInvalidParam    EventID (EventLevelError, EvtNetNDP, 54)
#define EvtNetNDP_ClearCacheClientBusy      EventID (EventLevelError, EvtNetNDP, 55) // End
#define EvtNetNDP_CacheEntryTimeout         EventID (EventLevelOp,    EvtNetNDP, 48)
#define EvtNetNDP_EntryReleased             EventID (EventLevelOp,    EvtNetNDP, 49)
#define EvtNetNDP_ResolveEntry              EventID (EventLevelOp,    EvtNetNDP, 50)
#define EvtNetNDP_RefreshEntry              EventID (EventLevelOp,    EvtNetNDP, 51)
#define EvtNetNDP_UninitCache               EventID (EventLevelOp,    EvtNetNDP, 52)
#endif

/**
  \brief  Event on NDP cache initialize (Op)
  \param  if_id         network interface identifier
  \param  num_entries   number of entries available in NDP cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_InitCache(uint16_t if_id, uint32_t num_entries) {
    EventRecord2 (EvtNetNDP_InitCache, if_id, num_entries);
  }
#else
  #define EvrNetNDP_InitCache(if_id, num_entries)
#endif

/**
  \brief  Event on NDP discovery frame consistency check failed (Error)
  \param  if_id         network interface identifier
  \param  msg_type      discovery message type
                         - 134: Router Advertisement
                         - 135: Neighbor Solicitation
                         - 136: Neighbor Advertisement
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_FrameCheckFailed(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetNDP_FrameCheckFailed, if_id, msg_type);
  }
#else
  #define EvrNetNDP_FrameCheckFailed(if_id, msg_type)
#endif

/**
  \brief  Event on NDP receive address not link-local (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 source address
  \remark Nodes must silently discard RA messages if source IP not link-local.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_AddressNotLinkLocal(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_AddressNotLinkLocal, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_AddressNotLinkLocal(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP receive Router Advertisement (RA) message (Op)
  \param  if_id         network interface identifier
  \param  hop_limit     current hop limit
  \param  flags         advertisement flags
                         - 0x80: M-flag (Managed address configuration)
                         - 0x40: O-flag (Other configuration)
  \param  lifetime      router lifetime in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_RouterAdvertisement(uint16_t if_id,
                           uint8_t hop_limit, uint8_t flags, uint16_t lifetime) {
    evr_buf.u16[0] = lifetime;
    evr_buf.u8[2]  = hop_limit;
    evr_buf.u8[3]  = flags;
    EventRecord2 (EvtNetNDP_RouterAdvertisement, if_id, evr_buf.u32[0]);
  }
#else
  #define EvrNetNDP_RouterAdvertisement(if_id, hop_limit, flags, lifetime)
#endif

/**
  \brief  Event on NDP option Maximum transmission unit (MTU) (Op)
  \param  if_id         network interface identifier
  \param  mtu           maximum transmission unit
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_OptionMtu(uint16_t if_id, uint32_t mtu) {
    EventRecord2 (EvtNetNDP_OptionMtu, if_id, mtu);
  }
#else
  #define EvrNetNDP_OptionMtu(if_id, mtu)
#endif

/**
  \brief  Event on NDP option Source link-layer (MAC) address (Op)
  \param  if_id         network interface identifier
  \param  src_addr      source link-layer address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_OptionSourceMacAddress(uint16_t if_id, const uint8_t *src_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], src_addr, 6);
    EventRecordData (EvtNetNDP_OptionSourceMacAddress, &evr_buf, 8);
  }
#else
  #define EvrNetNDP_OptionSourceMacAddress(if_id, src_addr)
#endif

/**
  \brief  Event on NDP option address prefix info (Op)
  \param  opt_prefix    pointer to prefix info structure (32 bytes)
                         - Type:      1 byte
                         - Length:    1 byte
                         - PrefixLen: 1 byte
                         - Flags:     1 byte
                         - Valid:     4 bytes
                         - Preferred: 4 bytes
                         - Reserved:  4 bytes
                         - Prefix:   16 bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_OptionPrefixInfo(const uint8_t *opt_prefix) {
    EventRecordData (EvtNetNDP_OptionPrefixInfo, opt_prefix, 32);
  }
#else
  #define EvrNetNDP_OptionPrefixInfo(opt_prefix)
#endif

/**
  \brief  Event on NDP prefix information option not valid (Op)
  \remark Nodes must silently discard not valid RA messages.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_PrefixInfoNotValid(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_PrefixInfoNotValid, if_id, 0);
  }
#else
  #define EvrNetNDP_PrefixInfoNotValid(if_id)
#endif

/**
  \brief  Event on NDP receive wrong target address (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     pointer to target address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_WrongTargetAddress(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_WrongTargetAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_WrongTargetAddress(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP receive Neighbor Solicitation (NS) message (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     pointer to target address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_NeighborSolicitation(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_NeighborSolicitation, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_NeighborSolicitation(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP receive probe request (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeRequest(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ProbeRequest, if_id, 0);
  }
#else
  #define EvrNetNDP_ProbeRequest(if_id)
#endif

/**
  \brief  Event on NDP receive Neighbor Advertisement (NA) message (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     pointer to target address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_NeighborAdvertisement(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_NeighborAdvertisement, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_NeighborAdvertisement(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP receive probe response (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeResponse(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ProbeResponse, if_id, 0);
  }
#else
  #define EvrNetNDP_ProbeResponse(if_id)
#endif

/**
  \brief  Event on NDP receive wrong destination address (Op)
  \param  if_id         network interface identifier
  \param  dst_addr      pointer to destination address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_WrongDestinationAddress(uint16_t if_id, const uint8_t *dst_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], dst_addr, 16);
    EventRecordData (EvtNetNDP_WrongDestinationAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_WrongDestinationAddress(if_id, dst_addr)
#endif

/**
  \brief  Event on NDP send advertisement reply to solicitation message (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     pointer to target address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_SendReply(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_SendReply, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_SendReply(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP display advertisement message flags (Op)
  \param  if_id         network interface identifier
  \param  flags         message flags
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ViewFlags(uint16_t if_id, uint32_t flags) {
    EventRecord2 (EvtNetNDP_ViewFlags, if_id, flags);
  }
#else
  #define EvrNetNDP_ViewFlags(if_id, flags)
#endif

/**
  \brief  Event on NDP send solicitation request (Op)
  \param  if_id         network interface identifier
  \param  type          request type
                         - 133: Router Solicitation
                         - 135: Neighbor Solicitation
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_SendRequest(uint16_t if_id, uint32_t type) {
    if (type == 255) type = 135;
    EventRecord2 (EvtNetNDP_SendRequest, if_id, type);
  }
#else
  #define EvrNetNDP_SendRequest(if_id, type)
#endif

/**
  \brief  Event on NDP display target IPv6 address (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     pointer to target address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ViewTargetAddress(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_ViewTargetAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_ViewTargetAddress(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP option target link-layer (MAC) address (Op)
  \param  if_id         network interface identifier
  \param  targ_addr     target link-layer address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_OptionTargetMacAddress(uint16_t if_id, const uint8_t *targ_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], targ_addr, 16);
    EventRecordData (EvtNetNDP_OptionTargetMacAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_OptionTargetMacAddress(if_id, targ_addr)
#endif

/**
  \brief  Event on NDP cache entry update (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheEntryUpdate(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetNDP_CacheEntryUpdate, if_id, entry);
  }
#else
  #define EvrNetNDP_CacheEntryUpdate(if_id, entry)
#endif

/**
  \brief  Event on NDP wrong response received (Error)
  \param  if_id         network interface identifier
  \param  msg_type      discovery message type
                         - 136: Neighbor Advertisement
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_WrongResponse(uint16_t if_id, uint8_t msg_type) {
    EventRecord2 (EvtNetNDP_WrongResponse, if_id, msg_type);
  }
#else
  #define EvrNetNDP_WrongResponse(if_id, msg_type)
#endif

/**
  \brief  Event on NDP make link-local address (Op)
  \param  if_id         network interface identifier
  \param  local_addr    pointer to link-local address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_MakeLinkLocalAddress(uint16_t if_id, const uint8_t *local_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], local_addr, 16);
    EventRecordData (EvtNetNDP_MakeLinkLocalAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_MakeLinkLocalAddress(if_id, local_addr)
#endif

/**
  \brief  Event on NDP make temporary address (SLAAC) using prefix info (Op)
  \param  if_id         network interface identifier
  \param  temp_addr     pointer to temporary address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_MakeTempAddress(uint16_t if_id, const uint8_t *temp_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], temp_addr, 16);
    EventRecordData (EvtNetNDP_MakeTempAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_MakeTempAddress(if_id, temp_addr)
#endif

/**
  \brief  Event on NDP cached IP address refreshed (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheIpRefreshed(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_CacheIpRefreshed, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_CacheIpRefreshed(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP add entry to NDP cache (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
  \param  mac_addr      pointer to MAC address
  \param  type          cache entry type:
                         - 0x01: Host
                         - 0x02: Router
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheAdd(uint16_t if_id,
                    const uint8_t *ip6_addr, const uint8_t *mac_addr, uint8_t type) {
    evr_buf.u16[0] = if_id;
    evr_buf.u8[2]  = 0;
    evr_buf.u8[3]  = type;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    memcpy (&evr_buf.u8[20], mac_addr, 6);
    EventRecordData (EvtNetNDP_CacheAdd, &evr_buf, 26);
  }
#else
  #define EvrNetNDP_CacheAdd(if_id, ip6_addr, mac_addr, type)
#endif

/**
  \brief  Event on NDP cache add router list full (Op)
  \param  if_id         network interface identifier
  \param  max_count     maximum number of routers cached
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_RouterListFull(uint16_t if_id, uint32_t max_count) {
    EventRecord2 (EvtNetNDP_RouterListFull, if_id, max_count);
  }
#else
  #define EvrNetNDP_RouterListFull(if_id, max_count)
#endif

/**
  \brief  Event on NDP allocate free cache entry failed (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheAllocFailed(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_CacheAllocFailed, if_id, 0);
  }
#else
  #define EvrNetNDP_CacheAllocFailed(if_id)
#endif

/**
  \brief  Event on NDP cache entry added (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
  \param  timeout       entry timeout in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheEntryAdded(uint16_t if_id, uint32_t entry, uint32_t timeout) {
    uint32_t val1 = (entry << 16) | if_id;
    EventRecord2 (EvtNetNDP_CacheEntryAdded, val1, timeout);
  }
#else
  #define EvrNetNDP_CacheEntryAdded(if_id, entry, timeout)
#endif

/**
  \brief  Event on NDP cache entry deleted (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
  \param  ip6_addr      pointer to IPv6 address
  \param  type          cache entry type:
                         - 0x01: Host
                         - 0x02: Router
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheEntryDeleted(uint16_t if_id,
                         uint32_t entry, const uint8_t *ip6_addr, uint8_t type) {
    evr_buf.u16[0] = if_id;
    evr_buf.u8[2]  = entry & 0xFF;
    evr_buf.u8[3]  = type  & 0xFF;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    EventRecordData (EvtNetNDP_CacheEntryDeleted, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_CacheEntryDeleted(if_id, entry, ip6_addr, type)
#endif

/**
  \brief  Event on NDP cache entry refreshed (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheEntryRefreshed(uint16_t if_id,
                         uint32_t entry, const uint8_t *ip6_addr) {
    evr_buf.u16[0] = if_id;
    evr_buf.u8[2]  = entry & 0xFF;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    EventRecordData (EvtNetNDP_CacheEntryRefreshed, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_CacheEntryRefreshed(if_id, entry, ip6_addr)
#endif

/**
  \brief  Event on NDP find IP address in cache (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheFind(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_CacheFind, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_CacheFind(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP cache add gateway unknown (Error)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GatewayUnknown(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_GatewayUnknown, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_GatewayUnknown(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP using gateway to access external address (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a gateway
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_UsingGateway(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_UsingGateway, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_UsingGateway(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP unresolved MAC address in cache (Error)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to requested IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_UnresolvedMacAddress(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_UnresolvedMacAddress, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_UnresolvedMacAddress(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP cache entry found (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_EntryFound(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetNDP_EntryFound, if_id, entry);
  }
#else
  #define EvrNetNDP_EntryFound(if_id, entry)
#endif

/**
  \brief  Event on netNDP_CacheIP execute (API)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to requested IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheIp(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    EventRecordData (EvtNetNDP_CacheIp, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_CacheIp(if_id, ip6_addr)
#endif

/**
  \brief  Event on netNDP_CacheIP invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheIpInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_CacheIpInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetNDP_CacheIpInvalidParameter(if_id)
#endif

/**
  \brief  Event on netNDP_GetIP resolving success (API)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to requested MAC address
  \param  ip6_addr      pointer to resolved IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetIp(uint16_t if_id, const uint8_t *mac_addr, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    memcpy (&evr_buf.u8[20], mac_addr, 6);
    EventRecordData (EvtNetNDP_GetIp, &evr_buf, 26);
  }
#else
  #define EvrNetNDP_GetIp(if_id, mac_addr, ip6_addr)
#endif

/**
  \brief  Event on netNDP_GetIP invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetIpInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_GetIpInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetNDP_GetIpInvalidParameter(if_id)
#endif

/**
  \brief  Event on netNDP_GetIP entry with MAC address not found in cache (Op)
  \param  if_id         network interface identifier
  \param  mac_addr      pointer to requested MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetIpEntryNotFound(uint16_t if_id, const uint8_t *mac_addr) {
    evr_buf.u16[0] = if_id;
    memcpy (&evr_buf.u16[1], mac_addr, 6);
    EventRecordData (EvtNetNDP_GetIpEntryNotFound, &evr_buf, 8);
  }
#else
  #define EvrNetNDP_GetIpEntryNotFound(if_id, mac_addr)
#endif

/**
  \brief  Event on netNDP_GetMAC resolving success (API)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to requested IPv6 address
  \param  mac_addr      pointer to resolved MAC address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetMac(uint16_t if_id, const uint8_t *ip6_addr, const uint8_t *mac_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    memcpy (&evr_buf.u8[20], mac_addr, 6);
    EventRecordData (EvtNetNDP_GetMac, &evr_buf, 26);
  }
#else
  #define EvrNetNDP_GetMac(if_id, ip6_addr, mac_addr)
#endif

/**
  \brief  Event on netNDP_GetMAC invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetMacInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_GetMacInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetNDP_GetMacInvalidParameter(if_id)
#endif

/**
  \brief  Event on netNDP_GetMAC entry with IP address not found in cache (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to requested IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_GetMacEntryNotFound(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_GetMacEntryNotFound, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_GetMacEntryNotFound(if_id, ip6_addr)
#endif

/**
  \brief  Event on netNDP_Probe execute (API)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to requested IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_Probe(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_Probe, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_Probe(if_id, ip6_addr)
#endif

/**
  \brief  Event on netNDP_Probe invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeInvalidParameter(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ProbeInvalidParameter, if_id, 0);
  }
#else
  #define EvrNetNDP_ProbeInvalidParameter(if_id)
#endif

/**
  \brief  Event on NDP probe process busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeBusy(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ProbeBusy, if_id, 0);
  }
#else
  #define EvrNetNDP_ProbeBusy(if_id)
#endif

/**
  \brief  Event on NDP probe retransmit (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of probed host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeRetransmit(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_ProbeRetransmit, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_ProbeRetransmit(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP probe timeout (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of probed host
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ProbeTimeout(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetNDP_ProbeTimeout, &evr_buf, 20);
  }
#else
  #define EvrNetNDP_ProbeTimeout(if_id, ip6_addr)
#endif

/**
  \brief  Event on NDP clear cache request (API)
  \param  if_id         network interface identifier
  \param  deleted       number of entries deleted from cache
  \param  available     number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ClearCache(uint16_t if_id, uint32_t deleted, uint32_t available) {
    uint32_t val2 = (available << 16) | deleted;
    EventRecord2 (EvtNetNDP_ClearCache, if_id, val2);
  }
#else
  #define EvrNetNDP_ClearCache(if_id, deleted, available)
#endif

/**
  \brief  Event on netNDP_ClearCache invalid parameter (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ClearCacheInvalidParam(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ClearCacheInvalidParam, if_id, 0);
  }
#else
  #define EvrNetNDP_ClearCacheInvalidParam(if_id)
#endif

/**
  \brief  Event on NDP clear cache client busy (Error)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ClearCacheClientBusy(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_ClearCacheClientBusy, if_id, 0);
  }
#else
  #define EvrNetNDP_ClearCacheClientBusy(if_id)
#endif

/**
  \brief  Event on NDP cache entry timeout (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
  \param  ip6_addr      pointer to IPv6 address
  \param  mac_addr      pointer to MAC address
  \param  type          cache entry type:
                         - 0x01: Host
                         - 0x02: Router
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_CacheEntryTimeout(uint16_t if_id, uint32_t entry,
                    const uint8_t *ip6_addr, const uint8_t *mac_addr, uint32_t type) {
    evr_buf.u16[0] = if_id;
    evr_buf.u8[2]  = entry & 0xFF;
    evr_buf.u8[3]  = type  & 0xFF;
    memcpy (&evr_buf.u8[4], ip6_addr, 16);
    memcpy (&evr_buf.u8[20], mac_addr, 6);
    EventRecordData (EvtNetNDP_CacheEntryTimeout, &evr_buf, 26);
  }
#else
  #define EvrNetNDP_CacheEntryTimeout(if_id, entry, type, ip6_addr, mac_addr)
#endif

/**
  \brief  Event on NDP cache entry released (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_EntryReleased(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetNDP_EntryReleased, if_id, entry);
  }
#else
  #define EvrNetNDP_EntryReleased(if_id, entry)
#endif

/**
  \brief  Event on resolve NDP cache entry (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_ResolveEntry(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetNDP_ResolveEntry, if_id, entry);
  }
#else
  #define EvrNetNDP_ResolveEntry(if_id, entry)
#endif

/**
  \brief  Event on refresh NDP cache entry (Op)
  \param  if_id         network interface identifier
  \param  entry         cache entry number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_RefreshEntry(uint16_t if_id, uint32_t entry) {
    EventRecord2 (EvtNetNDP_RefreshEntry, if_id, entry);
  }
#else
  #define EvrNetNDP_RefreshEntry(if_id, entry)
#endif

/**
  \brief  Event on NDP cache de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetNDP_UninitCache(uint16_t if_id) {
    EventRecord2 (EvtNetNDP_UninitCache, if_id, 0);
  }
#else
  #define EvrNetNDP_UninitCache(if_id)
#endif


// NetMLD event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetMLD_InitNode                  EventID (EventLevelOp,    EvtNetMLD,  0)
#define EvtNetMLD_Join                      EventID (EventLevelAPI,   EvtNetMLD,  1)
#define EvtNetMLD_AlreadyInGroup            EventID (EventLevelOp,    EvtNetMLD,  2)
#define EvtNetMLD_NoFreeEntries             EventID (EventLevelError, EvtNetMLD,  3)
#define EvtNetMLD_SendReport                EventID (EventLevelOp,    EvtNetMLD,  4)
#define EvtNetMLD_Leave                     EventID (EventLevelAPI,   EvtNetMLD,  5)
#define EvtNetMLD_NotInGroup                EventID (EventLevelError, EvtNetMLD,  6)
#define EvtNetMLD_SendLeave                 EventID (EventLevelOp,    EvtNetMLD,  7)
#define EvtNetMLD_FrameTooShort             EventID (EventLevelError, EvtNetMLD,  8)
#define EvtNetMLD_AddressSpecificQuery      EventID (EventLevelOp,    EvtNetMLD,  9)
#define EvtNetMLD_DestAddressWrong          EventID (EventLevelError, EvtNetMLD, 10)
#define EvtNetMLD_DelayedReportScheduled    EventID (EventLevelOp,    EvtNetMLD, 11)
#define EvtNetMLD_GeneralQuery              EventID (EventLevelOp,    EvtNetMLD, 12)
#define EvtNetMLD_MaxTimeForReport          EventID (EventLevelOp,    EvtNetMLD, 13)
#define EvtNetMLD_GroupReportsScheduled     EventID (EventLevelOp,    EvtNetMLD, 14)
#define EvtNetMLD_NoReportScheduled         EventID (EventLevelOp,    EvtNetMLD, 15)
#define EvtNetMLD_ReportReceived            EventID (EventLevelOp,    EvtNetMLD, 16)
#define EvtNetMLD_OwnReportCanceled         EventID (EventLevelOp,    EvtNetMLD, 17)
#define EvtNetMLD_SendDelayedReport         EventID (EventLevelOp,    EvtNetMLD, 18)
#define EvtNetMLD_UninitNode                EventID (EventLevelOp,    EvtNetMLD, 19)
#endif

/**
  \brief  Event on MLD node initialize (Op)
  \param  if_id         network interface identifier
  \param  num_entries   number of entries available in membership table
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_InitNode(uint16_t if_id, uint32_t num_entries) {
    EventRecord2 (EvtNetMLD_InitNode, if_id, num_entries);
  }
#else
  #define EvrNetMLD_InitNode(if_id, num_entries)
#endif

/**
  \brief  Event on MLD join group membership (API)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_Join(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_Join, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_Join(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD join already a member (Op)
  \param  if_id         network interface identifier
  \param  group_id      group membership identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_AlreadyInGroup(uint16_t if_id, uint8_t group_id) {
    EventRecord2 (EvtNetMLD_AlreadyInGroup, if_id, group_id);
  }
#else
  #define EvrNetMLD_AlreadyInGroup(if_id, group_id)
#endif

/**
  \brief  Event on MLD join no free entries available (Error)
  \param  if_id         network interface identifier
  \param  num_used      number of used membership table entries
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_NoFreeEntries(uint16_t if_id, uint8_t num_used) {
    EventRecord2 (EvtNetMLD_NoFreeEntries, if_id, num_used);
  }
#else
  #define EvrNetMLD_NoFreeEntries(if_id, num_used)
#endif

/**
  \brief  Event on MLD send report message (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_SendReport(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_SendReport, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_SendReport(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD leave group membership (API)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_Leave(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_Leave, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_Leave(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD leave not a member (Error)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_NotInGroup(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_NotInGroup, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_NotInGroup(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD send leave message (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address of a group
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_SendLeave(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_SendLeave, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_SendLeave(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD receive frame is too short (Error)
  \param  if_id         network interface identifier
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_FrameTooShort(uint16_t if_id, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetMLD_FrameTooShort, if_id, val2);
  }
#else
  #define EvrNetMLD_FrameTooShort(if_id, length, min_length)
#endif

/**
  \brief  Event on MLD address specific query (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_AddressSpecificQuery(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_AddressSpecificQuery, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_AddressSpecificQuery(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD destination address wrong (Error)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_DestAddressWrong(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_DestAddressWrong, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_DestAddressWrong(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD report message delayed (Op)
  \param  if_id         network interface identifier
  \param  max_time      maximum delay time in ms
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_DelayedReportScheduled(uint16_t if_id, uint16_t max_time) {
    EventRecord2 (EvtNetMLD_DelayedReportScheduled, if_id, max_time);
  }
#else
  #define EvrNetMLD_DelayedReportScheduled(if_id, max_time)
#endif

/**
  \brief  Event on MLD general query (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_GeneralQuery(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_GeneralQuery, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_GeneralQuery(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD maximum time for report messages (Op)
  \param  if_id         network interface identifier
  \param  max_time      maximum delay time in ms
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_MaxTimeForReport(uint16_t if_id, uint16_t max_time) {
    EventRecord2 (EvtNetMLD_MaxTimeForReport, if_id, max_time);
  }
#else
  #define EvrNetMLD_MaxTimeForReport(if_id, max_time)
#endif

/**
  \brief  Event on MLD scheduled reports for all active groups (Op)
  \param  if_id         network interface identifier
  \param  n_reports     number of scheduled reports
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_GroupReportsScheduled(uint16_t if_id, uint32_t n_reports) {
    EventRecord2 (EvtNetMLD_GroupReportsScheduled, if_id, n_reports);
  }
#else
  #define EvrNetMLD_GroupReportsScheduled(if_id, n_reports)
#endif

/**
  \brief  Event on MLD no report scheduled as no active group (Op)
  \param  if_id         network interface identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_NoReportScheduled(uint16_t if_id) {
    EventRecord2 (EvtNetMLD_NoReportScheduled, if_id, 0);
  }
#else
  #define EvrNetMLD_NoReportScheduled(if_id)
#endif

/**
  \brief  Event on MLD received report message (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv6 address
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_ReportReceived(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_ReportReceived, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_ReportReceived(if_id, ip6_addr)
#endif

/**
  \brief  Event on own report canceled (Op)
  \param  if_id         network interface identifier
  \param  group_id      group membership identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_OwnReportCanceled(uint16_t if_id, uint8_t group_id) {
    EventRecord2 (EvtNetMLD_OwnReportCanceled, if_id, group_id);
  }
#else
  #define EvrNetMLD_OwnReportCanceled(if_id, group_id)
#endif

/**
  \brief  Event on MLD send delayed report message (Op)
  \param  if_id         network interface identifier
  \param  ip6_addr      pointer to IPv4 address of a group
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_SendDelayedReport(uint16_t if_id, const uint8_t *ip6_addr) {
    evr_buf.u32[0] = if_id;
    memcpy (&evr_buf.u32[1], ip6_addr, 16);
    EventRecordData (EvtNetMLD_SendDelayedReport, &evr_buf, 20);
  }
#else
  #define EvrNetMLD_SendDelayedReport(if_id, ip6_addr)
#endif

/**
  \brief  Event on MLD node de-initialize (Op)
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetMLD_UninitNode(uint16_t if_id) {
    EventRecord2 (EvtNetMLD_UninitNode, if_id, 0);
  }
#else
  #define EvrNetMLD_UninitNode(if_id)
#endif


// NetUDP event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetUDP_InitSockets               EventID (EventLevelOp,    EvtNetUDP,  0)
#define EvtNetUDP_GetSocket                 EventID (EventLevelOp,    EvtNetUDP,  1)
#define EvtNetUDP_GetSocketInvalidParameter EventID (EventLevelError, EvtNetUDP,  2)
#define EvtNetUDP_NoSocketAvailable         EventID (EventLevelError, EvtNetUDP,  3)
#define EvtNetUDP_ReleaseSocket             EventID (EventLevelOp,    EvtNetUDP,  4)
#define EvtNetUDP_ReleaseSocketNotValid     EventID (EventLevelError, EvtNetUDP,  5)
#define EvtNetUDP_ReleaseSocketWrongState   EventID (EventLevelError, EvtNetUDP,  6)
#define EvtNetUDP_OpenSocket                EventID (EventLevelOp,    EvtNetUDP,  7)
#define EvtNetUDP_OpenSocketNotValid        EventID (EventLevelError, EvtNetUDP,  8)
#define EvtNetUDP_OpenSocketWrongState      EventID (EventLevelError, EvtNetUDP,  9)
#define EvtNetUDP_AssignLocalPort           EventID (EventLevelOp,    EvtNetUDP, 10)
#define EvtNetUDP_OpenLocalPortInUse        EventID (EventLevelError, EvtNetUDP, 11)
#define EvtNetUDP_CloseSocket               EventID (EventLevelOp,    EvtNetUDP, 12)
#define EvtNetUDP_CloseSocketNotValid       EventID (EventLevelError, EvtNetUDP, 13)
#define EvtNetUDP_SetOptionSocket           EventID (EventLevelOp,    EvtNetUDP, 14)
#define EvtNetUDP_SetOptionSocketNotValid   EventID (EventLevelError, EvtNetUDP, 15)
#define EvtNetUDP_SetOptionSocketWrongState EventID (EventLevelError, EvtNetUDP, 16)
#define EvtNetUDP_SetOptionTos              EventID (EventLevelDetail,EvtNetUDP, 17)
#define EvtNetUDP_SetOptionTtl              EventID (EventLevelDetail,EvtNetUDP, 18)
#define EvtNetUDP_SetOptionTclass           EventID (EventLevelDetail,EvtNetUDP, 19)
#define EvtNetUDP_SetOptionHopLimit         EventID (EventLevelDetail,EvtNetUDP, 20)
#define EvtNetUDP_SetOptionChecksum         EventID (EventLevelDetail,EvtNetUDP, 21)
#define EvtNetUDP_SetOptionInterface        EventID (EventLevelDetail,EvtNetUDP, 42) // End
#define EvtNetUDP_SetOptionWrongOption      EventID (EventLevelError, EvtNetUDP, 22)
#define EvtNetUDP_SetOptionWrongValue       EventID (EventLevelError, EvtNetUDP, 23)
#define EvtNetUDP_GetBufferFailed           EventID (EventLevelError, EvtNetUDP, 24)
#define EvtNetUDP_SendFrame                 EventID (EventLevelOp,    EvtNetUDP, 25)
#define EvtNetUDP_SendBufferInvalid         EventID (EventLevelError, EvtNetUDP, 26)
#define EvtNetUDP_SendSocketNotValid        EventID (EventLevelError, EvtNetUDP, 27)
#define EvtNetUDP_SendAddressUnspecified    EventID (EventLevelError, EvtNetUDP, 28)
#define EvtNetUDP_SendPortUndefined         EventID (EventLevelError, EvtNetUDP, 29)
#define EvtNetUDP_SendSocketNotOpen         EventID (EventLevelError, EvtNetUDP, 30)
#define EvtNetUDP_SendZeroLengthFrame       EventID (EventLevelOp,    EvtNetUDP, 31)
#define EvtNetUDP_SendNoRouteFound          EventID (EventLevelError, EvtNetUDP, 41)
#define EvtNetUDP_SendSizeTruncated         EventID (EventLevelOp,    EvtNetUDP, 32)
#define EvtNetUDP_ShowFrameHeader           EventID (EventLevelDetail,EvtNetUDP, 33)
#define EvtNetUDP_ReceiveFrame              EventID (EventLevelOp,    EvtNetUDP, 34)
#define EvtNetUDP_FrameTooShort             EventID (EventLevelError, EvtNetUDP, 35)
#define EvtNetUDP_MapFrameToSocket          EventID (EventLevelOp,    EvtNetUDP, 36)
#define EvtNetUDP_FrameNotMapped            EventID (EventLevelOp,    EvtNetUDP, 37)
#define EvtNetUDP_LinkLayerAddressed        EventID (EventLevelError, EvtNetUDP, 38)
#define EvtNetUDP_ChecksumFailed            EventID (EventLevelError, EvtNetUDP, 39)
#define EvtNetUDP_UninitSockets             EventID (EventLevelOp,    EvtNetUDP, 40)
#endif

/**
  \brief  Event on UDP socket initialize (Op)
  \param  num_sockets   number of available UDP sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_InitSockets(uint32_t num_sockets) {
    EventRecord2 (EvtNetUDP_InitSockets, num_sockets, 0);
  }
#else
  #define EvrNetUDP_InitSockets(num_sockets)
#endif

/**
  \brief  Event on UDP allocate a free socket (Op)
  \param  socket        allocated socket handle
  \param  num_socks     number of available UDP sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_GetSocket(int32_t socket, uint32_t num_socks) {
    EventRecord2 (EvtNetUDP_GetSocket, (uint32_t)socket, num_socks);
  }
#else
  #define EvrNetUDP_GetSocket(socket,num_socks)
#endif

/**
  \brief  Event on UDP get socket failed, invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_GetSocketInvalidParameter(void) {
    EventRecord2 (EvtNetUDP_GetSocketInvalidParameter, 0, 0);
  }
#else
  #define EvrNetUDP_GetSocketInvalidParameter()
#endif

/**
  \brief  Event on UDP get socket failed, no free socket available (Error)
  \param  num_socks     number of available UDP sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_NoSocketAvailable(uint32_t num_socks) {
    EventRecord2 (EvtNetUDP_NoSocketAvailable, num_socks, 0);
  }
#else
  #define EvrNetUDP_NoSocketAvailable(num_socks)
#endif

/**
  \brief  Event on UDP release socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ReleaseSocket(int32_t socket) {
    EventRecord2 (EvtNetUDP_ReleaseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_ReleaseSocket(socket)
#endif

/**
  \brief  Event on UDP release failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ReleaseSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetUDP_ReleaseSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_ReleaseSocketNotValid(socket)
#endif

/**
  \brief  Event on UDP release failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ReleaseSocketWrongState(int32_t socket, uint8_t state) {
    EventRecord2 (EvtNetUDP_ReleaseSocketWrongState, (uint32_t)socket, state);
  }
#else
  #define EvrNetUDP_ReleaseSocketWrongState(socket, state)
#endif

/**
  \brief  Event on UDP open socket for communication (Op)
  \param  socket        socket handle
  \param  port          local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_OpenSocket(int32_t socket, uint16_t port) {
    EventRecord2 (EvtNetUDP_OpenSocket, (uint32_t)socket, port);
  }
#else
  #define EvrNetUDP_OpenSocket(socket, port)
#endif

/**
  \brief  Event on UDP open failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_OpenSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetUDP_OpenSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_OpenSocketNotValid(socket)
#endif

/**
  \brief  Event on UDP open socket failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_OpenSocketWrongState(int32_t socket, uint8_t state) {
    EventRecord2 (EvtNetUDP_OpenSocketWrongState, (uint32_t)socket, state);
  }
#else
  #define EvrNetUDP_OpenSocketWrongState(socket, state)
#endif

/**
  \brief  Event on UDP local port assigned in open socket request (Op)
  \param  socket        socket handle
  \param  local_port    assigned local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_AssignLocalPort(int32_t socket, uint16_t local_port) {
    EventRecord2 (EvtNetUDP_AssignLocalPort, (uint32_t)socket, local_port);
  }
#else
  #define EvrNetUDP_AssignLocalPort(socket, local_port)
#endif

/**
  \brief  Event on UDP open socket failed, requested port already used (Error)
  \param  socket        socket handle
  \param  port          local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_OpenLocalPortInUse(int32_t socket, uint16_t port) {
    EventRecord2 (EvtNetUDP_OpenLocalPortInUse, (uint32_t)socket, port);
  }
#else
  #define EvrNetUDP_OpenLocalPortInUse(socket, port)
#endif

/**
  \brief  Event on UDP close socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_CloseSocket(int32_t socket) {
    EventRecord2 (EvtNetUDP_CloseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_CloseSocket(socket)
#endif

/**
  \brief  Event on UDP close socket failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_CloseSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetUDP_CloseSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_CloseSocketNotValid(socket)
#endif

/**
  \brief  Event on UDP set socket option (Op)
  \param  socket        socket handle
  \param  option        interface option as specified by \ref netUDP_Option
  \param  val           option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionSocket(int32_t socket, int32_t option, uint32_t val) {
    uint32_t val2 = (val << 8) | (option & 0xFF);
    EventRecord2 (EvtNetUDP_SetOptionSocket, (uint32_t)socket, val2);
  }
#else
  #define EvrNetUDP_SetOptionSocket(socket, option, val)
#endif

/**
  \brief  Event on UDP set socket option failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetUDP_SetOptionSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SetOptionSocketNotValid(socket)
#endif

/**
  \brief  Event on UDP set socket option failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionSocketWrongState(int32_t socket, uint8_t state) {
    EventRecord2 (EvtNetUDP_SetOptionSocketWrongState, (uint32_t)socket, state);
  }
#else
  #define EvrNetUDP_SetOptionSocketWrongState(socket, state)
#endif

/**
  \brief  Event on UDP set socket option Type Of Service for IPv4 (Detail)
  \param  socket        socket handle
  \param  ip4_tos       type of service value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionTos(int32_t socket, uint8_t ip4_tos) {
    EventRecord2 (EvtNetUDP_SetOptionTos, (uint32_t)socket, ip4_tos);
  }
#else
  #define EvrNetUDP_SetOptionTos(socket, ip4_tos)
#endif

/**
  \brief  Event on UDP set socket option Time To Live for IPv4 (Detail)
  \param  socket        socket handle
  \param  ip4_ttl       time to live value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionTtl(int32_t socket, uint8_t ip4_ttl) {
    EventRecord2 (EvtNetUDP_SetOptionTtl, (uint32_t)socket, ip4_ttl);
  }
#else
  #define EvrNetUDP_SetOptionTtl(socket, ip4_ttl)
#endif

/**
  \brief  Event on UDP set socket option Traffic Class for IPv6 (Detail)
  \param  socket        socket handle
  \param  ip6_tclass    traffic class value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionTclass(int32_t socket, uint8_t ip6_tclass) {
    EventRecord2 (EvtNetUDP_SetOptionTclass, (uint32_t)socket, ip6_tclass);
  }
#else
  #define EvrNetUDP_SetOptionTclass(socket, ip6_tclass)
#endif

/**
  \brief  Event on UDP set socket option Hop Limit for IPv6 (Detail)
  \param  socket        socket handle
  \param  ip6_hoplimit  hop limit value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionHopLimit(int32_t socket, uint8_t ip6_hoplimit) {
    EventRecord2 (EvtNetUDP_SetOptionHopLimit, (uint32_t)socket, ip6_hoplimit);
  }
#else
  #define EvrNetUDP_SetOptionHopLimit(socket, ip6_hoplimit)
#endif

/**
  \brief  Event on UDP set socket option checksum calculation options (Detail)
  \param  socket        socket handle
  \param  checksum      checksum calculation options bitmap
                         - 0x01: calculate checksum for send frames
                         - 0x02: verify checksum for received frames
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionChecksum(int32_t socket, uint32_t checksum) {
    EventRecord2 (EvtNetUDP_SetOptionChecksum, (uint32_t)socket, checksum);
  }
#else
  #define EvrNetUDP_SetOptionChecksum(socket, checksum)
#endif

/**
  \brief  Event on UDP set socket option broadcast interface (Detail)
  \param  socket        socket handle
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionInterface(int32_t socket, uint16_t if_id) {
    EventRecord2 (EvtNetUDP_SetOptionInterface, (uint32_t)socket, if_id);
  }
#else
  #define EvrNetUDP_SetOptionInterface(socket, if_id)
#endif

/**
  \brief  Event on UDP wrong set socket option (Error)
  \param  socket        socket handle
  \param  udp_option    wrong set option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionWrongOption(int32_t socket, int32_t udp_option) {
    EventRecord2 (EvtNetUDP_SetOptionWrongOption, (uint32_t)socket, (uint32_t)udp_option);
  }
#else
  #define EvrNetUDP_SetOptionWrongOption(socket, udp_option)
#endif

/**
  \brief  Event on UDP wrong value for set socket option (Error)
  \param  socket        socket handle
  \param  opt_value     wrong value for set option
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SetOptionWrongValue(int32_t socket, uint32_t opt_value) {
    EventRecord2 (EvtNetUDP_SetOptionWrongValue, (uint32_t)socket, opt_value);
  }
#else
  #define EvrNetUDP_SetOptionWrongValue(socket, opt_value)
#endif

/**
  \brief  Event on UDP get buffer failed, out of memory (Error)
  \param  mem_size      requested memory size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_GetBufferFailed(uint16_t mem_size) {
    EventRecord2 (EvtNetUDP_GetBufferFailed, mem_size, 0);
  }
#else
  #define EvrNetUDP_GetBufferFailed(mem_size)
#endif

/**
  \brief  Event on UDP send frame (Op)
  \param  socket        socket handle
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendFrame(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetUDP_SendFrame, (uint32_t)socket, length);
  }
#else
  #define EvrNetUDP_SendFrame(socket, length)
#endif

/**
  \brief  Event on UDP send buffer is invalid (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendBufferInvalid(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendBufferInvalid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendBufferInvalid(socket)
#endif

/**
  \brief  Event on UDP send socket handle is not valid (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendSocketNotValid(socket)
#endif

/**
  \brief  Event on UDP send destination address not specified (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendAddressUnspecified(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendAddressUnspecified, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendAddressUnspecified(socket)
#endif

/**
  \brief  Event on UDP send port undefined (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendPortUndefined(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendPortUndefined, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendPortUndefined(socket)
#endif

/**
  \brief  Event on UDP send socket not open, invalid socket state (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendSocketNotOpen(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendSocketNotOpen, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendSocketNotOpen(socket)
#endif

/**
  \brief  Event on UDP send 0-length frame, frame is dumped (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendZeroLengthFrame(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendZeroLengthFrame, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendZeroLengthFrame(socket)
#endif

/**
  \brief  Event on UDP send failed, no route to destination found (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendNoRouteFound(int32_t socket) {
    EventRecord2 (EvtNetUDP_SendNoRouteFound, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_SendNoRouteFound(socket)
#endif

/**
  \brief  Event on UDP send truncated over-size packet (Op)
  \param  socket        socket handle
  \param  new_length    truncated frame length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_SendSizeTruncated(int32_t socket, uint32_t new_length) {
    EventRecord2 (EvtNetUDP_SendSizeTruncated, (uint32_t)socket, new_length);
  }
#else
  #define EvrNetUDP_SendSizeTruncated(socket, new_length)
#endif

/**
  \brief  Event on UDP display send/receive frame header (Detail)
  \param  udp_header    pointer to UDP frame header of 8 bytes
                         - SrcPort (2 bytes)
                         - DstPort (2 bytes)
                         - Len     (2 bytes)
                         - Chksum  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ShowFrameHeader(const void *udp_header) {
    EventRecordData (EvtNetUDP_ShowFrameHeader, udp_header, 8);
  }
#else
  #define EvrNetUDP_ShowFrameHeader(udp_header)
#endif

/**
  \brief  Event on UDP receive frame (Op)
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ReceiveFrame(uint32_t length, uint8_t ip_version) {
    EventRecord2 (EvtNetUDP_ReceiveFrame, length, ip_version);
  }
#else
  #define EvrNetUDP_ReceiveFrame(length, ip_version)
#endif

/**
  \brief  Event on UDP receive frame too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetUDP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetUDP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on UDP received frame mapped to a socket (Op)
  \param  socket        mapped socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_MapFrameToSocket(int32_t socket) {
    EventRecord2 (EvtNetUDP_MapFrameToSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_MapFrameToSocket(socket)
#endif

/**
  \brief  Event on UDP received frame not mapped, no open sockets found (Op)
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_FrameNotMapped(uint32_t length) {
    EventRecord2 (EvtNetUDP_FrameNotMapped, length, 0);
  }
#else
  #define EvrNetUDP_FrameNotMapped(length)
#endif

/**
  \brief  Event on UDP received frame link-layer addressed (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_LinkLayerAddressed(int32_t socket) {
    EventRecord2 (EvtNetUDP_LinkLayerAddressed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_LinkLayerAddressed(socket)
#endif

/**
  \brief  Event on UDP frame checksum check failed (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_ChecksumFailed(int32_t socket) {
    EventRecord2 (EvtNetUDP_ChecksumFailed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetUDP_ChecksumFailed(socket)
#endif

/**
  \brief  Event on UDP de-initialize available sockets (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetUDP_UninitSockets(void) {
    EventRecord2 (EvtNetUDP_UninitSockets, 0, 0);
  }
#else
  #define EvrNetUDP_UninitSockets()
#endif


// NetTCP event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetTCP_InitSockets               EventID (EventLevelOp,    EvtNetTCP,  0)
#define EvtNetTCP_GetSocket                 EventID (EventLevelOp,    EvtNetTCP,  1)
#define EvtNetTCP_GetSocketInvalidParameter EventID (EventLevelError, EvtNetTCP,  2)
#define EvtNetTCP_NoSocketAvailable         EventID (EventLevelError, EvtNetTCP,  3)
#define EvtNetTCP_ReleaseSocket             EventID (EventLevelOp,    EvtNetTCP,  4)
#define EvtNetTCP_ReleaseSocketNotValid     EventID (EventLevelError, EvtNetTCP,  5)
#define EvtNetTCP_ReleaseSocketWrongState   EventID (EventLevelError, EvtNetTCP,  6)
#define EvtNetTCP_ListenSocket              EventID (EventLevelOp,    EvtNetTCP,  7)
#define EvtNetTCP_ListenSocketNotValid      EventID (EventLevelError, EvtNetTCP,  8)
#define EvtNetTCP_ListenPortUndefined       EventID (EventLevelError, EvtNetTCP,  9)
#define EvtNetTCP_ListenSocketWrongState    EventID (EventLevelError, EvtNetTCP, 10)
#define EvtNetTCP_ConnectSocket             EventID (EventLevelOp,    EvtNetTCP, 11)
#define EvtNetTCP_ConnectSocketNotValid     EventID (EventLevelError, EvtNetTCP, 12)
#define EvtNetTCP_ConnectAddressUnspecified EventID (EventLevelError, EvtNetTCP, 13)
#define EvtNetTCP_ConnectPortUndefined      EventID (EventLevelError, EvtNetTCP, 14)
#define EvtNetTCP_ShowNetAddress            EventID (EventLevelOp,    EvtNetTCP, 15)
// Gap
#define EvtNetTCP_ConnectLocalPortInvalid   EventID (EventLevelError, EvtNetTCP, 17)
#define EvtNetTCP_AssignLocalPort           EventID (EventLevelOp,    EvtNetTCP, 18)
#define EvtNetTCP_ConnectSocketWrongState   EventID (EventLevelError, EvtNetTCP, 19)
#define EvtNetTCP_ConnectNoRouteFound       EventID (EventLevelError, EvtNetTCP,115) // End
#define EvtNetTCP_ShowRttVariables          EventID (EventLevelDetail,EvtNetTCP, 20)
#define EvtNetTCP_GetBufferFailed           EventID (EventLevelError, EvtNetTCP, 21)
#define EvtNetTCP_SendFrame                 EventID (EventLevelOp,    EvtNetTCP, 22)
#define EvtNetTCP_SendBufferInvalid         EventID (EventLevelError, EvtNetTCP, 23)
#define EvtNetTCP_SendSocketNotValid        EventID (EventLevelError, EvtNetTCP, 24)
#define EvtNetTCP_SendSocketNotConnected    EventID (EventLevelError, EvtNetTCP, 25)
#define EvtNetTCP_SendSocketClosing         EventID (EventLevelError, EvtNetTCP, 26)
#define EvtNetTCP_SendReenteredCall         EventID (EventLevelError, EvtNetTCP, 27)
#define EvtNetTCP_SendDataUnacked           EventID (EventLevelError, EvtNetTCP, 28)
#define EvtNetTCP_SendMssExceeded           EventID (EventLevelError, EvtNetTCP, 29)
#define EvtNetTCP_SendZeroLengthFrame       EventID (EventLevelOp,    EvtNetTCP, 30)
#define EvtNetTCP_CloseSocket               EventID (EventLevelOp,    EvtNetTCP, 31)
#define EvtNetTCP_CloseSocketNotValid       EventID (EventLevelError, EvtNetTCP, 32)
#define EvtNetTCP_CloseDataUnacked          EventID (EventLevelOp,    EvtNetTCP, 33)
#define EvtNetTCP_CloseSocketWrongState     EventID (EventLevelError, EvtNetTCP, 34)
#define EvtNetTCP_AbortSocket               EventID (EventLevelOp,    EvtNetTCP, 35)
#define EvtNetTCP_AbortSocketNotValid       EventID (EventLevelError, EvtNetTCP, 36)
#define EvtNetTCP_AbortSocketWrongState     EventID (EventLevelError, EvtNetTCP, 37)
#define EvtNetTCP_SendReadySocketNotValid   EventID (EventLevelError, EvtNetTCP, 38)
#define EvtNetTCP_SendReadyReenteredCall    EventID (EventLevelError, EvtNetTCP, 39)
#define EvtNetTCP_ResetWindowSocketNotValid EventID (EventLevelError, EvtNetTCP, 40)
#define EvtNetTCP_ResetWindowNotConnected   EventID (EventLevelError, EvtNetTCP, 41)
#define EvtNetTCP_ResetWindowNoFlowControl  EventID (EventLevelError, EvtNetTCP, 42)
#define EvtNetTCP_ResetWindowUpdate         EventID (EventLevelOp,    EvtNetTCP, 43)
#define EvtNetTCP_SetOptionSocket           EventID (EventLevelOp,    EvtNetTCP, 44)
#define EvtNetTCP_SetOptionSocketNotValid   EventID (EventLevelError, EvtNetTCP, 45)
#define EvtNetTCP_SetOptionSocketWrongState EventID (EventLevelError, EvtNetTCP, 46)
#define EvtNetTCP_SetOptionTos              EventID (EventLevelDetail,EvtNetTCP, 47)
#define EvtNetTCP_SetOptionTclass           EventID (EventLevelDetail,EvtNetTCP, 48)
#define EvtNetTCP_SetOptionTimeout          EventID (EventLevelDetail,EvtNetTCP, 49)
#define EvtNetTCP_SetOptionKeepAlive        EventID (EventLevelDetail,EvtNetTCP, 50)
#define EvtNetTCP_SetOptionFlowControl      EventID (EventLevelDetail,EvtNetTCP, 51)
#define EvtNetTCP_SetOptionDelayedAck       EventID (EventLevelDetail,EvtNetTCP, 52)
#define EvtNetTCP_SetOptionWrongOption      EventID (EventLevelError, EvtNetTCP, 53)
#define EvtNetTCP_SetOptionWrongValue       EventID (EventLevelError, EvtNetTCP, 54)
#define EvtNetTCP_SendDelayedAck            EventID (EventLevelOp,    EvtNetTCP, 55)
#define EvtNetTCP_SendKeepAliveProbe        EventID (EventLevelOp,    EvtNetTCP, 56)
#define EvtNetTCP_KeepAliveTimeoutClosing   EventID (EventLevelOp,    EvtNetTCP, 57)
#define EvtNetTCP_CallbackEventAck          EventID (EventLevelOp,    EvtNetTCP, 58)
#define EvtNetTCP_ResendOnTimeout           EventID (EventLevelOp,    EvtNetTCP, 59)
#define EvtNetTCP_ShowCongestionVariables   EventID (EventLevelDetail,EvtNetTCP, 60)
#define EvtNetTCP_TimeoutInState            EventID (EventLevelOp,    EvtNetTCP, 61)
#define EvtNetTCP_TwaitTimeoutClosing       EventID (EventLevelOp,    EvtNetTCP, 62)
#define EvtNetTCP_ClosingTimeout            EventID (EventLevelOp,    EvtNetTCP, 63)
#define EvtNetTCP_NoRetriesLeft             EventID (EventLevelError, EvtNetTCP, 64)
#define EvtNetTCP_ReceiveFrame              EventID (EventLevelOp,    EvtNetTCP, 65)
#define EvtNetTCP_FrameTooShort             EventID (EventLevelError, EvtNetTCP, 66)
#define EvtNetTCP_MapFrameToSocket          EventID (EventLevelOp,    EvtNetTCP, 67)
#define EvtNetTCP_FrameNotMapped            EventID (EventLevelOp,    EvtNetTCP, 68)
#define EvtNetTCP_ChecksumFailed            EventID (EventLevelError, EvtNetTCP, 69)
#define EvtNetTCP_ShowFrameHeader           EventID (EventLevelDetail,EvtNetTCP, 70)
#define EvtNetTCP_RstInWindow               EventID (EventLevelOp,    EvtNetTCP, 71)
#define EvtNetTCP_RstNotValid               EventID (EventLevelOp,    EvtNetTCP, 72)
#define EvtNetTCP_RepeatedSynAck            EventID (EventLevelOp,    EvtNetTCP, 73)
#define EvtNetTCP_AckNotSet                 EventID (EventLevelOp,    EvtNetTCP, 74)
#define EvtNetTCP_ShowSendWindow            EventID (EventLevelOp,    EvtNetTCP, 75)
#define EvtNetTCP_KeepAliveSegment          EventID (EventLevelOp,    EvtNetTCP, 76)
#define EvtNetTCP_RetransmittedSegment      EventID (EventLevelOp,    EvtNetTCP, 77)
#define EvtNetTCP_OutOfRangeSegment         EventID (EventLevelError, EvtNetTCP, 78)
#define EvtNetTCP_ZeroWindowProbe           EventID (EventLevelOp,    EvtNetTCP, 79)
#define EvtNetTCP_RemotePeerClosing         EventID (EventLevelOp,    EvtNetTCP, 80)
#define EvtNetTCP_ShowReceiveWindow         EventID (EventLevelOp,    EvtNetTCP, 81)
#define EvtNetTCP_InvalidAck                EventID (EventLevelError, EvtNetTCP, 82)
#define EvtNetTCP_SynNotSet                 EventID (EventLevelError, EvtNetTCP, 83)
#define EvtNetTCP_UserConnectionReject      EventID (EventLevelOp,    EvtNetTCP, 84)
#define EvtNetTCP_NextState                 EventID (EventLevelOp,    EvtNetTCP, 85)
#define EvtNetTCP_WrongSynAck               EventID (EventLevelError, EvtNetTCP, 86)
#define EvtNetTCP_WrongAckNumber            EventID (EventLevelError, EvtNetTCP, 87)
#define EvtNetTCP_WrongSeqNumber            EventID (EventLevelError, EvtNetTCP, 88)
#define EvtNetTCP_RepeatedSyn               EventID (EventLevelOp,    EvtNetTCP, 89)
#define EvtNetTCP_FrameUnrecognised         EventID (EventLevelError, EvtNetTCP, 90)
#define EvtNetTCP_SimultOpenNextState       EventID (EventLevelOp,    EvtNetTCP, 91)
#define EvtNetTCP_WrongFinAck               EventID (EventLevelError, EvtNetTCP, 92)
#define EvtNetTCP_FinAckNextState           EventID (EventLevelOp,    EvtNetTCP, 93)
#define EvtNetTCP_SimultCloseNextState      EventID (EventLevelOp,    EvtNetTCP, 94)
#define EvtNetTCP_AckNextState              EventID (EventLevelOp,    EvtNetTCP, 95)
#define EvtNetTCP_FinNextState              EventID (EventLevelOp,    EvtNetTCP, 96)
#define EvtNetTCP_PshAckInHalfClosed        EventID (EventLevelOp,    EvtNetTCP, 97)
#define EvtNetTCP_RepeatedFin               EventID (EventLevelOp,    EvtNetTCP, 98)
#define EvtNetTCP_LastAckNextState          EventID (EventLevelOp,    EvtNetTCP, 99)
#define EvtNetTCP_RstReceived               EventID (EventLevelError, EvtNetTCP,100)
#define EvtNetTCP_InvalidState              EventID (EventLevelError, EvtNetTCP,101)
#define EvtNetTCP_SendData                  EventID (EventLevelOp,    EvtNetTCP,102)
#define EvtNetTCP_SendControl               EventID (EventLevelOp,    EvtNetTCP,103)
#define EvtNetTCP_SendReset                 EventID (EventLevelOp,    EvtNetTCP,104)
#define EvtNetTCP_ParseHeaderOptions        EventID (EventLevelOp,    EvtNetTCP,105)
#define EvtNetTCP_OptionMss                 EventID (EventLevelOp,    EvtNetTCP,106)
#define EvtNetTCP_DuplicateAck              EventID (EventLevelOp,    EvtNetTCP,107)
#define EvtNetTCP_FastRetransmit            EventID (EventLevelOp,    EvtNetTCP,108)
#define EvtNetTCP_DataAcked                 EventID (EventLevelOp,    EvtNetTCP,109)
#define EvtNetTCP_ResendData                EventID (EventLevelOp,    EvtNetTCP,110)
#define EvtNetTCP_MapSocketWrongFlagsSet    EventID (EventLevelError, EvtNetTCP,111)
#define EvtNetTCP_MapSocketSynNotSet        EventID (EventLevelError, EvtNetTCP,112)
#define EvtNetTCP_MapSocketNoListenSocket   EventID (EventLevelError, EvtNetTCP,113)
#define EvtNetTCP_UninitSockets             EventID (EventLevelOp,    EvtNetTCP,114)
#endif

/**
  \brief  Event on TCP socket initialize (Op)
  \param  num_sockets   number of available TCP sockets
  \param  max_segsize   maximum segment size in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_InitSockets(uint32_t num_sockets, uint16_t max_segsize) {
    EventRecord2 (EvtNetTCP_InitSockets, num_sockets, max_segsize);
  }
#else
  #define EvrNetTCP_InitSockets(num_sockets, max_segsize)
#endif

/**
  \brief  Event on TCP allocate a free socket (Op)
  \param  socket        allocated socket handle
  \param  num_socks     number of available TCP sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_GetSocket(int32_t socket, uint32_t num_socks) {
    EventRecord2 (EvtNetTCP_GetSocket, (uint32_t)socket, num_socks);
  }
#else
  #define EvrNetTCP_GetSocket(socket, num_socks)
#endif

/**
  \brief  Event on TCP get socket failed, invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_GetSocketInvalidParameter(void) {
    EventRecord2 (EvtNetTCP_GetSocketInvalidParameter, 0, 0);
  }
#else
  #define EvrNetTCP_GetSocketInvalidParameter()
#endif

/**
  \brief  Event on TCP get socket failed, no free socket available (Error)
  \param  num_socks     number of available TCP sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_NoSocketAvailable(uint32_t num_socks) {
    EventRecord2 (EvtNetTCP_NoSocketAvailable, num_socks, 0);
  }
#else
  #define EvrNetTCP_NoSocketAvailable(num_socks)
#endif

/**
  \brief  Event on TCP release socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ReleaseSocket(int32_t socket) {
    EventRecord2 (EvtNetTCP_ReleaseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ReleaseSocket(socket)
#endif

/**
  \brief  Event on TCP release failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ReleaseSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_ReleaseSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ReleaseSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP release failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ReleaseSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_ReleaseSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_ReleaseSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP listen socket request (Op)
  \param  socket        socket handle
  \param  port          local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ListenSocket(int32_t socket, uint16_t port) {
    EventRecord2 (EvtNetTCP_ListenSocket, (uint32_t)socket, port);
  }
#else
  #define EvrNetTCP_ListenSocket(socket, port)
#endif

/**
  \brief  Event on TCP listen failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ListenSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_ListenSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ListenSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP listen failed, listening port undefined (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ListenPortUndefined(int32_t socket) {
    EventRecord2 (EvtNetTCP_ListenPortUndefined, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ListenPortUndefined(socket)
#endif

/**
  \brief  Event on TCP listen failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ListenSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_ListenSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_ListenSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP connect socket request (Op)
  \param  socket        socket handle
  \param  local_port    local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectSocket(int32_t socket, uint16_t local_port) {
    EventRecord2 (EvtNetTCP_ConnectSocket, (uint32_t)socket, local_port);
  }
#else
  #define EvrNetTCP_ConnectSocket(socket, local_port)
#endif

/**
  \brief  Event on TCP connect failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_ConnectSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ConnectSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP connect failed, address unspecified (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectAddressUnspecified(int32_t socket) {
    EventRecord2 (EvtNetTCP_ConnectAddressUnspecified, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ConnectAddressUnspecified(socket)
#endif

/**
  \brief  Event on TCP connect failed, port undefined (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectPortUndefined(int32_t socket) {
    EventRecord2 (EvtNetTCP_ConnectPortUndefined, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ConnectPortUndefined(socket)
#endif

/**
  \brief  Event on TCP display net address (Op)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowNetAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTCP_ShowNetAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTCP_ShowNetAddress(net_addr)
#endif

/**
  \brief  Event on TCP connect failed, local port invalid (Error)
  \param  socket        socket handle
  \param  local_port    local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectLocalPortInvalid(int32_t socket, uint16_t local_port) {
    EventRecord2 (EvtNetTCP_ConnectLocalPortInvalid, (uint32_t)socket, local_port);
  }
#else
  #define EvrNetTCP_ConnectLocalPortInvalid(socket, local_port)
#endif

/**
  \brief  Event on TCP local port assigned in socket connect request (Op)
  \param  socket        socket handle
  \param  local_port    assigned local port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AssignLocalPort(int32_t socket, uint16_t local_port) {
    EventRecord2 (EvtNetTCP_AssignLocalPort, (uint32_t)socket, local_port);
  }
#else
  #define EvrNetTCP_AssignLocalPort(socket, local_port)
#endif

/**
  \brief  Event on TCP connect failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_ConnectSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_ConnectSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP connect failed, no route to destination found (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ConnectNoRouteFound(int32_t socket) {
    EventRecord2 (EvtNetTCP_ConnectNoRouteFound, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ConnectNoRouteFound(socket)
#endif

/**
  \brief  Event on TCP display RTT (round trip time) estimation variables (Detail)
  \param  socket        socket handle
  \param  sa            rtt average estimator
  \param  sv            rtt deviation estimator
  \remark Those variables are taken from VJs original code in his paper.
          (Congestion avoidance and control, page 20)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowRttVariables(int32_t socket, int16_t sa, int16_t sv) {
    /* uint32_t rto = ((sa>>3) + sv) * 100; */
    uint32_t val2 = (uint32_t)sa << 16  | (uint32_t)sv;
    EventRecord2 (EvtNetTCP_ShowRttVariables, (uint32_t)socket, val2);
  }
#else
  #define EvrNetTCP_ShowRttVariables(socket, sa, sv)
#endif

/**
  \brief  Event on TCP get buffer failed, out of memory (Error)
  \param  mem_size      requested memory size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_GetBufferFailed(uint16_t mem_size) {
    EventRecord2 (EvtNetTCP_GetBufferFailed, mem_size, 0);
  }
#else
  #define EvrNetTCP_GetBufferFailed(mem_size)
#endif

/**
  \brief  Event on TCP send frame (Op)
  \param  socket        socket handle
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendFrame(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetTCP_SendFrame, (uint32_t)socket, length);
  }
#else
  #define EvrNetTCP_SendFrame(socket, length)
#endif

/**
  \brief  Event on TCP send failed, invalid buffer (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendBufferInvalid(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendBufferInvalid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendBufferInvalid(socket)
#endif

/**
  \brief  Event on TCP send failed, socket handle not valid (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP send failed, socket not connected (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendSocketNotConnected(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendSocketNotConnected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendSocketNotConnected(socket)
#endif

/**
  \brief  Event on TCP send failed, socket closing (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendSocketClosing(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendSocketClosing, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendSocketClosing(socket)
#endif

/**
  \brief  Event on TCP send failed, called from a callback function (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendReenteredCall(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendReenteredCall, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendReenteredCall(socket)
#endif

/**
  \brief  Event on TCP send failed, unacked data pending (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendDataUnacked(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendDataUnacked, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendDataUnacked(socket)
#endif

/**
  \brief  Event on TCP send failed, Maximum Segment Size exceeded (Error)
  \param  socket        socket handle
  \remark Frame length is larger than MSS.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendMssExceeded(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendMssExceeded, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendMssExceeded(socket)
#endif

/**
  \brief  Event on TCP send 0-length frame, frame dumped (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendZeroLengthFrame(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendZeroLengthFrame, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendZeroLengthFrame(socket)
#endif

/**
  \brief  Event on TCP close socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_CloseSocket(int32_t socket) {
    EventRecord2 (EvtNetTCP_CloseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_CloseSocket(socket)
#endif

/**
  \brief  Event on TCP close socket failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_CloseSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_CloseSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_CloseSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP close pended, unacked data exist (Op)
  \param  socket        socket handle
  \param  unack_length  length of unacked data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_CloseDataUnacked(int32_t socket, uint32_t unack_length) {
    EventRecord2 (EvtNetTCP_CloseDataUnacked, (uint32_t)socket, unack_length);
  }
#else
  #define EvrNetTCP_CloseDataUnacked(socket, unack_length)
#endif

/**
  \brief  Event on TCP close failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_CloseSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_CloseSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_CloseSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP abort socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AbortSocket(int32_t socket) {
    EventRecord2 (EvtNetTCP_AbortSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_AbortSocket(socket)
#endif

/**
  \brief  Event on TCP abort failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AbortSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_AbortSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_AbortSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP abort failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AbortSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_AbortSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_AbortSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP send ready failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendReadySocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendReadySocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendReadySocketNotValid(socket)
#endif

/**
  \brief  Event on TCP send ready failed, called from a callback function (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendReadyReenteredCall(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendReadyReenteredCall, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendReadyReenteredCall(socket)
#endif

/**
  \brief  Event on TCP reset window failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResetWindowSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_ResetWindowSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ResetWindowSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP reset window failed, invalid socket state (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResetWindowNotConnected(int32_t socket) {
    EventRecord2 (EvtNetTCP_ResetWindowNotConnected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ResetWindowNotConnected(socket)
#endif

/**
  \brief  Event on TCP reset window failed, flow-control not enabled (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResetWindowNoFlowControl(int32_t socket) {
    EventRecord2 (EvtNetTCP_ResetWindowNoFlowControl, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ResetWindowNoFlowControl(socket)
#endif

/**
  \brief  Event on TCP reset window update (Error)
  \param  socket        socket handle
  \param  window_size   updated receive window size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResetWindowUpdate(int32_t socket, uint16_t window_size) {
    EventRecord2 (EvtNetTCP_ResetWindowUpdate, (uint32_t)socket, window_size);
  }
#else
  #define EvrNetTCP_ResetWindowUpdate(socket, window_size)
#endif

/**
  \brief  Event on TCP set socket option (Op)
  \param  socket        socket handle
  \param  option        interface option as specified by \ref netTCP_Option
  \param  val           option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionSocket(int32_t socket, int32_t option, uint32_t val) {
    uint32_t val2 = (val << 8) | (option & 0xFF);
    EventRecord2 (EvtNetTCP_SetOptionSocket, (uint32_t)socket, val2);
  }
#else
  #define EvrNetTCP_SetOptionSocket(socket, option, val)
#endif

/**
  \brief  Event on TCP set socket option failed, not valid socket (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionSocketNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_SetOptionSocketNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SetOptionSocketNotValid(socket)
#endif

/**
  \brief  Event on TCP set socket option failed, wrong socket state (Error)
  \param  socket        socket handle
  \param  state         socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionSocketWrongState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_SetOptionSocketWrongState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_SetOptionSocketWrongState(socket, state)
#endif

/**
  \brief  Event on TCP set socket option Type Of Service for IPv4 (Detail)
  \param  socket        socket handle
  \param  ip4_tos       type of service value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionTos(int32_t socket, uint8_t ip4_tos) {
    EventRecord2 (EvtNetTCP_SetOptionTos, (uint32_t)socket, ip4_tos);
  }
#else
  #define EvrNetTCP_SetOptionTos(socket, ip4_tos)
#endif

/**
  \brief  Event on TCP set socket option Traffic Class for IPv6 (Detail)
  \param  socket        socket handle
  \param  ip6_tclass    traffic class value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionTclass(int32_t socket, uint8_t ip6_tclass) {
    EventRecord2 (EvtNetTCP_SetOptionTclass, (uint32_t)socket, ip6_tclass);
  }
#else
  #define EvrNetTCP_SetOptionTclass(socket, ip6_tclass)
#endif

/**
  \brief  Event on TCP set socket option connection timeout (Detail)
  \param  socket        socket handle
  \param  timeout       connection or keep-alive timeout in seconds
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionTimeout(int32_t socket, uint16_t timeout) {
    EventRecord2 (EvtNetTCP_SetOptionTimeout, (uint32_t)socket, timeout);
  }
#else
  #define EvrNetTCP_SetOptionTimeout(socket, timeout)
#endif

/**
  \brief  Event on TCP set socket keep-alive option (Detail)
  \param  socket        socket handle
  \param  enable        keep-alive state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionKeepAlive(int32_t socket, uint32_t enable) {
    EventRecord2 (EvtNetTCP_SetOptionKeepAlive, (uint32_t)socket, enable);
  }
#else
  #define EvrNetTCP_SetOptionKeepAlive(socket, enable)
#endif

/**
  \brief  Event on TCP set socket flow-control option (Detail)
  \param  socket        socket handle
  \param  enable        flow-control state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionFlowControl(int32_t socket, uint32_t enable) {
    EventRecord2 (EvtNetTCP_SetOptionFlowControl, (uint32_t)socket, enable);
  }
#else
  #define EvrNetTCP_SetOptionFlowControl(socket, enable)
#endif

/**
  \brief  Event on TCP set socket delayed-acknowledge option (Detail)
  \param  socket        socket handle
  \param  enable        delayed-acknowledge state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionDelayedAck(int32_t socket, uint32_t enable) {
    EventRecord2 (EvtNetTCP_SetOptionDelayedAck, (uint32_t)socket, enable);
  }
#else
  #define EvrNetTCP_SetOptionDelayedAck(socket, enable)
#endif

/**
  \brief  Event on TCP wrong set socket option (Error)
  \param  socket        socket handle
  \param  tcp_option    wrong set option value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionWrongOption(int32_t socket, int32_t tcp_option) {
    EventRecord2 (EvtNetTCP_SetOptionWrongOption, (uint32_t)socket, (uint32_t)tcp_option);
  }
#else
  #define EvrNetTCP_SetOptionWrongOption(socket, tcp_option)
#endif

/**
  \brief  Event on TCP wrong value for set socket option (Error)
  \param  socket        socket handle
  \param  opt_value     wrong value for set option
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SetOptionWrongValue(int32_t socket, uint32_t opt_value) {
    EventRecord2 (EvtNetTCP_SetOptionWrongValue, (uint32_t)socket, opt_value);
  }
#else
  #define EvrNetTCP_SetOptionWrongValue(socket, opt_value)
#endif

/**
  \brief  Event on TCP send delayed-acknowledge (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendDelayedAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendDelayedAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendDelayedAck(socket)
#endif

/**
  \brief  Event on TCP send keep-alive probe (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendKeepAliveProbe(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendKeepAliveProbe, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendKeepAliveProbe(socket)
#endif

/**
  \brief  Event on TCP keep-alive timeout no response, close socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_KeepAliveTimeoutClosing(int32_t socket) {
    EventRecord2 (EvtNetTCP_KeepAliveTimeoutClosing, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_KeepAliveTimeoutClosing(socket)
#endif

/**
  \brief  Event on TCP acknowledge callback notification to the user (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_CallbackEventAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_CallbackEventAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_CallbackEventAck(socket)
#endif

/**
  \brief  Event on TCP resend data on timeout (Op)
  \param  socket        socket handle
  \param  length        length of data to resend
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResendOnTimeout(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetTCP_ResendOnTimeout, (uint32_t)socket, length);
  }
#else
  #define EvrNetTCP_ResendOnTimeout(socket, length)
#endif

/**
  \brief  Event on TCP display congestion avoidance/control variables (Detail)
  \param  socket        socket handle
  \param  cwnd          congestion window
  \param  ssth          slow start threshold
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowCongestionVariables(int32_t socket, uint16_t cwnd, uint16_t ssth) {
    uint32_t val2 = ((uint32_t)cwnd << 16) | ssth;
    EventRecord2 (EvtNetTCP_ShowCongestionVariables, (uint32_t)socket, val2);
  }
#else
  #define EvrNetTCP_ShowCongestionVariables(socket, cwnd, ssth)
#endif

/**
  \brief  Event on TCP timeout expired in current state (Op)
  \param  socket        socket handle
  \param  state         current socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_TimeoutInState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_TimeoutInState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_TimeoutInState(socket, state)
#endif

/**
  \brief  Event on TCP timeout in TIME_WAIT state, closing socket (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_TwaitTimeoutClosing(int32_t socket) {
    EventRecord2 (EvtNetTCP_TwaitTimeoutClosing, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_TwaitTimeoutClosing(socket)
#endif

/**
  \brief  Event on TCP timeout in CLOSING, FIN_WAIT_1 or LAST_ACK state (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ClosingTimeout(int32_t socket) {
    EventRecord2 (EvtNetTCP_ClosingTimeout, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ClosingTimeout(socket)
#endif

/**
  \brief  Event on TCP no retries left (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_NoRetriesLeft(int32_t socket) {
    EventRecord2 (EvtNetTCP_NoRetriesLeft, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_NoRetriesLeft(socket)
#endif

/**
  \brief  Event on TCP receive frame (Op)
  \param  length        frame length in bytes
  \param  ip_version    internet protocol version
                         - 0: IPv4
                         - 1: IPv6
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ReceiveFrame(uint32_t length, uint8_t ip_version) {
    EventRecord2 (EvtNetTCP_ReceiveFrame, length, ip_version);
  }
#else
  #define EvrNetTCP_ReceiveFrame(length, ip_version)
#endif

/**
  \brief  Event on TCP receive frame too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetTCP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetTCP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on TCP received frame not mapped, no active socket found (Op)
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FrameNotMapped(uint32_t length) {
    EventRecord2 (EvtNetTCP_FrameNotMapped, length, 0);
  }
#else
  #define EvrNetTCP_FrameNotMapped(length)
#endif

/**
  \brief  Event on TCP received frame mapped to a socket (Op)
  \param  socket        mapped socket handle
  \param  state         current socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_MapFrameToSocket(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_MapFrameToSocket, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_MapFrameToSocket(socket, state)
#endif

/**
  \brief  Event on TCP frame checksum check failed (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ChecksumFailed(int32_t socket) {
    EventRecord2 (EvtNetTCP_ChecksumFailed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ChecksumFailed(socket)
#endif

/**
  \brief  Event on TCP display send/receive frame header (Detail)
  \param  tcp_header    pointer to TCP frame header of 20 bytes
                         - SrcPort (2 bytes)
                         - DstPort (2 bytes)
                         - SeqNr   (4 bytes)
                         - AckNr   (4 bytes)
                         - HLen    (1 byte)
                         - Flags   (1 byte)
                         - Window  (2 bytes)
                         - Chksum  (2 bytes)
                         - UrgPtr  (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowFrameHeader(const void *tcp_header) {
    EventRecordData (EvtNetTCP_ShowFrameHeader, tcp_header, 20);
  }
#else
  #define EvrNetTCP_ShowFrameHeader(tcp_header)
#endif

/**
  \brief  Event on TCP RST-flag received within current receive window (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RstInWindow(int32_t socket) {
    EventRecord2 (EvtNetTCP_RstInWindow, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RstInWindow(socket)
#endif

/**
  \brief  Event on TCP RST-flag received outside current receive window (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RstNotValid(int32_t socket) {
    EventRecord2 (EvtNetTCP_RstNotValid, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RstNotValid(socket)
#endif

/**
  \brief  Event on TCP repeated SYN+ACK-flags received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RepeatedSynAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_RepeatedSynAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RepeatedSynAck(socket)
#endif

/**
  \brief  Event on TCP ACK-flag not set in received frame (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AckNotSet(int32_t socket) {
    EventRecord2 (EvtNetTCP_AckNotSet, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_AckNotSet(socket)
#endif

/**
  \brief  Event on TCP display current sending window size (Op)
  \param  socket        socket handle
  \param  send_window   current sending window size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowSendWindow(int32_t socket, uint16_t send_window) {
    EventRecord2 (EvtNetTCP_ShowSendWindow, (uint32_t)socket, send_window);
  }
#else
  #define EvrNetTCP_ShowSendWindow(socket, send_window)
#endif

/**
  \brief  Event on TCP keep-alive frame received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_KeepAliveSegment(int32_t socket) {
    EventRecord2 (EvtNetTCP_KeepAliveSegment, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_KeepAliveSegment(socket)
#endif

/**
  \brief  Event on TCP retransmitted frame received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RetransmittedSegment(int32_t socket) {
    EventRecord2 (EvtNetTCP_RetransmittedSegment, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RetransmittedSegment(socket)
#endif

/**
  \brief  Event on TCP out of range segment received (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_OutOfRangeSegment(int32_t socket) {
    EventRecord2 (EvtNetTCP_OutOfRangeSegment, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_OutOfRangeSegment(socket)
#endif

/**
  \brief  Event on TCP zero-window probe received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ZeroWindowProbe(int32_t socket) {
    EventRecord2 (EvtNetTCP_ZeroWindowProbe, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_ZeroWindowProbe(socket)
#endif

/**
  \brief  Event on TCP FIN-flag received, remote peer wants to close (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RemotePeerClosing(int32_t socket) {
    EventRecord2 (EvtNetTCP_RemotePeerClosing, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RemotePeerClosing(socket)
#endif

/**
  \brief  Event on TCP display current receiving window size (Op)
  \param  socket        socket handle
  \param  rec_window    current receiving window size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ShowReceiveWindow(int32_t socket, uint16_t rec_window) {
    EventRecord2 (EvtNetTCP_ShowReceiveWindow, (uint32_t)socket, rec_window);
  }
#else
  #define EvrNetTCP_ShowReceiveWindow(socket, rec_window)
#endif

/**
  \brief  Event on TCP ACK-flag set received, invalid in state LISTEN (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_InvalidAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_InvalidAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_InvalidAck(socket)
#endif

/**
  \brief  Event on TCP SYN-flag not set in received frame (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SynNotSet(int32_t socket) {
    EventRecord2 (EvtNetTCP_SynNotSet, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SynNotSet(socket)
#endif

/**
  \brief  Event on TCP user rejected inbound connection (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_UserConnectionReject(int32_t socket) {
    EventRecord2 (EvtNetTCP_UserConnectionReject, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_UserConnectionReject(socket)
#endif

/**
  \brief  Event on TCP transition to next state (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_NextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_NextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_NextState(socket, state)
#endif

/**
  \brief  Event on TCP SYN+ACK-flags set but wrong ack number (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_WrongSynAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_WrongSynAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_WrongSynAck(socket)
#endif

/**
  \brief  Event on TCP ACK-flag set but wrong ack number (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_WrongAckNumber(int32_t socket) {
    EventRecord2 (EvtNetTCP_WrongAckNumber, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_WrongAckNumber(socket)
#endif

/**
  \brief  Event on TCP ACK-flag set but wrong seq number (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_WrongSeqNumber(int32_t socket) {
    EventRecord2 (EvtNetTCP_WrongSeqNumber, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_WrongSeqNumber(socket)
#endif

/**
  \brief  Event on TCP repeated SYN-flag set frame received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RepeatedSyn(int32_t socket) {
    EventRecord2 (EvtNetTCP_RepeatedSyn, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RepeatedSyn(socket)
#endif

/**
  \brief  Event on TCP unrecognised frame received (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FrameUnrecognised(int32_t socket) {
    EventRecord2 (EvtNetTCP_FrameUnrecognised, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_FrameUnrecognised(socket)
#endif

/**
  \brief  Event on TCP SYN-flag set received in simultaneous open (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SimultOpenNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_SimultOpenNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_SimultOpenNextState(socket, state)
#endif

/**
  \brief  Event on TCP frame with FIN+ACK-flags set but wrong ack number (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_WrongFinAck(int32_t socket) {
    EventRecord2 (EvtNetTCP_WrongFinAck, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_WrongFinAck(socket)
#endif

/**
  \brief  Event on TCP frame with FIN+ACK-flags received (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FinAckNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_FinAckNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_FinAckNextState(socket, state)
#endif

/**
  \brief  Event on TCP frame with FIN-flag received in simultaneous close (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SimultCloseNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_SimultCloseNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_SimultCloseNextState(socket, state)
#endif

/**
  \brief  Event on TCP frame with ACK-flag received (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_AckNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_AckNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_AckNextState(socket, state)
#endif

/**
  \brief  Event on TCP frame with FIN-flag received (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FinNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_FinNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_FinNextState(socket, state)
#endif

/**
  \brief  Event on TCP PSH+ACK-flags received in half closed state FIN_WAIT_2 (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_PshAckInHalfClosed(int32_t socket) {
    EventRecord2 (EvtNetTCP_PshAckInHalfClosed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_PshAckInHalfClosed(socket)
#endif

/**
  \brief  Event on TCP repeated FIN-flag received (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RepeatedFin(int32_t socket) {
    EventRecord2 (EvtNetTCP_RepeatedFin, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RepeatedFin(socket)
#endif

/**
  \brief  Event on TCP last ACK-flag received (Op)
  \param  socket        socket handle
  \param  state         next socket state
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_LastAckNextState(int32_t socket, int32_t state) {
    EventRecord2 (EvtNetTCP_LastAckNextState, (uint32_t)socket, (uint32_t)state);
  }
#else
  #define EvrNetTCP_LastAckNextState(socket, state)
#endif

/**
  \brief  Event on TCP RST-flag received (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_RstReceived(int32_t socket) {
    EventRecord2 (EvtNetTCP_RstReceived, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_RstReceived(socket)
#endif

/**
  \brief  Event on TCP socket in invalid state (Error)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_InvalidState(int32_t socket) {
    EventRecord2 (EvtNetTCP_InvalidState, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_InvalidState(socket)
#endif

/**
  \brief  Event on TCP send data frame (Op)
  \param  socket        socket handle
  \param  length        data length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendData(int32_t socket, uint16_t length) {
    EventRecord2 (EvtNetTCP_SendData, (uint32_t)socket, length);
  }
#else
  #define EvrNetTCP_SendData(socket, length)
#endif

/**
  \brief  Event on TCP send control frame (Op)
  \param  socket        socket handle
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendControl(int32_t socket) {
    EventRecord2 (EvtNetTCP_SendControl, (uint32_t)socket, 0);
  }
#else
  #define EvrNetTCP_SendControl(socket)
#endif

/**
  \brief  Event on TCP send reset to remote host for unmapped frame (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_SendReset(void) {
    EventRecord2 (EvtNetTCP_SendReset, 0, 0);
  }
#else
  #define EvrNetTCP_SendReset()
#endif

/**
  \brief  Event on TCP parse header options (Op)
  \param  socket        socket handle
  \param  opt_length    length of header options
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ParseHeaderOptions(int32_t socket, uint32_t opt_length) {
    EventRecord2 (EvtNetTCP_ParseHeaderOptions, (uint32_t)socket, opt_length);
  }
#else
  #define EvrNetTCP_ParseHeaderOptions(socket, opt_length)
#endif

/**
  \brief  Event on TCP header option MSS (maximum segment size) (Op)
  \param  socket        socket handle
  \param  mss           maximum segment size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_OptionMss(int32_t socket, uint32_t mss) {
    EventRecord2 (EvtNetTCP_OptionMss, (uint32_t)socket, mss);
  }
#else
  #define EvrNetTCP_OptionMss(socket, mss)
#endif

/**
  \brief  Event on TCP duplicate acknowledge processing (Op)
  \param  socket        socket handle
  \param  dup_acks      number of duplicate acks
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_DuplicateAck(int32_t socket, uint32_t dup_acks) {
    EventRecord2 (EvtNetTCP_DuplicateAck, (uint32_t)socket, dup_acks);
  }
#else
  #define EvrNetTCP_DuplicateAck(socket, dup_acks)
#endif

/**
  \brief  Event on TCP fast retransmit, received 3 duplicate acks (Op)
  \param  socket        socket handle
  \param  data_length   length of data to fast retransmit
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_FastRetransmit(int32_t socket, uint32_t data_length) {
    EventRecord2 (EvtNetTCP_FastRetransmit, (uint32_t)socket, data_length);
  }
#else
  #define EvrNetTCP_FastRetransmit(socket, data_length)
#endif

/**
  \brief  Event on TCP data acknowledge processing (Op)
  \param  socket        socket handle
  \param  ack_length    acknowledged data length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_DataAcked(int32_t socket, uint32_t ack_length) {
    EventRecord2 (EvtNetTCP_DataAcked, (uint32_t)socket, ack_length);
  }
#else
  #define EvrNetTCP_DataAcked(socket, ack_length)
#endif

/**
  \brief  Event on TCP resend data from transmit queue (Op)
  \param  socket        socket handle
  \param  length        length of the data to resend
  \param  tout          retry timeout in 100ms ticks
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_ResendData(int32_t socket, uint32_t length, uint32_t tout) {
    uint32_t val2 = length << 16 | tout;
    EventRecord2 (EvtNetTCP_ResendData, (uint32_t)socket, val2);
  }
#else
  #define EvrNetTCP_ResendData(socket, length, tout)
#endif

/**
  \brief  Event on TCP map socket failed, wrong TCP flags set (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_MapSocketWrongFlagsSet(void) {
    EventRecord2 (EvtNetTCP_MapSocketWrongFlagsSet, 0, 0);
  }
#else
  #define EvrNetTCP_MapSocketWrongFlagsSet()
#endif

/**
  \brief  Event on TCP map socket failed, SYN-flag not set (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_MapSocketSynNotSet(void) {
    EventRecord2 (EvtNetTCP_MapSocketSynNotSet, 0, 0);
  }
#else
  #define EvrNetTCP_MapSocketSynNotSet()
#endif

/**
  \brief  Event on TCP map socket failed, no listening sockets found (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_MapSocketNoListenSocket(void) {
    EventRecord2 (EvtNetTCP_MapSocketNoListenSocket, 0, 0);
  }
#else
  #define EvrNetTCP_MapSocketNoListenSocket()
#endif

/**
  \brief  Event on TCP de-initialize available sockets (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTCP_UninitSockets(void) {
    EventRecord2 (EvtNetTCP_UninitSockets, 0, 0);
  }
#else
  #define EvrNetTCP_UninitSockets()
#endif


// NetBSD event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetBSD_InitSockets               EventID (EventLevelOp,    EvtNetBSD,  0)
#define EvtNetBSD_SocketCreate              EventID (EventLevelAPI,   EvtNetBSD,  1)
#define EvtNetBSD_SocketInvalidParameter    EventID (EventLevelError, EvtNetBSD,  2)
#define EvtNetBSD_NoSocketAvailable         EventID (EventLevelError, EvtNetBSD,  3)
#define EvtNetBSD_BindSocket                EventID (EventLevelAPI,   EvtNetBSD,  4)
#define EvtNetBSD_BindSocketNotValid        EventID (EventLevelError, EvtNetBSD,  5)
#define EvtNetBSD_BindInvalidParameter      EventID (EventLevelError, EvtNetBSD,  6)
#define EvtNetBSD_BindSocketNotCreated      EventID (EventLevelError, EvtNetBSD,  7)
#define EvtNetBSD_BindSocketAlreadyBound    EventID (EventLevelError, EvtNetBSD,  8)
#define EvtNetBSD_BindSocketConnected       EventID (EventLevelError, EvtNetBSD,  9)
#define EvtNetBSD_BindPortInUse             EventID (EventLevelError, EvtNetBSD, 10)
#define EvtNetBSD_ViewNetAddress            EventID (EventLevelOp,    EvtNetBSD, 11)
#define EvtNetBSD_ListenSocket              EventID (EventLevelAPI,   EvtNetBSD, 13)
#define EvtNetBSD_ListenSocketNotValid      EventID (EventLevelError, EvtNetBSD, 14)
#define EvtNetBSD_ListenInvalidParameter    EventID (EventLevelError, EvtNetBSD, 15)
#define EvtNetBSD_ListenSocketNotCreated    EventID (EventLevelError, EvtNetBSD,145)
#define EvtNetBSD_ListenSocketNotBound      EventID (EventLevelError, EvtNetBSD, 16)
#define EvtNetBSD_ListenSocketNotStreamType EventID (EventLevelError, EvtNetBSD, 17)
#define EvtNetBSD_ListenSockAlreadyListens  EventID (EventLevelError, EvtNetBSD, 18)
#define EvtNetBSD_ListenBacklogCreateFailed EventID (EventLevelError, EvtNetBSD, 19)
#define EvtNetBSD_AcceptSocket              EventID (EventLevelAPI,   EvtNetBSD, 20)
#define EvtNetBSD_AcceptSocketNotValid      EventID (EventLevelError, EvtNetBSD, 21)
#define EvtNetBSD_AcceptSocketNotCreated    EventID (EventLevelError, EvtNetBSD,146)
#define EvtNetBSD_AcceptSocketNotListening  EventID (EventLevelError, EvtNetBSD, 22)
#define EvtNetBSD_AcceptSocketNotStreamType EventID (EventLevelError, EvtNetBSD, 23)
#define EvtNetBSD_AcceptSocketLocked        EventID (EventLevelError, EvtNetBSD, 24)
#define EvtNetBSD_AcceptChildSockConnected  EventID (EventLevelOp,    EvtNetBSD, 25)
#define EvtNetBSD_AcceptSocketClosed        EventID (EventLevelError, EvtNetBSD, 26)
#define EvtNetBSD_AcceptSocketKilled        EventID (EventLevelError, EvtNetBSD, 27)
#define EvtNetBSD_ConnectSocket             EventID (EventLevelAPI,   EvtNetBSD, 28)
#define EvtNetBSD_ConnectSocketNotValid     EventID (EventLevelError, EvtNetBSD, 29)
#define EvtNetBSD_ConnectInvalidParameter   EventID (EventLevelError, EvtNetBSD, 30)
#define EvtNetBSD_ConnectSocketLocked       EventID (EventLevelError, EvtNetBSD, 31)
#define EvtNetBSD_ConnectSocketNotCreated   EventID (EventLevelError, EvtNetBSD, 32)
#define EvtNetBSD_ConnectSocketWrongState   EventID (EventLevelError, EvtNetBSD, 33)
#define EvtNetBSD_ConnectDatagramSuccess    EventID (EventLevelOp,    EvtNetBSD, 34)
#define EvtNetBSD_ConnectStreamSuccess      EventID (EventLevelOp,    EvtNetBSD, 35)
#define EvtNetBSD_ConnectStreamTimeout      EventID (EventLevelError, EvtNetBSD, 36)
#define EvtNetBSD_ConnectStreamRefused      EventID (EventLevelError, EvtNetBSD, 37)
#define EvtNetBSD_ConnectSocketKilled       EventID (EventLevelError, EvtNetBSD, 38)
#define EvtNetBSD_SendSocket                EventID (EventLevelAPI,   EvtNetBSD, 39)
#define EvtNetBSD_SendSocketNotValid        EventID (EventLevelError, EvtNetBSD, 40)
#define EvtNetBSD_SendInvalidParameter      EventID (EventLevelError, EvtNetBSD, 41)
#define EvtNetBSD_SendSocketNotCreated      EventID (EventLevelError, EvtNetBSD, 42)
#define EvtNetBSD_SendSocketLocked          EventID (EventLevelError, EvtNetBSD, 43)
#define EvtNetBSD_SendSocketNotConnected    EventID (EventLevelError, EvtNetBSD, 44)
#define EvtNetBSD_SendSocketWrongState      EventID (EventLevelError, EvtNetBSD, 45)
#define EvtNetBSD_SendSocketClosed          EventID (EventLevelError, EvtNetBSD, 46)
#define EvtNetBSD_SendCompleteNonBlocking   EventID (EventLevelOp,    EvtNetBSD, 47)
#define EvtNetBSD_SendSocketKilled          EventID (EventLevelError, EvtNetBSD, 48)
#define EvtNetBSD_SendStreamError           EventID (EventLevelError, EvtNetBSD, 49)
#define EvtNetBSD_SendtoInvalidParameter    EventID (EventLevelError, EvtNetBSD, 50)
#define EvtNetBSD_SendDestAddressUndefined  EventID (EventLevelError, EvtNetBSD, 51)
#define EvtNetBSD_SendtoMsgTooLarge         EventID (EventLevelError, EvtNetBSD,152)
#define EvtNetBSD_SendtoNoMemory            EventID (EventLevelError, EvtNetBSD,153)
#define EvtNetBSD_SendDatagramError         EventID (EventLevelError, EvtNetBSD, 52)
#define EvtNetBSD_SendCompleteBlocking      EventID (EventLevelOp,    EvtNetBSD, 53)
#define EvtNetBSD_SendTimeoutBlocking       EventID (EventLevelOp,    EvtNetBSD, 54)
#define EvtNetBSD_SendNoMemory              EventID (EventLevelOp,    EvtNetBSD, 55)
#define EvtNetBSD_SendmsgSocket             EventID (EventLevelAPI,   EvtNetBSD, 56)
#define EvtNetBSD_SendmsgSocketNotValid     EventID (EventLevelError, EvtNetBSD, 57)
#define EvtNetBSD_SendmsgInvalidParameter   EventID (EventLevelError, EvtNetBSD, 58)
#define EvtNetBSD_SendmsgSocketNotCreated   EventID (EventLevelError, EvtNetBSD, 59)
#define EvtNetBSD_SendmsgSocketLocked       EventID (EventLevelError, EvtNetBSD, 60)
#define EvtNetBSD_SendmsgDestAddrUndefined  EventID (EventLevelError, EvtNetBSD, 61)
#define EvtNetBSD_SendmsgMsgTooLarge        EventID (EventLevelError, EvtNetBSD,154) // End
#define EvtNetBSD_SendmsgNoMemory           EventID (EventLevelError, EvtNetBSD, 62)
#define EvtNetBSD_SendmsgDatagramError      EventID (EventLevelError, EvtNetBSD, 63)
#define EvtNetBSD_SendmsgSocketKilled       EventID (EventLevelError, EvtNetBSD, 64)
#define EvtNetBSD_SendmsgComplete           EventID (EventLevelOp,    EvtNetBSD, 65)
#define EvtNetBSD_RecvSocket                EventID (EventLevelAPI,   EvtNetBSD, 66)
#define EvtNetBSD_RecvSocketNotValid        EventID (EventLevelError, EvtNetBSD, 67)
#define EvtNetBSD_RecvInvalidParameter      EventID (EventLevelError, EvtNetBSD, 68)
#define EvtNetBSD_RecvSocketNotCreated      EventID (EventLevelError, EvtNetBSD, 69)
#define EvtNetBSD_RecvSocketNotConnected    EventID (EventLevelError, EvtNetBSD, 70)
#define EvtNetBSD_RecvSocketWrongState      EventID (EventLevelError, EvtNetBSD, 71)
#define EvtNetBSD_RecvSocketLocked          EventID (EventLevelError, EvtNetBSD, 72)
#define EvtNetBSD_RecvSocketClosed          EventID (EventLevelError, EvtNetBSD, 73)
#define EvtNetBSD_RecvTimeout               EventID (EventLevelOp,    EvtNetBSD, 74)
#define EvtNetBSD_RecvSocketKilled          EventID (EventLevelError, EvtNetBSD, 75)
#define EvtNetBSD_RecvQueueFree             EventID (EventLevelOp,    EvtNetBSD, 76)
#define EvtNetBSD_RecvComplete              EventID (EventLevelOp,    EvtNetBSD, 77)
#define EvtNetBSD_RecvmsgSocket             EventID (EventLevelAPI,   EvtNetBSD, 78)
#define EvtNetBSD_RecvmsgSocketNotValid     EventID (EventLevelError, EvtNetBSD, 79)
#define EvtNetBSD_RecvmsgInvalidParameter   EventID (EventLevelError, EvtNetBSD, 80)
#define EvtNetBSD_RecvmsgSocketNotCreated   EventID (EventLevelError, EvtNetBSD, 81)
#define EvtNetBSD_RecvmsgSocketLocked       EventID (EventLevelError, EvtNetBSD, 82)
#define EvtNetBSD_RecvmsgSocketWrongState   EventID (EventLevelError, EvtNetBSD, 83)
#define EvtNetBSD_RecvmsgTimeout            EventID (EventLevelOp,    EvtNetBSD, 84)
#define EvtNetBSD_RecvmsgSocketKilled       EventID (EventLevelError, EvtNetBSD, 85)
#define EvtNetBSD_RecvmsgComplete           EventID (EventLevelOp,    EvtNetBSD, 86)
#define EvtNetBSD_Closesocket               EventID (EventLevelAPI,   EvtNetBSD, 87)
#define EvtNetBSD_CloseSocketNotValid       EventID (EventLevelError, EvtNetBSD, 88)
#define EvtNetBSD_CloseSocketNotCreated     EventID (EventLevelError, EvtNetBSD,147)
#define EvtNetBSD_Select                    EventID (EventLevelAPI,   EvtNetBSD, 89)
#define EvtNetBSD_SelectInvalidParameter    EventID (EventLevelError, EvtNetBSD, 90)
#define EvtNetBSD_SelectSuspendFailed       EventID (EventLevelError, EvtNetBSD, 91)
#define EvtNetBSD_SelectComplete            EventID (EventLevelOp,    EvtNetBSD, 92)
#define EvtNetBSD_SelectCompleteBlocking    EventID (EventLevelOp,    EvtNetBSD, 93)
#define EvtNetBSD_Getpeername               EventID (EventLevelAPI,   EvtNetBSD, 94)
#define EvtNetBSD_GetpeerSocketNotValid     EventID (EventLevelError, EvtNetBSD, 95)
#define EvtNetBSD_GetpeerSocketNotCreated   EventID (EventLevelError, EvtNetBSD,148)
#define EvtNetBSD_GetpeerInvalidParameter   EventID (EventLevelError, EvtNetBSD, 96)
#define EvtNetBSD_GetpeerSocketNotConnected EventID (EventLevelError, EvtNetBSD, 97)
#define EvtNetBSD_Getsockname               EventID (EventLevelAPI,   EvtNetBSD, 98)
#define EvtNetBSD_GetsockSocketNotValid     EventID (EventLevelError, EvtNetBSD, 99)
#define EvtNetBSD_GetsockSocketNotCreated   EventID (EventLevelError, EvtNetBSD,149)
#define EvtNetBSD_GetsockInvalidParameter   EventID (EventLevelError, EvtNetBSD,100)
#define EvtNetBSD_GetsockSocketNotBound     EventID (EventLevelError, EvtNetBSD,101)
#define EvtNetBSD_Setsockopt                EventID (EventLevelAPI,   EvtNetBSD,102)
#define EvtNetBSD_SetoptSocketNotValid      EventID (EventLevelError, EvtNetBSD,103)
#define EvtNetBSD_SetoptInvalidParameter    EventID (EventLevelError, EvtNetBSD,104)
#define EvtNetBSD_SetoptSocketNotCreated    EventID (EventLevelError, EvtNetBSD,105)
#define EvtNetBSD_SetoptOptionNotSupported  EventID (EventLevelError, EvtNetBSD,106)
#define EvtNetBSD_SetoptKeepAlive           EventID (EventLevelOp,    EvtNetBSD,107)
#define EvtNetBSD_SetoptRecvTimeout         EventID (EventLevelOp,    EvtNetBSD,108)
#define EvtNetBSD_SetoptSendTimeout         EventID (EventLevelOp,    EvtNetBSD,109)
#define EvtNetBSD_SetoptBindToDevice        EventID (EventLevelOp,    EvtNetBSD, 12)
#define EvtNetBSD_SetoptIp4Tos              EventID (EventLevelOp,    EvtNetBSD,110)
#define EvtNetBSD_SetoptIp4Ttl              EventID (EventLevelOp,    EvtNetBSD,111)
#define EvtNetBSD_SetoptIp4RecvDstAddr      EventID (EventLevelOp,    EvtNetBSD,112)
#define EvtNetBSD_SetoptIp6Tclass           EventID (EventLevelOp,    EvtNetBSD,113)
#define EvtNetBSD_SetoptIp6HopLimit         EventID (EventLevelOp,    EvtNetBSD,114)
#define EvtNetBSD_SetoptIp6RecvDstAddr      EventID (EventLevelOp,    EvtNetBSD,115)
#define EvtNetBSD_SetoptIp6Only             EventID (EventLevelOp,    EvtNetBSD,150)
#define EvtNetBSD_SetoptSocketBound         EventID (EventLevelError, EvtNetBSD,151)
#define EvtNetBSD_Getsockopt                EventID (EventLevelAPI,   EvtNetBSD,116)
#define EvtNetBSD_GetoptSocketNotValid      EventID (EventLevelError, EvtNetBSD,117)
#define EvtNetBSD_GetoptInvalidParameter    EventID (EventLevelError, EvtNetBSD,118)
#define EvtNetBSD_GetoptSocketNotCreated    EventID (EventLevelError, EvtNetBSD,119)
#define EvtNetBSD_GetoptOptionNotSupported  EventID (EventLevelError, EvtNetBSD,120)
#define EvtNetBSD_Ioctlsocket               EventID (EventLevelAPI,   EvtNetBSD,121)
#define EvtNetBSD_IoctlSocketNotValid       EventID (EventLevelError, EvtNetBSD,122)
#define EvtNetBSD_IoctlInvalidParameter     EventID (EventLevelError, EvtNetBSD,123)
#define EvtNetBSD_IoctlSocketNotCreated     EventID (EventLevelError, EvtNetBSD,124)
#define EvtNetBSD_IoctlSocketNotStreamType  EventID (EventLevelError, EvtNetBSD,125)
#define EvtNetBSD_IoctlNonBlocking          EventID (EventLevelOp,    EvtNetBSD,126)
#define EvtNetBSD_IoctlDelayAck             EventID (EventLevelOp,    EvtNetBSD,127)
#define EvtNetBSD_IoctlKeepAlive            EventID (EventLevelOp,    EvtNetBSD,128)
#define EvtNetBSD_IoctlFlowControl          EventID (EventLevelOp,    EvtNetBSD,129)
#define EvtNetBSD_CbfuncTcpEvent            EventID (EventLevelOp,    EvtNetBSD,130)
#define EvtNetBSD_CbfuncTcpQueueAdd         EventID (EventLevelOp,    EvtNetBSD,131)
#define EvtNetBSD_CbfuncTcpNoMemory         EventID (EventLevelError, EvtNetBSD,132)
#define EvtNetBSD_CbfuncUdpQueueAdd         EventID (EventLevelOp,    EvtNetBSD,133)
#define EvtNetBSD_CbfuncUdpDumpData         EventID (EventLevelError, EvtNetBSD,134)
#define EvtNetBSD_UninitSockets             EventID (EventLevelOp,    EvtNetBSD,135)
/* BSD-Host resolver events */
#define EvtNetBSD_GetHostInit               EventID (EventLevelOp,    EvtNetBSD,136)
#define EvtNetBSD_GetHostByName             EventID (EventLevelAPI,   EvtNetBSD,137)
#define EvtNetBSD_GetHostNameResolved       EventID (EventLevelOp,    EvtNetBSD,138)
#define EvtNetBSD_GetHostNameNotExisting    EventID (EventLevelOp,    EvtNetBSD,139)
#define EvtNetBSD_GetHostResolverTimeout    EventID (EventLevelOp,    EvtNetBSD,140)
#define EvtNetBSD_GetHostResolverError      EventID (EventLevelError, EvtNetBSD,141)
#define EvtNetBSD_GetHostResolverBusy       EventID (EventLevelError, EvtNetBSD,142)
#define EvtNetBSD_GetHostInvalidParameter   EventID (EventLevelError, EvtNetBSD,143)
#define EvtNetBSD_GetHostUninit             EventID (EventLevelOp,    EvtNetBSD,144)
#endif

/**
  \brief  Event on BSD sockets API initialize (Op)
  \param  num_socks     number of available BSD sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_InitSockets(uint32_t num_socks) {
    EventRecord2 (EvtNetBSD_InitSockets, num_socks, 0);
  }
#else
  #define EvrNetBSD_InitSockets(num_socks)
#endif

/**
  \brief  Event on BSD \ref socket create (API)
  \param  sock          allocated socket descriptor
  \param  type          socket type
                         - 1: SOCK_STREAM
                         - 2: SOCK_DGRAM
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SocketCreate(int32_t sock, int32_t type) {
    EventRecord2 (EvtNetBSD_SocketCreate, (uint32_t)sock, (uint32_t)type);
  }
#else
  #define EvrNetBSD_SocketCreate(sock, type)
#endif

/**
  \brief  Event on BSD \ref socket create failed, invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SocketInvalidParameter(void) {
    EventRecord2 (EvtNetBSD_SocketInvalidParameter, 0, 0);
  }
#else
  #define EvrNetBSD_SocketInvalidParameter()
#endif

/**
  \brief  Event on BSD \ref socket create failed, no free socket available (Error)
  \param  num_socks     number of available BSD sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_NoSocketAvailable(uint32_t num_socks) {
    EventRecord2 (EvtNetBSD_NoSocketAvailable, num_socks, 0);
  }
#else
  #define EvrNetBSD_NoSocketAvailable(num_socks)
#endif

/**
  \brief  Event on BSD \ref bind socket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindSocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindSocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindSocket(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, socket already bound (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindSocketAlreadyBound(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindSocketAlreadyBound, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindSocketAlreadyBound(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, socket already connected (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindSocketConnected(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindSocketConnected, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindSocketConnected(sock)
#endif

/**
  \brief  Event on BSD \ref bind failed, port in use (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_BindPortInUse(int32_t sock) {
    EventRecord2 (EvtNetBSD_BindPortInUse, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_BindPortInUse(sock)
#endif

/**
  \brief  Event on BSD display network address (Op)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ViewNetAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetBSD_ViewNetAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetBSD_ViewNetAddress(net_addr)
#endif

/**
  \brief  Event on BSD \ref listen socket (API)
  \param  sock          socket descriptor
  \param  backlog       max number of queued connection requests
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSocket(int32_t sock, int32_t backlog) {
    EventRecord2 (EvtNetBSD_ListenSocket, (uint32_t)sock, (uint32_t)backlog);
  }
#else
  #define EvrNetBSD_ListenSocket(sock, backlog)
#endif

/**
  \brief  Event on BSD \ref listen failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, socket not bound (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSocketNotBound(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenSocketNotBound, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenSocketNotBound(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, socket not stream type (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSocketNotStreamType(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenSocketNotStreamType, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenSocketNotStreamType(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, socket already listens (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenSockAlreadyListens(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenSockAlreadyListens, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenSockAlreadyListens(sock)
#endif

/**
  \brief  Event on BSD \ref listen failed, creating socket backlog failed (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ListenBacklogCreateFailed(int32_t sock) {
    EventRecord2 (EvtNetBSD_ListenBacklogCreateFailed, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ListenBacklogCreateFailed(sock)
#endif

/**
  \brief  Event on BSD \ref accept socket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocket(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket not listening (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketNotListening(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketNotListening, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketNotListening(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket not stream type (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketNotStreamType(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketNotStreamType, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketNotStreamType(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref accept success, child socket connected (Op)
  \param  child_sock    child socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptChildSockConnected(int32_t child_sock) {
    EventRecord2 (EvtNetBSD_AcceptChildSockConnected, (uint32_t)child_sock, 0);
  }
#else
  #define EvrNetBSD_AcceptChildSockConnected(child_sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket closed by peer (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketClosed(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketClosed, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketClosed(sock)
#endif

/**
  \brief  Event on BSD \ref accept failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_AcceptSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_AcceptSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_AcceptSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref connect socket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocket(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, wrong socket state (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocketWrongState(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocketWrongState, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocketWrongState(sock)
#endif

/**
  \brief  Event on BSD \ref connect success, datagram socket connected (Op)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectDatagramSuccess(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectDatagramSuccess, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectDatagramSuccess(sock)
#endif

/**
  \brief  Event on BSD \ref connect success, stream socket connected (Op)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectStreamSuccess(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectStreamSuccess, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectStreamSuccess(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, connect request has timed out (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectStreamTimeout(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectStreamTimeout, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectStreamTimeout(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, remote host refused connection (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectStreamRefused(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectStreamRefused, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectStreamRefused(sock)
#endif

/**
  \brief  Event on BSD \ref connect failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_ConnectSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_ConnectSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_ConnectSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref send socket (API)
  \param  sock          socket descriptor
  \param  length        length of data to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocket(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_SendSocket, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_SendSocket(sock, length)
#endif

/**
  \brief  Event on BSD \ref send failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, socket not connected (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketNotConnected(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketNotConnected, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketNotConnected(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, wrong socket state (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketWrongState(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketWrongState, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketWrongState(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, socket closed by peer (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketClosed(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketClosed, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketClosed(sock)
#endif

/**
  \brief  Event on BSD \ref send complete non-blocking mode (Op)
  \param  sock          socket descriptor
  \param  num           number of bytes successfully sent
  \param  length        length of data requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendCompleteNonBlocking(int32_t sock, uint32_t num, uint32_t length) {
    uint32_t val2 = (num << 16) | length;
    EventRecord2 (EvtNetBSD_SendCompleteNonBlocking, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_SendCompleteNonBlocking(sock, num, length)
#endif

/**
  \brief  Event on BSD \ref send failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, stream socket unspecified error (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendStreamError(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendStreamError, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendStreamError(sock)
#endif

/**
  \brief  Event on BSD \ref sendto failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendtoInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendtoInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendtoInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref send failed, destination address unspecified (Error)
  \param  sock          socket descriptor
  \remark The \ref connect function was not previously called for datagram socket.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendDestAddressUndefined(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendDestAddressUndefined, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendDestAddressUndefined(sock)
#endif

/**
  \brief  Event on BSD \ref sendto message too large (Error)
  \param  sock          socket descriptor
  \param  length        length of data requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendtoMsgTooLarge(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_SendtoMsgTooLarge, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_SendtoMsgTooLarge(sock, length)
#endif

/**
  \brief  Event on BSD \ref sendto out of memory (Error)
  \param  sock          socket descriptor
  \param  length        length of data requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendtoNoMemory(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_SendtoNoMemory, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_SendtoNoMemory(sock, length)
#endif

/**
  \brief  Event on BSD \ref send failed, datagram socket unspecified error (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendDatagramError(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendDatagramError, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendDatagramError(sock)
#endif

/**
  \brief  Event on BSD \ref send complete in blocking mode (Op)
  \param  sock          socket descriptor
  \param  num           number of bytes successfully sent
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendCompleteBlocking(int32_t sock, uint32_t num) {
    EventRecord2 (EvtNetBSD_SendCompleteBlocking, (uint32_t)sock, num);
  }
#else
  #define EvrNetBSD_SendCompleteBlocking(sock, num)
#endif

/**
  \brief  Event on BSD \ref send timeout in blocking mode (Op)
  \param  sock          socket descriptor
  \param  num           number of bytes successfully sent
  \param  length        length of data requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendTimeoutBlocking(int32_t sock, uint32_t num, uint32_t length) {
    uint32_t val2 = (num << 16) | length;
    EventRecord2 (EvtNetBSD_SendTimeoutBlocking, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_SendTimeoutBlocking(sock, num, length)
#endif

/**
  \brief  Event on BSD \ref send out of memory (Op)
  \param  sock          socket descriptor
  \param  num           number of bytes successfully sent
  \param  length        length of data requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendNoMemory(int32_t sock, uint32_t num, uint32_t length) {
    uint32_t val2 = (num << 16) | length;
    EventRecord2 (EvtNetBSD_SendNoMemory, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_SendNoMemory(sock, num, length)
#endif

/**
  \brief  Event on BSD \ref sendmsg socket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgSocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgSocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgSocket(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, destination address unspecified (Error)
  \param  sock          socket descriptor
  \remark The \ref connect function was not previously called for datagram socket.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgDestAddrUndefined(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgDestAddrUndefined, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgDestAddrUndefined(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg message too large (Error)
  \param  sock          socket descriptor
  \param  length        length of message requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgMsgTooLarge(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_SendmsgMsgTooLarge, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_SendmsgMsgTooLarge(sock, length)
#endif

/**
  \brief  Event on BSD \ref sendmsg out of memory (Error)
  \param  sock          socket descriptor
  \param  length        length of message requested to send
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgNoMemory(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_SendmsgNoMemory, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_SendmsgNoMemory(sock, length)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, datagram socket unspecified error (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgDatagramError(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgDatagramError, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgDatagramError(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_SendmsgSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SendmsgSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref sendmsg complete (Op)
  \param  sock          socket descriptor
  \param  num           number of bytes successfully sent
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SendmsgComplete(int32_t sock, uint32_t num) {
    EventRecord2 (EvtNetBSD_SendmsgComplete, (uint32_t)sock, num);
  }
#else
  #define EvrNetBSD_SendmsgComplete(sock, num)
#endif

/**
  \brief  Event on BSD \ref recv socket (API)
  \param  sock          socket descriptor
  \param  length        length of data to receive
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocket(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_RecvSocket, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_RecvSocket(sock, length)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket not connected (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketNotConnected(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketNotConnected, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketNotConnected(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, wrong socket state (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketWrongState(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketWrongState, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketWrongState(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket closed by peer (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketClosed(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketClosed, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketClosed(sock)
#endif

/**
  \brief  Event on BSD \ref recv timeout (Op)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvTimeout(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvTimeout, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvTimeout(sock)
#endif

/**
  \brief  Event on BSD \ref recv failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref recv queue free (Op)
  \param  sock          socket descriptor
  \param  length        length of data to release
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvQueueFree(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_RecvQueueFree, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_RecvQueueFree(sock, length)
#endif

/**
  \brief  Event on BSD \ref recv complete (Op)
  \param  sock          socket descriptor
  \param  length        length of data received
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvComplete(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_RecvComplete, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_RecvComplete(sock, length)
#endif

/**
  \brief  Event on BSD \ref recvmsg socket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocket(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, socket locked by another thread (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocketLocked(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocketLocked, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocketLocked(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, wrong socket state (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocketWrongState(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocketWrongState, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocketWrongState(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg timeout (Op)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgTimeout(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgTimeout, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgTimeout(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg failed, socket killed locally (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgSocketKilled(int32_t sock) {
    EventRecord2 (EvtNetBSD_RecvmsgSocketKilled, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_RecvmsgSocketKilled(sock)
#endif

/**
  \brief  Event on BSD \ref recvmsg complete (Op)
  \param  sock          socket descriptor
  \param  length        length of data received
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_RecvmsgComplete(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_RecvmsgComplete, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_RecvmsgComplete(sock, length)
#endif

/**
  \brief  Event on BSD \ref closesocket (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Closesocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_Closesocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_Closesocket(sock)
#endif

/**
  \brief  Event on BSD \ref closesocket failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CloseSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_CloseSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_CloseSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref closesocket failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CloseSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_CloseSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_CloseSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref select (API)
  \param  nfds          range of sockets to be tested
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Select(int32_t nfds) {
    EventRecord2 (EvtNetBSD_Select, (uint32_t)nfds, 0);
  }
#else
  #define EvrNetBSD_Select(nfds)
#endif


/**
  \brief  Event on BSD \ref select failed, invalid parameter (Error)
  \param  nfds          range of sockets to be tested
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SelectInvalidParameter(int32_t nfds) {
    EventRecord2 (EvtNetBSD_SelectInvalidParameter, (uint32_t)nfds, 0);
  }
#else
  #define EvrNetBSD_SelectInvalidParameter(nfds)
#endif

/**
  \brief  Event on BSD \ref select suspend failed (Error)
  \param  nfds          range of sockets to be subscribed to resume events
  \remark Subscription to resume events has failed, no sockets subscribed.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SelectSuspendFailed(int32_t nfds) {
    EventRecord2 (EvtNetBSD_SelectSuspendFailed, (uint32_t)nfds, 0);
  }
#else
  #define EvrNetBSD_SelectSuspendFailed(nfds)
#endif

/**
  \brief  Event on BSD \ref select complete non-blocking (Op)
  \param  n_ready       number of ready sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SelectComplete(int32_t n_ready) {
    EventRecord2 (EvtNetBSD_SelectComplete, (uint32_t)n_ready, 0);
  }
#else
  #define EvrNetBSD_SelectComplete(n_ready)
#endif

/**
  \brief  Event on BSD \ref select complete blocking (Op)
  \param  n_ready       number of ready sockets
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SelectCompleteBlocking(int32_t n_ready) {
    EventRecord2 (EvtNetBSD_SelectCompleteBlocking, (uint32_t)n_ready, 0);
  }
#else
  #define EvrNetBSD_SelectCompleteBlocking(n_ready)
#endif

/**
  \brief  Event on BSD \ref getpeername (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Getpeername(int32_t sock) {
    EventRecord2 (EvtNetBSD_Getpeername, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_Getpeername(sock)
#endif

/**
  \brief  Event on BSD \ref getpeername failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetpeerSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetpeerSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetpeerSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref getpeername failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetpeerSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetpeerSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetpeerSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref getpeername failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetpeerInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetpeerInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetpeerInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref getpeername failed, socket not connected (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetpeerSocketNotConnected(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetpeerSocketNotConnected, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetpeerSocketNotConnected(sock)
#endif

/**
  \brief  Event on BSD \ref getsockname (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Getsockname(int32_t sock) {
    EventRecord2 (EvtNetBSD_Getsockname, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_Getsockname(sock)
#endif

/**
  \brief  Event on BSD \ref getsockname failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetsockSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetsockSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetsockSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref getsockname failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetsockSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetsockSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetsockSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref getsockname failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetsockInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetsockInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetsockInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref getsockname failed, socket not bound (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetsockSocketNotBound(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetsockSocketNotBound, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetsockSocketNotBound(sock)
#endif

/**
  \brief  Event on BSD \ref setsockopt (API)
  \param  sock          socket descriptor
  \param  level         level at which the option is defined
  \param  optname       socket option to be set
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Setsockopt(int32_t sock, int32_t level, int32_t optname) {
    uint32_t val2 = (uint32_t)((level << 4) | optname);
    EventRecord2 (EvtNetBSD_Setsockopt, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_Setsockopt(sock, level, optname)
#endif

/**
  \brief  Event on BSD \ref setsockopt failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_SetoptSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SetoptSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref setsockopt failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_SetoptInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SetoptInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref setsockopt failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_SetoptSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SetoptSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref setsockopt failed, option not supported by the protocol (Error)
  \param  sock          socket descriptor
  \param  level         level at which the option is defined
  \param  optname       socket option to be set
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptOptionNotSupported(int32_t sock, int32_t level, int32_t optname) {
    uint32_t val2 = (uint32_t)((level << 4) | optname);
    EventRecord2 (EvtNetBSD_SetoptOptionNotSupported, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_SetoptOptionNotSupported(sock, level, optname)
#endif

/**
  \brief  Event on BSD \ref setsockopt keep-alive option (Op)
  \param  sock          socket descriptor
  \param  enable        keep-alive state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptKeepAlive(int32_t sock, uint32_t enable) {
    EventRecord2 (EvtNetBSD_SetoptKeepAlive, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_SetoptKeepAlive(sock, enable)
#endif

/**
  \brief  Event on BSD \ref setsockopt receive timeout (Op)
  \param  sock          socket descriptor
  \param  timeout       receive timeout in ms
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptRecvTimeout(int32_t sock, uint32_t timeout) {
    EventRecord2 (EvtNetBSD_SetoptRecvTimeout, (uint32_t)sock, timeout);
  }
#else
  #define EvrNetBSD_SetoptRecvTimeout(sock, timeout)
#endif

/**
  \brief  Event on BSD \ref setsockopt send timeout (Op)
  \param  sock          socket descriptor
  \param  timeout       send timeout in ms
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptSendTimeout(int32_t sock, uint32_t timeout) {
    EventRecord2 (EvtNetBSD_SetoptSendTimeout, (uint32_t)sock, timeout);
  }
#else
  #define EvrNetBSD_SetoptSendTimeout(sock, timeout)
#endif

/**
  \brief  Event on BSD \ref setsockopt bind to device (Op)
  \param  sock          socket descriptor
  \param  if_id         network interface identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptBindToDevice(int32_t sock, uint16_t if_id) {
    EventRecord2 (EvtNetBSD_SetoptBindToDevice, (uint32_t)sock, if_id);
  }
#else
  #define EvrNetBSD_SetoptBindToDevice(sock, if_id)
#endif

/**
  \brief  Event on BSD \ref setsockopt TOS for IPv4 (Op)
  \param  sock          socket descriptor
  \param  ip4_tos       type of service value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp4Tos(int32_t sock, uint8_t ip4_tos) {
    EventRecord2 (EvtNetBSD_SetoptIp4Tos, (uint32_t)sock, ip4_tos);
  }
#else
  #define EvrNetBSD_SetoptIp4Tos(sock, ip4_tos)
#endif

/**
  \brief  Event on BSD \ref setsockopt TTL for IPv4 (Op)
  \param  sock          socket descriptor
  \param  ip4_ttl       time to live value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp4Ttl(int32_t sock, uint8_t ip4_ttl) {
    EventRecord2 (EvtNetBSD_SetoptIp4Ttl, (uint32_t)sock, ip4_ttl);
  }
#else
  #define EvrNetBSD_SetoptIp4Ttl(sock, ip4_ttl)
#endif

/**
  \brief  Event on BSD \ref setsockopt receive destination address for IPv4 (Op)
  \param  sock          socket descriptor
  \param  enable        receive dst address state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp4RecvDstAddr(int32_t sock, uint32_t enable) {
    EventRecord2 (EvtNetBSD_SetoptIp4RecvDstAddr, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_SetoptIp4RecvDstAddr(sock, enable)
#endif

/**
  \brief  Event on BSD \ref setsockopt Traffic Class for IPv6 (Op)
  \param  sock          socket descriptor
  \param  ip6_tclass    traffic class value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp6Tclass(int32_t sock, uint8_t ip6_tclass) {
    EventRecord2 (EvtNetBSD_SetoptIp6Tclass, (uint32_t)sock, ip6_tclass);
  }
#else
  #define EvrNetBSD_SetoptIp6Tclass(sock, ip6_tclass)
#endif

/**
  \brief  Event on BSD \ref setsockopt Hop Limit for IPv6 (Op)
  \param  sock          socket descriptor
  \param  ip6_hoplimit  hop limit value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp6HopLimit(int32_t sock, uint8_t ip6_hoplimit) {
    EventRecord2 (EvtNetBSD_SetoptIp6HopLimit, (uint32_t)sock, ip6_hoplimit);
  }
#else
  #define EvrNetBSD_SetoptIp6HopLimit(sock, ip6_hoplimit)
#endif

/**
  \brief  Event on BSD \ref setsockopt receive destination address for IPv6 (Op)
  \param  sock          socket descriptor
  \param  enable        receive dst address state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp6RecvDstAddr(int32_t sock, uint32_t enable) {
    EventRecord2 (EvtNetBSD_SetoptIp6RecvDstAddr, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_SetoptIp6RecvDstAddr(sock, enable)
#endif

/**
  \brief  Event on BSD \ref setsockopt IPv6 only for IPv6 socket (Op)
  \param  sock          socket descriptor
  \param  enable        IPv6 only mode value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptIp6Only(int32_t sock, uint32_t enable) {
    EventRecord2 (EvtNetBSD_SetoptIp6Only, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_SetoptIp6Only(sock, enable)
#endif

/**
  \brief  Event on BSD \ref setsockopt failed, socket already bound (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_SetoptSocketBound(int32_t sock) {
    EventRecord2 (EvtNetBSD_SetoptSocketBound, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_SetoptSocketBound(sock)
#endif

/**
  \brief  Event on BSD \ref getsockopt (API)
  \param  sock          socket descriptor
  \param  level         level at which the option is defined
  \param  optname       socket option to be retrieved
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Getsockopt(int32_t sock, int32_t level, int32_t optname) {
    uint32_t val2 = (uint32_t)((level << 4) | optname);
    EventRecord2 (EvtNetBSD_Getsockopt, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_Getsockopt(sock, level, optname)
#endif

/**
  \brief  Event on BSD \ref getsockopt failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetoptSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetoptSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetoptSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref getsockopt failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetoptInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetoptInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetoptInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref getsockopt failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetoptSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_GetoptSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_GetoptSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref getsockopt failed, option not supported by the protocol (Error)
  \param  sock          socket descriptor
  \param  level         level at which the option is defined
  \param  optname       socket option to be retrieved
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetoptOptionNotSupported(int32_t sock, int32_t level, int32_t optname) {
    uint32_t val2 = (uint32_t)((level << 4) | optname);
    EventRecord2 (EvtNetBSD_GetoptOptionNotSupported, (uint32_t)sock, val2);
  }
#else
  #define EvrNetBSD_GetoptOptionNotSupported(sock, level, optname)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket control (API)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_Ioctlsocket(int32_t sock) {
    EventRecord2 (EvtNetBSD_Ioctlsocket, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_Ioctlsocket(sock)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket control failed, socket not valid (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlSocketNotValid(int32_t sock) {
    EventRecord2 (EvtNetBSD_IoctlSocketNotValid, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_IoctlSocketNotValid(sock)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket control failed, invalid parameter (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlInvalidParameter(int32_t sock) {
    EventRecord2 (EvtNetBSD_IoctlInvalidParameter, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_IoctlInvalidParameter(sock)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket control failed, socket not created (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlSocketNotCreated(int32_t sock) {
    EventRecord2 (EvtNetBSD_IoctlSocketNotCreated, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_IoctlSocketNotCreated(sock)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket control failed, socket not stream type (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlSocketNotStreamType(int32_t sock) {
    EventRecord2 (EvtNetBSD_IoctlSocketNotStreamType, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_IoctlSocketNotStreamType(sock)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket enable non-blocking mode (Op)
  \param  sock          socket descriptor
  \param  enable        non-blocking state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlNonBlocking(int32_t sock, uint32_t enable) {
    if (enable) enable = 1;
    EventRecord2 (EvtNetBSD_IoctlNonBlocking, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_IoctlNonBlocking(sock, enable)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket enable TCP delay-ack mode (Op)
  \param  sock          socket descriptor
  \param  enable        delay-ack state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlDelayAck(int32_t sock, uint32_t enable) {
    if (enable) enable = 1;
    EventRecord2 (EvtNetBSD_IoctlDelayAck, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_IoctlDelayAck(sock, enable)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket enable TCP keep-alive mode (Op)
  \param  sock          socket descriptor
  \param  enable        keep-alive state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlKeepAlive(int32_t sock, uint32_t enable) {
    if (enable) enable = 1;
    EventRecord2 (EvtNetBSD_IoctlKeepAlive, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_IoctlKeepAlive(sock, enable)
#endif

/**
  \brief  Event on BSD \ref ioctlsocket enable TCP flow-control mode (Op)
  \param  sock          socket descriptor
  \param  enable        flow-control state value
                         - 0: disable
                         - 1: enable
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_IoctlFlowControl(int32_t sock, uint32_t enable) {
    if (enable) enable = 1;
    EventRecord2 (EvtNetBSD_IoctlFlowControl, (uint32_t)sock, enable);
  }
#else
  #define EvrNetBSD_IoctlFlowControl(sock, enable)
#endif

/**
  \brief  Event on BSD callback TCP function events (Op)
  \param  sock          socket descriptor
  \param  tcp_event     TCP socket event
                         - 0: netTCP_EventConnect
                         - 1: netTCP_EventEstablished
                         - 2: netTCP_EventClosed
                         - 3: netTCP_EventAborted
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CbfuncTcpEvent(int32_t sock, int32_t tcp_event) {
    EventRecord2 (EvtNetBSD_CbfuncTcpEvent, (uint32_t)sock, (uint32_t)tcp_event);
  }
#else
  #define EvrNetBSD_CbfuncTcpEvent(sock, tcp_event)
#endif

/**
  \brief  Event on BSD callback TCP data received (Op)
  \param  sock          socket descriptor
  \param  length        length of data received
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CbfuncTcpQueueAdd(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_CbfuncTcpQueueAdd, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_CbfuncTcpQueueAdd(sock, length)
#endif

/**
  \brief  Event on BSD callback no memory for TCP data (Error)
  \param  sock          socket descriptor
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CbfuncTcpNoMemory(int32_t sock) {
    EventRecord2 (EvtNetBSD_CbfuncTcpNoMemory, (uint32_t)sock, 0);
  }
#else
  #define EvrNetBSD_CbfuncTcpNoMemory(sock)
#endif

/**
  \brief  Event on BSD callback UDP data received (Op)
  \param  sock          socket descriptor
  \param  length        length of data received
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CbfuncUdpQueueAdd(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_CbfuncUdpQueueAdd, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_CbfuncUdpQueueAdd(sock, length)
#endif

/**
  \brief  Event on BSD callback no memory for UDP data (Error)
  \param  sock          socket descriptor
  \param  length        length of dumped data
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_CbfuncUdpDumpData(int32_t sock, uint32_t length) {
    EventRecord2 (EvtNetBSD_CbfuncUdpDumpData, (uint32_t)sock, length);
  }
#else
  #define EvrNetBSD_CbfuncUdpDumpData(sock, length)
#endif

/**
  \brief  Event on BSD de-initialize available sockets (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_UninitSockets(void) {
    EventRecord2 (EvtNetBSD_UninitSockets, 0, 0);
  }
#else
  #define EvrNetBSD_UninitSockets()
#endif

// BSD-Host resolver event recorder functions ----------------------------------

/**
  \brief  Event on BSD hostname resolver initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostInit(void) {
    EventRecord2 (EvtNetBSD_GetHostInit, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostInit()
#endif

/**
  \brief  Event on BSD \ref gethostbyname (API)
  \param  hostname      name of the host to resolve
  \param  length        length of the hostname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostByName(const char *hostname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetBSD_GetHostByName, hostname, length);
  }
#else
  #define EvrNetBSD_GetHostByName(hostname, length)
#endif

/**
  \brief  Event on BSD hostname resolved successfully (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostNameResolved(void) {
    EventRecord2 (EvtNetBSD_GetHostNameResolved, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostNameResolved()
#endif

/**
  \brief  Event on BSD requested hostname not existing (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostNameNotExisting(void) {
    EventRecord2 (EvtNetBSD_GetHostNameNotExisting, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostNameNotExisting()
#endif

/**
  \brief  Event on BSD resolver timeout expired (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostResolverTimeout(void) {
    EventRecord2 (EvtNetBSD_GetHostResolverTimeout, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostResolverTimeout()
#endif

/**
  \brief  Event on BSD resolver protocol error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostResolverError(void) {
    EventRecord2 (EvtNetBSD_GetHostResolverError, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostResolverError()
#endif

/**
  \brief  Event on BSD resolver busy error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostResolverBusy(void) {
    EventRecord2 (EvtNetBSD_GetHostResolverBusy, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostResolverBusy()
#endif

/**
  \brief  Event on BSD \ref gethostbyname invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostInvalidParameter(void) {
    EventRecord2 (EvtNetBSD_GetHostInvalidParameter, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostInvalidParameter()
#endif

/**
  \brief  Event on BSD hostname resolver de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetBSD_GetHostUninit(void) {
    EventRecord2 (EvtNetBSD_GetHostUninit, 0, 0);
  }
#else
  #define EvrNetBSD_GetHostUninit()
#endif


// NetHTTPs event identifiers --------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetHTTPs_InitServer              EventID (EventLevelOp,    EvtNetHTTPs,  0)
#define EvtNetHTTPs_ViewRootFolder          EventID (EventLevelOp,    EvtNetHTTPs,  1)
#define EvtNetHTTPs_GetSocketFailed         EventID (EventLevelError, EvtNetHTTPs,  2)
#define EvtNetHTTPs_SetRootPath             EventID (EventLevelAPI,   EvtNetHTTPs,  3)
#define EvtNetHTTPs_SetUsername             EventID (EventLevelAPI,   EvtNetHTTPs,  4)
#define EvtNetHTTPs_SetPassword             EventID (EventLevelAPI,   EvtNetHTTPs,  5)
#define EvtNetHTTPs_StartService            EventID (EventLevelOp,    EvtNetHTTPs,  6)
#define EvtNetHTTPs_StopService             EventID (EventLevelOp,    EvtNetHTTPs,  7)
#define EvtNetHTTPs_FileNotFound            EventID (EventLevelError, EvtNetHTTPs,  8)
#define EvtNetHTTPs_FileAccessForbidden     EventID (EventLevelError, EvtNetHTTPs,  9)
#define EvtNetHTTPs_FileCached              EventID (EventLevelOp,    EvtNetHTTPs, 10)
#define EvtNetHTTPs_CloseSession            EventID (EventLevelOp,    EvtNetHTTPs, 11)
#define EvtNetHTTPs_SendFile                EventID (EventLevelOp,    EvtNetHTTPs, 12)
#define EvtNetHTTPs_CgiSendFile             EventID (EventLevelOp,    EvtNetHTTPs, 13)
#define EvtNetHTTPs_CgiIncludeFile          EventID (EventLevelOp,    EvtNetHTTPs, 14)
#define EvtNetHTTPs_CgiStopEngine           EventID (EventLevelOp,    EvtNetHTTPs, 15)
#define EvtNetHTTPs_CgiScriptError          EventID (EventLevelError, EvtNetHTTPs, 16)
#define EvtNetHTTPs_UnauthorizedAccess      EventID (EventLevelError, EvtNetHTTPs, 17)
#define EvtNetHTTPs_MethodNotImplemented    EventID (EventLevelError, EvtNetHTTPs, 18)
#define EvtNetHTTPs_UserAccessDenied        EventID (EventLevelError, EvtNetHTTPs, 19)
#define EvtNetHTTPs_SetCookie               EventID (EventLevelAPI,   EvtNetHTTPs, 20)
#define EvtNetHTTPs_SessionOpen             EventID (EventLevelOp,    EvtNetHTTPs, 21)
#define EvtNetHTTPs_SocketAborted           EventID (EventLevelOp,    EvtNetHTTPs, 22)
#define EvtNetHTTPs_SocketClosed            EventID (EventLevelOp,    EvtNetHTTPs, 23)
#define EvtNetHTTPs_ReceiveFrame            EventID (EventLevelOp,    EvtNetHTTPs, 24)
#define EvtNetHTTPs_FrameTooShort           EventID (EventLevelError, EvtNetHTTPs, 25)
#define EvtNetHTTPs_RequestMethod           EventID (EventLevelOp,    EvtNetHTTPs, 26)
#define EvtNetHTTPs_CgiStartEngine          EventID (EventLevelOp,    EvtNetHTTPs, 27)
#define EvtNetHTTPs_RequestedFile           EventID (EventLevelOp,    EvtNetHTTPs, 28)
#define EvtNetHTTPs_FileOpenDrive           EventID (EventLevelOp,    EvtNetHTTPs, 29)
#define EvtNetHTTPs_ViewFileStatus          EventID (EventLevelOp,    EvtNetHTTPs, 30)
#define EvtNetHTTPs_ViewRedirectionUrl      EventID (EventLevelOp,    EvtNetHTTPs, 31)
#define EvtNetHTTPs_ViewUserCredentials     EventID (EventLevelOp,    EvtNetHTTPs, 32)
#define EvtNetHTTPs_ViewAcceptLanguage      EventID (EventLevelOp,    EvtNetHTTPs, 33)
#define EvtNetHTTPs_ViewCookie              EventID (EventLevelOp,    EvtNetHTTPs, 45) // End
#define EvtNetHTTPs_ViewXmlType             EventID (EventLevelOp,    EvtNetHTTPs, 34)
#define EvtNetHTTPs_PostXmlEncoded          EventID (EventLevelOp,    EvtNetHTTPs, 35)
#define EvtNetHTTPs_PostCompleteMultipacket EventID (EventLevelOp,    EvtNetHTTPs, 36)
#define EvtNetHTTPs_PostFormUrlEncoded      EventID (EventLevelOp,    EvtNetHTTPs, 37)
#define EvtNetHTTPs_PostDataTruncated       EventID (EventLevelError, EvtNetHTTPs, 38)
#define EvtNetHTTPs_FileUploadRequested     EventID (EventLevelOp,    EvtNetHTTPs, 39)
#define EvtNetHTTPs_FileUploadDataReceived  EventID (EventLevelOp,    EvtNetHTTPs, 40)
#define EvtNetHTTPs_FileUploadRemaining     EventID (EventLevelOp,    EvtNetHTTPs, 41)
#define EvtNetHTTPs_FileUploadComplete      EventID (EventLevelOp,    EvtNetHTTPs, 42)
#define EvtNetHTTPs_TlsGetContextFailed     EventID (EventLevelError, EvtNetHTTPs, 43)
#define EvtNetHTTPs_UninitServer            EventID (EventLevelOp,    EvtNetHTTPs, 44)
#endif

/**
  \brief  Event on HTTP server initialize (Op)
  \param  num_sessions  number of available HTTP sessions
  \param  port          listening port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_InitServer(uint32_t num_sessions, uint16_t port) {
    EventRecord2 (EvtNetHTTPs_InitServer, num_sessions, port);
  }
#else
  #define EvrNetHTTPs_InitServer(num_sessions, port)
#endif

/**
  \brief  Event on HTTP server display root folder (Op)
  \param  root_folder   path to server root folder
  \param  length        length of the root_folder string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewRootFolder(const char *root_folder, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetHTTPs_ViewRootFolder, root_folder, length);
  }
#else
  #define EvrNetHTTPs_ViewRootFolder(root_folder, length)
#endif

/**
  \brief  Event on HTTP server failed to allocate TCP socket (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_GetSocketFailed(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_GetSocketFailed, session, 0);
  }
#else
  #define EvrNetHTTPs_GetSocketFailed(session)
#endif

/**
  \brief  Event on HTTP server \ref netHTTPs_SetRootPath (API)
  \param  path          path to server root folder
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SetRootPath(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetHTTPs_SetRootPath, path, length);
  }
#else
  #define EvrNetHTTPs_SetRootPath(path, length)
#endif

/**
  \brief  Event on HTTP server \ref netHTTPs_SetUsername (API)
  \param  username      new username
  \param  length        length of the username string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SetUsername(const char *username, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetHTTPs_SetUsername, username, length);
  }
#else
  #define EvrNetHTTPs_SetUsername(username, length)
#endif

/**
  \brief  Event on HTTP server \ref netHTTPs_SetPassword (API)
  \param  password      new password
  \param  length        length of the password string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SetPassword(const char *password, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetHTTPs_SetPassword, password, length);
  }
#else
  #define EvrNetHTTPs_SetPassword(password, length)
#endif

/**
  \brief  Event on HTTP server start service (Op)
  \param  port          listening port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_StartService(uint16_t port) {
    EventRecord2 (EvtNetHTTPs_StartService, port, 0);
  }
#else
  #define EvrNetHTTPs_StartService(port)
#endif

/**
  \brief  Event on HTTP server stop service (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_StopService(void) {
    EventRecord2 (EvtNetHTTPs_StopService, 0, 0);
  }
#else
  #define EvrNetHTTPs_StopService()
#endif

/**
  \brief  Event on HTTP server requested resource file not found (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileNotFound(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_FileNotFound, session, 0);
  }
#else
  #define EvrNetHTTPs_FileNotFound(session)
#endif

/**
  \brief  Event on HTTP server access to requested resource not allowed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileAccessForbidden(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_FileAccessForbidden, session, 0);
  }
#else
  #define EvrNetHTTPs_FileAccessForbidden(session)
#endif

/**
  \brief  Event on HTTP server requested resource file cached on the client (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileCached(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_FileCached, session, 0);
  }
#else
  #define EvrNetHTTPs_FileCached(session)
#endif

/**
  \brief  Event on HTTP server session close (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CloseSession(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CloseSession, session, 0);
  }
#else
  #define EvrNetHTTPs_CloseSession(session)
#endif

/**
  \brief  Event on HTTP server send a resource file (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SendFile(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_SendFile, session, 0);
  }
#else
  #define EvrNetHTTPs_SendFile(session)
#endif

/**
  \brief  Event on HTTP server CGI send dynamic resource file (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CgiSendFile(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CgiSendFile, session, 0);
  }
#else
  #define EvrNetHTTPs_CgiSendFile(session)
#endif

/**
  \brief  Event on HTTP server CGI include a file (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CgiIncludeFile(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CgiIncludeFile, session, 0);
  }
#else
  #define EvrNetHTTPs_CgiIncludeFile(session)
#endif

/**
  \brief  Event on HTTP server stop CGI script interpreter engine (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CgiStopEngine(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CgiStopEngine, session, 0);
  }
#else
  #define EvrNetHTTPs_CgiStopEngine(session)
#endif

/**
  \brief  Event on HTTP server CGI script error found (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CgiScriptError(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CgiScriptError, session, 0);
  }
#else
  #define EvrNetHTTPs_CgiScriptError(session)
#endif

/**
  \brief  Event on HTTP server unauthorized access (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_UnauthorizedAccess(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_UnauthorizedAccess, session, 0);
  }
#else
  #define EvrNetHTTPs_UnauthorizedAccess(session)
#endif

/**
  \brief  Event on HTTP server requested method not implemented (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_MethodNotImplemented(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_MethodNotImplemented, session, 0);
  }
#else
  #define EvrNetHTTPs_MethodNotImplemented(session)
#endif

/**
  \brief  Event on HTTP server user denied access (Error)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_UserAccessDenied(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetHTTPs_UserAccessDenied, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetHTTPs_UserAccessDenied(net_addr)
#endif

/**
  \brief  Event on HTTP server \ref netCGI_SetCookie (API)
  \param  cookie        cookie to send to the client
  \param  length        length of the cookie string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SetCookie(const char *cookie, uint32_t length) {
    if (length > 120) length = 120;
    EventRecordData (EvtNetHTTPs_SetCookie, cookie, length);
  }
#else
  #define EvrNetHTTPs_SetCookie(cookie, length)
#endif

/**
  \brief  Event on HTTP server session open (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SessionOpen(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_SessionOpen, session, 0);
  }
#else
  #define EvrNetHTTPs_SessionOpen(session)
#endif

/**
  \brief  Event on HTTP server socket aborted (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SocketAborted(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_SocketAborted, session, 0);
  }
#else
  #define EvrNetHTTPs_SocketAborted(session)
#endif

/**
  \brief  Event on HTTP server socket closed (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_SocketClosed(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_SocketClosed, session, 0);
  }
#else
  #define EvrNetHTTPs_SocketClosed(session)
#endif

/**
  \brief  Event on HTTP server receive frame (Op)
  \param  session       session number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ReceiveFrame(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetHTTPs_ReceiveFrame, session, length);
  }
#else
  #define EvrNetHTTPs_ReceiveFrame(session, length)
#endif

/**
  \brief  Event on HTTP server receive frame too short (Error)
  \param  session       session number
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FrameTooShort(uint8_t session, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetHTTPs_FrameTooShort, session, val2);
  }
#else
  #define EvrNetHTTPs_FrameTooShort(session, length, min_length)
#endif

/**
  \brief  Event on HTTP server display requested method (Op)
  \param  session       session number
  \param  method        http request method
                         - 1: GET
                         - 2: POST
                         - 3: HEAD
                         - 4: PUT
                         - 5: DELETE
                         - 6: CONNECT
                         - 7: OPTIONS
                         - 8: TRACE
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_RequestMethod(uint8_t session, uint8_t method) {
    EventRecord2 (EvtNetHTTPs_RequestMethod, session, method);
  }
#else
  #define EvrNetHTTPs_RequestMethod(session, method)
#endif

/**
  \brief  Event on HTTP server start CGI script interpreter engine (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_CgiStartEngine(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_CgiStartEngine, session, 0);
  }
#else
  #define EvrNetHTTPs_CgiStartEngine(session)
#endif

/**
  \brief  Event on HTTP server requested resource file (Op)
  \param  fname         name of the requested file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_RequestedFile(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetHTTPs_RequestedFile, fname, length);
  }
#else
  #define EvrNetHTTPs_RequestedFile(fname, length)
#endif

/**
  \brief  Event on HTTP server resource file location (Op)
  \param  drive         where the resource file is stored
                         - 0: ROM Image
                         - 1: External Drive
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileOpenDrive(uint32_t drive) {
    EventRecord2 (EvtNetHTTPs_FileOpenDrive, drive, 0);
  }
#else
  #define EvrNetHTTPs_FileOpenDrive(drive)
#endif

/**
  \brief  Event on HTTP server display open file status (Op)
  \param  size          size of the open file
  \param  lm_time       last-modified time of the file
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewFileStatus(uint32_t size, uint32_t lm_time) {
    EventRecord2 (EvtNetHTTPs_ViewFileStatus, size, lm_time);
  }
#else
  #define EvrNetHTTPs_ViewFileStatus(size, lm_time)
#endif

/**
  \brief  Event on HTTP server display resource file redirection URL (Op)
  \param  url           redirection url in text format
  \param  length        length of the url string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewRedirectionUrl(const char *url, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetHTTPs_ViewRedirectionUrl, url, length);
  }
#else
  #define EvrNetHTTPs_ViewRedirectionUrl(url, length)
#endif

/**
  \brief  Event on HTTP server display user credentials (Op)
  \param  credentials   user credentials in the form of: "username:password"
  \param  length        length of the credentials string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewUserCredentials(const char *credentials, uint32_t length) {
    if (length > 40) length = 40;
    EventRecordData (EvtNetHTTPs_ViewUserCredentials, credentials, length);
  }
#else
  #define EvrNetHTTPs_ViewUserCredentials(credentials, length)
#endif

/**
  \brief  Event on HTTP server display accept language http header (Op)
  \param  accept_lang   accept language header content
  \param  length        length of the accept_lang string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewAcceptLanguage(const char *accept_lang, uint32_t length) {
    if (length > 8) length = 8;
    EventRecordData (EvtNetHTTPs_ViewAcceptLanguage, accept_lang, length);
  }
#else
  #define EvrNetHTTPs_ViewAcceptLanguage(accept_lang, length)
#endif

/**
  \brief  Event on HTTP server display cookie header (Op)
  \param  cookie        cookie header content
  \param  length        length of the cookie header
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewCookie(const char *cookie, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetHTTPs_ViewCookie, cookie, length);
  }
#else
  #define EvrNetHTTPs_ViewCookie(cookie, length)
#endif

/**
  \brief  Event on HTTP server display xml content type header (Op)
  \param  xml_type      xml content type in text format
  \param  length        length of the xml_type string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_ViewXmlType(const char *xml_type, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetHTTPs_ViewXmlType, xml_type, length);
  }
#else
  #define EvrNetHTTPs_ViewXmlType(xml_type, length)
#endif

/**
  \brief  Event on HTTP server POST type: xml-encoded (Op)
  \param  length        content-length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_PostXmlEncoded(uint32_t length) {
    EventRecord2 (EvtNetHTTPs_PostXmlEncoded, length, 0);
  }
#else
  #define EvrNetHTTPs_PostXmlEncoded(length)
#endif

/**
  \brief  Event on HTTP server POST complete for multipacket POST (Op)
  \param  length        data length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_PostCompleteMultipacket(uint32_t length) {
    EventRecord2 (EvtNetHTTPs_PostCompleteMultipacket, length, 0);
  }
#else
  #define EvrNetHTTPs_PostCompleteMultipacket(length)
#endif

/**
  \brief  Event on HTTP server POST type: www-form-urlencoded (Op)
  \param  length        content-length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_PostFormUrlEncoded(uint32_t length) {
    EventRecord2 (EvtNetHTTPs_PostFormUrlEncoded, length, 0);
  }
#else
  #define EvrNetHTTPs_PostFormUrlEncoded(length)
#endif

/**
  \brief  Event on HTTP server POST data truncated in buffer mode (Error)
  \param  new_length    truncated data length
  \remark Buffer mode is used for www-form-urlencoded POST type only.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_PostDataTruncated(uint32_t new_length) {
    EventRecord2 (EvtNetHTTPs_PostDataTruncated, new_length, 0);
  }
#else
  #define EvrNetHTTPs_PostDataTruncated(new_length)
#endif

/**
  \brief  Event on HTTP server file upload requested (Op)
  \param  fname         name of the requested file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileUploadRequested(const char *fname, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetHTTPs_FileUploadRequested, fname, length);
  }
#else
  #define EvrNetHTTPs_FileUploadRequested(fname, length)
#endif

/**
  \brief  Event on HTTP server data received in file upload process (Op)
  \param  length        length of a file data chunk
  \remark File data is received in multiple packets.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileUploadDataReceived(uint32_t length) {
    EventRecord2 (EvtNetHTTPs_FileUploadDataReceived, length, 0);
  }
#else
  #define EvrNetHTTPs_FileUploadDataReceived(length)
#endif

/**
  \brief  Event on HTTP server file upload remaining data size to complete (Op)
  \param  length        length of the remaining data
  \remark File data is received in multiple packets.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileUploadRemaining(uint32_t length) {
    EventRecord2 (EvtNetHTTPs_FileUploadRemaining, length, 0);
  }
#else
  #define EvrNetHTTPs_FileUploadRemaining(length)
#endif

/**
  \brief  Event on HTTP server file upload completed (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_FileUploadComplete(void) {
    EventRecord2 (EvtNetHTTPs_FileUploadComplete, 0, 0);
  }
#else
  #define EvrNetHTTPs_FileUploadComplete()
#endif

/**
  \brief  Event on HTTP server failed to allocate secure TLS context (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_TlsGetContextFailed(uint8_t session) {
    EventRecord2 (EvtNetHTTPs_TlsGetContextFailed, session, 0);
  }
#else
  #define EvrNetHTTPs_TlsGetContextFailed(session)
#endif

/**
  \brief  Event on HTTP server de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetHTTPs_UninitServer(void) {
    EventRecord2 (EvtNetHTTPs_UninitServer, 0, 0);
  }
#else
  #define EvrNetHTTPs_UninitServer()
#endif


// NetFTPs event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetFTPs_InitServer               EventID (EventLevelOp,    EvtNetFTPs,  0)
#define EvtNetFTPs_ShowRootFolder           EventID (EventLevelOp,    EvtNetFTPs,  1)
#define EvtNetFTPs_GetSocketFailed          EventID (EventLevelError, EvtNetFTPs,  2)
#define EvtNetFTPs_SetRootPath              EventID (EventLevelAPI,   EvtNetFTPs,  3)
#define EvtNetFTPs_SetUsername              EventID (EventLevelAPI,   EvtNetFTPs,  4)
#define EvtNetFTPs_SetPassword              EventID (EventLevelAPI,   EvtNetFTPs,  5)
#define EvtNetFTPs_UserAccessDenied         EventID (EventLevelError, EvtNetFTPs,  6)
// Gap
#define EvtNetFTPs_SessionOpen              EventID (EventLevelOp,    EvtNetFTPs,  8)
#define EvtNetFTPs_SocketAborted            EventID (EventLevelOp,    EvtNetFTPs,  9)
#define EvtNetFTPs_SocketClosed             EventID (EventLevelOp,    EvtNetFTPs, 10)
#define EvtNetFTPs_UnackedDataError         EventID (EventLevelError, EvtNetFTPs, 11)
#define EvtNetFTPs_ReceiveFrame             EventID (EventLevelOp,    EvtNetFTPs, 12)
#define EvtNetFTPs_FrameTooShort            EventID (EventLevelError, EvtNetFTPs, 13)
#define EvtNetFTPs_ShowCommand              EventID (EventLevelOp,    EvtNetFTPs, 14)
#define EvtNetFTPs_VerifyUsername           EventID (EventLevelOp,    EvtNetFTPs, 15)
#define EvtNetFTPs_VerifyPassword           EventID (EventLevelOp,    EvtNetFTPs, 16)
#define EvtNetFTPs_AuthenticationFailed     EventID (EventLevelError, EvtNetFTPs, 17)
#define EvtNetFTPs_UserLoginSuccess         EventID (EventLevelOp,    EvtNetFTPs, 18)
#define EvtNetFTPs_NotAuthenticated         EventID (EventLevelOp,    EvtNetFTPs, 19)
#define EvtNetFTPs_ShowSystemType           EventID (EventLevelOp,    EvtNetFTPs, 20)
#define EvtNetFTPs_NoOperation              EventID (EventLevelOp,    EvtNetFTPs, 21)
#define EvtNetFTPs_CurrentDirectory         EventID (EventLevelOp,    EvtNetFTPs, 22)
#define EvtNetFTPs_ChangeDirectory          EventID (EventLevelOp,    EvtNetFTPs, 23)
#define EvtNetFTPs_ChangeDirectoryFailed    EventID (EventLevelError, EvtNetFTPs, 67) // End
#define EvtNetFTPs_ChangeDirectoryLevelUp   EventID (EventLevelOp,    EvtNetFTPs, 24)
#define EvtNetFTPs_MakeDirectory            EventID (EventLevelOp,    EvtNetFTPs, 25)
#define EvtNetFTPs_OperationDenied          EventID (EventLevelOp,    EvtNetFTPs, 26)
#define EvtNetFTPs_RemoveDirectory          EventID (EventLevelOp,    EvtNetFTPs, 27)
#define EvtNetFTPs_RemoveDirectoryFailed    EventID (EventLevelError, EvtNetFTPs, 28)
#define EvtNetFTPs_DirectoryRemoved         EventID (EventLevelOp,    EvtNetFTPs, 29)
#define EvtNetFTPs_ShowDataMode             EventID (EventLevelOp,    EvtNetFTPs, 30)
#define EvtNetFTPs_ActiveModeStart          EventID (EventLevelOp,    EvtNetFTPs, 31)
#define EvtNetFTPs_PassiveModeStart         EventID (EventLevelOp,    EvtNetFTPs, 32)
#define EvtNetFTPs_GetFileSize              EventID (EventLevelOp,    EvtNetFTPs, 33)
#define EvtNetFTPs_GetFileLastModifiedTime  EventID (EventLevelOp,    EvtNetFTPs, 34)
#define EvtNetFTPs_ListDirectoryBasic       EventID (EventLevelOp,    EvtNetFTPs, 35)
#define EvtNetFTPs_ListDirectoryExtended    EventID (EventLevelOp,    EvtNetFTPs, 36)
#define EvtNetFTPs_ReadFile                 EventID (EventLevelOp,    EvtNetFTPs, 37)
#define EvtNetFTPs_FileNotFound             EventID (EventLevelError, EvtNetFTPs, 38)
#define EvtNetFTPs_WriteFile                EventID (EventLevelOp,    EvtNetFTPs, 39)
#define EvtNetFTPs_AppendFile               EventID (EventLevelOp,    EvtNetFTPs, 40)
#define EvtNetFTPs_FileCreateFailed         EventID (EventLevelError, EvtNetFTPs, 41)
#define EvtNetFTPs_DeleteFile               EventID (EventLevelOp,    EvtNetFTPs, 42)
#define EvtNetFTPs_FileDeleteFailed         EventID (EventLevelError, EvtNetFTPs, 43)
#define EvtNetFTPs_FileDeleted              EventID (EventLevelOp,    EvtNetFTPs, 44)
#define EvtNetFTPs_RenameFileFrom           EventID (EventLevelOp,    EvtNetFTPs, 45)
#define EvtNetFTPs_RenameFileTo             EventID (EventLevelOp,    EvtNetFTPs, 46)
#define EvtNetFTPs_FileRenamed              EventID (EventLevelOp,    EvtNetFTPs, 47)
#define EvtNetFTPs_FileRenameFailed         EventID (EventLevelError, EvtNetFTPs, 48)
#define EvtNetFTPs_UnknownCommand           EventID (EventLevelError, EvtNetFTPs, 49)
#define EvtNetFTPs_InvalidState             EventID (EventLevelError, EvtNetFTPs, 50)
#define EvtNetFTPs_InboundConnRejected      EventID (EventLevelError, EvtNetFTPs, 51)
#define EvtNetFTPs_DataSocketClosed         EventID (EventLevelOp,    EvtNetFTPs, 52)
#define EvtNetFTPs_DataSocketOpen           EventID (EventLevelOp,    EvtNetFTPs, 53)
#define EvtNetFTPs_LocalDiskWriteError      EventID (EventLevelError, EvtNetFTPs, 54)
#define EvtNetFTPs_StartService             EventID (EventLevelOp,    EvtNetFTPs, 55)
#define EvtNetFTPs_StopService              EventID (EventLevelOp,    EvtNetFTPs, 56)
#define EvtNetFTPs_ShowFileFindMask         EventID (EventLevelOp,    EvtNetFTPs, 57)
#define EvtNetFTPs_MakeDirectoryFailed      EventID (EventLevelError, EvtNetFTPs, 58)
#define EvtNetFTPs_DirectoryCreated         EventID (EventLevelOp,    EvtNetFTPs, 59)
#define EvtNetFTPs_ShowFileSize             EventID (EventLevelOp,    EvtNetFTPs, 60)
#define EvtNetFTPs_ShowFileLastModifiedTime EventID (EventLevelOp,    EvtNetFTPs, 61)
#define EvtNetFTPs_CloseDataConnection      EventID (EventLevelOp,    EvtNetFTPs, 62)
#define EvtNetFTPs_CloseSession             EventID (EventLevelOp,    EvtNetFTPs, 63)
#define EvtNetFTPs_SessionIdle              EventID (EventLevelOp,    EvtNetFTPs, 64)
#define EvtNetFTPs_ShowPath                 EventID (EventLevelOp,    EvtNetFTPs, 65)
#define EvtNetFTPs_UninitServer             EventID (EventLevelOp,    EvtNetFTPs, 66)
#endif

/**
  \brief  Event on FTP server initialize (Op)
  \param  num_sessions  number of available FTP sessions
  \param  port          listening port number
  \param  idle_tout     idle timeout in seconds (0= permanent connection)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_InitServer(uint32_t num_sessions, uint16_t port, uint16_t idle_tout) {
    uint32_t val2 = (uint32_t)idle_tout << 16 | port;
    EventRecord2 (EvtNetFTPs_InitServer, num_sessions, val2);
  }
#else
  #define EvrNetFTPs_InitServer(num_sessions, port, idle_tout)
#endif

/**
  \brief  Event on FTP server display root folder (Op)
  \param  root_folder   path to server root folder
  \param  length        length of the root_folder string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowRootFolder(const char *root_folder, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ShowRootFolder, root_folder, length);
  }
#else
  #define EvrNetFTPs_ShowRootFolder(root_folder, length)
#endif

/**
  \brief  Event on FTP server failed to allocate TCP socket (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_GetSocketFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_GetSocketFailed, session, 0);
  }
#else
  #define EvrNetFTPs_GetSocketFailed(session)
#endif

/**
  \brief  Event on FTP server \ref netFTPs_SetRootPath (API)
  \param  path          path to server root folder
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SetRootPath(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_SetRootPath, path, length);
  }
#else
  #define EvrNetFTPs_SetRootPath(path, length)
#endif

/**
  \brief  Event on FTP server \ref netFTPs_SetUsername (API)
  \param  username      new username
  \param  length        length of the username string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SetUsername(const char *username, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetFTPs_SetUsername, username, length);
  }
#else
  #define EvrNetFTPs_SetUsername(username, length)
#endif

/**
  \brief  Event on FTP server \ref netFTPs_SetPassword (API)
  \param  password      new password
  \param  length        length of the password string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SetPassword(const char *password, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetFTPs_SetPassword, password, length);
  }
#else
  #define EvrNetFTPs_SetPassword(password, length)
#endif

/**
  \brief  Event on FTP server user denied access (Error)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_UserAccessDenied(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetFTPs_UserAccessDenied, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetFTPs_UserAccessDenied(net_addr)
#endif

/**
  \brief  Event on FTP server session open (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SessionOpen(uint8_t session) {
    EventRecord2 (EvtNetFTPs_SessionOpen, session, 0);
  }
#else
  #define EvrNetFTPs_SessionOpen(session)
#endif

/**
  \brief  Event on FTP server socket aborted (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SocketAborted(uint8_t session) {
    EventRecord2 (EvtNetFTPs_SocketAborted, session, 0);
  }
#else
  #define EvrNetFTPs_SocketAborted(session)
#endif

/**
  \brief  Event on FTP server socket closed (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SocketClosed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_SocketClosed, session, 0);
  }
#else
  #define EvrNetFTPs_SocketClosed(session)
#endif

/**
  \brief  Event on FTP server unacked data error (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_UnackedDataError(uint8_t session) {
    EventRecord2 (EvtNetFTPs_UnackedDataError, session, 0);
  }
#else
  #define EvrNetFTPs_UnackedDataError(session)
#endif

/**
  \brief  Event on FTP server receive frame (Op)
  \param  session       session number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ReceiveFrame(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetFTPs_ReceiveFrame, session, length);
  }
#else
  #define EvrNetFTPs_ReceiveFrame(session, length)
#endif

/**
  \brief  Event on FTP server receive frame too short (Error)
  \param  session       session number
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FrameTooShort(uint8_t session, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetFTPs_FrameTooShort, session, val2);
  }
#else
  #define EvrNetFTPs_FrameTooShort(session, length, min_length)
#endif

/**
  \brief  Event on FTP server display command string (Op)
  \param  cmd_client    command received from the client in text format
  \param  length        length of the cmd_client string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowCommand(const uint8_t *cmd_client, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ShowCommand, cmd_client, length);
  }
#else
  #define EvrNetFTPs_ShowCommand(cmd_client, length)
#endif

/**
  \brief  Event on FTP server verify the client username (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_VerifyUsername(uint8_t session) {
    EventRecord2 (EvtNetFTPs_VerifyUsername, session, 0);
  }
#else
  #define EvrNetFTPs_VerifyUsername(session)
#endif

/**
  \brief  Event on FTP server verify the client password (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_VerifyPassword(uint8_t session) {
    EventRecord2 (EvtNetFTPs_VerifyPassword, session, 0);
  }
#else
  #define EvrNetFTPs_VerifyPassword(session)
#endif

/**
  \brief  Event on FTP server authentication failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_AuthenticationFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_AuthenticationFailed, session, 0);
  }
#else
  #define EvrNetFTPs_AuthenticationFailed(session)
#endif

/**
  \brief  Event on FTP server user login successful (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_UserLoginSuccess(uint8_t session) {
    EventRecord2 (EvtNetFTPs_UserLoginSuccess, session, 0);
  }
#else
  #define EvrNetFTPs_UserLoginSuccess(session)
#endif

/**
  \brief  Event on FTP server command ignored, user not authenticated (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_NotAuthenticated(uint8_t session) {
    EventRecord2 (EvtNetFTPs_NotAuthenticated, session, 0);
  }
#else
  #define EvrNetFTPs_NotAuthenticated(session)
#endif

/**
  \brief  Event on FTP server show system identification type command (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowSystemType(uint8_t session) {
    EventRecord2 (EvtNetFTPs_ShowSystemType, session, 0);
  }
#else
  #define EvrNetFTPs_ShowSystemType(session)
#endif

/**
  \brief  Event on FTP server execute no operation command (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_NoOperation(uint8_t session) {
    EventRecord2 (EvtNetFTPs_NoOperation, session, 0);
  }
#else
  #define EvrNetFTPs_NoOperation(session)
#endif

/**
  \brief  Event on FTP server display current working directory (Op)
  \param  path          path to the working directory
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_CurrentDirectory(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_CurrentDirectory, path, length);
  }
#else
  #define EvrNetFTPs_CurrentDirectory(path, length)
#endif

/**
  \brief  Event on FTP server change working directory command (Op)
  \param  path          path to the new working directory
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ChangeDirectory(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ChangeDirectory, path, length);
  }
#else
  #define EvrNetFTPs_ChangeDirectory(path, length)
#endif

/**
  \brief  Event on FTP server change working directory failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ChangeDirectoryFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_ChangeDirectoryFailed, session, 0);
  }
#else
  #define EvrNetFTPs_ChangeDirectoryFailed(session)
#endif

/**
  \brief  Event on FTP server change working directory one level up command (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ChangeDirectoryLevelUp(uint8_t session) {
    EventRecord2 (EvtNetFTPs_ChangeDirectoryLevelUp, session, 0);
  }
#else
  #define EvrNetFTPs_ChangeDirectoryLevelUp(session)
#endif

/**
  \brief  Event on FTP server make directory command (Op)
  \param  path          path to the new directory to make
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_MakeDirectory(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_MakeDirectory, path, length);
  }
#else
  #define EvrNetFTPs_MakeDirectory(path, length)
#endif

/**
  \brief  Event on FTP server access or operation denied (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_OperationDenied(uint8_t session) {
    EventRecord2 (EvtNetFTPs_OperationDenied, session, 0);
  }
#else
  #define EvrNetFTPs_OperationDenied(session)
#endif

/**
  \brief  Event on FTP server remove directory command (Op)
  \param  path          path to the directory to remove
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_RemoveDirectory(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_RemoveDirectory, path, length);
  }
#else
  #define EvrNetFTPs_RemoveDirectory(path, length)
#endif

/**
  \brief  Event on FTP server remove directory command failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_RemoveDirectoryFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_RemoveDirectoryFailed, session, 0);
  }
#else
  #define EvrNetFTPs_RemoveDirectoryFailed(session)
#endif

/**
  \brief  Event on FTP server requested directory successfuly removed (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_DirectoryRemoved(uint8_t session) {
    EventRecord2 (EvtNetFTPs_DirectoryRemoved, session, 0);
  }
#else
  #define EvrNetFTPs_DirectoryRemoved(session)
#endif

/**
  \brief  Event on FTP server display transfer data mode (Op)
  \param  session       session number
  \param  data_mode     transfer data mode
                         - 0: Binary mode
                         - 1: ASCII mode
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowDataMode(uint8_t session, uint32_t data_mode) {
    EventRecord2 (EvtNetFTPs_ShowDataMode, session, data_mode);
  }
#else
  #define EvrNetFTPs_ShowDataMode(session, data_mode)
#endif

/**
  \brief  Event on FTP server start active mode (Op)
  \param  session       session number
  \param  port          port number to connect to
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ActiveModeStart(uint8_t session, uint16_t port) {
    EventRecord2 (EvtNetFTPs_ActiveModeStart, session, port);
  }
#else
  #define EvrNetFTPs_ActiveModeStart(session, port)
#endif

/**
  \brief  Event on FTP server start passive mode (Op)
  \param  session       session number
  \param  local_port    local port number to accept data connection
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_PassiveModeStart(uint8_t session, uint16_t local_port) {
    EventRecord2 (EvtNetFTPs_PassiveModeStart, session, local_port);
  }
#else
  #define EvrNetFTPs_PassiveModeStart(session, local_port)
#endif

/**
  \brief  Event on FTP server get file size command (Op)
  \param  fname         name of the file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_GetFileSize(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_GetFileSize, fname, length);
  }
#else
  #define EvrNetFTPs_GetFileSize(fname, length)
#endif

/**
  \brief  Event on FTP server get file last-modified time command (Op)
  \param  fname         name of the file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_GetFileLastModifiedTime(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_GetFileLastModifiedTime, fname, length);
  }
#else
  #define EvrNetFTPs_GetFileLastModifiedTime(fname, length)
#endif

/**
  \brief  Event on FTP server list directory names command (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ListDirectoryBasic(uint8_t session) {
    EventRecord2 (EvtNetFTPs_ListDirectoryBasic, session, 0);
  }
#else
  #define EvrNetFTPs_ListDirectoryBasic(session)
#endif

/**
  \brief  Event on FTP server list directory in extended format (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ListDirectoryExtended(uint8_t session) {
    EventRecord2 (EvtNetFTPs_ListDirectoryExtended, session, 0);
  }
#else
  #define EvrNetFTPs_ListDirectoryExtended(session)
#endif

/**
  \brief  Event on FTP server read file command (Op)
  \param  fname         name of the file to read
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ReadFile(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ReadFile, fname, length);
  }
#else
  #define EvrNetFTPs_ReadFile(fname, length)
#endif

/**
  \brief  Event on FTP server requested file not found (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileNotFound(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileNotFound, session, 0);
  }
#else
  #define EvrNetFTPs_FileNotFound(session)
#endif

/**
  \brief  Event on FTP server write file command (Op)
  \param  fname         name of the file to write
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_WriteFile(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_WriteFile, fname, length);
  }
#else
  #define EvrNetFTPs_WriteFile(fname, length)
#endif

/**
  \brief  Event on FTP server append file command (Op)
  \param  fname         name of the file to append
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_AppendFile(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_AppendFile, fname, length);
  }
#else
  #define EvrNetFTPs_AppendFile(fname, length)
#endif

/**
  \brief  Event on FTP server requested file create failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileCreateFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileCreateFailed, session, 0);
  }
#else
  #define EvrNetFTPs_FileCreateFailed(session)
#endif

/**
  \brief  Event on FTP server delete file command (Op)
  \param  fname         name of the file to delete
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_DeleteFile(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_DeleteFile, fname, length);
  }
#else
  #define EvrNetFTPs_DeleteFile(fname, length)
#endif

/**
  \brief  Event on FTP server requested file delete failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileDeleteFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileDeleteFailed, session, 0);
  }
#else
  #define EvrNetFTPs_FileDeleteFailed(session)
#endif

/**
  \brief  Event on FTP server requested file deleted successfuly (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileDeleted(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileDeleted, session, 0);
  }
#else
  #define EvrNetFTPs_FileDeleted(session)
#endif

/**
  \brief  Event on FTP server rename file from name command (Op)
  \param  fname         name of the file to rename
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_RenameFileFrom(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_RenameFileFrom, fname, length);
  }
#else
  #define EvrNetFTPs_RenameFileFrom(fname, length)
#endif

/**
  \brief  Event on FTP server rename file to new name (Op)
  \param  new_name      new name of the file to rename to
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_RenameFileTo(const char *new_name, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_RenameFileTo, new_name, length);
  }
#else
  #define EvrNetFTPs_RenameFileTo(new_name, length)
#endif

/**
  \brief  Event on FTP server requested file renamed successfuly (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileRenamed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileRenamed, session, 0);
  }
#else
  #define EvrNetFTPs_FileRenamed(session)
#endif

/**
  \brief  Event on FTP server requested file rename failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_FileRenameFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_FileRenameFailed, session, 0);
  }
#else
  #define EvrNetFTPs_FileRenameFailed(session)
#endif

/**
  \brief  Event on FTP server unknown command received (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_UnknownCommand(uint8_t session) {
    EventRecord2 (EvtNetFTPs_UnknownCommand, session, 0);
  }
#else
  #define EvrNetFTPs_UnknownCommand(session)
#endif

/**
  \brief  Event on FTP server session in invalid state, command ignored (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_InvalidState(uint8_t session) {
    EventRecord2 (EvtNetFTPs_InvalidState, session, 0);
  }
#else
  #define EvrNetFTPs_InvalidState(session)
#endif

/**
  \brief  Event on FTP server inbound connection rejected for data socket (Error)
  \param  session       session number
  \remark Inbound data connections are accepted in passive mode, rejected in active mode.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_InboundConnRejected(uint8_t session) {
    EventRecord2 (EvtNetFTPs_InboundConnRejected, session, 0);
  }
#else
  #define EvrNetFTPs_InboundConnRejected(session)
#endif

/**
  \brief  Event on FTP server data socket closed when transfer completed (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_DataSocketClosed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_DataSocketClosed, session, 0);
  }
#else
  #define EvrNetFTPs_DataSocketClosed(session)
#endif

/**
  \brief  Event on FTP server data socket open for data transfer (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_DataSocketOpen(uint8_t session) {
    EventRecord2 (EvtNetFTPs_DataSocketOpen, session, 0);
  }
#else
  #define EvrNetFTPs_DataSocketOpen(session)
#endif

/**
  \brief  Event on FTP server write to local disk failed, disk full (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_LocalDiskWriteError(uint8_t session) {
    EventRecord2 (EvtNetFTPs_LocalDiskWriteError, session, 0);
  }
#else
  #define EvrNetFTPs_LocalDiskWriteError(session)
#endif

/**
  \brief  Event on FTP server start service (Op)
  \param  port          listening port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_StartService(uint16_t port) {
    EventRecord2 (EvtNetFTPs_StartService, port, 0);
  }
#else
  #define EvrNetFTPs_StartService(port)
#endif

/**
  \brief  Event on FTP server stop service (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_StopService(void) {
    EventRecord2 (EvtNetFTPs_StopService, 0, 0);
  }
#else
  #define EvrNetFTPs_StopService()
#endif

/**
  \brief  Event on FTP server display find file(s) mask filter (Op)
  \param  path          path to list directory
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowFileFindMask(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ShowFileFindMask, path, length);
  }
#else
  #define EvrNetFTPs_ShowFileFindMask(path, length)
#endif

/**
  \brief  Event on FTP server requested directory create failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_MakeDirectoryFailed(uint8_t session) {
    EventRecord2 (EvtNetFTPs_MakeDirectoryFailed, session, 0);
  }
#else
  #define EvrNetFTPs_MakeDirectoryFailed(session)
#endif

/**
  \brief  Event on FTP server requested directory created successfuly (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_DirectoryCreated(uint8_t session) {
    EventRecord2 (EvtNetFTPs_DirectoryCreated, session, 0);
  }
#else
  #define EvrNetFTPs_DirectoryCreated(session)
#endif

/**
  \brief  Event on FTP server display requested file size (Op)
  \param  session       session number
  \param  file_size     size of a file in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowFileSize(uint8_t session, uint32_t file_size) {
    EventRecord2 (EvtNetFTPs_ShowFileSize, session, file_size);
  }
#else
  #define EvrNetFTPs_ShowFileSize(session, file_size)
#endif

/**
  \brief  Event on FTP server display requested file last-modified time (Op)
  \param  session       session number
  \param  file_time     last-modified time of a file in text format
  \param  length        length of the file_time string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowFileLastModifiedTime(uint8_t session,
                                            const char *file_time, uint32_t length) {
    evr_buf.u32[0] = session;
    memcpy (&evr_buf.u32[1], file_time, length);
    EventRecordData (EvtNetFTPs_ShowFileLastModifiedTime, &evr_buf, length+4);
  }
#else
  #define EvrNetFTPs_ShowFileLastModifiedTime(session, file_time, length)
#endif

/**
  \brief  Event on FTP server closing data connection (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_CloseDataConnection(uint8_t session) {
    EventRecord2 (EvtNetFTPs_CloseDataConnection, session, 0);
  }
#else
  #define EvrNetFTPs_CloseDataConnection(session)
#endif

/**
  \brief  Event on FTP server closing session (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_CloseSession(uint8_t session) {
    EventRecord2 (EvtNetFTPs_CloseSession, session, 0);
  }
#else
  #define EvrNetFTPs_CloseSession(session)
#endif

/**
  \brief  Event on FTP server session in idle state (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_SessionIdle(uint8_t session) {
    EventRecord2 (EvtNetFTPs_SessionIdle, session, 0);
  }
#else
  #define EvrNetFTPs_SessionIdle(session)
#endif

/**
  \brief  Event on FTP server display an absolute path for file access (Op)
  \param  path          absolute path for file access
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_ShowPath(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetFTPs_ShowPath, path, length);
  }
#else
  #define EvrNetFTPs_ShowPath(path, length)
#endif

/**
  \brief  Event on FTP server de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPs_UninitServer(void) {
    EventRecord2 (EvtNetFTPs_UninitServer, 0, 0);
  }
#else
  #define EvrNetFTPs_UninitServer()
#endif


// NetFTPc event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetFTPc_InitClient               EventID (EventLevelOp,    EvtNetFTPc,  0)
#define EvtNetFTPc_GetSocketFailed          EventID (EventLevelError, EvtNetFTPc,  1)
#define EvtNetFTPc_Connect                  EventID (EventLevelAPI,   EvtNetFTPc,  2)
// Gap
#define EvtNetFTPc_ConnectInvalidParameter  EventID (EventLevelError, EvtNetFTPc,  4)
#define EvtNetFTPc_ConnectClientBusy        EventID (EventLevelError, EvtNetFTPc,  5)
#define EvtNetFTPc_InboundConnRejected      EventID (EventLevelOp,    EvtNetFTPc,  6)
#define EvtNetFTPc_SocketAborted            EventID (EventLevelError, EvtNetFTPc,  7)
#define EvtNetFTPc_SocketConnected          EventID (EventLevelOp,    EvtNetFTPc,  8)
#define EvtNetFTPc_SocketClosed             EventID (EventLevelOp,    EvtNetFTPc,  9)
#define EvtNetFTPc_ReceiveFrame             EventID (EventLevelOp,    EvtNetFTPc, 10)
#define EvtNetFTPc_FrameTooShort            EventID (EventLevelError, EvtNetFTPc, 11)
#define EvtNetFTPc_ShowReplyCode            EventID (EventLevelOp,    EvtNetFTPc, 12)
#define EvtNetFTPc_ResponseFragmented       EventID (EventLevelOp,    EvtNetFTPc, 13)
#define EvtNetFTPc_ServerReady              EventID (EventLevelOp,    EvtNetFTPc, 14)
#define EvtNetFTPc_UserOkNeedPassword       EventID (EventLevelOp,    EvtNetFTPc, 15)
#define EvtNetFTPc_UserLoginFailed          EventID (EventLevelError, EvtNetFTPc, 16)
#define EvtNetFTPc_UserLoginSuccess         EventID (EventLevelOp,    EvtNetFTPc, 17)
#define EvtNetFTPc_WorkingDirectoryInvalid  EventID (EventLevelError, EvtNetFTPc, 18)
#define EvtNetFTPc_ExecuteUserCommand       EventID (EventLevelOp,    EvtNetFTPc, 19)
#define EvtNetFTPc_BinaryModeEnabled        EventID (EventLevelOp,    EvtNetFTPc, 20)
#define EvtNetFTPc_PasvCommandFailed        EventID (EventLevelError, EvtNetFTPc, 21)
#define EvtNetFTPc_PassiveModeStart         EventID (EventLevelOp,    EvtNetFTPc, 22)
#define EvtNetFTPc_PortCommandFailed        EventID (EventLevelError, EvtNetFTPc, 23)
#define EvtNetFTPc_ActiveModeStart          EventID (EventLevelOp,    EvtNetFTPc, 24)
#define EvtNetFTPc_FileNotFoundOnServer     EventID (EventLevelError, EvtNetFTPc, 25)
#define EvtNetFTPc_OperationNotAllowed      EventID (EventLevelError, EvtNetFTPc, 26)
#define EvtNetFTPc_AboutToOpenDataConn      EventID (EventLevelOp,    EvtNetFTPc, 27)
#define EvtNetFTPc_DataConnAlreadyOpen      EventID (EventLevelOp,    EvtNetFTPc, 28)
#define EvtNetFTPc_TransferAborted          EventID (EventLevelOp,    EvtNetFTPc, 29)
#define EvtNetFTPc_TransferCompleted        EventID (EventLevelOp,    EvtNetFTPc, 30)
#define EvtNetFTPc_FileDeleted              EventID (EventLevelOp,    EvtNetFTPc, 31)
#define EvtNetFTPc_NewNameRequired          EventID (EventLevelOp,    EvtNetFTPc, 32)
#define EvtNetFTPc_FileOrDirectoryRenamed   EventID (EventLevelOp,    EvtNetFTPc, 33)
#define EvtNetFTPc_DirectoryCreated         EventID (EventLevelOp,    EvtNetFTPc, 34)
#define EvtNetFTPc_FileOrPathNotFound       EventID (EventLevelOp,    EvtNetFTPc, 35)
#define EvtNetFTPc_DirectoryRemoved         EventID (EventLevelOp,    EvtNetFTPc, 36)
#define EvtNetFTPc_CommandErrorResponse     EventID (EventLevelError, EvtNetFTPc, 37)
#define EvtNetFTPc_WrongResponse            EventID (EventLevelError, EvtNetFTPc, 38)
#define EvtNetFTPc_DataSocketClosed         EventID (EventLevelOp,    EvtNetFTPc, 39)
#define EvtNetFTPc_DataSocketOpened         EventID (EventLevelOp,    EvtNetFTPc, 40)
#define EvtNetFTPc_LocalDiskWriteError      EventID (EventLevelError, EvtNetFTPc, 41)
#define EvtNetFTPc_ClientStopTimeoutExpired EventID (EventLevelError, EvtNetFTPc, 42)
#define EvtNetFTPc_LocalPortAssigned        EventID (EventLevelOp,    EvtNetFTPc, 43)
#define EvtNetFTPc_OpenLocalFile            EventID (EventLevelOp,    EvtNetFTPc, 44)
#define EvtNetFTPc_LocalFileCreateFailed    EventID (EventLevelError, EvtNetFTPc, 45)
#define EvtNetFTPc_LocalFileNotFound        EventID (EventLevelError, EvtNetFTPc, 46)
#define EvtNetFTPc_OpenDataConnFailed       EventID (EventLevelError, EvtNetFTPc, 47)
#define EvtNetFTPc_SendCommand              EventID (EventLevelDetail,EvtNetFTPc, 48)
#define EvtNetFTPc_ClientCloseSocket        EventID (EventLevelOp,    EvtNetFTPc, 49)
#define EvtNetFTPc_ClientDone               EventID (EventLevelOp,    EvtNetFTPc, 50)
#define EvtNetFTPc_CloseLocalFile           EventID (EventLevelOp,    EvtNetFTPc, 51)
#define EvtNetFTPc_UninitClient             EventID (EventLevelOp,    EvtNetFTPc, 52)
#endif

/**
  \brief  Event on FTP client initialize (Op)
  \param  mode           client mode of operation
                          - 0: active mode (accepts inbound data connections)
                          - 1: passive mode (starts outbound data connections)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_InitClient(uint32_t mode) {
    EventRecord2 (EvtNetFTPc_InitClient, mode, 0);
  }
#else
  #define EvrNetFTPc_InitClient(mode)
#endif

/**
  \brief  Event on FTP client failed to allocate TCP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_GetSocketFailed(void) {
    EventRecord2 (EvtNetFTPc_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetFTPc_GetSocketFailed()
#endif

/**
  \brief  Event on FTP client \ref netFTPc_Connect to the server (API)
  \param  net_addr      pointer to \ref NET_ADDR server address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_Connect(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetFTPc_Connect, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetFTPc_Connect(net_addr)
#endif

/**
  \brief  Event on \ref netFTPc_Connect invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ConnectInvalidParameter(void) {
    EventRecord2 (EvtNetFTPc_ConnectInvalidParameter, 0, 0);
  }
#else
  #define EvrNetFTPc_ConnectInvalidParameter()
#endif

/**
  \brief  Event on \ref netFTPc_Connect failed, client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ConnectClientBusy(void) {
    EventRecord2 (EvtNetFTPc_ConnectClientBusy, 0, 0);
  }
#else
  #define EvrNetFTPc_ConnectClientBusy()
#endif

/**
  \brief  Event on FTP client inbound connection rejected (Error)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_InboundConnRejected(int32_t socket) {
    EventRecord2 (EvtNetFTPc_InboundConnRejected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_InboundConnRejected(socket)
#endif

/**
  \brief  Event on FTP client socket aborted (Error)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_SocketAborted(int32_t socket) {
    EventRecord2 (EvtNetFTPc_SocketAborted, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_SocketAborted(socket)
#endif

/**
  \brief  Event on FTP client socket connected (Op)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_SocketConnected(int32_t socket) {
    EventRecord2 (EvtNetFTPc_SocketConnected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_SocketConnected(socket)
#endif

/**
  \brief  Event on FTP client socket closed (Op)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_SocketClosed(int32_t socket) {
    EventRecord2 (EvtNetFTPc_SocketClosed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_SocketClosed(socket)
#endif

/**
  \brief  Event on FTP client received frame (Op)
  \param  socket        assigned TCP socket
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ReceiveFrame(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetFTPc_ReceiveFrame, (uint32_t)socket, length);
  }
#else
  #define EvrNetFTPc_ReceiveFrame(socket, length)
#endif

/**
  \brief  Event on FTP receive frame is too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetFTPc_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetFTPc_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on FTP client display numeric reply code received from server (Op)
  \param  reply_code    numeric reply code in text format (3 characters)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ShowReplyCode(const uint8_t *reply_code) {
    EventRecordData (EvtNetFTPc_ShowReplyCode, reply_code, 3);
  }
#else
  #define EvrNetFTPc_ShowReplyCode(reply_code)
#endif

/**
  \brief  Event on FTP client response divided into multiple frames (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ResponseFragmented(void) {
    EventRecord2 (EvtNetFTPc_ResponseFragmented, 0, 0);
  }
#else
  #define EvrNetFTPc_ResponseFragmented()
#endif

/**
  \brief  Event on FTP client server ready response (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ServerReady(void) {
    EventRecord2 (EvtNetFTPc_ServerReady, 0, 0);
  }
#else
  #define EvrNetFTPc_ServerReady()
#endif

/**
  \brief  Event on FTP client username ok, password needed (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_UserOkNeedPassword(void) {
    EventRecord2 (EvtNetFTPc_UserOkNeedPassword, 0, 0);
  }
#else
  #define EvrNetFTPc_UserOkNeedPassword()
#endif

/**
  \brief  Event on FTP client user login to server failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_UserLoginFailed(void) {
    EventRecord2 (EvtNetFTPc_UserLoginFailed, 0, 0);
  }
#else
  #define EvrNetFTPc_UserLoginFailed()
#endif

/**
  \brief  Event on FTP client user login successful (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_UserLoginSuccess(void) {
    EventRecord2 (EvtNetFTPc_UserLoginSuccess, 0, 0);
  }
#else
  #define EvrNetFTPc_UserLoginSuccess()
#endif

/**
  \brief  Event on FTP client change working directory failed, invalid path (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_WorkingDirectoryInvalid(void) {
    EventRecord2 (EvtNetFTPc_WorkingDirectoryInvalid, 0, 0);
  }
#else
  #define EvrNetFTPc_WorkingDirectoryInvalid()
#endif

/**
  \brief  Event on FTP client server ready response (Op)
  \param  command       user command to execute
                         - 0: Puts a file on FTP server
                         - 1: Retrieves a file from FTP server
                         - 2: Append file on FTP server (with create)
                         - 3: Deletes a file on FTP server
                         - 4: Lists files stored on FTP server
                         - 5: Renames a file on FTP server
                         - 6: Makes a directory on FTP server
                         - 7: Removes an empty directory on FTP serve
                         - 8: Lists file names only (short format)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ExecuteUserCommand(uint8_t command) {
    EventRecord2 (EvtNetFTPc_ExecuteUserCommand, command, 0);
  }
#else
  #define EvrNetFTPc_ExecuteUserCommand(command)
#endif

/**
  \brief  Event on FTP client binary transfer mode enabled (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_BinaryModeEnabled(void) {
    EventRecord2 (EvtNetFTPc_BinaryModeEnabled, 0, 0);
  }
#else
  #define EvrNetFTPc_BinaryModeEnabled()
#endif

/**
  \brief  Event on FTP client PASV command failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_PasvCommandFailed(void) {
    EventRecord2 (EvtNetFTPc_PasvCommandFailed, 0, 0);
  }
#else
  #define EvrNetFTPc_PasvCommandFailed()
#endif

/**
  \brief  Event on FTP client start passive server mode (Op)
  \param  port          server port number to connect to
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_PassiveModeStart(uint16_t port) {
    EventRecord2 (EvtNetFTPc_PassiveModeStart, port, 0);
  }
#else
  #define EvrNetFTPc_PassiveModeStart(port)
#endif

/**
  \brief  Event on FTP client PORT command failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_PortCommandFailed(void) {
    EventRecord2 (EvtNetFTPc_PortCommandFailed, 0, 0);
  }
#else
  #define EvrNetFTPc_PortCommandFailed()
#endif

/**
  \brief  Event on FTP client start active server mode (Op)
  \param  local_port    local port number to accept connection
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ActiveModeStart(uint16_t local_port) {
    EventRecord2 (EvtNetFTPc_ActiveModeStart, local_port, 0);
  }
#else
  #define EvrNetFTPc_ActiveModeStart(local_port)
#endif

/**
  \brief  Event on FTP client error, requested file not found on server (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_FileNotFoundOnServer(void) {
    EventRecord2 (EvtNetFTPc_FileNotFoundOnServer, 0, 0);
  }
#else
  #define EvrNetFTPc_FileNotFoundOnServer()
#endif

/**
  \brief  Event on FTP client error, operation not allowed on server (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_OperationNotAllowed(void) {
    EventRecord2 (EvtNetFTPc_OperationNotAllowed, 0, 0);
  }
#else
  #define EvrNetFTPc_OperationNotAllowed()
#endif

/**
  \brief  Event on FTP client about to open data connection (Op)
  \param  socket        assigned TCP data socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_AboutToOpenDataConn(int32_t socket) {
    EventRecord2 (EvtNetFTPc_AboutToOpenDataConn, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_AboutToOpenDataConn(socket)
#endif

/**
  \brief  Event on FTP client data connection already opened (Op)
  \param  socket        assigned TCP data socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_DataConnAlreadyOpen(int32_t socket) {
    EventRecord2 (EvtNetFTPc_DataConnAlreadyOpen, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_DataConnAlreadyOpen(socket)
#endif

/**
  \brief  Event on FTP client data transfer aborted (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_TransferAborted(void) {
    EventRecord2 (EvtNetFTPc_TransferAborted, 0, 0);
  }
#else
  #define EvrNetFTPc_TransferAborted()
#endif

/**
  \brief  Event on FTP client data transfer completed successfully (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_TransferCompleted(void) {
    EventRecord2 (EvtNetFTPc_TransferCompleted, 0, 0);
  }
#else
  #define EvrNetFTPc_TransferCompleted()
#endif

/**
  \brief  Event on FTP client file deleted on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_FileDeleted(void) {
    EventRecord2 (EvtNetFTPc_FileDeleted, 0, 0);
  }
#else
  #define EvrNetFTPc_FileDeleted()
#endif

/**
  \brief  Event on FTP client new name required to rename a file on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_NewNameRequired(void) {
    EventRecord2 (EvtNetFTPc_NewNameRequired, 0, 0);
  }
#else
  #define EvrNetFTPc_NewNameRequired()
#endif

/**
  \brief  Event on FTP client file or directory renamed successfully on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_FileOrDirectoryRenamed(void) {
    EventRecord2 (EvtNetFTPc_FileOrDirectoryRenamed, 0, 0);
  }
#else
  #define EvrNetFTPc_FileOrDirectoryRenamed()
#endif

/**
  \brief  Event on FTP client directory successfully created on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_DirectoryCreated(void) {
    EventRecord2 (EvtNetFTPc_DirectoryCreated, 0, 0);
  }
#else
  #define EvrNetFTPc_DirectoryCreated()
#endif

/**
  \brief  Event on FTP client requested file or path not found on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_FileOrPathNotFound(void) {
    EventRecord2 (EvtNetFTPc_FileOrPathNotFound, 0, 0);
  }
#else
  #define EvrNetFTPc_FileOrPathNotFound()
#endif

/**
  \brief  Event on FTP client directory removed on the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_DirectoryRemoved(void) {
    EventRecord2 (EvtNetFTPc_DirectoryRemoved, 0, 0);
  }
#else
  #define EvrNetFTPc_DirectoryRemoved()
#endif

/**
  \brief  Event on FTP client command error response received (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_CommandErrorResponse(void) {
    EventRecord2 (EvtNetFTPc_CommandErrorResponse, 0, 0);
  }
#else
  #define EvrNetFTPc_CommandErrorResponse()
#endif

/**
  \brief  Event on FTP client wrong response received (Error)
  \param  response      response from the server in text format
  \param  length        length of the response string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_WrongResponse(const uint8_t *response, uint32_t length) {
    if (length > 40) length = 40;
    EventRecordData (EvtNetFTPc_WrongResponse, response, length);
  }
#else
  #define EvrNetFTPc_WrongResponse(response, length)
#endif

/**
  \brief  Event on FTP client data socket closed (Op)
  \param  socket        assigned TCP data socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_DataSocketClosed(int32_t socket) {
    EventRecord2 (EvtNetFTPc_DataSocketClosed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_DataSocketClosed(socket)
#endif

/**
  \brief  Event on FTP client data socket opened (Op)
  \param  socket        assigned TCP data socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_DataSocketOpened(int32_t socket) {
    EventRecord2 (EvtNetFTPc_DataSocketOpened, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_DataSocketOpened(socket)
#endif

/**
  \brief  Event on FTP client write to local disk failed, disk full (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_LocalDiskWriteError(void) {
    EventRecord2 (EvtNetFTPc_LocalDiskWriteError, 0, 0);
  }
#else
  #define EvrNetFTPc_LocalDiskWriteError()
#endif

/**
  \brief  Event on FTP client stop operation timeout expired (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ClientStopTimeoutExpired(void) {
    EventRecord2 (EvtNetFTPc_ClientStopTimeoutExpired, 0, 0);
  }
#else
  #define EvrNetFTPc_ClientStopTimeoutExpired()
#endif

/**
  \brief  Event on FTP client local port assigned for server active mode (Op)
  \param  local_port    local port number to accept connection
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_LocalPortAssigned(uint16_t local_port) {
    EventRecord2 (EvtNetFTPc_LocalPortAssigned, local_port, 0);
  }
#else
  #define EvrNetFTPc_LocalPortAssigned(local_port)
#endif

/**
  \brief  Event on FTP client open local file (Op)
  \param  local_fname   name of the local file
  \param  length        length of the local_fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_OpenLocalFile(const char *local_fname, uint32_t length) {
    if (length > 120) length = 120;
    EventRecordData (EvtNetFTPc_OpenLocalFile, local_fname, length);
  }
#else
  #define EvrNetFTPc_OpenLocalFile(local_fname, length)
#endif

/**
  \brief  Event on FTP client local file create failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_LocalFileCreateFailed(void) {
    EventRecord2 (EvtNetFTPc_LocalFileCreateFailed, 0, 0);
  }
#else
  #define EvrNetFTPc_LocalFileCreateFailed()
#endif

/**
  \brief  Event on FTP client local file not found (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_LocalFileNotFound(void) {
    EventRecord2 (EvtNetFTPc_LocalFileNotFound, 0, 0);
  }
#else
  #define EvrNetFTPc_LocalFileNotFound()
#endif

/**
  \brief  Event on FTP client open data connection failed (Error)
  \param  socket        assigned TCP data socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_OpenDataConnFailed(int32_t socket) {
    EventRecord2 (EvtNetFTPc_OpenDataConnFailed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_OpenDataConnFailed(socket)
#endif

/**
  \brief  Event on FTP client send command to server (Detail)
  \param  command       server command in text form
  \param  length        length of the command string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_SendCommand(const uint8_t *command, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetFTPc_SendCommand, command, length);
  }
#else
  #define EvrNetFTPc_SendCommand(command, length)
#endif

/**
  \brief  Event on FTP client close control socket (Op)
  \param  socket        assigned TCP control socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ClientCloseSocket(int32_t socket) {
    EventRecord2 (EvtNetFTPc_ClientCloseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetFTPc_ClientCloseSocket(socket)
#endif

/**
  \brief  Event on FTP client completed operation (Op)
  \param  cb_event      user callback nofication event
                         - 0: File operation successful
                         - 1: Timeout on file operation
                         - 2: Login error, username/password invalid
                         - 3: File access not allowed
                         - 4: File not found
                         - 5: Working directory path not found
                         - 6: Local file read/write error
                         - 7: Generic FTP client error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_ClientDone(uint8_t cb_event) {
    EventRecord2 (EvtNetFTPc_ClientDone, cb_event, 0);
  }
#else
  #define EvrNetFTPc_ClientDone(cb_event)
#endif

/**
  \brief  Event on FTP client close local file (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_CloseLocalFile(void) {
    EventRecord2 (EvtNetFTPc_CloseLocalFile, 0, 0);
  }
#else
  #define EvrNetFTPc_CloseLocalFile()
#endif

/**
  \brief  Event on FTP client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetFTPc_UninitClient(void) {
    EventRecord2 (EvtNetFTPc_UninitClient, 0, 0);
  }
#else
  #define EvrNetFTPc_UninitClient()
#endif


// NetTeln event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetTeln_InitServer               EventID (EventLevelOp,    EvtNetTeln,  0)
#define EvtNetTeln_GetSocketFailed          EventID (EventLevelError, EvtNetTeln,  1)
#define EvtNetTeln_SetUsername              EventID (EventLevelAPI,   EvtNetTeln,  2)
#define EvtNetTeln_SetPassword              EventID (EventLevelAPI,   EvtNetTeln,  3)
#define EvtNetTeln_StartService             EventID (EventLevelOp,    EvtNetTeln,  4)
#define EvtNetTeln_StopService              EventID (EventLevelOp,    EvtNetTeln,  5)
#define EvtNetTeln_CloseSession             EventID (EventLevelOp,    EvtNetTeln,  6)
#define EvtNetTeln_ProcessData              EventID (EventLevelOp,    EvtNetTeln,  7)
#define EvtNetTeln_ProcessCommand           EventID (EventLevelOp,    EvtNetTeln,  8)
#define EvtNetTeln_EchoBackspace            EventID (EventLevelOp,    EvtNetTeln,  9)
#define EvtNetTeln_CommandHistory           EventID (EventLevelOp,    EvtNetTeln, 10)
#define EvtNetTeln_EchoCharacters           EventID (EventLevelOp,    EvtNetTeln, 11)
#define EvtNetTeln_LineBufferUsage          EventID (EventLevelOp,    EvtNetTeln, 12)
#define EvtNetTeln_SendAuthorizationRequest EventID (EventLevelOp,    EvtNetTeln, 13)
#define EvtNetTeln_SendInitialHeader        EventID (EventLevelOp,    EvtNetTeln, 14)
#define EvtNetTeln_LoginTimeoutExpired      EventID (EventLevelOp,    EvtNetTeln, 15)
#define EvtNetTeln_ShowPassword             EventID (EventLevelOp,    EvtNetTeln, 16)
#define EvtNetTeln_AuthenticationFailed     EventID (EventLevelError, EvtNetTeln, 17)
#define EvtNetTeln_UserLoginSuccess         EventID (EventLevelOp,    EvtNetTeln, 18)
#define EvtNetTeln_ShowUsername             EventID (EventLevelOp,    EvtNetTeln, 19)
#define EvtNetTeln_NegotiateStart           EventID (EventLevelOp,    EvtNetTeln, 20)
#define EvtNetTeln_NegotiateFailed          EventID (EventLevelError, EvtNetTeln, 21)
#define EvtNetTeln_UserAccessDenied         EventID (EventLevelError, EvtNetTeln, 22)
// Gap
#define EvtNetTeln_SessionOpen              EventID (EventLevelOp,    EvtNetTeln, 24)
#define EvtNetTeln_SocketAborted            EventID (EventLevelOp,    EvtNetTeln, 25)
#define EvtNetTeln_SocketClosed             EventID (EventLevelOp,    EvtNetTeln, 26)
#define EvtNetTeln_ReceiveFrame             EventID (EventLevelOp,    EvtNetTeln, 27)
#define EvtNetTeln_NegotiateSuccess         EventID (EventLevelOp,    EvtNetTeln, 28)
#define EvtNetTeln_UninitServer             EventID (EventLevelOp,    EvtNetTeln, 29)
#endif

/**
  \brief  Event on Telnet server initialize (Op)
  \param  num_sessions  number of available Telnet sessions
  \param  port          listening port number
  \param  idle_tout     idle timeout in seconds (0= permanent connection)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_InitServer(uint32_t num_sessions, uint16_t port, uint16_t idle_tout) {
    uint32_t val2 = (uint32_t)idle_tout << 16 | port;
    EventRecord2 (EvtNetTeln_InitServer, num_sessions, val2);
  }
#else
  #define EvrNetTeln_InitServer(num_sessions, port, idle_tout)
#endif

/**
  \brief  Event on Telnet server failed to allocate TCP socket (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_GetSocketFailed(uint8_t session) {
    EventRecord2 (EvtNetTeln_GetSocketFailed, session, 0);
  }
#else
  #define EvrNetTeln_GetSocketFailed(session)
#endif

/**
  \brief  Event on Telnet server \ref netTELNETs_SetUsername (API)
  \param  username      new username
  \param  length        length of the username string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SetUsername(const char *username, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetTeln_SetUsername, username, length);
  }
#else
  #define EvrNetTeln_SetUsername(username, length)
#endif

/**
  \brief  Event on Telnet server \ref netTELNETs_SetPassword (API)
  \param  password      new password
  \param  length        length of the password string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SetPassword(const char *password, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetTeln_SetPassword, password, length);
  }
#else
  #define EvrNetTeln_SetPassword(password, length)
#endif

/**
  \brief  Event on Telnet server start service (Op)
  \param  port          listening port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_StartService(uint16_t port) {
    EventRecord2 (EvtNetTeln_StartService, port, 0);
  }
#else
  #define EvrNetTeln_StartService(port)
#endif

/**
  \brief  Event on Telnet server stop service (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_StopService(void) {
    EventRecord2 (EvtNetTeln_StopService, 0, 0);
  }
#else
  #define EvrNetTeln_StopService()
#endif

/**
  \brief  Event on Telnet server session close (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_CloseSession(uint8_t session) {
    EventRecord2 (EvtNetTeln_CloseSession, session, 0);
  }
#else
  #define EvrNetTeln_CloseSession(session)
#endif

/**
  \brief  Event on Telnet server process client data (Op)
  \param  session       session number
  \param  length        length of client data to process
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_ProcessData(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetTeln_ProcessData, session, length);
  }
#else
  #define EvrNetTeln_ProcessData(session, length)
#endif

/**
  \brief  Event on Telnet server process client command (Op)
  \param  command       user entered command to process
  \param  length        length of the command string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_ProcessCommand(const char *command, uint32_t length) {
    if (length > 96) length = 96;
    EventRecordData (EvtNetTeln_ProcessCommand, command, length);
  }
#else
  #define EvrNetTeln_ProcessCommand(command, length)
#endif

/**
  \brief  Event on Telnet server echo backspace (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_EchoBackspace(uint8_t session) {
    EventRecord2 (EvtNetTeln_EchoBackspace, session, 0);
  }
#else
  #define EvrNetTeln_EchoBackspace(session)
#endif

/**
  \brief  Event on Telnet server process command history (Op)
  \param  command       command from the command history
  \param  length        length of the command string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_CommandHistory(const char *command, uint32_t length) {
    if (length > 96) length = 96;
    EventRecordData (EvtNetTeln_CommandHistory, command, length);
  }
#else
  #define EvrNetTeln_CommandHistory(command, length)
#endif

/**
  \brief  Event on Telnet server echo the characters from command line buffer (Op)
  \param  line_buffer   command line buffer
  \param  num_char      number of characters to echo
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_EchoCharacters(uint8_t *line_buffer, uint32_t num_char) {
    if (num_char > 96) num_char = 96;
    EventRecordData (EvtNetTeln_EchoCharacters, line_buffer, num_char);
  }
#else
  #define EvrNetTeln_EchoCharacters(line_buffer, num_char)
#endif

/**
  \brief  Event on Telnet server display command line buffer usage (Op)
  \param  session       session number
  \param  num_char      number of characters in command line buffer
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_LineBufferUsage(uint8_t session, uint32_t num_char) {
    EventRecord2 (EvtNetTeln_LineBufferUsage, session, num_char);
  }
#else
  #define EvrNetTeln_LineBufferUsage(session, num_char)
#endif

/**
  \brief  Event on Telnet server send authorization request to the client (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SendAuthorizationRequest(uint8_t session) {
    EventRecord2 (EvtNetTeln_SendAuthorizationRequest, session, 0);
  }
#else
  #define EvrNetTeln_SendAuthorizationRequest(session)
#endif

/**
  \brief  Event on Telnet server send initial telnet header to the client (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SendInitialHeader(uint8_t session) {
    EventRecord2 (EvtNetTeln_SendInitialHeader, session, 0);
  }
#else
  #define EvrNetTeln_SendInitialHeader(session)
#endif

/**
  \brief  Event on Telnet server login timeout expired (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_LoginTimeoutExpired(uint8_t session) {
    EventRecord2 (EvtNetTeln_LoginTimeoutExpired, session, 0);
  }
#else
  #define EvrNetTeln_LoginTimeoutExpired(session)
#endif

/**
  \brief  Event on Telnet server display password of the client (Op)
  \param  password      password entered by the client
  \param  length        length of the password string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_ShowPassword(const char *password, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetTeln_ShowPassword, password, length);
  }
#else
  #define EvrNetTeln_ShowPassword(password, length)
#endif

/**
  \brief  Event on Telnet server user authentication failed, invalid credentials (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_AuthenticationFailed(uint8_t session) {
    EventRecord2 (EvtNetTeln_AuthenticationFailed, session, 0);
  }
#else
  #define EvrNetTeln_AuthenticationFailed(session)
#endif

/**
  \brief  Event on Telnet server user login successful (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_UserLoginSuccess(uint8_t session) {
    EventRecord2 (EvtNetTeln_UserLoginSuccess, session, 0);
  }
#else
  #define EvrNetTeln_UserLoginSuccess(session)
#endif

/**
  \brief  Event on Telnet server display username of the client (Op)
  \param  username      username entered by the client
  \param  length        length of the username string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_ShowUsername(const char *username, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData (EvtNetTeln_ShowUsername, username, length);
  }
#else
  #define EvrNetTeln_ShowUsername(username, length)
#endif

/**
  \brief  Event on Telnet server option negotiation start (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_NegotiateStart(uint8_t session) {
    EventRecord2 (EvtNetTeln_NegotiateStart, session, 0);
  }
#else
  #define EvrNetTeln_NegotiateStart(session)
#endif

/**
  \brief  Event on Telnet server option negotiation failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_NegotiateFailed(uint8_t session) {
    EventRecord2 (EvtNetTeln_NegotiateFailed, session, 0);
  }
#else
  #define EvrNetTeln_NegotiateFailed(session)
#endif

/**
  \brief  Event on Telnet server user denied access (Error)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_UserAccessDenied(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTeln_UserAccessDenied, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTeln_UserAccessDenied(net_addr)
#endif

/**
  \brief  Event on Telnet server session open (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SessionOpen(uint8_t session) {
    EventRecord2 (EvtNetTeln_SessionOpen, session, 0);
  }
#else
  #define EvrNetTeln_SessionOpen(session)
#endif

/**
  \brief  Event on Telnet server socket aborted (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SocketAborted(uint8_t session) {
    EventRecord2 (EvtNetTeln_SocketAborted, session, 0);
  }
#else
  #define EvrNetTeln_SocketAborted(session)
#endif

/**
  \brief  Event on Telnet server socket closed (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_SocketClosed(uint8_t session) {
    EventRecord2 (EvtNetTeln_SocketClosed, session, 0);
  }
#else
  #define EvrNetTeln_SocketClosed(session)
#endif

/**
  \brief  Event on Telnet server receive frame (Op)
  \param  session       session number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_ReceiveFrame(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetTeln_ReceiveFrame, session, length);
  }
#else
  #define EvrNetTeln_ReceiveFrame(session, length)
#endif

/**
  \brief  Event on Telnet server option negotiation success (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_NegotiateSuccess(uint8_t session) {
    EventRecord2 (EvtNetTeln_NegotiateSuccess, session, 0);
  }
#else
  #define EvrNetTeln_NegotiateSuccess(session)
#endif

/**
  \brief  Event on Telnet server de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTeln_UninitServer(void) {
    EventRecord2 (EvtNetTeln_UninitServer, 0, 0);
  }
#else
  #define EvrNetTeln_UninitServer()
#endif


// NetTFTPs event identifiers --------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetTFTPs_InitServer              EventID (EventLevelOp,    EvtNetTFTPs,  0)
#define EvtNetTFTPs_ShowRootFolder          EventID (EventLevelOp,    EvtNetTFTPs,  1)
#define EvtNetTFTPs_GetSocketFailed         EventID (EventLevelError, EvtNetTFTPs,  2)
#define EvtNetTFTPs_SetRootPath             EventID (EventLevelAPI,   EvtNetTFTPs,  3)
#define EvtNetTFTPs_StartService            EventID (EventLevelOp,    EvtNetTFTPs,  4)
#define EvtNetTFTPs_StopService             EventID (EventLevelOp,    EvtNetTFTPs,  5)
#define EvtNetTFTPs_TimeoutExpiredAbort     EventID (EventLevelError, EvtNetTFTPs,  6)
#define EvtNetTFTPs_SendBlock               EventID (EventLevelOp,    EvtNetTFTPs,  7)
#define EvtNetTFTPs_ReceiveFrame            EventID (EventLevelOp,    EvtNetTFTPs,  8)
#define EvtNetTFTPs_FrameTooShort           EventID (EventLevelError, EvtNetTFTPs,  9)
#define EvtNetTFTPs_UserAccessDenied        EventID (EventLevelError, EvtNetTFTPs, 10)
// Gap
#define EvtNetTFTPs_AccessDeniedNoResources EventID (EventLevelError, EvtNetTFTPs, 12)
#define EvtNetTFTPs_ShowClientAddress       EventID (EventLevelOp,    EvtNetTFTPs, 13)
// Gap
#define EvtNetTFTPs_OperationRequest        EventID (EventLevelOp,    EvtNetTFTPs, 15)
#define EvtNetTFTPs_SessionRestart          EventID (EventLevelOp,    EvtNetTFTPs, 16)
#define EvtNetTFTPs_InvalidTransferId       EventID (EventLevelError, EvtNetTFTPs, 17)
#define EvtNetTFTPs_ErrorCodeReceived       EventID (EventLevelError, EvtNetTFTPs, 18)
#define EvtNetTFTPs_IllegalOpcodeReceived   EventID (EventLevelError, EvtNetTFTPs, 19)
#define EvtNetTFTPs_FileRequested           EventID (EventLevelOp,    EvtNetTFTPs, 20)
#define EvtNetTFTPs_TransferBlockSize       EventID (EventLevelOp,    EvtNetTFTPs, 21)
#define EvtNetTFTPs_TransferModeNotBinary   EventID (EventLevelError, EvtNetTFTPs, 22)
#define EvtNetTFTPs_LocalFileNotFound       EventID (EventLevelError, EvtNetTFTPs, 23)
#define EvtNetTFTPs_LocalFileCreateFailed   EventID (EventLevelError, EvtNetTFTPs, 24)
#define EvtNetTFTPs_ReceiveDataFrame        EventID (EventLevelOp,    EvtNetTFTPs, 25)
#define EvtNetTFTPs_DataFrameTooShort       EventID (EventLevelError, EvtNetTFTPs, 26)
#define EvtNetTFTPs_DuplicateBlockReceived  EventID (EventLevelOp,    EvtNetTFTPs, 27)
#define EvtNetTFTPs_TooManyRetries          EventID (EventLevelError, EvtNetTFTPs, 28)
#define EvtNetTFTPs_BlockReceived           EventID (EventLevelOp,    EvtNetTFTPs, 29)
#define EvtNetTFTPs_InvalidBlockReceived    EventID (EventLevelError, EvtNetTFTPs, 30)
#define EvtNetTFTPs_WriteErrorDiskFull      EventID (EventLevelError, EvtNetTFTPs, 31)
#define EvtNetTFTPs_BlockAckReceived        EventID (EventLevelOp,    EvtNetTFTPs, 32)
#define EvtNetTFTPs_BlockRetransmit         EventID (EventLevelOp,    EvtNetTFTPs, 33)
#define EvtNetTFTPs_InvalidBlockAck         EventID (EventLevelError, EvtNetTFTPs, 34)
#define EvtNetTFTPs_ShowRequestMode         EventID (EventLevelOp,    EvtNetTFTPs, 35)
#define EvtNetTFTPs_SendOptionAck           EventID (EventLevelOp,    EvtNetTFTPs, 36)
#define EvtNetTFTPs_SendBlockAck            EventID (EventLevelOp,    EvtNetTFTPs, 37)
#define EvtNetTFTPs_SendError               EventID (EventLevelOp,    EvtNetTFTPs, 38)
#define EvtNetTFTPs_AbortSession            EventID (EventLevelOp,    EvtNetTFTPs, 39)
#define EvtNetTFTPs_CloseSession            EventID (EventLevelOp,    EvtNetTFTPs, 40)
#define EvtNetTFTPs_UninitServer            EventID (EventLevelOp,    EvtNetTFTPs, 41)
#endif

/**
  \brief  Event on TFTP server initialize (Op)
  \param  num_sessions  number of available TFTP sessions
  \param  port          listening port number
  \param  firewall_en   firewall support enable
                         - 0: disabled
                         - 1: enabled
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_InitServer(uint32_t num_sessions, uint16_t port, uint8_t firewall_en) {
    uint32_t val2 = (uint32_t)firewall_en << 16 | port;
    EventRecord2 (EvtNetTFTPs_InitServer, num_sessions, val2);
  }
#else
  #define EvrNetTFTPs_InitServer(num_sessions, port, firewall_en)
#endif

/**
  \brief  Event on TFTP server display root folder (Op)
  \param  root_folder   path to server root folder
  \param  length        length of the root_folder string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ShowRootFolder(const char *root_folder, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetTFTPs_ShowRootFolder, root_folder, length);
  }
#else
  #define EvrNetTFTPs_ShowRootFolder(root_folder, length)
#endif

/**
  \brief  Event on TFTP server failed to allocate UDP socket (Error)
  \param  session       session number (0= control session)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_GetSocketFailed(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_GetSocketFailed, session, 0);
  }
#else
  #define EvrNetTFTPs_GetSocketFailed(session)
#endif

/**
  \brief  Event on TFTP server \ref netTFTPs_SetRootPath (API)
  \param  path          path to server root folder
  \param  length        length of the path string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SetRootPath(const char *path, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetTFTPs_SetRootPath, path, length);
  }
#else
  #define EvrNetTFTPs_SetRootPath(path, length)
#endif

/**
  \brief  Event on TFTP server start service (Op)
  \param  port          listening port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_StartService(uint16_t port) {
    EventRecord2 (EvtNetTFTPs_StartService, port, 0);
  }
#else
  #define EvrNetTFTPs_StartService(port)
#endif

/**
  \brief  Event on TFTP server stop service (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_StopService(void) {
    EventRecord2 (EvtNetTFTPs_StopService, 0, 0);
  }
#else
  #define EvrNetTFTPs_StopService()
#endif

/**
  \brief  Event on TFTP server session timeout expired, abort transfer (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_TimeoutExpiredAbort(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_TimeoutExpiredAbort, session, 0);
  }
#else
  #define EvrNetTFTPs_TimeoutExpiredAbort(session)
#endif

/**
  \brief  Event on TFTP server send block (Op)
  \param  session       session number
  \param  block_nr      block number
  \param  length        length of a block
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SendBlock(uint8_t session, uint32_t block_nr, uint32_t length) {
    uint32_t val2 = (block_nr << 16) | length;
    EventRecord2 (EvtNetTFTPs_SendBlock, session, val2);
  }
#else
  #define EvrNetTFTPs_SendBlock(session, block_nr, length)
#endif

/**
  \brief  Event on TFTP server receive frame (Op)
  \param  socket        assigned UDP socket
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ReceiveFrame(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetTFTPs_ReceiveFrame, (uint32_t)socket, length);
  }
#else
  #define EvrNetTFTPs_ReceiveFrame(socket, length)
#endif

/**
  \brief  Event on TFTP server receive frame too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetTFTPs_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetTFTPs_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on TFTP server user denied access (Error)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_UserAccessDenied(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTFTPs_UserAccessDenied, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTFTPs_UserAccessDenied(net_addr)
#endif

/**
  \brief  Event on TFTP server denied client access on out of resources (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_AccessDeniedNoResources(void) {
    EventRecord2 (EvtNetTFTPs_AccessDeniedNoResources, 0, 0);
  }
#else
  #define EvrNetTFTPs_AccessDeniedNoResources()
#endif

/**
  \brief  Event on TFTP server display client IP address and port number (Op)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ShowClientAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTFTPs_ShowClientAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTFTPs_ShowClientAddress(net_addr)
#endif

/**
  \brief  Event on TFTP server process operation request (Op)
  \param  session       session number
  \param  tftp_opcode   TFTP operation code
                         - 1: read request
                         - 2: write request
                         - 3: data
                         - 4: acknowledgment
                         - 5: error
                         - 6: option acknowledgment
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_OperationRequest(uint8_t session, uint16_t tftp_opcode) {
    EventRecord2 (EvtNetTFTPs_OperationRequest, session, tftp_opcode);
  }
#else
  #define EvrNetTFTPs_OperationRequest(session, tftp_opcode)
#endif

/**
  \brief  Event on TFTP server session restart, maybe our response was lost (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SessionRestart(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_SessionRestart, session, 0);
  }
#else
  #define EvrNetTFTPs_SessionRestart(session)
#endif

/**
  \brief  Event on TFTP server invalid transfer id (TID) detected (Error)
  \param  session       session number
  \remark TID is treated in UDP socket as the source and destination port.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_InvalidTransferId(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_InvalidTransferId, session, 0);
  }
#else
  #define EvrNetTFTPs_InvalidTransferId(session)
#endif

/**
  \brief  Event on TFTP server error code received from the client (Error)
  \param  session       session number
  \param  error_code    TFTP error code
                         - 0: not defined error
                         - 1: file not found error
                         - 2: access violation error
                         - 3: disk full error
                         - 4: illegal opcode error
                         - 5: unknown TID error
                         - 6: file already exists error
                         - 7: no such user error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ErrorCodeReceived(uint8_t session, uint16_t error_code) {
    EventRecord2 (EvtNetTFTPs_ErrorCodeReceived, session, error_code);
  }
#else
  #define EvrNetTFTPs_ErrorCodeReceived(session, error_code)
#endif

/**
  \brief  Event on TFTP server illegal operation code received from the client (Error)
  \param  session       session number
  \param  tftp_opcode   TFTP operation code
                         - 1: read request
                         - 2: write request
                         - 3: data
                         - 4: acknowledgment
                         - 5: error
                         - 6: option acknowledgment
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_IllegalOpcodeReceived(uint8_t session, uint16_t tftp_opcode) {
    EventRecord2 (EvtNetTFTPs_IllegalOpcodeReceived, session, tftp_opcode);
  }
#else
  #define EvrNetTFTPs_IllegalOpcodeReceived(session, tftp_opcode)
#endif

/**
  \brief  Event on TFTP server received file request from the client (Op)
  \param  fname         name of the requested file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_FileRequested(const char *fname, uint32_t length) {
    if (length > 40) length = 40;
    EventRecordData (EvtNetTFTPs_FileRequested, fname, length);
  }
#else
  #define EvrNetTFTPs_FileRequested(fname, length)
#endif

/**
  \brief  Event on TFTP server display transfer block size (Op)
  \param  session       session number
  \param  block_size    size of transfer block
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_TransferBlockSize(uint8_t session, uint16_t block_size) {
    EventRecord2 (EvtNetTFTPs_TransferBlockSize, session, block_size);
  }
#else
  #define EvrNetTFTPs_TransferBlockSize(session, block_size)
#endif

/**
  \brief  Event on TFTP server transfer mode not set to binary (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_TransferModeNotBinary(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_TransferModeNotBinary, session, 0);
  }
#else
  #define EvrNetTFTPs_TransferModeNotBinary(session)
#endif

/**
  \brief  Event on TFTP server error, requested file not found on server (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_LocalFileNotFound(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_LocalFileNotFound, session, 0);
  }
#else
  #define EvrNetTFTPs_LocalFileNotFound(session)
#endif

/**
  \brief  Event on TFTP server local file create failed (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_LocalFileCreateFailed(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_LocalFileCreateFailed, session, 0);
  }
#else
  #define EvrNetTFTPs_LocalFileCreateFailed(session)
#endif

/**
  \brief  Event on TFTP server receive data frame (Op)
  \param  session       session number
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ReceiveDataFrame(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetTFTPs_ReceiveDataFrame, session, length);
  }
#else
  #define EvrNetTFTPs_ReceiveDataFrame(session, length)
#endif

/**
  \brief  Event on TFTP server receive data frame too short (Error)
  \param  session       session number
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_DataFrameTooShort(uint8_t session, uint32_t length, uint32_t min_length) {
    uint32_t val2 = (min_length << 16) | length;
    EventRecord2 (EvtNetTFTPs_DataFrameTooShort, session, val2);
  }
#else
  #define EvrNetTFTPs_DataFrameTooShort(session, length, min_length)
#endif

/**
  \brief  Event on TFTP server duplicate block received (Op)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_DuplicateBlockReceived(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_DuplicateBlockReceived, session, block_nr);
  }
#else
  #define EvrNetTFTPs_DuplicateBlockReceived(session, block_nr)
#endif

/**
  \brief  Event on TFTP server too many retransmissions (Error)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_TooManyRetries(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_TooManyRetries, session, 0);
  }
#else
  #define EvrNetTFTPs_TooManyRetries(session)
#endif

/**
  \brief  Event on TFTP server data block received (Op)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_BlockReceived(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_BlockReceived, session, block_nr);
  }
#else
  #define EvrNetTFTPs_BlockReceived(session, block_nr)
#endif

/**
  \brief  Event on TFTP server invalid block received (Error)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_InvalidBlockReceived(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_InvalidBlockReceived, session, block_nr);
  }
#else
  #define EvrNetTFTPs_InvalidBlockReceived(session, block_nr)
#endif

/**
  \brief  Event on TFTP server write local disk failed, disk full (Error)
  \param  session       session number
  \param  length        block data length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_WriteErrorDiskFull(uint8_t session, uint32_t length) {
    EventRecord2 (EvtNetTFTPs_WriteErrorDiskFull, session, length);
  }
#else
  #define EvrNetTFTPs_WriteErrorDiskFull(session, length)
#endif

/**
  \brief  Event on TFTP server block acknowledge received (Op)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_BlockAckReceived(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_BlockAckReceived, session, block_nr);
  }
#else
  #define EvrNetTFTPs_BlockAckReceived(session, block_nr)
#endif

/**
  \brief  Event on TFTP server block retransmit (Op)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_BlockRetransmit(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_BlockRetransmit, session, block_nr);
  }
#else
  #define EvrNetTFTPs_BlockRetransmit(session, block_nr)
#endif

/**
  \brief  Event on TFTP server invalid block acknowledge received (Error)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_InvalidBlockAck(uint8_t session, uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPs_InvalidBlockAck, session, block_nr);
  }
#else
  #define EvrNetTFTPs_InvalidBlockAck(session, block_nr)
#endif

/**
  \brief  Event on TFTP server display received request mode parameter (Op)
  \param  mode          mode parameter in text format
  \param  length        length of the mode string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_ShowRequestMode(const uint8_t *mode, uint32_t length) {
    if (length > 16) length = 16;
    EventRecordData (EvtNetTFTPs_ShowRequestMode, mode, length);
  }
#else
  #define EvrNetTFTPs_ShowRequestMode(mode, length)
#endif

/**
  \brief  Event on TFTP server send option acknowledgment (Op)
  \param  session       session number
  \param  block_size    block size
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SendOptionAck(uint8_t session, uint32_t block_size) {
    EventRecord2 (EvtNetTFTPs_SendOptionAck, session, block_size);
  }
#else
  #define EvrNetTFTPs_SendOptionAck(session, block_size)
#endif

/**
  \brief  Event on TFTP server send block acknowledgment (Op)
  \param  session       session number
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SendBlockAck(uint8_t session, uint16_t block_nr) {
    EventRecord2 (EvtNetTFTPs_SendBlockAck, session, block_nr);
  }
#else
  #define EvrNetTFTPs_SendBlockAck(session, block_nr)
#endif

/**
  \brief  Event on TFTP server send error code (Op)
  \param  session       session number
  \param  error_nr      TFTP error number
                         - 0: not defined error
                         - 1: file not found error
                         - 2: access violation error
                         - 3: disk full error
                         - 4: illegal opcode error
                         - 5: unknown TID error
                         - 6: file already exists error
                         - 7: no such user error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_SendError(uint8_t session, uint16_t error_nr) {
    EventRecord2 (EvtNetTFTPs_SendError, session, error_nr);
  }
#else
  #define EvrNetTFTPs_SendError(session, error_nr)
#endif

/**
  \brief  Event on TFTP server abnormal session close on error (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_AbortSession(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_AbortSession, session, 0);
  }
#else
  #define EvrNetTFTPs_AbortSession(session)
#endif

/**
  \brief  Event on TFTP server normal session close (Op)
  \param  session       session number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_CloseSession(uint8_t session) {
    EventRecord2 (EvtNetTFTPs_CloseSession, session, 0);
  }
#else
  #define EvrNetTFTPs_CloseSession(session)
#endif

/**
  \brief  Event on TFTP server de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPs_UninitServer(void) {
    EventRecord2 (EvtNetTFTPs_UninitServer, 0, 0);
  }
#else
  #define EvrNetTFTPs_UninitServer()
#endif


// NetTFTPc event identifiers --------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetTFTPc_InitClient              EventID (EventLevelOp,    EvtNetTFTPc,  0)
#define EvtNetTFTPc_GetSocketFailed         EventID (EventLevelError, EvtNetTFTPc,  1)
#define EvtNetTFTPc_PutFile                 EventID (EventLevelAPI,   EvtNetTFTPc,  2)
#define EvtNetTFTPc_PutRemoteName           EventID (EventLevelOp,    EvtNetTFTPc,  3)
#define EvtNetTFTPc_PutInvalidParameter     EventID (EventLevelError, EvtNetTFTPc,  4)
#define EvtNetTFTPc_PutWrongState           EventID (EventLevelError, EvtNetTFTPc,  5)
#define EvtNetTFTPc_ShowServerAddress       EventID (EventLevelOp,    EvtNetTFTPc,  6)
// Gap
#define EvtNetTFTPc_OpenLocalFile           EventID (EventLevelOp,    EvtNetTFTPc,  8)
#define EvtNetTFTPc_OpenLocalFileFailed     EventID (EventLevelError, EvtNetTFTPc,  9)
#define EvtNetTFTPc_GetFile                 EventID (EventLevelAPI,   EvtNetTFTPc, 10)
#define EvtNetTFTPc_GetLocalName            EventID (EventLevelOp,    EvtNetTFTPc, 11)
#define EvtNetTFTPc_GetInvalidParameter     EventID (EventLevelError, EvtNetTFTPc, 12)
#define EvtNetTFTPc_GetWrongState           EventID (EventLevelError, EvtNetTFTPc, 13)
#define EvtNetTFTPc_TimeoutBlockRetransmit  EventID (EventLevelOp,    EvtNetTFTPc, 14)
#define EvtNetTFTPc_SendBlock               EventID (EventLevelOp,    EvtNetTFTPc, 15)
#define EvtNetTFTPc_StopClient              EventID (EventLevelOp,    EvtNetTFTPc, 16)
#define EvtNetTFTPc_CloseLocalFile          EventID (EventLevelOp,    EvtNetTFTPc, 17)
#define EvtNetTFTPc_WrongServerAddress      EventID (EventLevelError, EvtNetTFTPc, 18)
// Gap
#define EvtNetTFTPc_WrongServerPort         EventID (EventLevelError, EvtNetTFTPc, 20)
#define EvtNetTFTPc_ServerTidAssigned       EventID (EventLevelOp,    EvtNetTFTPc, 21)
#define EvtNetTFTPc_FrameTooShort           EventID (EventLevelError, EvtNetTFTPc, 22)
#define EvtNetTFTPc_ReceiveFrame            EventID (EventLevelOp,    EvtNetTFTPc, 23)
#define EvtNetTFTPc_ErrorCodeReceived       EventID (EventLevelError, EvtNetTFTPc, 24)
#define EvtNetTFTPc_DuplicateBlockAck       EventID (EventLevelOp,    EvtNetTFTPc, 25)
#define EvtNetTFTPc_InvalidBlockAck         EventID (EventLevelError, EvtNetTFTPc, 26)
#define EvtNetTFTPc_BlockAckReceived        EventID (EventLevelOp,    EvtNetTFTPc, 27)
#define EvtNetTFTPc_DuplicateBlockReceived  EventID (EventLevelOp,    EvtNetTFTPc, 28)
#define EvtNetTFTPc_InvalidBlockReceived    EventID (EventLevelError, EvtNetTFTPc, 29)
#define EvtNetTFTPc_BlockReceived           EventID (EventLevelOp,    EvtNetTFTPc, 30)
#define EvtNetTFTPc_WriteErrorDiskFull      EventID (EventLevelError, EvtNetTFTPc, 31)
#define EvtNetTFTPc_IllegalServerOperation  EventID (EventLevelError, EvtNetTFTPc, 32)
#define EvtNetTFTPc_SendRequest             EventID (EventLevelOp,    EvtNetTFTPc, 33)
#define EvtNetTFTPc_OptionBlockSize         EventID (EventLevelOp,    EvtNetTFTPc, 34)
#define EvtNetTFTPc_SendAck                 EventID (EventLevelOp,    EvtNetTFTPc, 35)
#define EvtNetTFTPc_SendError               EventID (EventLevelOp,    EvtNetTFTPc, 36)
#define EvtNetTFTPc_OptionAckReceived       EventID (EventLevelOp,    EvtNetTFTPc, 37)
#define EvtNetTFTPc_UninitClient            EventID (EventLevelOp,    EvtNetTFTPc, 38)
#endif

/**
  \brief  Event on TFTP client initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_InitClient(void) {
    EventRecord2 (EvtNetTFTPc_InitClient, 0, 0);
  }
#else
  #define EvrNetTFTPc_InitClient()
#endif

/**
  \brief  Event on TFTP client failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_GetSocketFailed(void) {
    EventRecord2 (EvtNetTFTPc_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetTFTPc_GetSocketFailed()
#endif

/**
  \brief  Event on TFTP client \ref netTFTPc_Put (API)
  \param  local_fname   name of the local file
  \param  length        length of the local_fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_PutFile(const char *local_fname, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData (EvtNetTFTPc_PutFile, local_fname, length);
  }
#else
  #define EvrNetTFTPc_PutFile(local_fname, length)
#endif

/**
  \brief  Event on TFTP client \ref netTFTPc_Put remote filename (Op)
  \param  fname         name of the remote file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_PutRemoteName(const char *fname, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData (EvtNetTFTPc_PutRemoteName, fname, length);
  }
#else
  #define EvrNetTFTPc_PutRemoteName(fname, length)
#endif

/**
  \brief  Event on \ref netTFTPc_Put invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_PutInvalidParameter(void) {
    EventRecord2 (EvtNetTFTPc_PutInvalidParameter, 0, 0);
  }
#else
  #define EvrNetTFTPc_PutInvalidParameter()
#endif

/**
  \brief  Event on \ref netTFTPc_Put wrong client state (Error)
  \param  state         client state
                         - 0: idle
                         - 1: put file
                         - 2: get file
                         - 3: stop client
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_PutWrongState(uint8_t state) {
    EventRecord2 (EvtNetTFTPc_PutWrongState, state, 0);
  }
#else
  #define EvrNetTFTPc_PutWrongState(state)
#endif

/**
  \brief  Event on TFTP client display server IP address (Op)
  \param  net_addr      pointer to \ref NET_ADDR server address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_ShowServerAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTFTPc_ShowServerAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTFTPc_ShowServerAddress(net_addr)
#endif

/**
  \brief  Event on TFTP client open local file (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_OpenLocalFile(void) {
    EventRecord2 (EvtNetTFTPc_OpenLocalFile, 0, 0);
  }
#else
  #define EvrNetTFTPc_OpenLocalFile()
#endif

/**
  \brief  Event on TFTP client open local file failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_OpenLocalFileFailed(void) {
    EventRecord2 (EvtNetTFTPc_OpenLocalFileFailed, 0, 0);
  }
#else
  #define EvrNetTFTPc_OpenLocalFileFailed()
#endif

/**
  \brief  Event on TFTP client \ref netTFTPc_Get (API)
  \param  fname         name of the remote file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_GetFile(const char *fname, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData (EvtNetTFTPc_GetFile, fname, length);
  }
#else
  #define EvrNetTFTPc_GetFile(fname, length)
#endif

/**
  \brief  Event on TFTP client \ref netTFTPc_Get local filename (Op)
  \param  local_fname   name of the local file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_GetLocalName(const char *local_fname, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData (EvtNetTFTPc_GetLocalName, local_fname, length);
  }
#else
  #define EvrNetTFTPc_GetLocalName(local_fname, length)
#endif

/**
  \brief  Event on \ref netTFTPc_Get invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_GetInvalidParameter(void) {
    EventRecord2 (EvtNetTFTPc_GetInvalidParameter, 0, 0);
  }
#else
  #define EvrNetTFTPc_GetInvalidParameter()
#endif

/**
  \brief  Event on \ref netTFTPc_Get wrong client state (Error)
  \param  state         client state
                         - 0: idle
                         - 1: put file
                         - 2: get file
                         - 3: stop client
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_GetWrongState(uint8_t state) {
    EventRecord2 (EvtNetTFTPc_GetWrongState, state, 0);
  }
#else
  #define EvrNetTFTPc_GetWrongState(state)
#endif

/**
  \brief  Event on TFTP client block retransmit on timeout (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_TimeoutBlockRetransmit(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_TimeoutBlockRetransmit, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_TimeoutBlockRetransmit(block_nr)
#endif

/**
  \brief  Event on TFTP client send block (Op)
  \param  block_nr      block number
  \param  length        length of a block
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_SendBlock(uint32_t block_nr, uint32_t length) {
    EventRecord2 (EvtNetTFTPc_SendBlock, block_nr, length);
  }
#else
  #define EvrNetTFTPc_SendBlock(block_nr, length)
#endif

/**
  \brief  Event on TFTP client stop operation (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_StopClient(void) {
    EventRecord2 (EvtNetTFTPc_StopClient, 0, 0);
  }
#else
  #define EvrNetTFTPc_StopClient()
#endif

/**
  \brief  Event on TFTP client close local file (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_CloseLocalFile(void) {
    EventRecord2 (EvtNetTFTPc_CloseLocalFile, 0, 0);
  }
#else
  #define EvrNetTFTPc_CloseLocalFile()
#endif

/**
  \brief  Event on TFTP client wrong server IP address (Error)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_WrongServerAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetTFTPc_WrongServerAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetTFTPc_WrongServerAddress(net_addr)
#endif

/**
  \brief  Event on TFTP client wrong server port (Error)
  \param  udp_port      wrong UDP port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_WrongServerPort(uint16_t udp_port) {
    EventRecord2 (EvtNetTFTPc_WrongServerPort, udp_port, 0);
  }
#else
  #define EvrNetTFTPc_WrongServerPort(udp_port)
#endif

/**
  \brief  Event on TFTP client assigned transfer identifier (TID) of the server (Op)
  \param  tid           server transfer identifier
  \remark Transfer identifier is used as UDP port number.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_ServerTidAssigned(uint16_t tid) {
    EventRecord2 (EvtNetTFTPc_ServerTidAssigned, tid, 0);
  }
#else
  #define EvrNetTFTPc_ServerTidAssigned(tid)
#endif

/**
  \brief  Event on TFTP client receive frame too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetTFTPc_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetTFTPc_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on TFTP client receive frame (Op)
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_ReceiveFrame(uint32_t length) {
    EventRecord2 (EvtNetTFTPc_ReceiveFrame, length, 0);
  }
#else
  #define EvrNetTFTPc_ReceiveFrame(length)
#endif

/**
  \brief  Event on TFTP client error code received from server (Error)
  \param  error_code    TFTP error code
                         - 0: not defined error
                         - 1: file not found error
                         - 2: access violation error
                         - 3: disk full error
                         - 4: illegal opcode error
                         - 5: unknown TID error
                         - 6: file already exists error
                         - 7: no such user error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_ErrorCodeReceived(uint16_t error_code) {
    EventRecord2 (EvtNetTFTPc_ErrorCodeReceived, error_code, 0);
  }
#else
  #define EvrNetTFTPc_ErrorCodeReceived(error_code)
#endif

/**
  \brief  Event on TFTP client duplicate block acknowledge received (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_DuplicateBlockAck(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_DuplicateBlockAck, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_DuplicateBlockAck(block_nr)
#endif

/**
  \brief  Event on TFTP client invalid block acknowledge received (Error)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_InvalidBlockAck(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_InvalidBlockAck, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_InvalidBlockAck(block_nr)
#endif

/**
  \brief  Event on TFTP client block acknowledge received (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_BlockAckReceived(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_BlockAckReceived, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_BlockAckReceived(block_nr)
#endif

/**
  \brief  Event on TFTP client duplicate block received (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_DuplicateBlockReceived(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_DuplicateBlockReceived, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_DuplicateBlockReceived(block_nr)
#endif

/**
  \brief  Event on TFTP client invalid block received (Error)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_InvalidBlockReceived(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_InvalidBlockReceived, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_InvalidBlockReceived(block_nr)
#endif

/**
  \brief  Event on TFTP client block data received (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_BlockReceived(uint32_t block_nr) {
    EventRecord2 (EvtNetTFTPc_BlockReceived, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_BlockReceived(block_nr)
#endif

/**
  \brief  Event on TFTP client write local disk failed, disk full (Error)
  \param  length        block data length
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_WriteErrorDiskFull(uint32_t length) {
    EventRecord2 (EvtNetTFTPc_WriteErrorDiskFull, length, 0);
  }
#else
  #define EvrNetTFTPc_WriteErrorDiskFull(length)
#endif

/**
  \brief  Event on TFTP client deteced illegal server operation (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_IllegalServerOperation(void) {
    EventRecord2 (EvtNetTFTPc_IllegalServerOperation, 0, 0);
  }
#else
  #define EvrNetTFTPc_IllegalServerOperation()
#endif

/**
  \brief  Event on TFTP client send operation request (Op)
  \param  tftp_opcode   TFTP operation code
                         - 1: read request
                         - 2: write request
                         - 3: data
                         - 4: acknowledgment
                         - 5: error
                         - 6: option acknowledgment
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_SendRequest(uint16_t tftp_opcode) {
    EventRecord2 (EvtNetTFTPc_SendRequest, tftp_opcode, 0);
  }
#else
  #define EvrNetTFTPc_SendRequest(tftp_opcode)
#endif

/**
  \brief  Event on TFTP client add block size option (Op)
  \param  block_size    size of transfer block
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_OptionBlockSize(uint16_t block_size) {
    EventRecord2 (EvtNetTFTPc_OptionBlockSize, block_size, 0);
  }
#else
  #define EvrNetTFTPc_OptionBlockSize(block_size)
#endif

/**
  \brief  Event on TFTP client send acknowledgment (Op)
  \param  block_nr      block number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_SendAck(uint16_t block_nr) {
    EventRecord2 (EvtNetTFTPc_SendAck, block_nr, 0);
  }
#else
  #define EvrNetTFTPc_SendAck(block_nr)
#endif

/**
  \brief  Event on TFTP client send error code (Op)
  \param  error_nr      TFTP error number
                         - 0: not defined error
                         - 1: file not found error
                         - 2: access violation error
                         - 3: disk full error
                         - 4: illegal opcode error
                         - 5: unknown TID error
                         - 6: file already exists error
                         - 7: no such user error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_SendError(uint16_t error_nr) {
    EventRecord2 (EvtNetTFTPc_SendError, error_nr, 0);
  }
#else
  #define EvrNetTFTPc_SendError(error_nr)
#endif

/**
  \brief  Event on TFTP client option acknowledgment received (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_OptionAckReceived(void) {
    EventRecord2 (EvtNetTFTPc_OptionAckReceived, 0, 0);
  }
#else
  #define EvrNetTFTPc_OptionAckReceived()
#endif

/**
  \brief  Event on TFTP client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetTFTPc_UninitClient(void) {
    EventRecord2 (EvtNetTFTPc_UninitClient, 0, 0);
  }
#else
  #define EvrNetTFTPc_UninitClient()
#endif


// NetSMTP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetSMTP_InitClient               EventID (EventLevelOp,    EvtNetSMTP,  0)
#define EvtNetSMTP_GetSocketFailed          EventID (EventLevelError, EvtNetSMTP,  1)
#define EvtNetSMTP_Connect                  EventID (EventLevelAPI,   EvtNetSMTP,  2)
// Gap
#define EvtNetSMTP_ConnectInvalidParameter  EventID (EventLevelError, EvtNetSMTP,  4)
#define EvtNetSMTP_ConnectClientBusy        EventID (EventLevelError, EvtNetSMTP,  5)
#define EvtNetSMTP_SendMail                 EventID (EventLevelAPI,   EvtNetSMTP,  6)
#define EvtNetSMTP_SendMailInvalidParameter EventID (EventLevelError, EvtNetSMTP,  7)
#define EvtNetSMTP_SendMailTlsNotEnabled    EventID (EventLevelError, EvtNetSMTP,  8)
#define EvtNetSMTP_SendMailClientBusy       EventID (EventLevelError, EvtNetSMTP,  9)
#define EvtNetSMTP_SendMailNoRecipients     EventID (EventLevelError, EvtNetSMTP, 10)
#define EvtNetSMTP_SendMailServerNotValid   EventID (EventLevelError, EvtNetSMTP, 11)
#define EvtNetSMTP_SendMailDnsError         EventID (EventLevelError, EvtNetSMTP, 12)
#define EvtNetSMTP_SendMailAttachNotEnabled EventID (EventLevelError, EvtNetSMTP, 13)
#define EvtNetSMTP_SendMailAttachFailed     EventID (EventLevelError, EvtNetSMTP, 14)
#define EvtNetSMTP_SendMailMultipart        EventID (EventLevelOp,    EvtNetSMTP, 15)
#define EvtNetSMTP_SendMailAttachment       EventID (EventLevelOp,    EvtNetSMTP, 16)
#define EvtNetSMTP_InboundConnRejected      EventID (EventLevelOp,    EvtNetSMTP, 17)
#define EvtNetSMTP_SocketAborted            EventID (EventLevelError, EvtNetSMTP, 18)
#define EvtNetSMTP_SocketConnected          EventID (EventLevelOp,    EvtNetSMTP, 19)
#define EvtNetSMTP_SocketClosed             EventID (EventLevelOp,    EvtNetSMTP, 20)
#define EvtNetSMTP_ReceiveFrame             EventID (EventLevelOp,    EvtNetSMTP, 21)
#define EvtNetSMTP_UnackedDataError         EventID (EventLevelError, EvtNetSMTP, 22)
#define EvtNetSMTP_FrameTooShort            EventID (EventLevelError, EvtNetSMTP, 23)
#define EvtNetSMTP_ServerReady              EventID (EventLevelOp,    EvtNetSMTP, 24)
#define EvtNetSMTP_EsmtpModeNotSupported    EventID (EventLevelOp,    EvtNetSMTP, 25)
#define EvtNetSMTP_EhloResponseFragmented   EventID (EventLevelOp,    EvtNetSMTP, 26)
#define EvtNetSMTP_EsmtpModeActive          EventID (EventLevelOp,    EvtNetSMTP, 27)
#define EvtNetSMTP_StartAuthentication      EventID (EventLevelOp,    EvtNetSMTP, 28)
#define EvtNetSMTP_AuthenticationDenied     EventID (EventLevelOp,    EvtNetSMTP, 29)
#define EvtNetSMTP_AuthMethodNotSupported   EventID (EventLevelError, EvtNetSMTP, 30)
#define EvtNetSMTP_SmtpModeActive           EventID (EventLevelOp,    EvtNetSMTP, 31)
#define EvtNetSMTP_AuthenticationSuccessful EventID (EventLevelOp,    EvtNetSMTP, 32)
#define EvtNetSMTP_AuthenticationFailed     EventID (EventLevelError, EvtNetSMTP, 33)
#define EvtNetSMTP_ServerAcknowledge        EventID (EventLevelOp,    EvtNetSMTP, 34)
#define EvtNetSMTP_WrongResponse            EventID (EventLevelError, EvtNetSMTP, 35)
#define EvtNetSMTP_ClientStopTimeoutExpired EventID (EventLevelError, EvtNetSMTP, 36)
#define EvtNetSMTP_SendMessageBody          EventID (EventLevelOp,    EvtNetSMTP, 37)
#define EvtNetSMTP_SendMessageEnd           EventID (EventLevelOp,    EvtNetSMTP, 38)
#define EvtNetSMTP_SendCommand              EventID (EventLevelDetail,EvtNetSMTP, 39)
#define EvtNetSMTP_ClientCloseSocket        EventID (EventLevelOp,    EvtNetSMTP, 40)
#define EvtNetSMTP_ClientDone               EventID (EventLevelOp,    EvtNetSMTP, 41)
#define EvtNetSMTP_TlsSupportIndicated      EventID (EventLevelOp,    EvtNetSMTP, 42)
#define EvtNetSMTP_StartTlsAccepted         EventID (EventLevelOp,    EvtNetSMTP, 43)
#define EvtNetSMTP_TlsGetContextFailed      EventID (EventLevelError, EvtNetSMTP, 44)
#define EvtNetSMTP_TlsModeStarted           EventID (EventLevelOp,    EvtNetSMTP, 45)
#define EvtNetSMTP_TlsModeEstablished       EventID (EventLevelOp,    EvtNetSMTP, 46)
#define EvtNetSMTP_UninitClient             EventID (EventLevelOp,    EvtNetSMTP, 47)
#endif

/**
  \brief  Event on SMTP client initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_InitClient(void) {
    EventRecord2 (EvtNetSMTP_InitClient, 0, 0);
  }
#else
  #define EvrNetSMTP_InitClient()
#endif

/**
  \brief  Event on SMTP client failed to allocate TCP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_GetSocketFailed(void) {
    EventRecord2 (EvtNetSMTP_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetSMTP_GetSocketFailed()
#endif

/**
  \brief  Event on SMTP client \ref netSMTPc_Connect to the server (API)
  \param  net_addr      pointer to \ref NET_ADDR server address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_Connect(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetSMTP_Connect, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetSMTP_Connect(net_addr)
#endif

/**
  \brief  Event on \ref netSMTPc_Connect invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ConnectInvalidParameter(void) {
    EventRecord2 (EvtNetSMTP_ConnectInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSMTP_ConnectInvalidParameter()
#endif

/**
  \brief  Event on \ref netSMTPc_Connect failed, client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ConnectClientBusy(void) {
    EventRecord2 (EvtNetSMTP_ConnectClientBusy, 0, 0);
  }
#else
  #define EvrNetSMTP_ConnectClientBusy()
#endif

/**
  \brief  Event on SMTP client \ref netSMTPc_SendMail to mail server (API)
  \param  num_rcpt      number of mail recipients (To, CC, BCC)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMail(uint8_t num_rcpt) {
    EventRecord2 (EvtNetSMTP_SendMail, num_rcpt, 0);
  }
#else
  #define EvrNetSMTP_SendMail(num_rcpt)
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailInvalidParameter(void) {
    EventRecord2 (EvtNetSMTP_SendMailInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailInvalidParameter()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail secure TLS not enabled (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailTlsNotEnabled(void) {
    EventRecord2 (EvtNetSMTP_SendMailTlsNotEnabled, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailTlsNotEnabled()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail failed, client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailClientBusy(void) {
    EventRecord2 (EvtNetSMTP_SendMailClientBusy, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailClientBusy()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail failed, mail recipient not specified (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailNoRecipients(void) {
    EventRecord2 (EvtNetSMTP_SendMailNoRecipients, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailNoRecipients()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail failed, mail server not valid (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailServerNotValid(void) {
    EventRecord2 (EvtNetSMTP_SendMailServerNotValid, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailServerNotValid()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail failed, DNS host resolver error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailDnsError(void) {
    EventRecord2 (EvtNetSMTP_SendMailDnsError, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailDnsError()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail failed, support for attachments not enabled (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailAttachNotEnabled(void) {
    EventRecord2 (EvtNetSMTP_SendMailAttachNotEnabled, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMailAttachNotEnabled()
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail attach file failed, fopen failed (Error)
  \param  fname         name of the requested file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailAttachFailed(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetSMTP_SendMailAttachFailed, fname, length);
  }
#else
  #define EvrNetSMTP_SendMailAttachFailed(fname, length)
#endif

/**
  \brief  Event on SMTP client \ref netSMTPc_SendMail multipart with attachments (Op)
  \param  num_files     number of files to attach
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailMultipart(uint8_t num_files) {
    EventRecord2 (EvtNetSMTP_SendMailMultipart, num_files, 0);
  }
#else
  #define EvrNetSMTP_SendMailMultipart(num_files)
#endif

/**
  \brief  Event on \ref netSMTPc_SendMail attach file (Op)
  \param  fname         name of the requested file
  \param  length        length of the fname string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMailAttachment(const char *fname, uint32_t length) {
    if (length > 80) length = 80;
    EventRecordData (EvtNetSMTP_SendMailAttachment, fname, length);
  }
#else
  #define EvrNetSMTP_SendMailAttachment(fname, length)
#endif

/**
  \brief  Event on SMTP client inbound connection rejected (Error)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_InboundConnRejected(int32_t socket) {
    EventRecord2 (EvtNetSMTP_InboundConnRejected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_InboundConnRejected(socket)
#endif

/**
  \brief  Event on SMTP client socket aborted (Error)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SocketAborted(int32_t socket) {
    EventRecord2 (EvtNetSMTP_SocketAborted, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_SocketAborted(socket)
#endif

/**
  \brief  Event on SMTP client socket connected (Op)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SocketConnected(int32_t socket) {
    EventRecord2 (EvtNetSMTP_SocketConnected, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_SocketConnected(socket)
#endif

/**
  \brief  Event on SMTP client socket closed (Op)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SocketClosed(int32_t socket) {
    EventRecord2 (EvtNetSMTP_SocketClosed, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_SocketClosed(socket)
#endif

/**
  \brief  Event on SMTP client received frame (Op)
  \param  socket        assigned TCP socket
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ReceiveFrame(int32_t socket, uint32_t length) {
    EventRecord2 (EvtNetSMTP_ReceiveFrame, (uint32_t)socket, length);
  }
#else
  #define EvrNetSMTP_ReceiveFrame(socket, length)
#endif

/**
  \brief  Event on SMTP client unacked data error (Error)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_UnackedDataError(int32_t socket) {
    EventRecord2 (EvtNetSMTP_UnackedDataError, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_UnackedDataError(socket)
#endif

/**
  \brief  Event on SMTP receive frame is too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetSMTP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetSMTP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on SMTP client server ready response (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ServerReady(void) {
    EventRecord2 (EvtNetSMTP_ServerReady, 0, 0);
  }
#else
  #define EvrNetSMTP_ServerReady()
#endif

/**
  \brief  Event on SMTP client Extended SMTP mode (ESMTP) not supported (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_EsmtpModeNotSupported(void) {
    EventRecord2 (EvtNetSMTP_EsmtpModeNotSupported, 0, 0);
  }
#else
  #define EvrNetSMTP_EsmtpModeNotSupported()
#endif

/**
  \brief  Event on SMTP client EHLO response divided into multiple frames (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_EhloResponseFragmented(void) {
    EventRecord2 (EvtNetSMTP_EhloResponseFragmented, 0, 0);
  }
#else
  #define EvrNetSMTP_EhloResponseFragmented()
#endif

/**
  \brief  Event on SMTP client Extended SMTP mode (ESMTP) active (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_EsmtpModeActive(void) {
    EventRecord2 (EvtNetSMTP_EsmtpModeActive, 0, 0);
  }
#else
  #define EvrNetSMTP_EsmtpModeActive()
#endif

/**
  \brief  Event on SMTP client start authentication (Op)
  \param  auth_mode     SMTP authentication mode
                         - 0: not required
                         - 1: PLAIN
                         - 2: LOGIN
                         - 3: DIGEST-MD5
                         - 4: CRAM-MD5
                         - 5: NTLM
                         - 6: unknown
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_StartAuthentication(uint8_t auth_mode) {
    EventRecord2 (EvtNetSMTP_StartAuthentication, auth_mode, 0);
  }
#else
  #define EvrNetSMTP_StartAuthentication(auth_mode)
#endif

/**
  \brief  Event on SMTP client authentication denied by the user (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_AuthenticationDenied(void) {
    EventRecord2 (EvtNetSMTP_AuthenticationDenied, 0, 0);
  }
#else
  #define EvrNetSMTP_AuthenticationDenied()
#endif

/**
  \brief  Event on SMTP client authentication method not supported (Error)
  \param  auth_mode     SMTP authentication mode
                         - 0: not required
                         - 1: PLAIN
                         - 2: LOGIN
                         - 3: DIGEST-MD5
                         - 4: CRAM-MD5
                         - 5: NTLM
                         - 6: unknown
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_AuthMethodNotSupported(uint8_t auth_mode) {
    EventRecord2 (EvtNetSMTP_AuthMethodNotSupported, auth_mode, 0);
  }
#else
  #define EvrNetSMTP_AuthMethodNotSupported(auth_mode)
#endif

/**
  \brief  Event on SMTP client basic SMTP mode active (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SmtpModeActive(void) {
    EventRecord2 (EvtNetSMTP_SmtpModeActive, 0, 0);
  }
#else
  #define EvrNetSMTP_SmtpModeActive()
#endif

/**
  \brief  Event on SMTP client authentication completed successfully (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_AuthenticationSuccessful(void) {
    EventRecord2 (EvtNetSMTP_AuthenticationSuccessful, 0, 0);
  }
#else
  #define EvrNetSMTP_AuthenticationSuccessful()
#endif

/**
  \brief  Event on SMTP client authentication failed (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_AuthenticationFailed(void) {
    EventRecord2 (EvtNetSMTP_AuthenticationFailed, 0, 0);
  }
#else
  #define EvrNetSMTP_AuthenticationFailed()
#endif

/**
  \brief  Event on SMTP client received server acknowledgment (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ServerAcknowledge(void) {
    EventRecord2 (EvtNetSMTP_ServerAcknowledge, 0, 0);
  }
#else
  #define EvrNetSMTP_ServerAcknowledge()
#endif

/**
  \brief  Event on SMTP client received wrong response from server (Error)
  \param  response      server response in text form
  \param  length        length of the response string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_WrongResponse(const uint8_t *response, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetSMTP_WrongResponse, response, length);
  }
#else
  #define EvrNetSMTP_WrongResponse(response, length)
#endif

/**
  \brief  Event on SMTP client stop operation, timeout expired (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ClientStopTimeoutExpired(void) {
    EventRecord2 (EvtNetSMTP_ClientStopTimeoutExpired, 0, 0);
  }
#else
  #define EvrNetSMTP_ClientStopTimeoutExpired()
#endif

/**
  \brief  Event on SMTP client send message body start (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMessageBody(void) {
    EventRecord2 (EvtNetSMTP_SendMessageBody, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMessageBody()
#endif

/**
  \brief  Event on SMTP client send end of message sequence (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendMessageEnd(void) {
    EventRecord2 (EvtNetSMTP_SendMessageEnd, 0, 0);
  }
#else
  #define EvrNetSMTP_SendMessageEnd()
#endif

/**
  \brief  Event on SMTP client send command to server (Detail)
  \param  command       server command in text form
  \param  length        length of the command string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_SendCommand(const uint8_t *command, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetSMTP_SendCommand, command, length);
  }
#else
  #define EvrNetSMTP_SendCommand(command, length)
#endif

/**
  \brief  Event on SMTP client close socket (Op)
  \param  socket        assigned TCP socket
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ClientCloseSocket(int32_t socket) {
    EventRecord2 (EvtNetSMTP_ClientCloseSocket, (uint32_t)socket, 0);
  }
#else
  #define EvrNetSMTP_ClientCloseSocket(socket)
#endif

/**
  \brief  Event on SMTP client completed operation (Op)
  \param  cb_event      user callback nofication event
                         - 0: Email successfully sent
                         - 1: Timeout on sending email
                         - 2: Login failed, username/password invalid
                         - 3: Generic SMTP client error
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_ClientDone(uint8_t cb_event) {
    EventRecord2 (EvtNetSMTP_ClientDone, cb_event, 0);
  }
#else
  #define EvrNetSMTP_ClientDone(cb_event)
#endif

/**
  \brief  Event on SMTP client TLS support indicated by the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_TlsSupportIndicated(void) {
    EventRecord2 (EvtNetSMTP_TlsSupportIndicated, 0, 0);
  }
#else
  #define EvrNetSMTP_TlsSupportIndicated()
#endif

/**
  \brief  Event on SMTP client STARTTLS command accepted by the server (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_StartTlsAccepted(void) {
    EventRecord2 (EvtNetSMTP_StartTlsAccepted, 0, 0);
  }
#else
  #define EvrNetSMTP_StartTlsAccepted()
#endif

/**
  \brief  Event on SMTP client failed to allocate secure TLS context (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_TlsGetContextFailed(void) {
    EventRecord2 (EvtNetSMTP_TlsGetContextFailed, 0, 0);
  }
#else
  #define EvrNetSMTP_TlsGetContextFailed()
#endif

/**
  \brief  Event on SMTP client started secure TLS mode (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_TlsModeStarted(void) {
    EventRecord2 (EvtNetSMTP_TlsModeStarted, 0, 0);
  }
#else
  #define EvrNetSMTP_TlsModeStarted()
#endif

/**
  \brief  Event on SMTP client established secure TLS session (Op)
  \param  tls_id        TLS session identifier
*/
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_TlsModeEstablished(uint8_t tls_id) {
    EventRecord2 (EvtNetSMTP_TlsModeEstablished, tls_id, 0);
  }
#else
  #define EvrNetSMTP_TlsModeEstablished(tls_id)
#endif

/**
  \brief  Event on SMTP client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSMTP_UninitClient(void) {
    EventRecord2 (EvtNetSMTP_UninitClient, 0, 0);
  }
#else
  #define EvrNetSMTP_UninitClient()
#endif


// NetDNS event identifiers ----------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetDNS_InitClient                EventID (EventLevelOp,    EvtNetDNS,  0)
#define EvtNetDNS_GetSocketFailed           EventID (EventLevelError, EvtNetDNS,  1)
#define EvtNetDNS_ChangeDnsServer           EventID (EventLevelOp,    EvtNetDNS,  2)
#define EvtNetDNS_ClientStopTimeoutExpired  EventID (EventLevelError, EvtNetDNS,  3)
#define EvtNetDNS_ReceiveFrame              EventID (EventLevelOp,    EvtNetDNS,  4)
// Gap
#define EvtNetDNS_WrongServerPort           EventID (EventLevelError, EvtNetDNS,  6)
#define EvtNetDNS_FrameTooShort             EventID (EventLevelError, EvtNetDNS,  7)
#define EvtNetDNS_WrongServerAddress        EventID (EventLevelError, EvtNetDNS,  8)
#define EvtNetDNS_ShowFrameHeader           EventID (EventLevelDetail,EvtNetDNS,  9)
#define EvtNetDNS_WrongTransactionId        EventID (EventLevelError, EvtNetDNS, 10)
#define EvtNetDNS_DnsRequestReceived        EventID (EventLevelError, EvtNetDNS, 11)
#define EvtNetDNS_OpcodeNotQuery            EventID (EventLevelError, EvtNetDNS, 12)
#define EvtNetDNS_MessageTruncated          EventID (EventLevelOp,    EvtNetDNS, 35) // End
#define EvtNetDNS_NoSuchNameFound           EventID (EventLevelOp,    EvtNetDNS, 13)
#define EvtNetDNS_RcodeAndRecursion         EventID (EventLevelError, EvtNetDNS, 14)
#define EvtNetDNS_MoreAnswersReceived       EventID (EventLevelError, EvtNetDNS, 15)
#define EvtNetDNS_QnameNotTheSame           EventID (EventLevelError, EvtNetDNS, 16)
#define EvtNetDNS_QtypeNotTheSame           EventID (EventLevelError, EvtNetDNS, 17)
#define EvtNetDNS_QclassNotInet             EventID (EventLevelError, EvtNetDNS, 18)
#define EvtNetDNS_GotHostAddress            EventID (EventLevelOp,    EvtNetDNS, 19)
// Gap
#define EvtNetDNS_GotAuthorityAddress       EventID (EventLevelOp,    EvtNetDNS, 21)
// Gap
#define EvtNetDNS_RecordTypeNotSupported    EventID (EventLevelError, EvtNetDNS, 23)
#define EvtNetDNS_Resolve                   EventID (EventLevelOp,    EvtNetDNS, 24)
#define EvtNetDNS_ResolveInvalidParameter   EventID (EventLevelError, EvtNetDNS, 25)
#define EvtNetDNS_ResolveClientBusy         EventID (EventLevelError, EvtNetDNS, 26)
#define EvtNetDNS_ResolvedAddress           EventID (EventLevelOp,    EvtNetDNS, 27)
// Gap
#define EvtNetDNS_ResolveDnsServerUnknown   EventID (EventLevelError, EvtNetDNS, 29)
#define EvtNetDNS_SendRequest               EventID (EventLevelOp,    EvtNetDNS, 30)
// Gap
#define EvtNetDNS_ClearCache                EventID (EventLevelAPI,   EvtNetDNS, 32)
#define EvtNetDNS_ClearCacheClientBusy      EventID (EventLevelError, EvtNetDNS, 33)
#define EvtNetDNS_UninitClient              EventID (EventLevelOp,    EvtNetDNS, 34)
#endif

/**
  \brief  Event on DNS client initialize (Op)
  \param  num_entries   number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_InitClient(uint32_t num_entries) {
    EventRecord2 (EvtNetDNS_InitClient, num_entries, 0);
  }
#else
  #define EvrNetDNS_InitClient(num_entries)
#endif

/**
  \brief  Event on DNS client failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_GetSocketFailed(void) {
    EventRecord2 (EvtNetDNS_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetDNS_GetSocketFailed()
#endif

/**
  \brief  Event on DNS client change active DNS server (Error)
  \param  server        DNS server to use
                         - 0: primary
                         - 1: secondary
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ChangeDnsServer(int32_t server) {
    EventRecord2 (EvtNetDNS_ChangeDnsServer, (uint32_t)server, 0);
  }
#else
  #define EvrNetDNS_ChangeDnsServer(server)
#endif

/**
  \brief  Event on DNS client stop operation timeout expired (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ClientStopTimeoutExpired(void) {
    EventRecord2 (EvtNetDNS_ClientStopTimeoutExpired, 0, 0);
  }
#else
  #define EvrNetDNS_ClientStopTimeoutExpired()
#endif

/**
  \brief  Event on DNS client receive frame from the server (Op)
  \param  net_addr      pointer to \ref NET_ADDR address of the server
  \param  length        frame length in bytes
  \remark Port member is undefined and reused for the length parameter.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ReceiveFrame(const void *net_addr, uint32_t length) {
    const EVR_ADDR *addr = net_addr;
    evr_buf.u16[0] = (uint16_t)addr->type;
    evr_buf.u16[1] = length & 0xFFFF;
    memcpy (&evr_buf.u8[4], &addr->addr,  addr->type ? 16 : 4);
    EventRecordData (EvtNetDNS_ReceiveFrame, &evr_buf, addr->type ? 20 : 8);
  }
#else
  #define EvrNetDNS_ReceiveFrame(net_addr, length)
#endif

/**
  \brief  Event on DNS client wrong server port (Error)
  \param  udp_port      wrong UDP port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_WrongServerPort(uint16_t udp_port) {
    EventRecord2 (EvtNetDNS_WrongServerPort, udp_port, 0);
  }
#else
  #define EvrNetDNS_WrongServerPort(udp_port)
#endif

/**
  \brief  Event on DNS client receive frame is too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetDNS_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetDNS_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on DNS client wrong DNS server address received (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_WrongServerAddress(void) {
    EventRecord2 (EvtNetDNS_WrongServerAddress, 0, 0);
  }
#else
  #define EvrNetDNS_WrongServerAddress()
#endif

/**
  \brief  Event on DNS client display send/receive frame header (Detail)
  \param  dns_header    pointer to DNS frame header of 12 bytes
                         - TID     (2 bytes)
                         - Flags   (2 bytes)
                         - QDCOUNT (2 bytes)
                         - ANCOUNT (2 bytes)
                         - NSCOUNT (2 bytes)
                         - ARCOUNT (2 bytes)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ShowFrameHeader(const void *dns_header) {
    EventRecordData (EvtNetDNS_ShowFrameHeader, dns_header, 12);
  }
#else
  #define EvrNetDNS_ShowFrameHeader(dns_header)
#endif

/**
  \brief  Event on DNS client wrong transaction identifier (TID) received (Error)
  \param  tid           received transaction identifier
  \param  tid_valid     valid transaction identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_WrongTransactionId(uint16_t tid, uint16_t tid_valid) {
    EventRecord2 (EvtNetDNS_WrongTransactionId, tid, tid_valid);
  }
#else
  #define EvrNetDNS_WrongTransactionId(tid, tid_valid)
#endif

/**
  \brief  Event on DNS client received a DNS request (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_DnsRequestReceived(void) {
    EventRecord2 (EvtNetDNS_DnsRequestReceived, 0, 0);
  }
#else
  #define EvrNetDNS_DnsRequestReceived()
#endif

/**
  \brief  Event on DNS client OPCODE in a response is not QUERY (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_OpcodeNotQuery(void) {
    EventRecord2 (EvtNetDNS_OpcodeNotQuery, 0, 0);
  }
#else
  #define EvrNetDNS_OpcodeNotQuery()
#endif

/**
  \brief  Event on DNS client truncated response message received (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_MessageTruncated(void) {
    EventRecord2 (EvtNetDNS_MessageTruncated, 0, 0);
  }
#else
  #define EvrNetDNS_MessageTruncated()
#endif

/**
  \brief  Event on DNS client no such name found message received (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_NoSuchNameFound(void) {
    EventRecord2 (EvtNetDNS_NoSuchNameFound, 0, 0);
  }
#else
  #define EvrNetDNS_NoSuchNameFound()
#endif

/**
  \brief  Event on DNS client RCODE not zero and recursion available received (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_RcodeAndRecursion(void) {
    EventRecord2 (EvtNetDNS_RcodeAndRecursion, 0, 0);
  }
#else
  #define EvrNetDNS_RcodeAndRecursion()
#endif

/**
  \brief  Event on DNS client more than one answer received (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_MoreAnswersReceived(void) {
    EventRecord2 (EvtNetDNS_MoreAnswersReceived, 0, 0);
  }
#else
  #define EvrNetDNS_MoreAnswersReceived()
#endif

/**
  \brief  Event on DNS client QNAME in a response not the same as requested (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_QnameNotTheSame(void) {
    EventRecord2 (EvtNetDNS_QnameNotTheSame, 0, 0);
  }
#else
  #define EvrNetDNS_QnameNotTheSame()
#endif

/**
  \brief  Event on DNS client QTYPE in a response not the same as requested (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_QtypeNotTheSame(void) {
    EventRecord2 (EvtNetDNS_QtypeNotTheSame, 0, 0);
  }
#else
  #define EvrNetDNS_QtypeNotTheSame()
#endif

/**
  \brief  Event on DNS client QCLASS in a response is not CLASS_INET (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_QclassNotInet(void) {
    EventRecord2 (EvtNetDNS_QclassNotInet, 0, 0);
  }
#else
  #define EvrNetDNS_QclassNotInet()
#endif

/**
  \brief  Event on DNS client receive resolved IP address (Op)
  \param  net_addr      pointer to network address of the host
  \param  ttl           address expiration timeout
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_GotHostAddress(const void *net_addr, uint32_t ttl) {
    const EVR_ADDR *addr = net_addr;
    evr_buf.u32[0] = (ttl << 1) | (addr->type & 0x1);
    memcpy (&evr_buf.u8[4], &addr->addr, addr->type ? 16 : 4);
    EventRecordData (EvtNetDNS_GotHostAddress, &evr_buf, addr->type ? 20 : 8);
  }
#else
  #define EvrNetDNS_GotHostAddress(net_addr, ttl)
#endif

/**
  \brief  Event on DNS client receive IP address of the authority (Op)
  \param  net_addr      pointer to net address of the authority
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_GotAuthorityAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetDNS_GotAuthorityAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetDNS_GotAuthorityAddress(net_addr)
#endif

/**
  \brief  Event on DNS client can not handle resource record type (Error)
  \param  rr_type       recource record type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_RecordTypeNotSupported(uint16_t rr_type) {
    EventRecord2 (EvtNetDNS_RecordTypeNotSupported, rr_type, 0);
  }
#else
  #define EvrNetDNS_RecordTypeNotSupported(rr_type)
#endif

/**
  \brief  Event on DNS client resolve host requested (Op)
  \param  host_name     name of the host to resolve
  \param  length        length of the host_name string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_Resolve(const char *host_name, uint32_t length) {
    if (length > 40) length = 40;
    EventRecordData (EvtNetDNS_Resolve, host_name, length);
  }
#else
  #define EvrNetDNS_Resolve(host_name, length)
#endif

/**
  \brief  Event on DNS client invalid parameter for resolve request (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ResolveInvalidParameter(void) {
    EventRecord2 (EvtNetDNS_ResolveInvalidParameter, 0, 0);
  }
#else
  #define EvrNetDNS_ResolveInvalidParameter()
#endif

/**
  \brief  Event on DNS client resolve request failed, client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ResolveClientBusy(void) {
    EventRecord2 (EvtNetDNS_ResolveClientBusy, 0, 0);
  }
#else
  #define EvrNetDNS_ResolveClientBusy()
#endif

/**
  \brief  Event on DNS client resolved IP address internally (Op)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ResolvedAddress(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetDNS_ResolvedAddress, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetDNS_ResolvedAddress(net_addr)
#endif

/**
  \brief  Event on DNS client resolve failed, DNS server unknown (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ResolveDnsServerUnknown(void) {
    EventRecord2 (EvtNetDNS_ResolveDnsServerUnknown, 0, 0);
  }
#else
  #define EvrNetDNS_ResolveDnsServerUnknown()
#endif

/**
  \brief  Event on DNS client send resolve request to the server (Op)
  \param  net_addr      pointer to \ref NET_ADDR address structure
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_SendRequest(const void *net_addr) {
    const EVR_ADDR *addr = net_addr;
    EventRecordData (EvtNetDNS_SendRequest, addr, addr->type ? 20 : 8);
  }
#else
  #define EvrNetDNS_SendRequest(net_addr)
#endif

/**
  \brief  Event on DNS client clear cache requested (API)
  \param  deleted       number of entries deleted from cache
  \param  available     number of entries available in cache
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ClearCache(uint32_t deleted, uint32_t available) {
    EventRecord2 (EvtNetDNS_ClearCache, deleted, available);
  }
#else
  #define EvrNetDNS_ClearCache(deleted, available)
#endif

/**
  \brief  Event on DNS client clear cache request failed, client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_ClearCacheClientBusy(void) {
    EventRecord2 (EvtNetDNS_ClearCacheClientBusy, 0, 0);
  }
#else
  #define EvrNetDNS_ClearCacheClientBusy()
#endif

/**
  \brief  Event on DNS client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetDNS_UninitClient(void) {
    EventRecord2 (EvtNetDNS_UninitClient, 0, 0);
  }
#else
  #define EvrNetDNS_UninitClient()
#endif


// NetSNMP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetSNMP_InitAgent                EventID (EventLevelOp,    EvtNetSNMP,  0)
#define EvtNetSNMP_GetSocketFailed          EventID (EventLevelError, EvtNetSNMP,  1)
#define EvtNetSNMP_ViewCommunity            EventID (EventLevelOp,    EvtNetSNMP,  2)
#define EvtNetSNMP_SetCommunity             EventID (EventLevelAPI,   EvtNetSNMP,  3)
#define EvtNetSNMP_SetCommunityInvalidParam EventID (EventLevelError, EvtNetSNMP,  4)
#define EvtNetSNMP_SetMibTable              EventID (EventLevelAPI,   EvtNetSNMP,  5)
#define EvtNetSNMP_SetMibTableInvalidParam  EventID (EventLevelError, EvtNetSNMP,  6)
#define EvtNetSNMP_Trap                     EventID (EventLevelAPI,   EvtNetSNMP,  7)
#define EvtNetSNMP_TrapInvalidParameter     EventID (EventLevelError, EvtNetSNMP,  8)
#define EvtNetSNMP_TrapMibTableNotSet       EventID (EventLevelError, EvtNetSNMP,  9)
#define EvtNetSNMP_TrapMissingSysObjectId   EventID (EventLevelError, EvtNetSNMP, 10)
#define EvtNetSNMP_TrapGenericTrapInvalid   EventID (EventLevelError, EvtNetSNMP, 11)
#define EvtNetSNMP_TrapTooManyObjects       EventID (EventLevelError, EvtNetSNMP, 12)
#define EvtNetSNMP_TrapObjectNotExisting    EventID (EventLevelError, EvtNetSNMP, 13)
#define EvtNetSNMP_TrapMessageTooBig        EventID (EventLevelError, EvtNetSNMP, 14)
#define EvtNetSNMP_MibAddObject             EventID (EventLevelDetail,EvtNetSNMP, 15)
#define EvtNetSNMP_ViewObjectId             EventID (EventLevelDetail,EvtNetSNMP, 32)
#define EvtNetSNMP_ViewObjectVal            EventID (EventLevelDetail,EvtNetSNMP, 33)
#define EvtNetSNMP_ViewObjectAddr           EventID (EventLevelDetail,EvtNetSNMP, 34)
#define EvtNetSNMP_ViewObjectVar            EventID (EventLevelDetail,EvtNetSNMP, 35) // End
#define EvtNetSNMP_ReceiveFrame             EventID (EventLevelOp,    EvtNetSNMP, 16)
#define EvtNetSNMP_FrameTooShort            EventID (EventLevelError, EvtNetSNMP, 17)
#define EvtNetSNMP_FrameProtocolError       EventID (EventLevelError, EvtNetSNMP, 18)
#define EvtNetSNMP_VersionNotSupported      EventID (EventLevelOp,    EvtNetSNMP, 19)
#define EvtNetSNMP_WrongCommunityReceived   EventID (EventLevelOp,    EvtNetSNMP, 20)
#define EvtNetSNMP_InvalidRequestType       EventID (EventLevelError, EvtNetSNMP, 21)
#define EvtNetSNMP_ViewRequest              EventID (EventLevelOp,    EvtNetSNMP, 22)
#define EvtNetSNMP_TooManyObjectsReceived   EventID (EventLevelError, EvtNetSNMP, 23)
#define EvtNetSNMP_ObjectNotFound           EventID (EventLevelError, EvtNetSNMP, 24)
#define EvtNetSNMP_ObjectWrongType          EventID (EventLevelError, EvtNetSNMP, 25)
#define EvtNetSNMP_ObjectReadOnly           EventID (EventLevelError, EvtNetSNMP, 26)
#define EvtNetSNMP_ObjectWriteFailed        EventID (EventLevelError, EvtNetSNMP, 27)
#define EvtNetSNMP_SendErrorStatus          EventID (EventLevelOp,    EvtNetSNMP, 28)
#define EvtNetSNMP_ResponseMessageTooBig    EventID (EventLevelError, EvtNetSNMP, 29)
#define EvtNetSNMP_SendGetResponse          EventID (EventLevelOp,    EvtNetSNMP, 30)
#define EvtNetSNMP_UninitAgent              EventID (EventLevelOp,    EvtNetSNMP, 31)
#endif

/**
  \brief  Event on SNMP agent initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_InitAgent(void) {
    EventRecord2 (EvtNetSNMP_InitAgent, 0, 0);
  }
#else
  #define EvrNetSNMP_InitAgent()
#endif

/**
  \brief  Event on SNMP agent failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_GetSocketFailed(void) {
    EventRecord2 (EvtNetSNMP_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetSNMP_GetSocketFailed()
#endif

/**
  \brief  Event on SNMP agent display community (Op)
  \param  community     pointer to SNMP community string
  \param  length        length of the community string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewCommunity(const char *community, uint32_t length) {
    if (length > 24) length = 24;
    EventRecordData(EvtNetSNMP_ViewCommunity, community, length);
  }
#else
  #define EvrNetSNMP_ViewCommunity(community, length)
#endif

/**
  \brief  Event on \ref netSNMP_SetCommunity (API)
  \param  community     pointer to SNMP community string
  \param  length        length of the community string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SetCommunity(const char *community, uint32_t length) {
    if (length > 32) length = 32;
    EventRecordData(EvtNetSNMP_SetCommunity, community, length);
  }
#else
  #define EvrNetSNMP_SetCommunity(community, length)
#endif

/**
  \brief  Event on \ref netSNMP_SetCommunity invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SetCommunityInvalidParam(void) {
    EventRecord2 (EvtNetSNMP_SetCommunityInvalidParam, 0, 0);
  }
#else
  #define EvrNetSNMP_SetCommunityInvalidParam()
#endif

/**
  \brief  Event on \ref netSNMP_SetMIB_Table (API)
  \param  num_entries   number of entries in MIB table
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SetMibTable(uint32_t num_entries) {
    EventRecord2 (EvtNetSNMP_SetMibTable, num_entries, 0);
  }
#else
  #define EvrNetSNMP_SetMibTable(num_entries)
#endif

/**
  \brief  Event on \ref netSNMP_SetMIB_Table invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SetMibTableInvalidParam(void) {
    EventRecord2 (EvtNetSNMP_SetMibTableInvalidParam, 0, 0);
  }
#else
  #define EvrNetSNMP_SetMibTableInvalidParam()
#endif

/**
  \brief  Event on \ref netSNMP_Trap (API)
  \param  ip4_addr      trap server IPv4 address
  \param  generic       generic trap type
                         - 0: coldStart trap
                         - 1: warmStart trap
                         - 2: linkDown trap
                         - 3: linkUp trap
                         - 4: authenticationFailure trap
                         - 5: egpNeighborLoss trap
                         - 6: enterpriseSpecific trap
  \param  specific      specific trap type for generic enterpriseSpecific trap
                         - must be set to 0 for generic traps 0 .. 5
  \param  nobj          number of objects in a trap message
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_Trap(const uint8_t *ip4_addr, uint8_t generic, uint8_t specific, uint16_t nobj) {
    evr_buf.u16[0] = nobj;
    evr_buf.u8[2]  = generic;
    evr_buf.u8[3]  = specific;
    memcpy (&evr_buf.u8[4], ip4_addr, 4);
    EventRecordData (EvtNetSNMP_Trap, &evr_buf, 8);
  }
#else
  #define EvrNetSNMP_Trap(ip4_addr, generic, specific, nobj)
#endif

/**
  \brief  Event on \ref netSNMP_Trap invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapInvalidParameter(void) {
    EventRecord2 (EvtNetSNMP_TrapInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSNMP_TrapInvalidParameter()
#endif

/**
  \brief  Event on \ref netSNMP_Trap invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapMibTableNotSet(void) {
    EventRecord2 (EvtNetSNMP_TrapMibTableNotSet, 0, 0);
  }
#else
  #define EvrNetSNMP_TrapMibTableNotSet()
#endif

/**
  \brief  Event on \ref netSNMP_Trap missing 'sysObjectID' object in MIB table (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapMissingSysObjectId(void) {
    EventRecord2 (EvtNetSNMP_TrapMissingSysObjectId, 0, 0);
  }
#else
  #define EvrNetSNMP_TrapMissingSysObjectId()
#endif

/**
  \brief  Event on \ref netSNMP_Trap generic trap invalid (Error)
  \param  generic       invalid generic trap type
  \param  max_generic   maximum value for generic trap type
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapGenericTrapInvalid(uint8_t generic, uint8_t max_generic) {
    EventRecord2 (EvtNetSNMP_TrapGenericTrapInvalid, generic, max_generic);
  }
#else
  #define EvrNetSNMP_TrapGenericTrapInvalid(generic, max_generic)
#endif

/**
  \brief  Event on \ref netSNMP_Trap maximum number of objects exceeded (Error)
  \param  nobj          number of TLV objects
  \param  max_nobj      maximum number of TLV objects
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapTooManyObjects(int32_t nobj, int32_t max_nobj) {
    EventRecord2 (EvtNetSNMP_TrapTooManyObjects, (uint32_t)nobj, (uint32_t)max_nobj);
  }
#else
  #define EvrNetSNMP_TrapTooManyObjects(nobj, max_nobj)
#endif

/**
  \brief  Event on \ref netSNMP_Trap requested TLV object not existing in MIB table (Error)
  \param  obj           object identification number
  \param  max_obj       maximum value for object identification number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapObjectNotExisting(int32_t obj, int32_t max_obj) {
    EventRecord2 (EvtNetSNMP_TrapObjectNotExisting, (uint32_t)obj, (uint32_t)max_obj);
  }
#else
  #define EvrNetSNMP_TrapObjectNotExisting(obj, max_obj)
#endif

/**
  \brief  Event on \ref netSNMP_Trap generated message too big (Error)
  \param  size          requested message size
  \param  max_size      maximum message size accepted
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TrapMessageTooBig(uint16_t size, uint16_t max_size) {
    EventRecord2 (EvtNetSNMP_TrapMessageTooBig, size, max_size);
  }
#else
  #define EvrNetSNMP_TrapMessageTooBig(size, max_size)
#endif

/**
  \brief  Event on SNMP add MIB object to SNMP message (Detail)
  \param  obj           object identification number
  \param  obj_type      object data type
                         - 0x02: Integer Data Type (signed integer)
                         - 0x04: Octet String Data Type
                         - 0x05: Null Item Data Type
                         - 0x06: Object Identifier Data Type
                         - 0x30: Sequence Data Type
                         - 0x40: IP Address Data Type
                         - 0x41: Counter Data Type (pos.integer)
                         - 0x42: Gauge Data Type (pos.integer)
                         - 0x43: Time Ticks Data Type (pos.integer)
                         - 0x44: Opaque Data Type (ASN.1 encoded)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_MibAddObject(int32_t obj, uint8_t obj_type) {
    EventRecord2 (EvtNetSNMP_MibAddObject, (uint32_t)obj, obj_type);
  }
#else
  #define EvrNetSNMP_MibAddObject(obj, obj_type)
#endif

/**
  \brief  Event on SNMP display object ID (Detail)
  \param  oid           object identification string
  \param  length        length of the oid string
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewObjectId(const char *oid, uint32_t length) {
    if (length > 64) length = 64;
    EventRecordData (EvtNetSNMP_ViewObjectId, oid, length);
  }
#else
  #define EvrNetSNMP_ViewObjectId(oid, length)
#endif

/**
  \brief  Event on SNMP display object value (Detail)
  \param  type          object type
  \param  val           4-byte object value
  \note   Object types: INTEGER, COUNTER, GAUGE, TICKS
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewObjectVal(uint8_t type, uint32_t val) {
    EventRecord2 (EvtNetSNMP_ViewObjectVal, type, val);
  }
#else
  #define EvrNetSNMP_ViewObjectVal(type, val)
#endif

/**
  \brief  Event on SNMP display object IP address (Detail)
  \param  ip4_addr      pointer to IPv4 address object value
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewObjectAddr(const uint8_t *ip4_addr) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetSNMP_ViewObjectAddr, evr_buf.u32[0], 0);
  }
#else
  #define EvrNetSNMP_ViewObjectAddr(ip4_addr)
#endif

/**
  \brief  Event on SNMP display object value (Detail)
  \param  type          object type
  \param  value         pointer to object value
  \param  length        length of the object value
  \note   Object types: OID, OCTET_STRING, BYTE_STRING
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewObjectVar(uint8_t type, const void *value, uint32_t length) {
    if (length > 32) length = 32;
    evr_buf.u16[0] = type;
    evr_buf.u16[1] = length & 0xFFFF;
    memcpy (&evr_buf.u16[2], value, length);
    EventRecordData (EvtNetSNMP_ViewObjectVar, &evr_buf, length+4);
  }
#else
  #define EvrNetSNMP_ViewObjectVar(type, value, length)
#endif

/**
  \brief  Event on SNMP receive frame (Op)
  \param  ip4_addr      pointer to IPv4 address of the sender
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ReceiveFrame(const uint8_t *ip4_addr, uint32_t length) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetSNMP_ReceiveFrame, length, evr_buf.u32[0]);
  }
#else
  #define EvrNetSNMP_ReceiveFrame(ip4_addr, length)
#endif

/**
  \brief  Event on SNMP receive frame is too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetSNMP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetSNMP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on SNMP received frame protocol error detected (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_FrameProtocolError(void) {
    EventRecord2 (EvtNetSNMP_FrameProtocolError, 0, 0);
  }
#else
  #define EvrNetSNMP_FrameProtocolError()
#endif

/**
  \brief  Event on SNMP received protocol version not supported (Op)
  \param  version       received SNMP protocol version
                         - 1: SNMPv1
                         - 2: SNMPv2
                         - 3: SNMPv3
  \remark Only SNMPv1 is currently supported.
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_VersionNotSupported(int32_t version) {
    if (version == 0) version = 1;
    EventRecord2 (EvtNetSNMP_VersionNotSupported, (uint32_t)version, 0);
  }
#else
  #define EvrNetSNMP_VersionNotSupported(version)
#endif

/**
  \brief  Event on SNMP received community wrong (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_WrongCommunityReceived(void) {
    EventRecord2 (EvtNetSNMP_WrongCommunityReceived, 0, 0);
  }
#else
  #define EvrNetSNMP_WrongCommunityReceived()
#endif

/**
  \brief  Event on SNMP received invalid PDU request type (Error)
  \param  req_type      PDU request type
                         - 0xA0: GetRequest PDU
                         - 0xA1: GetNextRequest PDU
                         - 0xA2: Response PDU
                         - 0xA3: SetRequest PDU
                         - 0xA4: Trap PDU
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_InvalidRequestType(uint8_t req_type) {
    EventRecord2 (EvtNetSNMP_InvalidRequestType, req_type, 0);
  }
#else
  #define EvrNetSNMP_InvalidRequestType(req_type)
#endif

/**
  \brief  Event on SNMP display received PDU request type and id (Error)
  \param  req_type      PDU request type
                         - 0xA0: GetRequest PDU
                         - 0xA1: GetNextRequest PDU
                         - 0xA2: Response PDU
                         - 0xA3: SetRequest PDU
                         - 0xA4: Trap PDU
  \param  req_id        PDU request identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ViewRequest(uint8_t req_type, int32_t req_id) {
    EventRecord2 (EvtNetSNMP_ViewRequest, req_type, (uint32_t)req_id);
  }
#else
  #define EvrNetSNMP_ViewRequest(req_type, req_id)
#endif

/**
  \brief  Event on SNMP received too many TLV objects (Error)
  \param  nobj          number of TLV objects
  \param  max_nobj      maximum number of TLV objects
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_TooManyObjectsReceived(int32_t nobj, int32_t max_nobj) {
    EventRecord2 (EvtNetSNMP_TooManyObjectsReceived, (uint32_t)nobj, (uint32_t)max_nobj);
  }
#else
  #define EvrNetSNMP_TooManyObjectsReceived(nobj, max_nobj)
#endif

/**
  \brief  Event on SNMP receive message object not found in the MIB table (Error)
  \param  obj           object identification number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ObjectNotFound(int32_t obj) {
    EventRecord2 (EvtNetSNMP_ObjectNotFound, (uint32_t)obj, 0);
  }
#else
  #define EvrNetSNMP_ObjectNotFound(obj)
#endif

/**
  \brief  Event on SNMP receive message object type not the same as in the MIB table (Error)
  \param  obj           object identification number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ObjectWrongType(int32_t obj) {
    EventRecord2 (EvtNetSNMP_ObjectWrongType, (uint32_t)obj, 0);
  }
#else
  #define EvrNetSNMP_ObjectWrongType(obj)
#endif

/**
  \brief  Event on SNMP write read-only object not allowed (Error)
  \param  obj           object identification number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ObjectReadOnly(int32_t obj) {
    EventRecord2 (EvtNetSNMP_ObjectReadOnly, (uint32_t)obj, 0);
  }
#else
  #define EvrNetSNMP_ObjectReadOnly(obj)
#endif

/**
  \brief  Event on SNMP object write attempt failed for unknown reason (Error)
  \param  obj           object identification number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ObjectWriteFailed(int32_t obj) {
    EventRecord2 (EvtNetSNMP_ObjectWriteFailed, (uint32_t)obj, 0);
  }
#else
  #define EvrNetSNMP_ObjectWriteFailed(obj)
#endif

/**
  \brief  Event on SNMP send error status response to a SNMP request (Op)
  \param  stat          error status
  \param  obj_index     error object index
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SendErrorStatus(uint8_t stat, uint8_t obj_index) {
    EventRecord2 (EvtNetSNMP_SendErrorStatus, stat, obj_index);
  }
#else
  #define EvrNetSNMP_SendErrorStatus(stat, obj_index)
#endif

/**
  \brief  Event on SNMP response message would be too big (Error)
  \param  size          response message size
  \param  max_size      maximum message size allowed
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_ResponseMessageTooBig(uint16_t size, uint16_t max_size) {
    EventRecord2 (EvtNetSNMP_ResponseMessageTooBig, size, max_size);
  }
#else
  #define EvrNetSNMP_ResponseMessageTooBig(size, max_size)
#endif

/**
  \brief  Event on SNMP send get-response to a SNMP request (Op)
  \param  req_id        PDU request identifier
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_SendGetResponse(int32_t req_id) {
    EventRecord2 (EvtNetSNMP_SendGetResponse, (uint32_t)req_id, 0);
  }
#else
  #define EvrNetSNMP_SendGetResponse(req_id)
#endif

/**
  \brief  Event on SNMP agent de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNMP_UninitAgent(void) {
    EventRecord2 (EvtNetSNMP_UninitAgent, 0, 0);
  }
#else
  #define EvrNetSNMP_UninitAgent()
#endif


// NetSNTP event identifiers ---------------------------------------------------
#ifdef Network_Debug_EVR
#define EvtNetSNTP_InitClient               EventID (EventLevelOp,    EvtNetSNTP,  0)
#define EvtNetSNTP_GetSocketFailed          EventID (EventLevelError, EvtNetSNTP,  1)
#define EvtNetSNTP_GetTime                  EventID (EventLevelAPI,   EvtNetSNTP,  2)
#define EvtNetSNTP_GetTimeInvalidParameter  EventID (EventLevelError, EvtNetSNTP,  3)
#define EvtNetSNTP_GetTimeWrongState        EventID (EventLevelError, EvtNetSNTP,  4)
#define EvtNetSNTP_GetTimeOpen              EventID (EventLevelOp,    EvtNetSNTP,  5)
#define EvtNetSNTP_GetTimeClose             EventID (EventLevelOp,    EvtNetSNTP,  6)
#define EvtNetSNTP_GetTimeServerUnknown     EventID (EventLevelError, EvtNetSNTP,  7)
#define EvtNetSNTP_GetTimexInvalidParameter EventID (EventLevelError, EvtNetSNTP,  8)
#define EvtNetSNTP_GetTimexClientBusy       EventID (EventLevelError, EvtNetSNTP,  9)
#define EvtNetSNTP_GetTimexServerNotValid   EventID (EventLevelError, EvtNetSNTP, 10)
#define EvtNetSNTP_GetTimexDnsError         EventID (EventLevelError, EvtNetSNTP, 11)
#define EvtNetSNTP_SetMode                  EventID (EventLevelAPI,   EvtNetSNTP, 12)
#define EvtNetSNTP_SetModeInvalidParameter  EventID (EventLevelError, EvtNetSNTP, 13)
#define EvtNetSNTP_SetModeWrongState        EventID (EventLevelError, EvtNetSNTP, 14)
#define EvtNetSNTP_SendMessage              EventID (EventLevelOp,    EvtNetSNTP, 15)
#define EvtNetSNTP_ServerNotResponding      EventID (EventLevelOp,    EvtNetSNTP, 16)
#define EvtNetSNTP_ReceiveFrame             EventID (EventLevelOp,    EvtNetSNTP, 17)
#define EvtNetSNTP_WrongServerPort          EventID (EventLevelError, EvtNetSNTP, 18)
#define EvtNetSNTP_FrameTooShort            EventID (EventLevelError, EvtNetSNTP, 19)
#define EvtNetSNTP_WrongServerAddress       EventID (EventLevelError, EvtNetSNTP, 20)
#define EvtNetSNTP_ModeNotServer            EventID (EventLevelError, EvtNetSNTP, 21)
#define EvtNetSNTP_ModeNotBroadcast         EventID (EventLevelError, EvtNetSNTP, 22)
#define EvtNetSNTP_AnswerInWrongState       EventID (EventLevelError, EvtNetSNTP, 23)
#define EvtNetSNTP_ShowTimeStamp            EventID (EventLevelOp,    EvtNetSNTP, 24)
#define EvtNetSNTP_TimeStampInvalid         EventID (EventLevelOp,    EvtNetSNTP, 25)
#define EvtNetSNTP_UninitClient             EventID (EventLevelOp,    EvtNetSNTP, 26)
#endif

/**
  \brief  Event on SNTP client initialize (Op)
  \param  mode          operation mode of the client
                         - 0: unicast
                         - 1: broadcast
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_InitClient(uint8_t mode) {
    EventRecord2 (EvtNetSNTP_InitClient, mode, 0);
  }
#else
  #define EvrNetSNTP_InitClient(mode)
#endif

/**
  \brief  Event on SNTP failed to allocate UDP socket (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetSocketFailed(void) {
    EventRecord2 (EvtNetSNTP_GetSocketFailed, 0, 0);
  }
#else
  #define EvrNetSNTP_GetSocketFailed()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime (API)
  \param  mode          operation mode of the client
                         - 0: unicast
                         - 1: broadcast
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTime(uint8_t mode) {
    EventRecord2 (EvtNetSNTP_GetTime, mode, 0);
  }
#else
  #define EvrNetSNTP_GetTime(mode)
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimeInvalidParameter(void) {
    EventRecord2 (EvtNetSNTP_GetTimeInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimeInvalidParameter()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime wrong client state (Error)
  \param  state         client state
                         - 0: idle
                         - 1: busy in unicast send
                         - 2: open in broadcast receive
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimeWrongState(uint8_t state) {
    EventRecord2 (EvtNetSNTP_GetTimeWrongState, state, 0);
  }
#else
  #define EvrNetSNTP_GetTimeWrongState(state)
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime open broadcast receive (Op)
  \param  ip4_addr      pointer to IPv4 address of NTP server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimeOpen(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSNTP_GetTimeOpen, ip4_addr, 4);
  }
#else
  #define EvrNetSNTP_GetTimeOpen(ip4_addr)
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime close broadcast receive (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimeClose(void) {
    EventRecord2 (EvtNetSNTP_GetTimeClose, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimeClose()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTime unknown server for unicast mode (Error)
  \param  ip4_addr      pointer to IPv4 address of NTP server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimeServerUnknown(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSNTP_GetTimeServerUnknown, ip4_addr, 4);
  }
#else
  #define EvrNetSNTP_GetTimeServerUnknown(ip4_addr)
#endif

/**
  \brief  Event on \ref netSNTPc_GetTimeX invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimexInvalidParameter(void) {
    EventRecord2 (EvtNetSNTP_GetTimexInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimexInvalidParameter()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTimeX client busy (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimexClientBusy(void) {
    EventRecord2 (EvtNetSNTP_GetTimexClientBusy, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimexClientBusy()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTimeX server name not valid (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimexServerNotValid(void) {
    EventRecord2 (EvtNetSNTP_GetTimexServerNotValid, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimexServerNotValid()
#endif

/**
  \brief  Event on \ref netSNTPc_GetTimeX host name resolver error (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_GetTimexDnsError(void) {
    EventRecord2 (EvtNetSNTP_GetTimexDnsError, 0, 0);
  }
#else
  #define EvrNetSNTP_GetTimexDnsError()
#endif

/**
  \brief  Event on \ref netSNTPc_SetMode (API)
  \param  mode          operation mode of the client
                         - 0: unicast
                         - 1: broadcast
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_SetMode(uint8_t mode) {
    EventRecord2 (EvtNetSNTP_SetMode, mode, 0);
  }
#else
  #define EvrNetSNTP_SetMode(mode)
#endif

/**
  \brief  Event on \ref netSNTPc_SetMode invalid parameter (Error)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_SetModeInvalidParameter(void) {
    EventRecord2 (EvtNetSNTP_SetModeInvalidParameter, 0, 0);
  }
#else
  #define EvrNetSNTP_SetModeInvalidParameter()
#endif

/**
  \brief  Event on \ref netSNTPc_SetMode wrong client state (Error)
  \param  state         client state
                         - 0: idle
                         - 1: busy in unicast send
                         - 2: open in broadcast receive
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_SetModeWrongState(uint8_t state) {
    EventRecord2 (EvtNetSNTP_SetModeWrongState, state, 0);
  }
#else
  #define EvrNetSNTP_SetModeWrongState(state)
#endif

/**
  \brief  Event on SNTP send message to NTP server (Op)
  \param  ip4_addr      pointer to IPv4 address of NTP server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_SendMessage(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSNTP_SendMessage, ip4_addr, 4);
  }
#else
  #define EvrNetSNTP_SendMessage(ip4_addr)
#endif

/**
  \brief  Event on SNTP send message timeout (Op)
  \param  ip4_addr      pointer to IPv4 address of NTP server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_ServerNotResponding(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSNTP_ServerNotResponding, ip4_addr, 4);
  }
#else
  #define EvrNetSNTP_ServerNotResponding(ip4_addr)
#endif

/**
  \brief  Event on SNTP receive frame (Op)
  \param  ip4_addr      pointer to IPv4 address of the server
  \param  length        frame length in bytes
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_ReceiveFrame(const uint8_t *ip4_addr, uint32_t length) {
    memcpy (&evr_buf.u32[0], ip4_addr, 4);
    EventRecord2 (EvtNetSNTP_ReceiveFrame, evr_buf.u32[0], length);
  }
#else
  #define EvrNetSNTP_ReceiveFrame(ip4_addr, length)
#endif

/**
  \brief  Event on SNTP wrong server port (Error)
  \param  udp_port      wrong UDP port number
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_WrongServerPort(uint16_t udp_port) {
    EventRecord2 (EvtNetSNTP_WrongServerPort, udp_port, 0);
  }
#else
  #define EvrNetSNTP_WrongServerPort(udp_port)
#endif

/**
  \brief  Event on SNTP receive frame is too short (Error)
  \param  length        frame length in bytes
  \param  min_length    minimum length of the frame
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_FrameTooShort(uint32_t length, uint32_t min_length) {
    EventRecord2 (EvtNetSNTP_FrameTooShort, length, min_length);
  }
#else
  #define EvrNetSNTP_FrameTooShort(length, min_length)
#endif

/**
  \brief  Event on SNTP wrong NTP server address (Error)
  \param  ip4_addr      pointer to IPv4 address of NTP server
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_WrongServerAddress(const uint8_t *ip4_addr) {
    EventRecordData (EvtNetSNTP_WrongServerAddress, ip4_addr, 4);
  }
#else
  #define EvrNetSNTP_WrongServerAddress(ip4_addr)
#endif

/**
  \brief  Event on SNTP message mode not server (Error)
  \param  mode          message mode
                         - 0: reserved
                         - 1: symetric active
                         - 2: symetric passive
                         - 3: client
                         - 4: server
                         - 5: broadcast
                         - 6: reserved
                         - 7: reserved
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_ModeNotServer(uint8_t mode) {
    EventRecord2 (EvtNetSNTP_ModeNotServer, mode, 0);
  }
#else
  #define EvrNetSNTP_ModeNotServer(mode)
#endif

/**
  \brief  Event on SNTP message mode not broadcast (Error)
  \param  mode          message mode
                         - 0: reserved
                         - 1: symetric active
                         - 2: symetric passive
                         - 3: client
                         - 4: server
                         - 5: broadcast
                         - 6: reserved
                         - 7: reserved
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_ModeNotBroadcast(uint8_t mode) {
    EventRecord2 (EvtNetSNTP_ModeNotBroadcast, mode, 0);
  }
#else
  #define EvrNetSNTP_ModeNotBroadcast(mode)
#endif

/**
  \brief  Event on SNTP answer received in wrong state (Error)
  \param  state         client state
                         - 0: idle
                         - 1: busy in unicast send
                         - 2: open in broadcast receive
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_AnswerInWrongState(uint8_t state) {
    EventRecord2 (EvtNetSNTP_AnswerInWrongState, state, 0);
  }
#else
  #define EvrNetSNTP_AnswerInWrongState(state)
#endif

/**
  \brief  Event on SNTP display time stamp in seconds (Op)
  \param  ref_time      reference time (since Jan 1, 1900)
  \param  utc_time      utc time       (since Jan 1, 1970)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_ShowTimeStamp(uint32_t ref_time, uint32_t utc_time) {
    EventRecord2 (EvtNetSNTP_ShowTimeStamp, ref_time, utc_time);
  }
#else
  #define EvrNetSNTP_ShowTimeStamp(ref_time, utc_time)
#endif

/**
  \brief  Event on SNTP invalid time stamp (Op)
  \param  ref_time      reference time (since Jan 1, 1900)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_TimeStampInvalid(uint32_t ref_time) {
    EventRecord2 (EvtNetSNTP_TimeStampInvalid, ref_time, 0);
  }
#else
  #define EvrNetSNTP_TimeStampInvalid(ref_time)
#endif

/**
  \brief  Event on SNTP client de-initialize (Op)
 */
#ifdef Network_Debug_EVR
  __STATIC_INLINE void EvrNetSNTP_UninitClient(void) {
    EventRecord2 (EvtNetSNTP_UninitClient, 0, 0);
  }
#else
  #define EvrNetSNTP_UninitClient()
#endif
