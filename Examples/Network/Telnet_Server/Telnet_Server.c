/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server.c
 * Purpose: Telnet Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_vio.h"                  // ::CMSIS Driver:VIO

#include "rl_net.h"                     // Keil::Network&MDK:CORE

bool LEDrun;

/* IP address change notification */
void netDHCP_Notify (uint32_t if_id, uint8_t option, const uint8_t *val, uint32_t len) {
  char ip_ascii[16];
  (void)len;

  if ((if_id == (NET_IF_CLASS_ETH | 0)) && (option == NET_DHCP_OPTION_IP_ADDRESS)) {
    netIP_ntoa (NET_ADDR_IP4, val, ip_ascii, sizeof(ip_ascii));
    printf("IP4: %s\n",ip_ascii);
  }
}

/*-----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *----------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *argument) {
  (void)argument;

  LEDrun = true;
  while(1) {
    if (LEDrun == true) {
      // Blink LED 0
      vioSetSignal(vioLED0, vioLEDon);
      osDelay(100U);
      vioSetSignal(vioLED0, vioLEDoff);
      osDelay(400U);
    }
    osDelay(500U);
  }
}

/*-----------------------------------------------------------------------------
  Application Main Thread 'app_main_thread': Run Network
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  uint8_t ip_addr[NET_ADDR_IP6_LEN];
  char    ip_ascii[40];
  (void)argument;

  printf("Network Telnet Server example\n");

  netInitialize ();

  osThreadNew(BlinkLed, NULL, NULL);

  printf("IP4: Waiting for DHCP\n");
  if (netIF_GetOption(NET_IF_CLASS_ETH | 0,
                      netIF_OptionIP6_LinkLocalAddress,
                      ip_addr, sizeof(ip_addr)) == netOK) {
    /* IPv6 enabled on ETH0, print Link-local address */
    netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));
    printf("IP6: %s\n", ip_ascii);
  }

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
