/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    BSD_Client.c
 * Purpose: BSD socket client example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "main.h"

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_vio.h"                  // ::CMSIS Driver:VIO

#include "rl_net.h"                     // Keil::Network&MDK:CORE

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

//   <o>Socket type <0=>Stream <1=>Datagram
//   <i>Stream (TCP) or Datagram (UDP) socket type.
#define SOCK_TYPE       0

//   <o>Remote Port <1-65535>
//   <i>Remote port number.
//   <i> Default: 1001
#define SOCK_PORT       1001

//   <e>Use IPv4
//   <i>Use IPv4 Protocol to connect.
#define IP4_ENABLE      1

//     <s.15>Remote IP Address
//     <i>Remote IPv4 Address in text representation.
//     <i>Default: "192.168.0.100"
#define IP4_ADDR        "192.168.0.100"

//   </e>
//   <e>Use IPv6
//   <i>Use IPv6 Protocol to connect
#define IP6_ENABLE      0

//     <s.40>Remote IP Address
//     <i>Remote IPv6 Address in text representation.
//     <i>Default: "fe80::1c30:6cff:fea2:455e"
#define IP6_ADDR        "fe80::1c30:6cff:fea2:455e"

//   </e>

//------------- <<< end of configuration section >>> ---------------------------

#if (!(IP4_ENABLE ^ IP6_ENABLE))
  #error Select only IPv4 or only IPv6!
#endif

#define BLINKLED        0x01
#define SOCKTYPE        ((SOCK_TYPE == 0) ? SOCK_STREAM : SOCK_DGRAM)

/*-----------------------------------------------------------------------------
  Thread 'Client': BSD Client socket process
 *----------------------------------------------------------------------------*/
static __NO_RETURN void Client (void *argument) {
  int32_t sock, retv, led;
  char    dbuf[4];

  (void)argument;

  /* Connect and send LED state */
  while (1) {
    osDelay(2000U);

#if (IP4_ENABLE)
    /* Create IPv4 socket address */
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(SOCK_PORT);
    if (!inet_pton(AF_INET, IP4_ADDR, &addr.sin_addr)) {
      /* Address configuration error */
      for (;;);
    }
    printf("Connect to %s... ", IP4_ADDR);
    sock = socket(AF_INET, SOCKTYPE, 0);
    retv = connect(sock, (SOCKADDR *)&addr, sizeof(addr));
#endif

#if (IP6_ENABLE)
    /* Create IPv6 socket address (MDK-Pro only) */
    SOCKADDR_IN6 addr6;
    addr6.sin6_family   = AF_INET6;
    addr6.sin6_port     = htons(SOCK_PORT);
    addr6.sin6_flowinfo = 0;
    if (!inet_pton(AF_INET6, IP6_ADDR, &addr6.sin6_addr)) {
      /* Address configuration error or IPv6 not supported */
      for (;;);
    }
    printf("Connect to %s... ", IP6_ADDR);
    sock = socket(AF_INET6, SOCKTYPE, 0);
    retv = connect(sock, (SOCKADDR *)&addr6, sizeof(addr6));
#endif

    if (retv < 0) {
      printf("Failed\n");
      closesocket(sock);
      continue;
    }
    printf("Connected\n");

    led = 0;
    while (1) {
      // Send data to BSD Server
      dbuf[0] = BLINKLED;
      dbuf[1] = (char)0x01 << led;
      retv = send(sock, dbuf, 2, 0);
      if (retv < 0) {
        break;
      }
      if (++led >= 8) {
        led = 0;
      }
      // Blink LED 0
      vioSetSignal(vioLED0, vioLEDon);
      osDelay(100U);
      vioSetSignal(vioLED0, vioLEDoff);
      osDelay(900U);
    }
    closesocket(sock);
  }
}

/*-----------------------------------------------------------------------------
  Application Main Thread 'app_main_thread': Run Network
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  (void)argument;

  printf("Network BSD Client example\n");

  netInitialize();

  // Create networking task
  osThreadNew(Client, NULL, NULL);

  osThreadExit();
}

/*-----------------------------------------------------------------------------
 *        Application main function
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, NULL);
  osKernelStart();
  return 0;
}
