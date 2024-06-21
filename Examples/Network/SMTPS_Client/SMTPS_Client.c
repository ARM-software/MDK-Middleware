/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client.c
 * Purpose: SMTP Client example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#include "rl_net.h"                     // Keil::Network&MDK:CORE

static const NET_SMTP_MAIL mail = {
  /* Addresses: From */
  "Alex  <alex@gmail.com>;",
  /* Addresses: To */
  "Alister <alister@example.com>;"
  "Susan   <susan@example.com>;",
  /* Cc */
  NULL,
  /* Bcc */
  NULL,
  /* Subject */
  "Subject text",
  /* Message */
  "This is email body.",
  /* Attachments */
  NULL,
  /* Encoding */
  NULL
};

static const NET_SMTP_MTA server = {
  /* Server address (FQDN or IP address) */
  "smtp.gmail.com",
  /* Server port */
  25,
  /* Service control flags */
  0,
  /* Account user name, can be NULL */
  "username",
  /* Account password, can be NULL */
  "password"
};

/*-----------------------------------------------------------------------------
  Application Main Thread 'app_main_thread': Run Network
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  netStatus retv;
  (void)argument;

  printf("MW Network      \n");
  printf("SMTP Client     \n");
  printf("Sending email...\n");

  netInitialize();

  retv = netSMTPc_SendMail(&mail, &server);
  if (retv == netOK) {
    printf("Success, email sent.\n");
  } else {
    printf("Send email failed.\n");
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
