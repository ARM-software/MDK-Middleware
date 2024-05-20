/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_snmp_agent.h
 * Purpose: Simple Network Management Agent Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_SNMP_AGENT_H
#define __NET_SNMP_AGENT_H

#include "rl_net_lib.h"

#define SNMP_GP_PORT        161         // SNMP General Purpose standard port
#define SNMP_TRAP_PORT      162         // SNMP Trap standard port
#define SNMP_MAX_NUM_OBJ    20          // Max.num. of objects in a message
#define SNMP_MAX_MSG_SIZE   1452        // Max. SNMP response message size

/* SNMP Versions */
#define SNMP_VERSION_1      0           // Version code for SNMP Version 1 !!!
#define SNMP_VERSION_2      2           // Version code for SNMP Version 2
#define SNMP_VERSION_3      3           // Version code for SNMP Version 3

/* SNMP Flags */
#define SNMP_FLAG_TCORR     0x01        // Flag Tick Correction for 1/100 sec

/* SNMP Protocol Data Units */
#define SNMP_GET_REQUEST    0xA0        // GetRequest PDU
#define SNMP_GET_NEXT       0xA1        // GetNextRequest PDU
#define SNMP_GET_RESPONSE   0xA2        // Response PDU
#define SNMP_SET_REQUEST    0xA3        // SetRequest PDU
#define SNMP_TRAP           0xA4        // Trap PDU

/* SNMP Data Types */
#define SNMP_TYPE_INTEGER   0x02        // Integer Data Type (signed integer)
#define SNMP_TYPE_STRING    0x04        // Octet String Data Type
#define SNMP_TYPE_NULL      0x05        // Null Item Data Type
#define SNMP_TYPE_OID       0x06        // Object Identifier Data Type
#define SNMP_TYPE_BSTRING   0x07        // Byte String Data Type (not ASN.1 tag)
#define SNMP_TYPE_SEQUENCE  0x30        // Sequence Data Type
#define SNMP_TYPE_IP        0x40        // IP Address Data Type
#define SNMP_TYPE_COUNTER   0x41        // Counter Data Type (pos.integer)
#define SNMP_TYPE_GAUGE     0x42        // Gauge Data Type (pos.integer)
#define SNMP_TYPE_TICKS     0x43        // Time Ticks Data Type (pos.integer)
#define SNMP_TYPE_OPAQUE    0x44        // Opaque Data Type (ASN.1 encoded)
#define SNMP_MIB_TYPE_MASK  0x7F        // Data Type bitmask (bit 7 is ATTR_RO)

/* SNMP Error Codes */
#define SNMP_ERR_NONE       0           // No Error Occured
#define SNMP_ERR_TOO_BIG    1           // Response message too large to transport
#define SNMP_ERR_NO_NAME    2           // Name of requested object not found
#define SNMP_ERR_BAD_VAL    3           // Data type in req. different from local
#define SNMP_ERR_RD_ONLY    4           // Can not modify, variable is read-only
#define SNMP_ERR_GENERAL    5           // General Unspecified Error

/* SNMP Control info */
typedef struct net_snmp_ctrl {
  NET_SNMP_MIB_INFO *Mib;               // MIB table registered
  uint16_t MibLen;                      // MIB length in number of entries
  uint8_t  Socket;                      // UDP Socket
  uint8_t  ReqType;                     // Request Type
  int32_t  ReqId;                       // Request Identification value
  uint8_t  ComLen;                      // Length of community string
  char     ComStr[19];                  // Community string value
} NET_SNMP_CTRL;

/* SNMP Type-Length-Value info */
typedef struct net_snmp_tlv {
  uint8_t  Type;                        // Object Data Type
  uint8_t  Check;                       // Check Object Data Type (0= no check)
  uint8_t  MibType;                     // MIB Object Data Type (BSTRING)
  uint16_t Len;                         // Length of TLV value
  uint16_t Start;                       // Start index of TLV object
  uint16_t vStart;                      // Start index of TLV value
  uint16_t Next;                        // Start index of Next TLV object
  uint16_t End;                         // End index of TLV data
  uint16_t vSize;                       // Size of MIB variables for response
} NET_SNMP_TLV;

/* SNMP Error Status info */
typedef struct net_snmp_err {
  uint8_t  Stat;                        // Error Status
  uint8_t  Idx;                         // Error Object Index
  uint16_t ReqPos;                      // Position of Request
  uint16_t StatPos;                     // Position of Error Status
  uint16_t IdxPos;                      // Position of Error Object Index
} NET_SNMP_ERR;

/* Variables */
extern uint32_t netSNMP_sysUpTime;
extern NET_SNMP_CFG  net_snmp_config;
#define snmpc      (&net_snmp_config)

/* Functions */
extern void net_snmp_agent_init (void);
extern void net_snmp_agent_uninit (void);
extern void net_snmp_agent_run (void);

#endif /* __NET_SNMP_AGENT_H */
