/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_ACM_0.c
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM) User module
 * Rev.:    V6.4.1
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_ACM_0.c implements the application specific functionality
 * of the CDC ACM class and is used to receive and send data to the USB Host.
 *
 * The implementation must match the configuration file USBD_Config_CDC_0.h.
 * The following values in USBD_Config_CDC_0.h affect the user code:
 *
 *  - 'Maximum Communication Device Send Buffer Size' specifies the maximum
 *    value for \em len in \ref USBD_CDC_ACM_WriteData
 *
 *  - 'Maximum Communication Device Receive Buffer Size' specifies the maximum
 *    value for \em len in \ref USBD_CDC_ACM_ReadData
 *
 */


//! [code_USBD_User_CDC_ACM]

#include "MW_CV_Config.h"
#if    (((MW_CV_USBD != 0U) && (MW_CV_USBD_CDC_ACM != 0U)) || \
        ((MW_CV_USBH != 0U) && (MW_CV_USBH_CDC_ACM != 0U)))

#include "USBD_User_CDC_ACM_0.h"

#include <stdint.h>
#include <string.h>

#include "rl_usb.h"

// Global structure used for testing control and results
MW_CV_USBD_CDC_ACM_t mw_cv_usbd_cdc0_acm;

// Local Variables
static   uint8_t                rece_len;
static   uint8_t                rece_buf[64];
static   uint8_t                send_buf[64];
static   CDC_LINE_CODING        cdc_acm_line_coding = { 0U, 0U, 0U, 0U };
static   const CDC_LINE_CODING  line_coding_115200  = { 115200U, 0U, 0U, 8U };


// Called during USBD_Initialize to initialize the USB CDC class instance (ACM).
void USBD_CDC0_ACM_Initialize (void) {
  if (!mw_cv_usbd_cdc0_acm.testing_active) { return; }
  mw_cv_usbd_cdc0_acm.cb_initialize = true;
}


// Called during USBD_Uninitialize to de-initialize the USB CDC class instance (ACM).
void USBD_CDC0_ACM_Uninitialize (void) {
  if (!mw_cv_usbd_cdc0_acm.testing_active) { return; }
  mw_cv_usbd_cdc0_acm.cb_uninitialize = true;
}


// Called upon USB Bus Reset Event.
void USBD_CDC0_ACM_Reset (void) {
  if (!mw_cv_usbd_cdc0_acm.testing_active) { return; }
  mw_cv_usbd_cdc0_acm.cb_reset = true;
}


// Callback function called upon reception of request send encapsulated command sent by the USB Host.
// \param[in]   buf           buffer that contains send encapsulated command request.
// \param[in]   len           length of send encapsulated command request.
// \return      true          send encapsulated command request processed.
// \return      false         send encapsulated command request not supported or not processed.
bool USBD_CDC0_ACM_SendEncapsulatedCommand (const uint8_t *buf, uint16_t len) {
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
bool USBD_CDC0_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len) {
  (void)max_len;
  (void)buf;
  (void)len;

  return true;
}


// Called upon USB Host request to change communication settings.
// \param[in]   line_coding   pointer to CDC_LINE_CODING structure.
// \return      true          set line coding request processed.
// \return      false         set line coding request not supported or not processed.
bool USBD_CDC0_ACM_SetLineCoding (const CDC_LINE_CODING *line_coding) {

  // Store requested settings to local variable
  cdc_acm_line_coding = *line_coding;

  if (mw_cv_usbd_cdc0_acm.testing_active) {
    if (memcmp((void *)&cdc_acm_line_coding, (void *)&line_coding_115200, sizeof(CDC_LINE_CODING)) == 0U) {
      mw_cv_usbd_cdc0_acm.cb_set_line_coding = true;
    }
  }

  return true;
}


// Called upon USB Host request to retrieve communication settings.
// \param[out]  line_coding   pointer to CDC_LINE_CODING structure.
// \return      true          get line coding request processed.
// \return      false         get line coding request not supported or not processed.
bool USBD_CDC0_ACM_GetLineCoding (CDC_LINE_CODING *line_coding) {

  // Load settings from ones stored on USBD_CDC0_ACM_SetLineCoding callback
  *line_coding = cdc_acm_line_coding;

  if (mw_cv_usbd_cdc0_acm.testing_active) {
    if (memcmp((void *)&cdc_acm_line_coding, (void *)&line_coding_115200, sizeof(CDC_LINE_CODING)) == 0U) {
      mw_cv_usbd_cdc0_acm.cb_get_line_coding = true;
    }
  }

  return true;
}


// Called upon USB Host request to set control line states.
// \param [in]  state         control line settings bitmap.
//                - bit 0: DTR state
//                - bit 1: RTS state
// \return      true          set control line state request processed.
// \return      false         set control line state request not supported or not processed.
bool USBD_CDC0_ACM_SetControlLineState (uint16_t state) {

  if (mw_cv_usbd_cdc0_acm.testing_active) {
    mw_cv_usbd_cdc0_acm.cb_set_control_line_state = true;

    if (mw_cv_usbd_cdc0_acm.echo_active) {      // If echo active for some tests
      USBD_CDC_ACM_Notify(0U, state);
    }
  }

  return true;
}


// Called when new data was received.
// \param [in]  len           number of bytes available for reading.
void USBD_CDC0_ACM_DataReceived (uint32_t len) {
  (void)len;

  if (!mw_cv_usbd_cdc0_acm.testing_active) { return; }

  if (mw_cv_usbd_cdc0_acm.echo_active) {        // If echo active for some tests
    rece_len = USBD_CDC_ACM_ReadData(0U, rece_buf, 64U);
    memcpy(send_buf, rece_buf, rece_len);
    USBD_CDC_ACM_WriteData(0U, send_buf, rece_len);
  }

  mw_cv_usbd_cdc0_acm.cb_data_received = true;
}


// Called when when all data was sent.
void USBD_CDC0_ACM_DataSent (void) {
  if (!mw_cv_usbd_cdc0_acm.testing_active) { return; }
  mw_cv_usbd_cdc0_acm.cb_data_sent = true;
}

#endif

//! [code_USBD_User_CDC_ACM]
