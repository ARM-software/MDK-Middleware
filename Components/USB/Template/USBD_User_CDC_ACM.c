/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_ACM_%Instance%.c
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM) User module
 * Rev.:    V6.4.1
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_ACM_%Instance%.c implements the application specific functionality
 * of the CDC ACM class and is used to receive and send data to the USB Host.
 *
 * The implementation must match the configuration file USBD_Config_CDC_%Instance%.h.
 * The following values in USBD_Config_CDC_%Instance%.h affect the user code:
 *
 *  - 'Maximum Communication Device Send Buffer Size' specifies the maximum
 *    value for \em len in \ref USBD_CDC_ACM_WriteData
 *
 *  - 'Maximum Communication Device Receive Buffer Size' specifies the maximum
 *    value for \em len in \ref USBD_CDC_ACM_ReadData
 *
 */
 
 
//! [code_USBD_User_CDC_ACM]
 
#include <stdint.h>
#include <stdbool.h>
 
#include "rl_usb.h"
 
// Local Variables
static   CDC_LINE_CODING        cdc_acm_line_coding = { 0U, 0U, 0U, 0U };
 
 
// Called during USBD_Initialize to initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Initialize (void) {
  // Add code for initialization
}
 
 
// Called during USBD_Uninitialize to de-initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Uninitialize (void) {
  // Add code for de-initialization
}
 
 
// Called upon USB Bus Reset Event.
void USBD_CDC%Instance%_ACM_Reset (void) {
  // Add code for reset
}
 
 
// Callback function called upon reception of request send encapsulated command sent by the USB Host.
// \param[in]   buf           buffer that contains send encapsulated command request.
// \param[in]   len           length of send encapsulated command request.
// \return      true          send encapsulated command request processed.
// \return      false         send encapsulated command request not supported or not processed.
bool USBD_CDC%Instance%_ACM_SendEncapsulatedCommand (const uint8_t *buf, uint16_t len) {
  (void)buf;
  (void)len;

  return true;
}
 
 
// Callback function called upon reception of request to get encapsulated response sent by the USB Host.
// \param[in]   max_len       maximum number of data bytes that USB Host expects to receive
// \param[out]  buf           pointer to buffer containing get encapsulated response to be returned to USB Host.
// \param[out]  len           pointer to number of data bytes to be returned to USB Host.
// \return      true          get encapsulated response request processed.
// \return      false         get encapsulated response request not supported or not processed.
bool USBD_CDC%Instance%_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len) {
  (void)max_len;
  (void)buf;
  (void)len;

  return true;
}
 
 
// Called upon USB Host request to change communication settings.
// \param[in]   line_coding   pointer to CDC_LINE_CODING structure.
// \return      true          set line coding request processed.
// \return      false         set line coding request not supported or not processed.
bool USBD_CDC%Instance%_ACM_SetLineCoding (const CDC_LINE_CODING *line_coding) {
  // Add code for set line coding
 
  // Store requested settings to local variable
  cdc_acm_line_coding = *line_coding;
 
  return true;
}
 
 
// Called upon USB Host request to retrieve communication settings.
// \param[out]  line_coding   pointer to CDC_LINE_CODING structure.
// \return      true          get line coding request processed.
// \return      false         get line coding request not supported or not processed.
bool USBD_CDC%Instance%_ACM_GetLineCoding (CDC_LINE_CODING *line_coding) {
 
  // Load settings from ones stored on USBD_CDC%Instance%_ACM_SetLineCoding callback
  *line_coding = cdc_acm_line_coding;
 
  return true;
}
 
 
// Called upon USB Host request to set control line states.
// \param [in]  state         control line settings bitmap.
//                - bit 0: DTR state
//                - bit 1: RTS state
// \return      true          set control line state request processed.
// \return      false         set control line state request not supported or not processed.
bool USBD_CDC%Instance%_ACM_SetControlLineState (uint16_t state) {
  // Add code for set control line state
 
  (void)(state);
 
  return true;
}
 
 
// Called when new data was received.
// \param [in]  len           number of bytes available for reading.
void USBD_CDC%Instance%_ACM_DataReceived (uint32_t len) {
  // Add code for handling new data reception
  (void)len;
}
 
 
// Called when when all data was sent.
void USBD_CDC%Instance%_ACM_DataSent (void) {
  // Add code for handling new data send
}

//! [code_USBD_User_CDC_ACM]
