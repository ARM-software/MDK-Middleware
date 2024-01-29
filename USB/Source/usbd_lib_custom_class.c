/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_custom_class.c
 * Purpose: USB Device - Custom Class module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_custom_class.h"

#include "usb_lib_rte.h"
#include "usbd_lib_config_def.h"
#include "usbd_evr.h"


/************************** Local function declarations ***********************/

static usbStatus USBD_CustomClass_CheckInstance (uint8_t instance);


/************************** User accessible functions *************************/
// There are no user accessible functions as all interaction with user is
// happening through callbacks


/************************** Configuration accessible functions ****************/

/// \brief Initialize Custom Class device instance and create Endpoint handling Thread(s)
/// \param[in]     instance      instance of custom class.
/// \return                      status code that indicates the execution status of the function.
__WEAK usbStatus USBD_CustomClass_Initialize (uint8_t instance) {
  uint32_t  ep_msk;
  usbStatus status;
  uint8_t   index;

  status = USBD_CustomClass_CheckInstance(instance);
  if (status != usbOK) {
    return status;
  }

  if (fpUSBD_CustomClass_Initialize[instance] != NULL) {
    EvrUSBD_CC_OnInitialize(instance);
    fpUSBD_CustomClass_Initialize[instance] ();
  }

  ep_msk = usbd_custom_class_ptr[instance]->ep_msk;

  for (index = 1U; index < usbd_ep_max_num; index ++) {
    if ((usbd_ep_thread_id_ptr[(instance * usbd_ep_max_num) + index] != NULL) && ((ep_msk & (1UL << index)) != 0U)) {
      if (usbd_custom_class_ep_thread_id[instance][index] == NULL) {
        usbd_custom_class_ep_thread_id[instance][index] = USBD_ThreadCreate (usbdThreadCC, (uint8_t)(instance * usbd_ep_max_num) + index);
        if (usbd_custom_class_ep_thread_id[instance][index] == NULL) {
          return usbThreadError;
        }
      }
    }
  }

  return usbOK;
}

/// \brief Uninitialize Custom Class device instance and terminate Endpoint handling thread(s)
/// \param[in]     instance      instance of custom class.
/// \return                      status code that indicates the execution status of the function.
__WEAK usbStatus USBD_CustomClass_Uninitialize (uint8_t instance) {
  uint32_t  ep_msk;
  usbStatus status;
  uint8_t   index;

  status = USBD_CustomClass_CheckInstance(instance);
  if (status != usbOK) {
    return status;
  }

  ep_msk = usbd_custom_class_ptr[instance]->ep_msk;

  for (index = 1U; index < usbd_ep_max_num; index ++) {
    if ((usbd_ep_thread_id_ptr[(instance * usbd_ep_max_num) + index] != NULL) && ((ep_msk & (1UL << index)) != 0U)) {
      if (usbd_custom_class_ep_thread_id[instance][index] != NULL) {
        if (USBD_ThreadTerminate (usbd_custom_class_ep_thread_id[instance][index]) != 0) {
          return usbThreadError;
        }
        usbd_custom_class_ep_thread_id[instance][index] = NULL;
      }
    }
  }

  if (fpUSBD_CustomClass_Uninitialize[instance] != NULL) {
    EvrUSBD_CC_OnUninitialize(instance);
    fpUSBD_CustomClass_Uninitialize[instance] ();
  }

  return usbOK;
}

/// \brief Custom Class USB Bus Reset event handling
/// \param[in]     instance      instance of CDC class.
__WEAK void USBD_CustomClass_Reset (uint8_t instance) {

  if (USBD_CustomClass_CheckInstance(instance) != usbOK) {
    return;
  }

  if (fpUSBD_CustomClass_Reset[instance] != NULL) {
    EvrUSBD_CC_OnReset(instance);
    fpUSBD_CustomClass_Reset[instance] ();
  }
}

/// \brief Custom Class Endpoint Start handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
__WEAK void USBD_CustomClass_EndpointStart (uint8_t instance, uint8_t ep_addr) {

  if (USBD_CustomClass_CheckInstance(instance) != usbOK) {
    return;
  }

  if (fpUSBD_CustomClass_EndpointStart[instance] != NULL) {
    EvrUSBD_CC_OnEndpointStart(instance, ep_addr);
    fpUSBD_CustomClass_EndpointStart[instance] (ep_addr);
  }
}

/// \brief Custom Class Endpoint Stop handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
__WEAK void USBD_CustomClass_EndpointStop (uint8_t instance, uint8_t ep_addr) {

  if (USBD_CustomClass_CheckInstance(instance) != usbOK) {
    return;
  }

  if (fpUSBD_CustomClass_EndpointStop[instance] != NULL) {
    EvrUSBD_CC_OnEndpointStop(instance, ep_addr);
    fpUSBD_CustomClass_EndpointStop[instance] (ep_addr);
  }
}


/************************** Local functions ***********************************/

/// \brief Check all Custom Class device instance pointers and variables
/// \param[in]     instance      instance of Custom Class class.
/// \return                      status code that indicates the execution status of the function.
static usbStatus USBD_CustomClass_CheckInstance (uint8_t instance) {

  if (instance >= usbd_custom_class_num)                                        { return usbClassErrorCustom; }
  if (usbd_custom_class_ptr[instance]->dev_num >= usbd_dev_num)                 { return usbDeviceError;      }
  if (usbd_dev_ptr[usbd_custom_class_ptr[instance]->dev_num]           == NULL) { return usbDeviceError;      }
  if (usbd_dev_ptr[usbd_custom_class_ptr[instance]->dev_num]->data_ptr == NULL) { return usbDeviceError;      }

  return usbOK;
}


/************************** Thread functions **********************************/

/// \brief Thread Function: Custom Class Endpoints Event handling
/// \param[in]     instance      instance of Custom Class class.
void USBD_CustomClass_EP_Thread (void *arg) {
  uint32_t event;
  uint8_t  ep_num, ep_evt, idx;
  uint8_t  instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_CustomClass_CheckInstance(instance) != usbOK) {
    return;
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      // (event >> 12) & 0x0F = endpoint number
      // (event >>  8) & 0x0F = endpoint event
      ep_num = (uint8_t)((event >> 12) & 0x0FU);
      ep_evt = (uint8_t)((event >>  8) & 0x0FU);
      idx = (uint8_t)(instance * usbd_ep_max_num) + ep_num;
      if (fpUSBD_Endpoint_Event[idx] != NULL) {
        EvrUSBD_CC_OnEndpointmEvent(instance, ep_num, ep_evt);
        fpUSBD_Endpoint_Event[idx](ep_evt);
      }
    }
  }
}
