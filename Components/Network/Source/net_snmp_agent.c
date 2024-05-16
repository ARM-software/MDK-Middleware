/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_snmp_agent.c
 * Purpose: Simple Network Management Agent
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_udp.h"
#include "net_ip4.h"
#include "net_snmp_agent.h"
#include "net_dbg.h"

/* Global variables */
uint32_t netSNMP_sysUpTime;

/* Local variables */
static NET_SNMP_CTRL  snmp_control;
#define snmp        (&snmp_control)

/* Local Functions */
static uint32_t snmp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static bool parse_tlv (const uint8_t *buf, NET_SNMP_TLV *tlv);
static uint32_t get_value (const uint8_t *buf, NET_SNMP_TLV *tlv);
static int32_t set_value (uint8_t *buf, NET_SNMP_TLV *tlv, uint32_t val);
static int32_t ilen (int32_t val);
static int32_t ulen (uint32_t val);
static void mib_add_object (uint8_t *buf, NET_SNMP_TLV *tlv, int32_t id);
static void bind_tlv (uint8_t *buf, NET_SNMP_TLV *tlv);
static void bind_val (uint8_t *buf, NET_SNMP_TLV *tlv, uint32_t val);
static int32_t  mib_find_object (const uint8_t *buf, NET_SNMP_TLV *tlv);
static bool mib_write (const uint8_t *buf, NET_SNMP_TLV *tlv, int32_t id);
#ifdef __DEBUG_ENABLED
 static int32_t oid_ntoa  (char *buf, const uint8_t *oid);
 static int32_t get_int   (const uint8_t *oid);
 static uint32_t get_uint (const uint8_t *oid);
#endif
#ifdef Network_Debug_STDIO
 static void debug_info (const uint8_t *oid);
 static void debug_inf2 (uint8_t mib_type, const uint8_t *oid);
 static void obj_ascii (char *buf, uint8_t mib_type, const uint8_t *oid);
#endif
#ifdef Network_Debug_EVR
 static void record_oid (const uint8_t *oid);
 static void record_val (uint8_t mib_type, const uint8_t *oid);
#endif

/**
  \brief       Initialize SNMP agent.
*/
void net_snmp_agent_init (void) {
  int32_t sock;

  DEBUGF (SNMP,"Init Agent\n");
  EvrNetSNMP_InitAgent ();
  snmp->Mib    = NULL;
  snmp->MibLen = 0;
  snmp->ComLen = net_strcpy (snmp->ComStr, snmpc->Community) & 0xFF;
  sock = net_udp_get_socket (snmp_listener);
  if (sock < 0) sock = 0;
  snmp->Socket = sock & 0xFF;
  if (snmp->Socket == 0) {
    ERRORF (SNMP,"Get socket failed\n");
    EvrNetSNMP_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
  /* Open UDP port for communication */
  net_udp_open (snmp->Socket, snmpc->PortNum);
  DEBUGF (SNMP," Community : %s\n",snmp->ComStr);
  EvrNetSNMP_ViewCommunity (snmp->ComStr, snmp->ComLen);
  netSNMP_sysUpTime = 0;
}

/**
  \brief       De-initialize SNMP agent.
*/
void net_snmp_agent_uninit (void) {
  DEBUGF (SNMP,"Uninit Agent\n");
  EvrNetSNMP_UninitAgent ();

  netSNMP_sysUpTime = 0;
  memset (snmp, 0, sizeof (*snmp));
}

/**
  \brief       Change a community name to a new community.
  \param[in]   community  new community, null-terminated string.
  \return      status code as defined with netStatus.
*/
netStatus netSNMP_SetCommunity (const char *community) {

  START_LOCK (netStatus);

  if (community == NULL) {
    ERRORF (SNMP,"SetCommunity, Invalid parameter\n");
    EvrNetSNMP_SetCommunityInvalidParam ();
    RETURN (netInvalidParameter);
  }
  DEBUGF (SNMP,"SetCommunity to '%s'\n",community);
  EvrNetSNMP_SetCommunity (community, strlen(community));
  if (strlen (community) > 18) {
    ERRORF (SNMP,"SetCommunity, Name too long\n");
    EvrNetSNMP_SetCommunityInvalidParam ();
    RETURN (netInvalidParameter);
  }
  snmp->ComLen = net_strcpy (snmp->ComStr, community) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Register MIB table to SNMP Agent.
  \param[in]   info  pointer to MIB table.
  \param[in]   size  size of MIB table in bytes.
  \return      status code as defined with netStatus.
*/
netStatus netSNMP_SetMIB_Table (const NET_SNMP_MIB_INFO *info, uint32_t size) {

  START_LOCK (netStatus);

  if (info != NULL && (size == 0 || size > (65535 * sizeof(NET_SNMP_MIB_INFO)) ||
                                    (size % sizeof(NET_SNMP_MIB_INFO)) != 0)) {
    ERRORF (SNMP,"SetMIB, Invalid parameter\n");
    EvrNetSNMP_SetMibTableInvalidParam ();
    RETURN (netInvalidParameter);
  }
  snmp->Mib    = info;
  snmp->MibLen = (size / sizeof(NET_SNMP_MIB_INFO)) & 0xFFFF;
  DEBUGF (SNMP,"SetMIB for %d entries\n",snmp->MibLen);
  EvrNetSNMP_SetMibTable (snmp->MibLen);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Send a trap message to the Trap manager.
  \param[in]   addr      structure containing IP address of the Trap server or
                           NULL to use IP address from system configuration.
  \param[in]   generic   generic trap type:
                         - 0  = ColdStart trap
                         - 1  = WarmStart trap
                         - 2  = LinkDown trap
                         - 3  = LinkUp trap
                         - 4  = AuthenticationFailure trap
                         - 5  = EgpNeighborLoss trap
                         - 6  = EnterpriseSpecific trap
  \param[in]   specific  specific trap type for generic enterpriseSpecific trap:
                         - must be set to 0 for generic traps 0 ... 5
  \param[in]   obj_list  object list included in trap message.
  \return      status code as defined with netStatus.
*/
netStatus netSNMP_Trap (const NET_ADDR *addr, uint8_t generic, uint8_t specific,
                                              const uint16_t *obj_list) {
  NET_SNMP_MIB_INFO *mib;
  NET_SNMP_TLV tlv;
  NET_IF_CFG *net_if;
  uint8_t *sendbuf;
  NET_ADDR4 trap_srv;
  int32_t i,sysobj,nobj;

  START_LOCK (netStatus);

  /* Check Trap server address */
  if (addr != NULL) {
    /* Currently only IPv4 is supported  */
    if (addr->addr_type != NET_ADDR_IP4) {
      goto invalid_param;
    }
    /* Use the specified address */
    net_addr_copy ((__ADDR *)&trap_srv, (const __ADDR *)addr);
  }
  else {
    /* Use address from the system configuration */
    net_addr4_aton (snmpc->TrapIp, trap_srv.addr);
    trap_srv.addr_type = NET_ADDR_IP4;
  }
  if (net_addr_is_unspec ((const __ADDR *)&trap_srv)) {
    /* Server IP address not specified */
    goto invalid_param;
  }
  trap_srv.port = snmpc->TrapPort;

  net_if = net_addr_find_route (NULL, (const __ADDR *)&trap_srv);
  if (net_if == NULL) {
invalid_param:
    ERRORF (SNMP,"Trap, Invalid parameter\n");
    EvrNetSNMP_TrapInvalidParameter ();
    RETURN (netInvalidParameter);
  }

  /* Check MIB table */
  if (snmp->Mib == NULL) {
    /* MIB table not registered */
    ERRORF (SNMP,"Trap, MIB table not set\n");
    EvrNetSNMP_TrapMibTableNotSet ();
    RETURN (netWrongState);
  }

  DEBUGF (SNMP,"Sending Trap message\n");
  /* Search for 'sysObjectID' in MIB */
  mib = &snmp->Mib[0];
  for (sysobj = 0; sysobj < snmp->MibLen; mib ++, sysobj++) {
    if (memcmp("\x08\x2b\x06\x01\x02\x01\x01\x02\x00", &mib->oid_len, 9) == 0) {
      break;
    }
  }
  if (sysobj == snmp->MibLen) {
    ERRORF (SNMP,"Trap, 'sysObjectID' missing in MIB\n");
    EvrNetSNMP_TrapMissingSysObjectId ();
    RETURN (netError);
  }
  /* Check if generic trap type is valid */
  if (generic > 6) {
    ERRORF (SNMP,"Trap, Generic_Trap invalid\n");
    EvrNetSNMP_TrapGenericTrapInvalid (generic, 6);
    RETURN (netInvalidParameter);
  }
  if (generic < 6) {
    /* Trap type is not enterpriseSpecific */
    specific = 0;
  }
  /* Check the object list for valid entries */
  nobj = 0;
  if (obj_list != NULL) {
    /* Number of object in Trap PDU */
    nobj = *obj_list++;
  }
  if (nobj > SNMP_MAX_NUM_OBJ) {
    ERRORF (SNMP,"Trap, Too many objects (max.20)\n");
    EvrNetSNMP_TrapTooManyObjects (nobj, SNMP_MAX_NUM_OBJ);
    RETURN (netInvalidParameter);
  }
  for (i = 0; i < nobj; i++) {
    if (obj_list[i] >= snmp->MibLen) {
      ERRORF (SNMP,"Trap, Object %d not existing in MIB\n",obj_list[i]);
      EvrNetSNMP_TrapObjectNotExisting (obj_list[i], snmp->MibLen-1);
      RETURN (netInvalidParameter);
    }
  }

  DEBUGF (SNMP," Community [%s]\n",snmp->ComStr);
  DEBUGF (SNMP," Manager   [%s]\n",net_addr_ntoa((const __ADDR *)&trap_srv));
  DEBUGF (SNMP," Type Gen=%d, Spec=%d\n",generic,specific);
  EvrNetSNMP_Trap (trap_srv.addr, generic, specific, nobj & 0xFFFF);
  EvrNetSNMP_ViewCommunity (snmp->ComStr, snmp->ComLen);

  /* Calculate the message packet size */
  tlv.vSize = 0;
  for (i = 0; i < nobj; i++) {
    mib = &snmp->Mib[obj_list[i]];
    tlv.vSize += 6 + mib->oid_len + mib->var_size;
    /* Read and update variable status */
    if (mib->cb_func != NULL) {
      mib->cb_func(NET_SNMP_MIB_READ);
    }
  }
  tlv.End = 37 + snmp->ComLen + snmp->Mib[sysobj].var_size;
  tlv.vSize += tlv.End;
  if (tlv.vSize > SNMP_MAX_MSG_SIZE) {
    ERRORF (SNMP,"Trap, Out message too big\n");
    EvrNetSNMP_TrapMessageTooBig (tlv.vSize, SNMP_MAX_MSG_SIZE);
    RETURN (netError);
  }
  sendbuf = net_udp_get_buf (tlv.vSize);

  /* Start writing objects first */
  tlv.Next = tlv.End;
  for (i = 0; i < nobj; i++) {
    mib_add_object (sendbuf, &tlv, obj_list[i]);
    DEBUG_INFO (&sendbuf[tlv.Start]);
    DEBUG_INF2 (tlv.MibType,&sendbuf[tlv.vStart]);
    EvrNetSNMP_MibAddObject (obj_list[i], sendbuf[tlv.vStart]);
#ifdef Network_Debug_EVR
    record_oid (&sendbuf[tlv.Start]);
    record_val (tlv.MibType, &sendbuf[tlv.vStart]);
#endif
  }

  /* Add varbind list */
  tlv.Type  = SNMP_TYPE_SEQUENCE;
  tlv.Start = tlv.End;
  bind_tlv (sendbuf, &tlv);

  /* Add time stamp */
  tlv.Type  = SNMP_TYPE_TICKS;
  bind_val (sendbuf, &tlv, netSNMP_sysUpTime);

  /* Add specific and generic trap */
  tlv.Type = SNMP_TYPE_INTEGER;
  bind_val (sendbuf, &tlv, specific);
  bind_val (sendbuf, &tlv, generic);

  /* Add agent IP address */
  tlv.Start -= 2+NET_ADDR_IP4_LEN;
  sendbuf[tlv.Start]   = SNMP_TYPE_IP;
  sendbuf[tlv.Start+1] = NET_ADDR_IP4_LEN;
  net_addr4_copy (&sendbuf[tlv.Start+2],
                  net_addr_get_local (net_if, (const __ADDR *)&trap_srv));

  /* Add enterprise object ID, copy from sysObjectID */
  tlv.Len    = snmp->Mib[sysobj].var_size;
  tlv.vStart = tlv.Start - tlv.Len;
  tlv.Start  = tlv.vStart - 2;
  sendbuf[tlv.Start]  = SNMP_TYPE_OID;
  sendbuf[tlv.Start+1]= tlv.Len & 0xFF;
  memcpy (&sendbuf[tlv.vStart], snmp->Mib[sysobj].var, tlv.Len);

  /* Add PDU type */
  tlv.Type = SNMP_TRAP;
  bind_tlv (sendbuf, &tlv);

  /* Add SNMP community and version */
  tlv.vStart = tlv.Start - snmp->ComLen;
  tlv.Start  = tlv.vStart - 2;
  sendbuf[tlv.Start]   = SNMP_TYPE_STRING;
  sendbuf[tlv.Start+1] = snmp->ComLen;
  memcpy (&sendbuf[tlv.vStart], snmp->ComStr, snmp->ComLen);
  tlv.Type = SNMP_TYPE_INTEGER;  
  bind_val (sendbuf, &tlv, 0);

  /* Add SNMP message type */
  tlv.Type = SNMP_TYPE_SEQUENCE;
  bind_tlv (sendbuf, &tlv);
  i = tlv.Next - tlv.Start;
  if (tlv.Start > 0) {
    /* Align message to start of buffer */
    memcpy (sendbuf, &sendbuf[tlv.Start], (uint32_t)i);
  }
  RETURN (net_udp_send_if (net_if, snmp->Socket,
                           (const __ADDR *)&trap_srv, sendbuf, (uint32_t)i));

  END_LOCK;
}

/**
  \brief       Run SNMP agent main function.
*/
void net_snmp_agent_run (void) {
  if (sys->Flags & SYS_FLAG_TICK) {
    /* UpTime counter resolution 1/100 sec */
    netSNMP_sysUpTime += SYS_TICK_INTERVAL/10;
  }
}

/**
  \brief       Socket event callback notification.
  \param[in]   socket  native UDP socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t snmp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  NET_SNMP_TLV tlv;
  NET_SNMP_ERR err;
  uint16_t id[SNMP_MAX_NUM_OBJ];
  uint8_t *sendbuf;
  uint32_t ver;
  int32_t i;

  /* Check if this is our socket and version IP4 */
  if ((snmp->Socket != socket) || (addr->addr_type != NET_ADDR_IP4)) {
    return (false);
  }
  if (snmp->Mib == NULL) {
    /* No MIB registered, ignore request */
    return (false);
  }

  DEBUGF (SNMP,"*** Process_frame ***\n");
  /* Initial size checking */
  if (len < 23) {
    ERRORF (SNMP,"Process, Frame too short\n");
    EvrNetSNMP_FrameTooShort (len, 23);
    return (false);
  }

  tlv.Next = 0;
  tlv.vSize= 0;
  tlv.End  = len & 0xFFFF;

  /* Check the Message Type */
  tlv.Check = SNMP_TYPE_SEQUENCE;
  if (parse_tlv (buf, &tlv) == false) {
err:ERRORF (SNMP,"Process, Protocol error\n");
    EvrNetSNMP_FrameProtocolError ();
    return (false);
  }

  /* Check SNMP Version */
  tlv.Check = SNMP_TYPE_INTEGER;
  if (parse_tlv (buf, &tlv) == false) goto err;
  ver = get_value (buf, &tlv);
  if (ver != SNMP_VERSION_1) {
    DEBUGF (SNMP," Not SNMP version 1\n");
    EvrNetSNMP_VersionNotSupported ((int32_t)ver);
    return (false);
  }

  /* Check Community String */
  tlv.Check = SNMP_TYPE_STRING;
  if (parse_tlv (buf, &tlv) == false) goto err;
  if ((tlv.Len != snmp->ComLen)    ||
      memcmp (&buf[tlv.vStart],&snmp->ComStr[0],tlv.Len) != 0) {
    /* Not our community, ignore */
    DEBUGF (SNMP,"Not our community\n");
    EvrNetSNMP_WrongCommunityReceived ();
    return (false);
  }
  EvrNetSNMP_ReceiveFrame (addr->addr, len);

  /* Check PDU Request Type */
  tlv.Check = 0;
  if (parse_tlv (buf, &tlv) == false) goto err;
  snmp->ReqType = tlv.Type;
  switch (snmp->ReqType) {
    case SNMP_GET_REQUEST:
      DEBUGF (SNMP," Request  : GET\n");
      break;
    case SNMP_GET_NEXT:
      DEBUGF (SNMP," Request  : GETNEXT\n");
      break;
    case SNMP_SET_REQUEST:
      DEBUGF (SNMP," Request  : SET\n");
      break;
    default:
      ERRORF (SNMP,"Invalid SNMP Request: 0x%02X\n",tlv.Type);
      EvrNetSNMP_InvalidRequestType (tlv.Type);
      return (false);
  }
  err.ReqPos = tlv.Start;

  /* Parse request ID */
  tlv.Check = SNMP_TYPE_INTEGER;
  if (parse_tlv (buf, &tlv) == false) goto err;
  snmp->ReqId = (int32_t)get_value (buf, &tlv);
  DEBUGF (SNMP," RequestID: %d\n",snmp->ReqId);
  EvrNetSNMP_ViewRequest (snmp->ReqType, snmp->ReqId);

  /* Parse error status */
  if (parse_tlv (buf, &tlv) == false) goto err;
  i = (int32_t)get_value (buf, &tlv);
  if (i != SNMP_ERR_NONE) goto err;
  err.StatPos = tlv.vStart;

  /* Parse error index */
  if (parse_tlv (buf, &tlv) == false) goto err;
  i = (int32_t)get_value (buf, &tlv);
  if (i != 0) goto err;
  err.IdxPos = tlv.vStart;

  /* Parse varbind list */
  tlv.Check = SNMP_TYPE_SEQUENCE;
  if (parse_tlv (buf, &tlv) == false) goto err;
  
  /* Parse all PDUs */
  tlv.MibType = 0;
  for (err.Idx = err.Stat = 0; tlv.Next < tlv.End; ) {
    /* Check the size limitation */
    if (++err.Idx > SNMP_MAX_NUM_OBJ) {
      ERRORF (SNMP,"Received too many Objects\n");
      EvrNetSNMP_TooManyObjectsReceived (err.Idx, SNMP_MAX_NUM_OBJ);
      err.Stat = SNMP_ERR_TOO_BIG;
      goto ex0;
    }
    /* Parse varbind type */
    tlv.Check = SNMP_TYPE_SEQUENCE;
    if (parse_tlv (buf, &tlv) == false) goto err;

    /* Parse object ID */
    tlv.Check = SNMP_TYPE_OID;
    if (parse_tlv (buf, &tlv) == false) goto err;
    DEBUG_INFO (&buf[tlv.Start]);
#ifdef Network_Debug_EVR
    record_oid (&buf[tlv.Start]);
#endif

    /* Find MIB entry, set tlv.MibType */
    i = mib_find_object (buf, &tlv);
    if (i < 0) {
      ERRORF (SNMP,"Object %d - Not Found\n",err.Idx);
      EvrNetSNMP_ObjectNotFound (err.Idx);
      err.Stat = SNMP_ERR_NO_NAME;
      goto ex;
    }
    id[err.Idx-1] = i & 0xFFFF;

    /* Parse object value */
    tlv.Check = 0;
    if (parse_tlv (buf, &tlv) == false) goto err;

    if (snmp->ReqType == SNMP_SET_REQUEST) {
      /* SNMP write request */
      uint8_t mib_type = tlv.MibType & SNMP_MIB_TYPE_MASK;
      /* Check object type */
      if (mib_type == SNMP_TYPE_BSTRING) {
        /* Handled as standard type OCTET_STRING */
        mib_type = SNMP_TYPE_STRING;
      }
      if (tlv.Type != mib_type) {
        /* Object type not the same */
        ERRORF (SNMP,"Object %d - Wrong Type Received\n",err.Idx);
        EvrNetSNMP_ObjectWrongType (err.Idx);
        err.Stat = SNMP_ERR_BAD_VAL;
        goto ex;
      }
      DEBUG_INF2 (tlv.MibType,&buf[tlv.Start]);
#ifdef Network_Debug_EVR
      record_val (tlv.MibType, &buf[tlv.Start]);
#endif
      /* Check access attribute */
      if (tlv.MibType & NET_SNMP_MIB_ATR_RO) {
        ERRORF (SNMP,"Object %d - Read Only\n",err.Idx);
        EvrNetSNMP_ObjectReadOnly (err.Idx);
        err.Stat = SNMP_ERR_RD_ONLY;
        goto ex;
      }
      /* Update MIB object value */
      if (mib_write (buf, &tlv, i) == false) {
        ERRORF (SNMP,"Object %d - Write Failed\n",err.Idx);
        EvrNetSNMP_ObjectWriteFailed (err.Idx);
        err.Stat = SNMP_ERR_GENERAL;
        goto ex;
      }
      /* Call an object callback function */
      if (snmp->Mib[i].cb_func != NULL) {
        snmp->Mib[i].cb_func(NET_SNMP_MIB_WRITE);
      }
    }
    else {
      /* SNMP read request */
      if (snmp->Mib[i].cb_func != NULL) {
        snmp->Mib[i].cb_func(NET_SNMP_MIB_READ);
      }
    }
  }
  if (snmp->ReqType == SNMP_SET_REQUEST) {
    /* Respond with GET_RESPONSE to set request or error */
ex0:err.Idx = 0;
ex: sendbuf = net_udp_get_buf (tlv.End);
    DEBUGF (SNMP,"Sending Response\n");
    DEBUGF (SNMP," Error Stat=%d, Idx=%d\n",err.Stat,err.Idx);
    EvrNetSNMP_SendErrorStatus (err.Stat, err.Idx);
    memcpy (sendbuf, buf, tlv.End);
    sendbuf[err.ReqPos]  = SNMP_GET_RESPONSE;
    sendbuf[err.StatPos] = err.Stat;
    sendbuf[err.IdxPos]  = err.Idx;
    /* Send response to the same interface */
    net_udp_send_if ((__ADDR_X(addr))->net_if,
                     snmp->Socket, (const __ADDR *)addr, sendbuf, tlv.End);
    return (true);
  }
  /* Process GET and GET NEXT requests */
  tlv.vSize += tlv.End + 6;
  if (tlv.vSize > SNMP_MAX_MSG_SIZE) {
    ERRORF (SNMP,"Response message too big\n");
    EvrNetSNMP_ResponseMessageTooBig (tlv.vSize, SNMP_MAX_MSG_SIZE);
    err.Stat = SNMP_ERR_TOO_BIG;
    goto ex0;
  }
  DEBUGF (SNMP,"Sending Response\n");
  DEBUGF (SNMP," RequestID: %d\n",snmp->ReqId);
  EvrNetSNMP_SendGetResponse (snmp->ReqId);
  sendbuf = net_udp_get_buf (tlv.vSize);
  /* Start writing objects */
  tlv.Next = err.IdxPos + 9;
  for (i = 0; i < err.Idx; i++) {
    mib_add_object (sendbuf, &tlv, id[i]);
    DEBUG_INFO (&sendbuf[tlv.Start]);
    DEBUG_INF2 (tlv.MibType,&sendbuf[tlv.vStart]);
    EvrNetSNMP_MibAddObject (id[i], sendbuf[tlv.vStart]);
#ifdef Network_Debug_EVR
    record_oid (&sendbuf[tlv.Start]);
    record_val (tlv.MibType, &sendbuf[tlv.vStart]);
#endif
  }

  /* Add varbind list */
  tlv.Type  = SNMP_TYPE_SEQUENCE;
  tlv.Start = err.IdxPos + 9;
  bind_tlv (sendbuf, &tlv);

  /* Add request ID, error and error index */
  i = 8 + ilen(snmp->ReqId);
  tlv.Start -= i;
  memcpy (&sendbuf[tlv.Start], &buf[err.IdxPos+1-i], (uint32_t)i);

  /* Add PDU type */
  tlv.Type   = SNMP_GET_RESPONSE;
  bind_tlv (sendbuf, &tlv);

  /* Add SNMP version and community string */
  i = snmp->ComLen + 5;
  tlv.Start -= i;
  memcpy (&sendbuf[tlv.Start], &buf[err.ReqPos-i], (uint32_t)i);

  /* Add SNMP message type */
  tlv.Type   = SNMP_TYPE_SEQUENCE;
  bind_tlv (sendbuf, &tlv);
  i = tlv.Next - tlv.Start;
  if (tlv.Start > 0) {
    /* Align message to start of buffer */
    memcpy (sendbuf, &sendbuf[tlv.Start], (uint32_t)i);
  }
  /* Send response to the same interface */
  net_udp_send_if ((__ADDR_X(addr))->net_if,
                   snmp->Socket, (const __ADDR *)addr, sendbuf, (uint32_t)i);
  return (true);
}

/**
  \brief       Parse SNMP packet for TLVs (Type-Length-Values).
  \param[in]   buf  buffer containing SNMP data.
  \param[in]   tlv  pointer to TLV state.
  \return      status:
               - true  = success,
               - false = error found.
*/
static bool parse_tlv (const uint8_t *buf, NET_SNMP_TLV *tlv) {
  int32_t i,len,tlen;

  tlv->Start = tlv->Next;
  buf += tlv->Start;
  tlv->Type  = buf[0];
  tlv->vStart= tlv->Start + 2;
  len = buf[1];
  if (len & 0x80) {
    /* Extended length encoding (Hi bit set) */
    tlen = len & 0x7f;
    if (tlen > 4) {
      /* Length range error */
      return (false);
    }
    tlv->vStart += tlen;
    for (i = len = 0; i < tlen; i++) {
      len <<= 8;
      len |= buf[i+2];
    }
  }
  /* Size checking */
  if (tlv->vStart + len > tlv->End) {
    return (false);
  }
  tlv->Len = len & 0xFFFF;
  tlv->Next= tlv->vStart;
  switch (tlv->Type) {
    case SNMP_GET_REQUEST:
    case SNMP_GET_NEXT:
    case SNMP_SET_REQUEST:
    case SNMP_TYPE_SEQUENCE:
      break;
    default:
      tlv->Next += len;
      break;
  }
  if (tlv->Check && (tlv->Check != tlv->Type)) {
    return (false);
  }
  return (true);
}

/**
  \brief       Decode integer value from current TLV.
  \param[in]   buf  buffer containing SNMP data.
  \param[in]   tlv  pointer to TLV state.
  \return      decoded value.
*/
static uint32_t get_value (const uint8_t *buf, NET_SNMP_TLV *tlv) {
  uint32_t val;
  int32_t i;

  buf += tlv->vStart;
  val = 0;
  if (tlv->Type == SNMP_TYPE_INTEGER && buf[0] & 0x80) {
    /* Sign extend a negative number */
    val = 0xFFFFFFFF;
  }
  for (i = 0; i < tlv->Len; i++) {
    val = (val << 8) | buf[i];
  }
  return (val);
}

/**
  \brief       Write integer value to buffer in TLV format.
  \param[in]   buf  output buffer.
  \param[in]   tlv  pointer to TLV state.
  \return      number of bytes written.
*/
static int32_t set_value (uint8_t *buf, NET_SNMP_TLV *tlv, uint32_t val) {
  int32_t i,n;

  if (tlv->Type == SNMP_TYPE_INTEGER) n = ilen ((int32_t)val);
  else                                n = ulen (val);
  buf += tlv->vStart;
  for (i = 4; i; val <<= 8, i--) {
    if (i > n) continue;
    *buf++ = val >> 24;
  }
  return (n);
}

/**
  \brief       Calculate encoding length of unsigned integer.
  \param[in]   val  integer value.
  \return      number of bytes required.
*/
static int32_t ulen (uint32_t val) {
  int32_t len;

  val |= 0x01;
  for (len = 4; val < 0x01000000; val <<= 8, len--);
  return (len);
}

/**
  \brief       Calculate encoding length of signed integer.
  \param[in]   val  integer value.
  \return      number of bytes required.
*/
static int32_t ilen (int32_t val) {
  if (val < 0) {
    val = -1 - val;
  }
  /* Add a leading zero if hi-bit is 1 */
  return (ulen((uint32_t)val<<1));
}

/**
  \brief       Add an object to SNMP response message.
  \param[out]  buf  output buffer.
  \param[in]   tlv  pointer to TLV state.
  \param[in]   id   object index in MIB data table.
*/
static void mib_add_object (uint8_t *buf, NET_SNMP_TLV *tlv, int32_t id) {
  NET_SNMP_MIB_INFO *mib;
  NET_SNMP_BYTE_STR *bp;
  uint32_t val;

  buf += tlv->Next;
  mib  = &snmp->Mib[id];

  /* Varbind */
  buf[0] = SNMP_TYPE_SEQUENCE;
  buf[1] = mib->oid_len + 4;

  /* Object ID */
  buf[2] = SNMP_TYPE_OID;
  buf[3] = mib->oid_len;
  memcpy (&buf[4], mib->oid, mib->oid_len);

  /* Object Value */
  tlv->Start  = mib->oid_len + 4;
  tlv->vStart = tlv->Start + 2;

  tlv->Type   = mib->type & SNMP_MIB_TYPE_MASK;
  tlv->MibType= mib->type;

  switch (tlv->Type) {
    case SNMP_TYPE_STRING:
      tlv->Len = strlen(mib->var) & 0xFFFF;
      memcpy (&buf[tlv->vStart], mib->var, tlv->Len);
      break;
    case SNMP_TYPE_BSTRING:
      bp = (NET_SNMP_BYTE_STR *)mib->var;
      /* Handled as standard type OCTET_STRING */
      tlv->Type = SNMP_TYPE_STRING;
      tlv->Len  = bp->len;
      memcpy (&buf[tlv->vStart], &bp->data, bp->len);
      break;
    case SNMP_TYPE_OID:
    case SNMP_TYPE_IP:
      tlv->Len = mib->var_size;
      memcpy (&buf[tlv->vStart], mib->var, tlv->Len);
      break;
    case SNMP_TYPE_INTEGER:
      val = 0;
      switch (mib->var_size) {
        case 1: /* Read int8_t */
          val = (uint32_t)((int8_t *)mib->var)[0];
          break;
        case 2: /* Read int16_t */
          val = (uint32_t)((int16_t *)mib->var)[0];
          break;
        case 4: /* Read int32_t */
          val = (uint32_t)((int32_t *)mib->var)[0];
          break;
      }
      tlv->Len = set_value (buf, tlv, val) & 0xFFFF;
      break;
    case SNMP_TYPE_COUNTER:
    case SNMP_TYPE_GAUGE:
    case SNMP_TYPE_TICKS:
      val = 0;
      switch (mib->var_size) {
        case 1: /* Read uint8_t */
          val = ((uint8_t *)mib->var)[0];
          break;
        case 2: /* Read uint16_t */
          val = ((uint16_t *)mib->var)[0];
          break;
        case 4: /* Read uint32_t */
          val = ((uint32_t *)mib->var)[0];
          break;
      }
      tlv->Len = set_value (buf, tlv, val) & 0xFFFF;
      break;
  }
  buf[tlv->Start]   = tlv->Type;
  buf[tlv->Start+1] = tlv->Len & 0xFF;
  /* Correct varbind length */
  buf[1]    += tlv->Len;
  tlv->Next += tlv->vStart + tlv->Len;
#ifdef __DEBUG_ENABLED
  /* Start index of Value for debug */
  tlv->vStart = tlv->Next - tlv->Len - 2;
  /* Start index of OID for debug */
  tlv->Start  = tlv->vStart - tlv->Start + 2;
#endif
}

/**
  \brief       Add framing Type and Length in SNMP message.
  \param[out]  buf  output buffer.
  \param[in]   tlv  pointer to TLV state.
*/
static void bind_tlv (uint8_t *buf, NET_SNMP_TLV *tlv) {
  int32_t idx;

  idx = tlv->Start - 2;
  tlv->Len = tlv->Next - tlv->Start;
  if (tlv->Len < 128) {
    buf[idx+1] = tlv->Len & 0xFF;
  }
  else if (tlv->Len < 256) {
    idx--;
    buf[idx+1] = 0x81;
    buf[idx+2] = tlv->Len & 0xFF;
  }
  else {
    idx -= 2;
    buf[idx+1] = 0x82;
    buf[idx+2] = tlv->Len >> 8;
    buf[idx+3] = tlv->Len & 0xFF;
  }
  /* Add type at unit start */
  buf[idx]   = tlv->Type;
  tlv->Start = idx & 0xFFFF;
}

/**
  \brief       Add integer value in SNMP message.
  \param[out]  buf  output buffer.
  \param[in]   tlv  pointer to TLV state.
  \param[in]   val  integer value.
*/
static void bind_val (uint8_t *buf, NET_SNMP_TLV *tlv, uint32_t val) {
  int32_t len;
  
  if (tlv->Type == SNMP_TYPE_INTEGER) len = ilen ((int32_t)val);
  else                                len = ulen (val);
  tlv->vStart = (tlv->Start  - len) & 0xFFFF;
  tlv->Start  = tlv->vStart - 2;
  buf[tlv->Start]   = tlv->Type;
  buf[tlv->Start+1] = len & 0xFF;
  set_value (buf, tlv, val);
}

/**
  \brief       Find an object in MIB data table.
  \param[in]   buf  buffer containing OID.
  \param[in]   tlv  pointer to TLV state.
  \return      index of an object or -1 for not found.
*/
static int32_t mib_find_object (const uint8_t *buf, NET_SNMP_TLV *tlv) {
  NET_SNMP_MIB_INFO *mib;
  int32_t i;

  mib  = &snmp->Mib[0];
  for (i = 0; i < snmp->MibLen; mib++, i++) {
    if (tlv->Len > mib->oid_len) {
      /* OID length different */
      continue;
    }
    if (memcmp (mib->oid, &buf[tlv->vStart], tlv->Len) != 0) {
      /* OID content different */
      continue;
    }
    if (snmp->ReqType != SNMP_GET_NEXT) {
      if (tlv->Len < mib->oid_len) {
        /* Need exact OID match for GET and SET req */
        continue;
      }
    }
    else {
      if (tlv->Len == mib->oid_len) {
        /* If Scalar OID received for GET NEXT request find */
        /* next scalar object from MIB, else use current. */
        if (++i == snmp->MibLen) {
          /* Next scalar not found */
          break;
        }
        mib++;
      }
      if (mib->oid_len > tlv->Len) {
        /* Add space if new OID size is bigger */
        tlv->vSize += (mib->oid_len - tlv->Len);
      }
    }
    /* Calculate space for response */
    tlv->vSize  += mib->var_size;
    tlv->MibType = mib->type;
    return (i);
  }
  return (-1);
}

/**
  \brief       Write an object value to MIB data table.
  \param[in]   buf  buffer containing OID.
  \param[in]   tlv  pointer to TLV state.
  \param[in]   id   object sequence number.
  \return      status:
               - true  = success,
               - false = error.
*/
static bool mib_write (const uint8_t *buf, NET_SNMP_TLV *tlv, int32_t id) {
  NET_SNMP_MIB_INFO *mib;
  uint32_t val;

  mib = &snmp->Mib[id];
  switch (tlv->MibType) {
    case SNMP_TYPE_INTEGER:
    case SNMP_TYPE_COUNTER:
    case SNMP_TYPE_GAUGE:
    case SNMP_TYPE_TICKS:
      val = get_value (buf, tlv);
      switch (mib->var_size) {
        case 1: /* Write uint8_t,int8_t */
          ((uint8_t *)mib->var)[0]  = val & 0xFF;
          return (true);
        case 2: /* Write uint16_t,int16_t */
          ((uint16_t *)mib->var)[0] = val & 0xFFFF;
          return (true);
        case 4: /* Write uint32_t,int32_t */
          ((uint32_t *)mib->var)[0] = val;
          return (true);
      }
      break;

    case SNMP_TYPE_IP:
      /* Write an IP address */
      if (tlv->Len == 4) {
        memcpy (mib->var, &buf[tlv->vStart], 4);
        return (true);
      }
      break;

    case SNMP_TYPE_STRING:
      /* Write octet-string, null-terminated */
      if (tlv->Len <= mib->var_size) {
        uint8_t *cp = (uint8_t *)mib->var;
        cp[tlv->Len] = 0;
        memcpy (cp, &buf[tlv->vStart], tlv->Len);
        return (true);
      }
      break;

    case SNMP_TYPE_BSTRING:
      /* Write byte-string, length-encoded */
      if (tlv->Len <= mib->var_size) {
        NET_SNMP_BYTE_STR *bp = (NET_SNMP_BYTE_STR *)mib->var;
        bp->len = tlv->Len & 0xFF;
        memcpy (&bp->data, &buf[tlv->vStart], tlv->Len);
        return (true);
      }
      break;
  }
  return (false);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print object ID.
  \param[in]   oid  pointer to object ID structure.
*/
static void debug_info (const uint8_t *oid) {
  char *buf;

  buf = (char *)net_mem_alloc (64);
  oid_ntoa (buf, &oid[1]);
  DEBUGF (SNMP," ObjectID : %s\n",buf);
  net_mem_free (__FRAME(buf));
}

/**
  \brief       Debug print object value.
  \param[in]   mib_type  MIB object type.
  \param[in]   oid       pointer to object ID structure.
*/
static void debug_inf2 (uint8_t mib_type, const uint8_t *oid) {
  const char *ty;
  char *buf;

  buf = (char *)net_mem_alloc (64);
  mib_type &= SNMP_MIB_TYPE_MASK;
  obj_ascii (buf, mib_type, oid);
  switch (mib_type) {
    case SNMP_TYPE_INTEGER: ty = "Integer";   break;
    case SNMP_TYPE_COUNTER: ty = "Counter";   break;
    case SNMP_TYPE_GAUGE:   ty = "Gauge";     break;
    case SNMP_TYPE_TICKS:   ty = "Timeticks"; break;
    case SNMP_TYPE_OID:     ty = "OID";       break;
    case SNMP_TYPE_STRING:  ty = "String";    break;
    case SNMP_TYPE_BSTRING: ty = "BString";   break;
    case SNMP_TYPE_IP:      ty = "IpAddress"; break;
    default:                ty = "Unknown";   break;
  }
  DEBUGF (SNMP," + Type   : %s\n",ty);
  DEBUGF (SNMP," + Value  : %s\n",buf);
  net_mem_free (__FRAME(buf));
}

/**
  \brief       Convert object value to ascii string.
  \param[out]  buf       output buffer.
  \param[in]   mib_type  MIB object type.
  \param[in]   oid       pointer to object ID structure.
*/
static void obj_ascii (char *buf, uint8_t mib_type, const uint8_t *oid) {
  int32_t len;

  switch (mib_type) {
    case SNMP_TYPE_INTEGER:
      /* Signed Integer */
      len = net_itoa (buf, get_int(oid));
      break;

    case SNMP_TYPE_COUNTER:
    case SNMP_TYPE_GAUGE:
    case SNMP_TYPE_TICKS:
      /* Unsigned integer */
      len = net_ultoa (buf, get_uint(oid));
      break;

    case SNMP_TYPE_OID:
      /* Object ID */
      len = oid_ntoa (buf, &oid[1]);
      break;

    case SNMP_TYPE_STRING:
      /* Octet-string */
      len = (oid[1] > 62) ? 62 : oid[1];
      memcpy (buf, &oid[2], (uint32_t)len);
      break;

    case SNMP_TYPE_BSTRING:
      /* Byte-string */
      len = (oid[1] > 31) ? 31 : oid[1];
      len = net_bin2hex (buf, &oid[2], (uint32_t)len);
      break;

    case SNMP_TYPE_IP:
      /* IPv4 address */
      len = net_strcpy (buf, net_addr4_ntoa(&oid[2]));
      break;

    default:
      len = 0;
      break;
  }
  /* Add 0-termination */
  buf[len] = 0;
}
#endif

#ifdef Network_Debug_EVR
/**
  \brief       Record object ID in event recorder.
  \param[in]   oid  pointer to object id.
*/
static void record_oid (const uint8_t *oid) {
  char *buf;
  uint32_t len;

  buf = (char *)net_mem_alloc (64);
  len = (uint32_t)oid_ntoa (buf, &oid[1]);
  EvrNetSNMP_ViewObjectId (buf, len);
  net_mem_free (__FRAME(buf));
}

/**
  \brief       Record object value.
  \param[in]   mib_type  MIB object type.
  \param[in]   oid       pointer to object id.
*/
static void record_val (uint8_t mib_type, const uint8_t *oid) {
  uint32_t len;
  char *buf;

  switch (mib_type & SNMP_MIB_TYPE_MASK) {
    case SNMP_TYPE_INTEGER:
      EvrNetSNMP_ViewObjectVal(0,(uint32_t)get_int(oid));
      break;
    case SNMP_TYPE_COUNTER:
      EvrNetSNMP_ViewObjectVal(1,get_uint(oid));
      break;
    case SNMP_TYPE_GAUGE:
      EvrNetSNMP_ViewObjectVal(2,get_uint(oid));
      break;
    case SNMP_TYPE_TICKS:
      EvrNetSNMP_ViewObjectVal(3,get_uint(oid));
      break;
    case SNMP_TYPE_IP:
      EvrNetSNMP_ViewObjectAddr(&oid[2]);
      break;
    case SNMP_TYPE_OID:
      buf = (char *)net_mem_alloc (64);
      len = (uint32_t)oid_ntoa (buf, &oid[1]);
      EvrNetSNMP_ViewObjectVar(0,buf,len);
      net_mem_free (__FRAME(buf));
      break;
    case SNMP_TYPE_STRING:
      EvrNetSNMP_ViewObjectVar(1,&oid[2],oid[1]);
      break;
    case SNMP_TYPE_BSTRING:
      buf = (char *)net_mem_alloc (36);
      len = (uint32_t)net_bin2hex (buf, &oid[2], (oid[1] > 16) ? 16 : oid[1]);
      EvrNetSNMP_ViewObjectVar(2,buf,len);
      net_mem_free (__FRAME(buf));
      break;
  }
}
#endif

#ifdef __DEBUG_ENABLED
/**
  \brief       Convert object id to decimal string.
  \param[out]  buf  output buffer.
  \param[in]   oid  length encoded object id.
  \return      length of the output string.
*/
static int32_t oid_ntoa (char *buf, const uint8_t *oid) {
  int32_t  v,i,len;

  v   = oid[1] / 40;
  len = net_itoa (buf, v);
  buf[len++] = '.';
  len += net_itoa (&buf[len], (oid[1]-v*40));
  for (v = 0, i = 2; i < oid[0]; i++) {
    if (oid[i] & 0x80) {
      /* Continuation bit set */
      v |= (oid[i] & 0x7F);
      v <<= 7;
      continue;
    }
    v |= oid[i];
    buf[len++] = '.';
    len += net_itoa (&buf[len], v);
    v = 0;
  }
  return (len);
}

/**
  \brief       Decode signed integer value.
  \param[in]   oid  pointer to object id.
  \return      decoded value.
*/
static int32_t get_int (const uint8_t *oid) {
  uint32_t val = 0U;
  int32_t i;

  /* Sign extend a negative number */
  if (oid[2] & 0x80) {
    val = 0xFFFFFFFF;
  }
  for (i = 0; i < oid[1]; i++) {
    val = (val << 8) | oid[i+2];
  }
  return ((int32_t)val);
}

/**
  \brief       Decode unsigned integer value.
  \param[in]   oid  pointer to object id.
  \return      decoded value.
*/
static uint32_t get_uint (const uint8_t *oid) {
  uint32_t val = 0U;
  int32_t i;

  for (i = 0; i < oid[1]; i++) {
    val = (val << 8) | oid[i+2];
  }
  return (val);
}
#endif
