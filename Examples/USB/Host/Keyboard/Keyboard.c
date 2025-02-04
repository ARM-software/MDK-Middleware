/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Keyboard.c
 * Purpose: USB Host - HID Keyboard example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_usb.h"

/*-----------------------------------------------------------------------------
 * Application main thread
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  usbStatus usb_status;                 // USB status
  usbStatus hid_status;                 // HID status
  int       ch;                         // Character
  uint8_t   con = 0U;                   // Connection status of keyboard

  (void)argument;

  printf("USB Host HID Keyboard example\n");

  usb_status = USBH_Initialize (0U);    // Initialize USB Host 0
  if (usb_status != usbOK) {
    for (;;) {}                         // Handle USB Host 0 init failure
  }

  for (;;) {
    hid_status = USBH_HID_GetDeviceStatus (0U);  // Get HID device status
    if (hid_status == usbOK) {
      if (con == 0U) {                  // If keyboard was not connected previously
        con = 1U;                       // Keyboard got connected
        printf ("Keyboard connected!\n");
      }
    } else {
      if (con == 1U) {                  // If keyboard was connected previously
        con = 0U;                       // Keyboard got disconnected
        printf ("\nKeyboard disconnected!\n");
      }
    }
    if (con != 0U) {                    // If keyboard is active
      ch = USBH_HID_GetKeyboardKey (0U); // Get pressed key
      if (ch != -1) {                   // If valid key value
        if ((ch & 0x10000) != 0) {      // Handle non-ASCII translated keys (Keypad 0 .. 9)
                                        // Bit  16:    non-ASCII bit (0 = ASCII, 1 = not ASCII)
                                        // Bits 15..8: modifiers (SHIFT, ALT, CTRL, GUI)
                                        // Bits  7..0: ASCII or HID key Usage ID if not ASCII
                                        // HID Usage ID values can be found in following link:
                                        // http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
          ch &= 0xFF;                   // Remove non-ASCII bit and modifiers
          if ((ch>=0x59)&&(ch<=0x61)) { // Keypad 1 .. 9 key convert to
            ch = (ch - 0x59) + '1';     // ASCII  1 .. 9
          } else if (ch == 0x62) {      // Keypad 0 key convert to
            ch = '0';                   // ASCII  0
          } else {                      // If not Keypad 0 .. 9
            ch = -1;                    // invalidate the key
          }
        }
        if ((ch > 0) && (ch < 128)) {   // Output ASCII 0 .. 127 range
          putchar (ch);
          fflush (stdout);
        }
      }
    }
    osDelay(10U);
  }
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
