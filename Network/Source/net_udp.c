/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_udp.c
 * Purpose: User Datagram Protocol
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_ip4.h"
#ifdef Network_IPv6
#include "net_ip6.h"
#endif
#include "net_udp.h"
#include "net_dbg.h"

/* Code shortening macros */
#define udp       (&net_udp_config)

/* Local Functions */
static uint16_t udp_alloc_port (void);
static bool udp_port_in_use (uint16_t port);
#ifdef Network_Debug_STDIO
 static void debug_info (const NET_UDP_HEADER *udp_hdr);
#endif

/* ==== Protected UDP Socket API functions ==== */

int32_t netUDP_GetSocket (netUDP_cb_t cb_func) {
  START_LOCK (int32_t);
  RETURN (net_udp_get_socket (cb_func));
  END_LOCK;
}
netStatus netUDP_ReleaseSocket (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_udp_release_socket (socket));
  END_LOCK;
}
netStatus netUDP_Open (int32_t socket, uint16_t port) {
  START_LOCK (netStatus);
  RETURN (net_udp_open (socket, port));
  END_LOCK;
}
netStatus netUDP_Close (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_udp_close (socket));
  END_LOCK;
}
uint8_t *netUDP_GetBuffer (uint32_t size) {
  START_LOCK_DBG (uint8_t *);
  RETURN_DBG (net_udp_get_buf (size | 0x80000000));
  END_LOCK_DBG;
}
netStatus netUDP_Send (int32_t socket,
                       const NET_ADDR *addr, uint8_t *buf, uint32_t len) {
  START_LOCK (netStatus);
  RETURN (net_udp_send_if (NULL, socket, (const __ADDR *)addr, buf, len));
  END_LOCK;
}
netStatus netUDP_SetOption (int32_t socket, netUDP_Option option, uint32_t val) {
  START_LOCK (netStatus);
  RETURN (net_udp_set_option (socket, option, val));
  END_LOCK;
}
uint16_t netUDP_GetLocalPort (int32_t socket) {
  return (net_udp_get_local_port (socket));
}

/* ==== Internal UDP Socket functions ==== */

/**
  \brief       Initialize UDP sockets.
*/
void net_udp_socket_init (void) {
  DEBUGF (UDP,"Init %d Sockets\n",udp->NumSocks);
  EvrNetUDP_InitSockets (udp->NumSocks);

  /* Clear SCB for all sockets */
  memset (udp->Scb, 0, sizeof (*udp->Scb) * udp->NumSocks);
}

/**
  \brief       De-initialize UDP sockets.
*/
void net_udp_socket_uninit (void) {
  DEBUGF (UDP,"Uninit Sockets\n");
  EvrNetUDP_UninitSockets ();

  /* Clear SCB for all sockets */
  memset (udp->Scb, 0, sizeof (*udp->Scb) * udp->NumSocks);
}

/**
  \brief       Allocate a free UDP socket.
  \param[in]   cb_func  event listening callback function.
  \return      socket handle number or execution status when < 0
*/
int32_t net_udp_get_socket (netUDP_cb_t cb_func) {
  NET_UDP_INFO *udp_s;
  int32_t i;

  if (cb_func == NULL) {
    ERRORF (UDP,"GetSocket, Invalid parameter\n");
    EvrNetUDP_GetSocketInvalidParameter ();
    return (-netInvalidParameter);
  }

  for (i = 1, udp_s = &udp->Scb[0]; i <= udp->NumSocks; udp_s++, i++) {
    if (udp_s->State == UDP_STATE_FREE) {
      /* Free UDP Socket found */
      DEBUGF (UDP,"Get Socket %d\n",i);
      EvrNetUDP_GetSocket (i, udp->NumSocks);
      udp_s->State   = UDP_STATE_CLOSED;
      udp_s->Flags   = NET_UDP_CHECKSUM_SEND | NET_UDP_CHECKSUM_VERIFY;
      udp_s->LocPort = 0;
      udp_s->Tos     = UDP_TOS_NORMAL;
      udp_s->McastTtl= 1;
      udp_s->TClass  = UDP_TCLASS_NORMAL;
      udp_s->HopLimit= 1;
      udp_s->net_if  = NULL;
      udp_s->cb_func = cb_func;
      /* Return socket handle */
      return (i);
    }
  }
  /* No free UDP Socket found, return error */
  ERRORF (UDP,"GetSocket, Out of sockets\n");
  EvrNetUDP_NoSocketAvailable (udp->NumSocks);
  return (-netError);
}

/**
  \brief       Release UDP socket and free resources.
  \param[in]   socket  socket handle.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_release_socket (int32_t socket) {
  NET_UDP_INFO *udp_s;

  DEBUGF (UDP,"Release Socket %d\n",socket);
  EvrNetUDP_ReleaseSocket (socket);
  if (socket <= 0 || socket > udp->NumSocks) {
    ERRORF (UDP,"Release, Socket %d not valid\n",socket);
    EvrNetUDP_ReleaseSocketNotValid (socket);
    return (netInvalidParameter);
  }
  udp_s = &udp->Scb[socket-1];
  if (udp_s->State > UDP_STATE_CLOSED) {
    /* Wrong Socket State, can not release */
    ERRORF (UDP,"Release, Socket %d wrong state\n",socket);
    EvrNetUDP_ReleaseSocketWrongState (socket, udp_s->State);
    return (netWrongState);
  }
  /* Clear the socket control block */
  memset (udp_s, 0, sizeof (*udp_s));
  return (netOK);
}

/**
  \brief       Open UDP socket for communication.
  \param[in]   socket  socket handle.
  \param[in]   port    local port number or
                       0 for system assigned local port.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_open (int32_t socket, uint16_t port) {
  NET_UDP_INFO *udp_s;

  DEBUGF (UDP,"Open Socket %d, Port %d\n",socket,port);
  EvrNetUDP_OpenSocket (socket, port);
  if (socket <= 0 || socket > udp->NumSocks) {
    ERRORF (UDP,"Open, Socket %d not valid\n",socket);
    EvrNetUDP_OpenSocketNotValid (socket);
    return (netInvalidParameter);
  }
  udp_s = &udp->Scb[socket-1];
  if (udp_s->State != UDP_STATE_CLOSED) {
    /* Not possible to open, wrong socket state */
    ERRORF (UDP,"Open, Socket %d wrong state\n",socket);
    EvrNetUDP_OpenSocketWrongState (socket, udp_s->State);
    return (netWrongState);
  }
  if (port == 0) {
    /* Local port not specified, allocate a free one */
    port = udp_alloc_port ();
    DEBUGF (UDP," Port %d assigned\n",port);
    EvrNetUDP_AssignLocalPort (socket, port);
  }
  else if (udp_port_in_use (port) == true) {
    /* Local port not available, already in use */
    ERRORF (UDP,"Open, Socket %d Port %d in use\n",socket,port);
    EvrNetUDP_OpenLocalPortInUse (socket, port);
    return (netInvalidParameter);
  }
  udp_s->LocPort = port;
  udp_s->State   = UDP_STATE_OPENED;
  return (netOK);
}

/**
  \brief       Stop UDP communication and close socket.
  \param[in]   socket  socket handle.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_close (int32_t socket) {
  NET_UDP_INFO *udp_s;

  DEBUGF (UDP,"Close Socket %d\n",socket);
  EvrNetUDP_CloseSocket (socket);
  if (socket <= 0 || socket > udp->NumSocks) {
    ERRORF (UDP,"Close, Socket %d not valid\n",socket);
    EvrNetUDP_CloseSocketNotValid (socket);
    return (netInvalidParameter);
  }
  udp_s = &udp->Scb[socket-1];
  udp_s->State = UDP_STATE_CLOSED;
  return (netOK);
}

/**
  \brief       Set UDP/IP options for UDP socket.
  \param[in]   socket  socket handle.
  \param[in]   option  option name as defined with netUDP_Option.
  \param[in]   val     option value.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_set_option (int32_t socket, netUDP_Option option, uint32_t val) {
  NET_UDP_INFO *udp_s;
  NET_IF_CFG *net_if;

  DEBUGF (UDP,"SetOption Socket %d\n",socket);
  EvrNetUDP_SetOptionSocket (socket, option, val);
  if (socket <= 0 || socket > udp->NumSocks) {
    ERRORF (UDP,"SetOption, Socket %d not valid\n",socket);
    EvrNetUDP_SetOptionSocketNotValid (socket);
    return (netInvalidParameter);
  }
  udp_s = &udp->Scb[socket-1];
  if (udp_s->State == UDP_STATE_FREE) {
    ERRORF (UDP,"SetOption, Socket %d wrong state\n",socket);
    EvrNetUDP_SetOptionSocketWrongState (socket, udp_s->State);
    return (netWrongState);
  }
  switch ((int32_t)option) {
    case netUDP_OptionTOS:
      if (val > 255) break;
      DEBUGF (UDP," TOS=%d\n",val);
      EvrNetUDP_SetOptionTos (socket, val & 0xFF);
      udp_s->Tos = val & 0xFF;
      return (netOK);

    case netUDP_OptionTTL:
      if (val > 255) break;
      DEBUGF (UDP," Mcast TTL=%d\n",val);
      EvrNetUDP_SetOptionTtl (socket, val & 0xFF);
      udp_s->McastTtl = val & 0xFF;
      return (netOK);

#ifdef Network_IPv6
    case netUDP_OptionTrafficClass:
      if (val > 255) break;
      DEBUGF (UDP," Tclass=%d\n",val);
      EvrNetUDP_SetOptionTclass (socket, val & 0xFF);
      udp_s->TClass = val & 0xFF;
      return (netOK);

    case netUDP_OptionHopLimit:
      if (val > 255) break;
      DEBUGF (UDP," HopLimit=%d\n",val);
      EvrNetUDP_SetOptionHopLimit (socket, val & 0xFF);
      udp_s->HopLimit = val & 0xFF;
      return (netOK);
#endif

    case netUDP_OptionInterface:
      net_if = net_if_map_lan (val);
      if (net_if == NULL) break;
      DEBUGF (UDP," Interface=%s\n",net_if->Name);
      EvrNetUDP_SetOptionInterface (socket, net_if->Id);
      udp_s->net_if = net_if;
      return (netOK);

    case netUDP_OptionChecksum:
      if (val & ~UDP_FLAG_CKS_MASK) break;
      DEBUGF (UDP," CksumOpt=%d\n",val);
      EvrNetUDP_SetOptionChecksum (socket, val);
      udp_s->Flags &= ~UDP_FLAG_CKS_MASK;
      udp_s->Flags |=  val;
      return (netOK);

    default:
      ERRORF (UDP,"SetOption, Socket %d wrong option\n",socket);
      EvrNetUDP_SetOptionWrongOption (socket, option);
      return (netInvalidParameter);
  }
  ERRORF (UDP,"SetOption, Socket %d wrong val (%d)\n",socket,val);
  EvrNetUDP_SetOptionWrongValue (socket, val);
  return (netInvalidParameter);
}

/**
  \brief       Get UDP/IP options for UDP socket.
  \param[in]   socket  socket handle.
  \param[in]   option  option name as defined with netUDP_Option.
  \return      option value.
*/
uint32_t net_udp_get_option (int32_t socket, netUDP_Option option) {
  NET_UDP_INFO *udp_s;

  if (socket <= 0 || socket > udp->NumSocks) {
    return (0);
  }
  udp_s = &udp->Scb[socket-1];
  if (udp_s->State == UDP_STATE_FREE) {
    return (0);
  }
  switch ((int32_t)option) {
    case netUDP_OptionTOS:
      return (udp_s->Tos);

    case netUDP_OptionTTL:
      return (udp_s->McastTtl);

#ifdef Network_IPv6
    case netUDP_OptionTrafficClass:
      return (udp_s->TClass);

    case netUDP_OptionHopLimit:
      return (udp_s->HopLimit);
#endif

    case netUDP_OptionInterface:
      return (udp_s->net_if->Id);

    case netUDP_OptionChecksum:
      return (udp_s->Flags & UDP_FLAG_CKS_MASK);

    default:
      break;
  }
  return (0);
}

/**
  \brief       Retrieve local port number of UDP socket.
  \param[in]   socket  socket handle.
  \return      local port number or
                 0 = socket invalid or in invalid state.
*/
uint16_t net_udp_get_local_port (int32_t socket) {
  NET_UDP_INFO *udp_s;

  if ((socket <= 0) || (socket > udp->NumSocks)) {
    return (0);
  }
  udp_s = &udp->Scb[socket-1];
  if (udp_s->State < UDP_STATE_OPENED) {
    return (0);
  }
  return (udp_s->LocPort);
}

/**
  \brief       Allocate memory for UDP send buffer.
  \param[in]   size  number of bytes to allocate.
  \return      pointer to the allocated memory.
  \note        If the size has high bit set, (or-ed with 0x80000000),
               the function returns NULL on error, instead of calling 
               system error function sys_error().
*/
uint8_t *net_udp_get_buf (uint32_t size) {
  NET_FRAME *frame;
  uint32_t sz = size & 0xFFFF;

  if (size & 0x80000000) {
    /* Disables sys_error() call, when out of memory */
    sz |= 0x40000000;
  }
  frame = net_mem_alloc (sz + UDP_DATA_OFFS);
  if (frame == NULL) {
    /* Only if sys_error() call disabled */
    ERRORF (UDP,"GetBuffer, Out of memory\n");
    EvrNetUDP_GetBufferFailed (sz & 0xFFFF);
    return (NULL);
  }
  /* Reserve the space for protocol headers */
  frame->index = UDP_DATA_OFFS;
  return (&frame->data[UDP_DATA_OFFS]);
}

/**
  \brief       Release udp frame buffer.
  \param[in]   buf  frame buffer.
  \note        When not auto-released in net_udp_send().
*/
void net_udp_free_buf (uint8_t *buf) {
  net_mem_free(__FRAME(buf - (UDP_DATA_OFFS + NET_HEADER_LEN)));
}

/**
  \brief       Send data to a remote node.
  \param[in]   socket  socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     buffer containing the data.
  \param[in]   len     length of data in bytes.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_send (int32_t socket, const __ADDR *addr,
                                        uint8_t *buf, uint32_t len) {
  return (net_udp_send_if (NULL, socket, addr, buf, len));
}

/**
  \brief       Send data on selected interface.
  \param[in]   net_if  network interface descriptor.
  \param[in]   socket  socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     buffer containing the data.
  \param[in]   len     length of data in bytes.
  \return      status code as defined with netStatus.
*/
netStatus net_udp_send_if (NET_IF_CFG *net_if, int32_t socket,
                           const __ADDR *addr, uint8_t *buf, uint32_t len) {
  NET_UDP_INFO *udp_s;
  NET_UDP_HEADER *udp_hdr;
  NET_FRAME *frame;
  const uint8_t *src_addr;
  netStatus retv;

  DEBUGF (UDP,"Send Socket %d, %d bytes\n",socket,len);
  EvrNetUDP_SendFrame (socket, len);
  frame = __FRAME(buf - (UDP_DATA_OFFS + NET_HEADER_LEN));
  /* Check if this is a valid frame (allocated by net_udp_get_buf) */
  if ((buf == NULL) || (frame->index != UDP_DATA_OFFS)) {
    /* Wrong buffer passed, do nothing and return */
    ERRORF (UDP,"Send, Socket %d invalid buffer\n",socket);
    EvrNetUDP_SendBufferInvalid (socket);
    return (netInvalidParameter);
  }
  if (socket <= 0 || socket > udp->NumSocks) {
    ERRORF (UDP,"Send, Socket %d not valid\n",socket);
    EvrNetUDP_SendSocketNotValid (socket);
    net_mem_free (frame);
    return (netInvalidParameter);
  }

  udp_s = &udp->Scb[socket-1];
  if (net_addr_is_unspec (addr)) {
    /* Destination IP address not specified */
    ERRORF (UDP,"Send, Socket %d IpAddr unspecified\n",socket);
    EvrNetUDP_SendAddressUnspecified (socket);
    retv = netInvalidParameter;
    goto retf;
  }
  if (addr->port == 0) {
    /* Remote UDP port not specified */
    ERRORF (UDP,"Send, Socket %d Port undefined\n",socket);
    EvrNetUDP_SendPortUndefined (socket);
    retv = netInvalidParameter;
    goto retf;
  }
  if (udp_s->State != UDP_STATE_OPENED) {
    /* This socket is not opened, return */
    ERRORF (UDP,"Send, Socket %d not open\n",socket);
    EvrNetUDP_SendSocketNotOpen (socket);
    retv = netWrongState;
    goto retf;
  }
  if (len == 0) {
    /* For 0-length packets, free buffer and return */
    DEBUGF (UDP," Dumping 0-length packet\n");
    EvrNetUDP_SendZeroLengthFrame (socket);
    retv = netInvalidParameter;
retf:
    if (udp_s->Flags & UDP_FLAG_KEEP) {
      /* Clear the one-shot KEEP flag */
      udp_s->Flags &= ~UDP_FLAG_KEEP;
    }
    else {
      /* Auto-free the packet on error */
      net_mem_free (frame);
    }
    return (retv);
  }

  /* Check local scope address */
  if (net_if == NULL) {
    if (addr->addr_type == NET_ADDR_IP4) {
      if (net_addr4_comp (addr->addr, net_addr_bcast)) {
        /* Broadcast address */
        net_if = udp_s->net_if;
      }
    }
#ifdef Network_IPv6
    else {
      if (addr->addr[0] >= 0xFE) {
        /* Link-local address */
        net_if = udp_s->net_if;
      }   
    }
#endif
  }

  /* Confirm the route interface or find a new one */
  net_if = net_addr_find_route (net_if, addr);
  if (net_if == NULL) {
    /* No route to destination */
    ERRORF (UDP,"Send, Socket %d no route found\n",socket);
    EvrNetUDP_SendNoRouteFound (socket);
    retv = netError;
    goto retf;
  }

  /* Check the UDP data size limits */
  if (len > UDP_MTU) {
    len = UDP_MTU;
    DEBUGF (UDP," Size truncated to %d bytes\n",len);
    EvrNetUDP_SendSizeTruncated (socket, len);
  }

  udp_hdr = __ALIGN_CAST(NET_UDP_HEADER *)&frame->data[IP6_DATA_OFFS];

  /* Put UDP header into frame */
  udp_hdr->SrcPort = htons(udp_s->LocPort);
  udp_hdr->DstPort = htons(addr->port);
  udp_hdr->Len     = htons(len + UDP_HEADER_LEN);
  udp_hdr->Chksum  = 0;

  /* Calculate frame length (including UDP header) */
  frame->length = (len + UDP_HEADER_LEN) & 0xFFFF;

  src_addr = net_addr_get_local (net_if, addr);

  /* Calculate UDP frame checksum */
  if (udp_s->Flags & NET_UDP_CHECKSUM_SEND) {
    if (addr->addr_type == NET_ADDR_IP4) {
      if (!net_ip4_tx_offl_hl (net_if, SYS_OFFL_UDP4_TX, frame->length)) {
        uint16_t cks = net_ip4_chksum (src_addr, addr->addr, udp_hdr,
                                       IP4_PROT_UDP, frame->length);
        /* Chksum=0 turns off checksum check at destination */
        if (cks == 0) cks = 0xFFFF;
        udp_hdr->Chksum = htons(cks);
      }
    }
#ifdef Network_IPv6
    else {
      if (!net_ip6_tx_offl_hl (net_if, SYS_OFFL_UDP6_TX)) {
        uint16_t cks = net_ip6_chksum (src_addr, addr->addr, udp_hdr,
                                       IP6_PROT_UDP, frame->length);
        if (cks == 0) cks = 0xFFFF;
        udp_hdr->Chksum = htons(cks);
      }
    }
#endif
  }

  DEBUG_INFO (udp_hdr);
  EvrNetUDP_ShowFrameHeader (udp_hdr);

  retv = netOK;

  /* Send the frame to IP */
  if (addr->addr_type == NET_ADDR_IP4) {
    uint8_t ttl = 128;
    if ((addr->addr[0] & 0xF0) == 0xE0) {
      /* This is a Multicast UDP datagram */
      ttl = udp_s->McastTtl;
    }
    /* IPv4 header is shorter than IPv6, so the start of frame is  */
    /* moved to 20 byte offset within the original frame. This new */
    /* origin is used only temporary in IPv4 and ethernet layer to */
    /* construct headers and send the frame. With this concept, we */
    /* avoid large memcpy() of UDP data !!!                        */
    NET_WI4(frame)->length = frame->length;
    if (!net_ip4_send_frame (net_if, NET_WI4(frame), addr->addr,
                             IP4_PROT_UDP, udp_s->Tos, ttl)) {
      retv = netError;
    }
  }
  else {
#ifdef Network_IPv6
    uint8_t hlimit = 0;
    if (addr->addr[0] == 0xFF) {
      /* Multicast destination, prefix "ff00::/8" */
      hlimit = udp_s->HopLimit;
    }
    /* Initial position of UDP data in a frame is calculated for  */
    /* IPv6, so the data is already in place. No memcpy() needed. */
    if (!net_ip6_send_frame (net_if, frame, src_addr, addr->addr,
                             IP6_PROT_UDP, udp_s->TClass, hlimit)) {
      retv = netError;
    }
#else
    retv = netError;
#endif
  }

  if (udp_s->Flags & UDP_FLAG_KEEP) {
    /* Keep the packet for retransmission */
    udp_s->Flags &= ~UDP_FLAG_KEEP;
    frame->index = UDP_DATA_OFFS;
  }
  else {
    /* Auto-free the packet */
    net_mem_free (frame);
  }
  return (retv);
}

/**
  \brief       Prevent net_udp_send() to auto-release packet.
  \param[in]   socket   socket handle.
  \param[in]   buf      frame buffer.
  \return      status:
               - true  = keep attribute set,
               - false = error, invalid packet.
*/
bool net_udp_keep_buf (int32_t socket, uint8_t *buf) {
  NET_UDP_INFO *udp_s;
  NET_FRAME *frame;

  if (socket <= 0 || socket > udp->NumSocks) {
    return (false);
  }

  frame = __FRAME(buf - (UDP_DATA_OFFS + NET_HEADER_LEN));
  /* Check if this is a valid frame */
  if ((buf == NULL) || (frame->index != UDP_DATA_OFFS)) {
    return (false);
  }
  udp_s = &udp->Scb[socket-1];
  udp_s->Flags |= UDP_FLAG_KEEP;
  /* The packet will be kept by net_udp_send() */
  return (true);
}

/**
  \brief       Enable reception of link-layer addressed frames.
  \param[in]   socket  socket handle.
  \details     Used for DHCP client.
*/
void net_udp_enable_lla (int32_t socket) {
  NET_UDP_INFO *udp_s;

  if (socket <= 0 || socket > udp->NumSocks) {
    return;
  }
  udp_s = &udp->Scb[socket-1];
  udp_s->Flags |= UDP_FLAG_RECV_LLA;
}

/**
  \brief       Process received UDP frame.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
  \details     In order to keep the user API unchanged, we use a hack
               to provide a destination IP in the callback function.
*/
void net_udp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver) {
  static NET_ADDR_X addr;
  NET_UDP_INFO *udp_s;
  NET_UDP_HEADER *udp_hdr;
  uint16_t dlen,port;
  uint8_t socket;
  int32_t i;

  DEBUGF (UDP,"*** Process_frame ***\n");
  udp_hdr = __ALIGN_CAST(NET_UDP_HEADER *)&frame->data[frame->index];

  dlen = ntohs(udp_hdr->Len);
  /* Check the frame length:                  */
  /* - minimum length  >= (UDP header length) */
  /* - physical length >= (UDP frame length)  */
  if ((dlen < UDP_HEADER_LEN) || (dlen > frame->length)) {
    /* Frame too short */
    ERRORF (UDP,"Process, Frame too short\n");
    EvrNetUDP_FrameTooShort (frame->length, UDP_HEADER_LEN);
    return;
  }

  DEBUG_INFO (udp_hdr);
  EvrNetUDP_ReceiveFrame (frame->length, ip_ver);
  EvrNetUDP_ShowFrameHeader (udp_hdr);

  /* Check destination port */
  port = ntohs(udp_hdr->DstPort);
  if (port == 0) {
    /* Port 0 is reserved, never open */
    goto not_mapped;
  }

  /* Map to UDP socket */
  socket = 0;
  for (i = 1, udp_s = &udp->Scb[0]; i <= udp->NumSocks; udp_s++, i++) {
    if ((udp_s->State == UDP_STATE_OPENED) && (udp_s->LocPort == port)) {
      /* Open socket found */
      socket = i & 0xFF;
      DEBUGF (UDP," Mapped to Socket %d\n",i);
      EvrNetUDP_MapFrameToSocket (i);
      break;
    }
  }
  if (socket == 0) {
not_mapped:
    /* We should send back an ICMP frame here to report non existing socket */
    DEBUGF (UDP," Discarded, Frame not mapped\n");
    EvrNetUDP_FrameNotMapped (frame->length);
    return;
  }
  /* Check if link-layer addressed */
  if (!(udp_s->Flags & UDP_FLAG_RECV_LLA) && (sys->Flags & SYS_FLAG_LLA_UDP)) {
    /* Link-layer addressing not enabled for the socket  */
    /*  (link-layer addr = local MAC, IP addr don't care */
    ERRORF (UDP,"Socket %d, Link-layer addressed\n",socket);
    EvrNetUDP_LinkLayerAddressed (socket);
    return;
  }

  /* Optionally verify checksum of received frame */
  if ((udp_s->Flags & NET_UDP_CHECKSUM_VERIFY) && (udp_hdr->Chksum != 0)) {
    /* Verify checksum if UDP header checksum not 0 */
    if (ip_ver == IP_VER4) {
      if (!(sys->RxOffload & SYS_OFFL_UDP4_RX) &&
          (net_ip4_chksum (IP4_FRAME(frame)->SrcAddr, IP4_FRAME(frame)->DstAddr,
                           udp_hdr, IP4_PROT_UDP, ntohs(udp_hdr->Len)) != 0)) {
        /* IPv4 Checksum check failed */
        ERRORF (UDP,"Socket %d, Receive checksum failed\n",socket);
        EvrNetUDP_ChecksumFailed (socket);
        return;
      }
    }
    else {
#ifdef Network_IPv6
      if (!(sys->RxOffload & SYS_OFFL_UDP6_RX) &&
          (net_ip6_chksum (IP6_FRAME(frame)->SrcAddr, IP6_FRAME(frame)->DstAddr,
                           udp_hdr, IP6_PROT_UDP, ntohs(udp_hdr->Len)) != 0)) {
        /* IPv6 Checksum check failed */
        ERRORF (UDP,"Socket %d, Receive checksum failed\n",socket);
        EvrNetUDP_ChecksumFailed (socket);
        return;
      }
#else
      return;
#endif
    }
  }
  /* Build source net address structure */
  addr.net_if = net_if;
  addr.port   = ntohs(udp_hdr->SrcPort);
  if (ip_ver == IP_VER4) {
    addr.addr_type = NET_ADDR_IP4;
    net_addr4_copy (addr.addr, IP4_FRAME(frame)->SrcAddr);
    addr.dst_addr  = IP4_FRAME(frame)->DstAddr;
  }
  else {
#ifdef Network_IPv6
    addr.addr_type = NET_ADDR_IP6;
    net_addr6_copy (addr.addr, IP6_FRAME(frame)->SrcAddr);
    addr.dst_addr  = IP6_FRAME(frame)->DstAddr;
#else
    return;
#endif
  }

  /* Generate data event, call callback event function */
  udp_s->cb_func (socket, (NET_ADDR *)&addr,
                           udp_hdr->Data, frame->length - UDP_HEADER_LEN);
}

/**
  \brief       Allocate a free dynamic UDP port.
  \return      port number.
*/
static uint16_t udp_alloc_port (void) {
  static uint16_t port;

  /* IANA categories of UDP ports       */
  /* a) well-known ports: 0     - 1023  */
  /* b) registered ports: 1024  - 49151 */
  /* c) dynamic ports   : 49152 - 65535 */
  while (1) {
    /* Scan all available private ports */
    if ((++port < udp->DynPortStart) || (port > udp->DynPortEnd)) {
      /* Use custom dynamic/private ports */
      port = udp->DynPortStart;
    }
    if (udp_port_in_use (port) == false) {
      /* This port is free, stop searching */
      return (port);
    }
  }
}

/**
  \brief       Check if UDP port is in use.
  \param[in]   port  UDP port to check.
  \return      check status:
               - true  = port in use,
               - false = port free.
*/
static bool udp_port_in_use (uint16_t port) {
  NET_UDP_INFO *udp_s;
  int32_t i;

  for (i = 0, udp_s = &udp->Scb[0]; i < udp->NumSocks; udp_s++, i++) {
    if ((port == udp_s->LocPort) && (udp_s->State > UDP_STATE_CLOSED)) {
      /* This port is in use */
      return (true);
    }
  }
  return (false);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print UDP header information.
  \param[in]   udp_hdr  UDP frame header.
*/
static void debug_info (const NET_UDP_HEADER *udp_hdr) {

  DEBUGF (UDP," Ports: Src=%d, Dst=%d\n",ntohs(udp_hdr->SrcPort),
                                         ntohs(udp_hdr->DstPort));
  DEBUGF (UDP," Cksum: 0x%04X\n",ntohs(udp_hdr->Chksum));
  DEBUGF (UDP," Length %d bytes\n",ntohs(udp_hdr->Len));
}
#endif
