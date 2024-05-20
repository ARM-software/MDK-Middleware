/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_msc.c
 * Purpose: USB Device - Mass Storage device Class (MSC) module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_msc.h"

#include <string.h>

#include "usb_lib_debug.h"
#include "usbd_lib.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_drv.h"
#include "usbd_evr.h"


/************************** Constant definitions ******************************/

/// USB Device HID Class API enumerated constants
#define USBD_MSC_REQUEST_SENSE_NO_ERROR                 0U      ///< No error
#define USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA             1U      ///< Error - Not Ready - medium not present
#define USBD_MSC_REQUEST_SENSE_ERR_WRITE                2U      ///< Error - Medium Error - write fault
#define USBD_MSC_REQUEST_SENSE_ERR_READ                 3U      ///< Error - Medium Error - unrecoverable read error
#define USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD          4U      ///< Error - Illegal Request - invalid/unsupported command code
#define USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB          5U      ///< Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
#define USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM        6U      ///< Error - Illegal Request - invalid parameter value
#define USBD_MSC_REQUEST_SENSE_ERR_WP                   7U      ///< Error - Write Protect - command not allowed


/************************** Local function declarations ***********************/

static usbStatus USBD_MSC_CheckInstance       (uint8_t instance);
static void      USBD_MSC_SetStallEP          (uint8_t instance, uint8_t ep_addr);
static void      USBD_MSC_ClrStallEP          (uint8_t instance, uint8_t ep_addr);
static bool      USBD_MSC_CheckCommandNoData  (uint8_t instance);
static bool      USBD_MSC_CheckCommandDataIn  (uint8_t instance);
static bool      USBD_MSC_CheckCommandDataOut (uint8_t instance);
static void      USBD_MSC_StatusPassed        (uint8_t instance);
static void      USBD_MSC_StatusFailed        (uint8_t instance, uint8_t ep_stall, uint8_t sense);
static void      USBD_MSC_StatusPhaseError    (uint8_t instance, uint8_t ep_stall);
static void      USBD_MSC_DataInTransfer      (uint8_t instance);
static uint32_t  USBD_MSC_CheckMediaHW        (uint8_t instance);
static bool      USBD_MSC_RW_Setup            (uint8_t instance);
static void      USBD_MSC_MemoryRead          (uint8_t instance);
static void      USBD_MSC_MemoryWrite         (uint8_t instance);
static void      USBD_MSC_TestUnitReady       (uint8_t instance);
static void      USBD_MSC_RequestSense        (uint8_t instance);
static void      USBD_MSC_Inquiry             (uint8_t instance);
static void      USBD_MSC_StartStopUnit       (uint8_t instance);
static void      USBD_MSC_MediaRemoval        (uint8_t instance);
static void      USBD_MSC_ModeSense6          (uint8_t instance);
static void      USBD_MSC_ModeSense10         (uint8_t instance);
static void      USBD_MSC_ReadFormatCapacity  (uint8_t instance);
static void      USBD_MSC_ReadCapacity        (uint8_t instance);
static void      USBD_MSC_GetCBW              (uint8_t instance);
static void      USBD_MSC_SetCSW              (uint8_t instance);
static void      USBD_MSC_EpBulkIn            (uint8_t instance);
static void      USBD_MSC_EpBulkOut           (uint8_t instance);


/************************** User accessible functions *************************/
// There are no user accessible functions as all interaction with user is
// happening through callbacks


/************************** Library accessible functions **********************/

/// \brief MSC Mass Storage Bulk-Only Mass Storage Reset request
/// \param[in]     instance      instance of MSC class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_MSC_BulkOnlyMassStorageReset (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        usbd_data_t     *ptr_dev_data;

  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    EvrUSBD_MSC_BOMSResetFailed(instance);
    return false;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_msc_cfg->dev_num]->data_ptr;

  ptr_dev_data->endpoint_no_halt_clr = 0U;      // Allow halt clear
  ptr_msc_data->csw.dSignature       = 0U;      // Invalidate signature

  ptr_msc_data->bulk_stage = MSC_BS_RESET;

  EvrUSBD_MSC_BOMSReset(instance);

  return true;
}

/// \brief MSC Get Max LUN request
/// \param[in]     instance      instance of MSC class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_MSC_GetMaxLUN (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
  const usbd_dev_t      *ptr_dev_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint8_t          max_lun;

  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    EvrUSBD_MSC_GetMaxLUNFailed(instance);
    return false;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_dev_cfg  = usbd_dev_ptr[ptr_msc_cfg->dev_num];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  // Number of configured LUNs (get from callback)
  // if value is 0 use the setting from configuration file
  // otherwise use returned value (decremented by 1)
  max_lun = fpUSBD_MSC_GetMaxLUN[instance]();
  if (max_lun == 0U) {
    max_lun = ptr_msc_cfg->max_lun;
  } else if (max_lun > 4U) {
    max_lun = 3U;
  } else {
    max_lun--;
  }
  ptr_msc_data->max_lun   = max_lun;
  ptr_dev_cfg->ep0_buf[0] = max_lun;

  EvrUSBD_MSC_GetMaxLUN(instance, ptr_msc_data->max_lun);

  return true;
}

/// \brief MSC Endpoint Clear Stall Feature request handling
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_addr       endpoint address.
void USBD_MSC_EndpointClearFeatureStall (uint8_t instance, uint8_t ep_addr) {
  USBD_MSC_ClrStallEP(instance, ep_addr);
}


/************************** Configuration accessible functions ****************/

/// \brief Initialize MSC device instance and create event handling Thread
/// \param[in]     instance      instance of MSC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_MSC_Initialize (uint8_t instance) {
  const  usbd_msc_t      *ptr_msc_cfg;
         usbd_msc_data_t *ptr_msc_data;
  static uint32_t         cache_buf;
  static uint32_t         cache_size;
         usbStatus        status;

  EvrUSBD_MSC_Initialize(instance);

  status = USBD_MSC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  memset(ptr_msc_data, 0, sizeof(usbd_msc_data_t));     // Initialize runtime information

  if (fpUSBD_MSC_Initialize [instance] != NULL) {
    EvrUSBD_MSC_OnInitialize(instance);
    fpUSBD_MSC_Initialize   [instance] ();              // Call MSC initialize callback
  }

  if (ptr_msc_cfg->max_lun <= 3U) {
    ptr_msc_data->max_lun = ptr_msc_cfg->max_lun;       // Initialize max LUN to configured value
  } else {
    ptr_msc_data->max_lun = 3U;                         // Initialize max LUN to 4 LUNs if configured value is invalid
  }

  ptr_msc_data->lun[0].block_size  = 512U;              // Force LUN0 block size of Mass Storage to 512 bytes
  ptr_msc_data->lun[1].block_size  = 512U;              // Force LUN1 block size of Mass Storage to 512 bytes
  ptr_msc_data->lun[2].block_size  = 512U;              // Force LUN2 block size of Mass Storage to 512 bytes
  ptr_msc_data->lun[3].block_size  = 512U;              // Force LUN3 block size of Mass Storage to 512 bytes

  if (fpUSBD_MSC_GetCacheInfo [instance] == NULL) {
    return usbUnknownError;
  }

  if (fpUSBD_MSC_GetCacheInfo [instance] (&cache_buf, &cache_size)) {
    // If get cache information succeeded
    ptr_msc_data->block_buf      = (uint8_t *)cache_buf; // Cache buffer from File System
    ptr_msc_data->block_buf_size = cache_size;           // Size of cache buffer
    EvrUSBD_MSC_OnGetCacheInfo(instance, cache_size);
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  } else {
    EvrUSBD_MSC_OnGetCacheInfoFailed(instance);
#endif
  }

  if (usbd_msc_thread_id[instance] == NULL) {
    usbd_msc_thread_id[instance] = USBD_ThreadCreate (usbdThreadMSC, instance);
    if (usbd_msc_thread_id[instance] == NULL) {
      status = usbThreadError;
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_MSC_InitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief De-initialize MSC device instance and terminate event handling Thread
/// \param[in]     instance      instance of MSC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_MSC_Uninitialize (uint8_t instance) {
  usbd_msc_data_t *ptr_msc_data;
  usbStatus status;

  EvrUSBD_MSC_Uninitialize(instance);

  status = USBD_MSC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc_data = usbd_msc_ptr[instance]->data_ptr;

  // Clear media size
  ptr_msc_data->lun[0].block_count = 0U;
  ptr_msc_data->lun[1].block_count = 0U;
  ptr_msc_data->lun[2].block_count = 0U;
  ptr_msc_data->lun[3].block_count = 0U;

  if (usbd_msc_thread_id[instance] != NULL) {
    if (USBD_ThreadTerminate (usbd_msc_thread_id[instance]) != 0) {
      status = usbThreadError;
      goto exit;
    }
    usbd_msc_thread_id[instance] = NULL;
  }

  if (fpUSBD_MSC_Uninitialize [instance] != NULL) {
    EvrUSBD_MSC_OnUninitialize(instance);
    fpUSBD_MSC_Uninitialize   [instance] ();
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_MSC_UninitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief MSC USB Bus Reset event handling
/// \param[in]     instance      instance of MSC class.
void USBD_MSC_Reset (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        usbStatus        status;

  EvrUSBD_MSC_Reset(instance);

  status = USBD_MSC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_MSC_ResetFailed(instance, status);
    return;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  (void)USBD_DriverEndpointTransferAbort (ptr_msc_cfg->dev_num, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out));
  (void)USBD_DriverEndpointTransferAbort (ptr_msc_cfg->dev_num, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in));

  memset ((void *)&ptr_msc_data->cbw, 0, sizeof(MSC_CBW));
  memset ((void *)&ptr_msc_data->csw, 0, sizeof(MSC_CSW));

  ptr_msc_data->block                = 0U;
  ptr_msc_data->offset               = 0U;
  ptr_msc_data->length               = 0U;
  ptr_msc_data->bulk_len             = 0U;
  ptr_msc_data->bulk_req_len         = 0U;
  ptr_msc_data->mem_ok               = false;
  ptr_msc_data->bulk_stage           = 0U;
  ptr_msc_data->lun[0].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;
  ptr_msc_data->lun[1].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;
  ptr_msc_data->lun[2].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;
  ptr_msc_data->lun[3].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;
}

/// \brief MSC Endpoint Start handling
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_MSC_EndpointStart (uint8_t instance, uint8_t ep_addr) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint16_t         max_packet_size;
        uint8_t          device;

  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_msc_cfg = usbd_msc_ptr[instance];

  if (USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out) == ep_addr) {
    ptr_msc_data    = ptr_msc_cfg->data_ptr;
    device          = ptr_msc_cfg->dev_num;
    max_packet_size = ptr_msc_cfg->max_packet_size[usbd_dev_ptr[device]->data_ptr->high_speed];

    ptr_msc_data->bulk_req_len = max_packet_size;
    (void)USBD_DriverEndpointTransfer (device, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), ptr_msc_cfg->bulk_buf, max_packet_size);
  }
}


/************************** Local functions ***********************************/

/// \brief Check all MSC device instance pointers and variables
/// \param[in]     instance      instance of MSC class.
/// \return                      status code that indicates the execution status of the function.
static usbStatus USBD_MSC_CheckInstance (uint8_t instance) {

  if (instance >= usbd_msc_num)                                        { return usbClassErrorMSC; }
  if (usbd_msc_ptr[instance]                                  == NULL) { return usbClassErrorMSC; }
  if (usbd_msc_ptr[instance]->data_ptr                        == NULL) { return usbClassErrorMSC; }
  if (usbd_msc_ptr[instance]->dev_num >= usbd_dev_num)                 { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_msc_ptr[instance]->dev_num]           == NULL) { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_msc_ptr[instance]->dev_num]->data_ptr == NULL) { return usbDeviceError;   }

  return usbOK;
}

/// \brief MSC Set Stall request for Endpoint
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_addr       endpoint address
///                                - bit 0..3: address
///                                - bit 7: direction
static void USBD_MSC_SetStallEP (uint8_t instance, uint8_t ep_addr) {
  usbd_data_t *ptr_dev_data;
  uint8_t      device;

  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    return;
  }

  device       = usbd_msc_ptr[instance]->dev_num;
  ptr_dev_data = usbd_dev_ptr[device]->data_ptr;

  (void)USBD_DriverEndpointStall (device, ep_addr, true);

  ptr_dev_data->endpoint_halt |= 1UL << ((ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3));
}

/// \brief MSC Clear Stall request for Endpoint
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_addr       endpoint address
///                                - bit 0..3: address
///                                - bit 7: direction
static void USBD_MSC_ClrStallEP (uint8_t instance, uint8_t ep_addr) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint8_t          device, ep_bulk_in;

  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  device       = ptr_msc_cfg->dev_num;
  ep_bulk_in   = USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in);

  if (ep_addr == (uint16_t)ep_bulk_in) {
    ptr_msc_data = ptr_msc_cfg->data_ptr;

    // Compliance Test: rewrite CSW after clear halt
    if (ptr_msc_data->csw.dSignature == MSC_CSW_Signature) {
      (void)USBD_DriverEndpointTransfer (device, USB_ENDPOINT_IN(ep_bulk_in), (uint8_t *)&ptr_msc_data->csw, sizeof(ptr_msc_data->csw));
    }
  }
}

/// \brief MSC Check if Command is specified without Data stage
/// \param[in]     instance      instance of MSC class.
/// \return        true          command correct
/// \return        false         command incorrect
static bool USBD_MSC_CheckCommandNoData (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
  const usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  if (ptr_msc_data->cbw.dDataLength != 0U) {
    if ((ptr_msc_data->cbw.bmFlags & 0x80U) != 0U) {
      USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in),  USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    } else {
      USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    }
    return false;
  }

  return true;
}

/// \brief MSC Check if Command is specified with Data In stage
/// \param[in]     instance      instance of MSC class.
/// \return        true          command correct
/// \return        false         command incorrect
static bool USBD_MSC_CheckCommandDataIn (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
  const usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  if (ptr_msc_data->cbw.dDataLength == 0U) {
    USBD_MSC_StatusFailed (instance,                                         0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return false;
  }
  if ((ptr_msc_data->cbw.bmFlags & 0x80U) == 0U) {
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return false;
  }

  return true;
}

/// \brief MSC Check if Command is specified with Data Out stage
/// \param[in]     instance      instance of MSC class.
/// \return        true          command correct
/// \return        false         command incorrect
static bool USBD_MSC_CheckCommandDataOut (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
  const usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  if (ptr_msc_data->cbw.dDataLength == 0U) {
    USBD_MSC_StatusFailed (instance,                                       0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return false;
  }
  if ((ptr_msc_data->cbw.bmFlags & 0x80U) != 0U) {
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return false;
  }

  return true;
}

/// \brief MSC Status Passed
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_StatusPassed (uint8_t instance) {
  usbd_msc_data_t *ptr_msc_data;
  uint8_t          lun;

  ptr_msc_data = usbd_msc_ptr[instance]->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  ptr_msc_data->lun[lun].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;
  ptr_msc_data->csw.bStatus            = CSW_CMD_PASSED;

  USBD_MSC_SetCSW(instance);
}

/// \brief MSC Status Failed
///        also stalls endpoint if requested and prepares fail status and request sense status
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_stall      endpoint to stall (0 = none).
/// \param[in]     sense         request sense response of failure.
static void USBD_MSC_StatusFailed (uint8_t instance, uint8_t ep_stall, uint8_t sense) {
  usbd_msc_data_t *ptr_msc_data;
  uint8_t          lun;

  ptr_msc_data = usbd_msc_ptr[instance]->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (ep_stall != 0U) {
    USBD_MSC_SetStallEP(instance, ep_stall);
  }

  ptr_msc_data->lun[lun].request_sense  = sense;
  ptr_msc_data->csw.bStatus             = CSW_CMD_FAILED;
  ptr_msc_data->csw.dSignature          = MSC_CSW_Signature;

  if ((ep_stall & 0x80U) != 0U) {       // If IN endpoint was stalled
    ptr_msc_data->bulk_stage   = MSC_BS_CSW;
  } else {
    USBD_MSC_SetCSW(instance);
  }
}

/// \brief MSC Status Phase Error (requires reset recovery form Host)
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_stall      endpoint to stall (0 = none).
static void USBD_MSC_StatusPhaseError (uint8_t instance, uint8_t ep_stall) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        usbd_data_t     *ptr_dev_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_msc_cfg->dev_num]->data_ptr;

  if (ep_stall != 0U) {
    USBD_MSC_SetStallEP(instance, ep_stall);

    // Prevent clear halt on Stalled Endpoint
    ptr_dev_data->endpoint_no_halt_clr |= 1UL << ((ep_stall & 0x0FU) + ((ep_stall & 0x80U) >> 3));
  }

  ptr_msc_data->csw.bStatus    = CSW_PHASE_ERROR;
  ptr_msc_data->csw.dSignature = MSC_CSW_Signature;
  ptr_msc_data->bulk_stage     = MSC_BS_ERROR;
}

/// \brief MSC Perform Data In Transfer
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_DataInTransfer (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  if (ptr_msc_data->bulk_len >= ptr_msc_data->cbw.dDataLength) {
    ptr_msc_data->bulk_len   = ptr_msc_data->cbw.dDataLength;
    ptr_msc_data->bulk_stage = MSC_BS_DATA_IN_LAST;
  } else {
    ptr_msc_data->bulk_stage = MSC_BS_DATA_IN_LAST_STALL;       // Short or ZLP
  }

  (void)USBD_DriverEndpointTransfer (ptr_msc_cfg->dev_num, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), ptr_msc_cfg->bulk_buf, ptr_msc_data->bulk_len);

  ptr_msc_data->csw.dDataResidue -= ptr_msc_data->bulk_len;
  ptr_msc_data->csw.bStatus       = CSW_CMD_PASSED;
}

/// \brief MSC Check Media (Hardware) presence and write protect status
/// \param[in]     instance      instance of MSC class.
/// \return                      media presence and write protected status.
///                bit 1:        write protect bit:
///                - value 1:      media is write protected
///                - value 0:      media is not write protected
///                bit 0:        media presence bit.
///                - value 1:      media is present
///                - value 0:      media is not present
static uint32_t USBD_MSC_CheckMediaHW (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         media_status;
        uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (fpUSBD_MSC_LUN_CheckMedia[instance] != NULL) {
    media_status = fpUSBD_MSC_LUN_CheckMedia[instance] (lun);
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
    if ((media_status & USBD_MSC_MEDIA_READY) != 0U) {
      if ((media_status & USBD_MSC_MEDIA_PROTECTED) != 0U) {
        EvrUSBD_MSC_OnCheckMedia(instance, lun, true,  true);
      } else {
        EvrUSBD_MSC_OnCheckMedia(instance, lun, false, true);
      }
    } else {
      if ((media_status & USBD_MSC_MEDIA_PROTECTED) != 0U) {
        EvrUSBD_MSC_OnCheckMedia(instance, lun, true,  false);
      } else {
        EvrUSBD_MSC_OnCheckMedia(instance, lun, false, false);
      }
    }
#endif
  } else {
    media_status = 0U;
  }

  if ((media_status & USBD_MSC_MEDIA_READY) == 0U) {
    ptr_msc_data->lun[lun].block_count = 0U;
    if (ptr_msc_data->cbw.dDataLength != 0U) {
      if ((ptr_msc_data->cbw.bmFlags & 0x80U) != 0U) {
        USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in),  USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
      } else {
        if (ptr_msc_data->csw.dDataResidue != ptr_msc_data->bulk_len) {
          // If all OUT data was not yet received STALL the bulk OUT endpoint
          USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
        } else {
          // If all OUT data was already received do not STALL the bulk OUT endpoint
          // so new CBW can be received
          USBD_MSC_StatusFailed (instance,                                         0U, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
        }
      }
    } else {
      USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    }
  }

  return media_status;
}

/// \brief MSC SCSI Read/Write Setup Callback
/// \param[in]     instance      instance of MSC class.
/// \return        true          setup succeeded
/// \return        false         setup failed
static bool USBD_MSC_RW_Setup (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         cnt, max_len;
        uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
    switch (ptr_msc_data->cbw.CB[0]) {
      case SCSI_READ10:
      case SCSI_READ12:
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
        break;
      case SCSI_WRITE10:
      case SCSI_WRITE12:
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
        break;
      default:                          // Not read or write command
        break;
    }
#endif
    return false;
  }

  ptr_msc_data->block  = ((uint32_t)ptr_msc_data->cbw.CB[2] << 24) |    // Logical Block Address of First Block
                         ((uint32_t)ptr_msc_data->cbw.CB[3] << 16) |
                         ((uint32_t)ptr_msc_data->cbw.CB[4] <<  8) |
                         ((uint32_t)ptr_msc_data->cbw.CB[5] <<  0);
  ptr_msc_data->offset = 0U;

  /* Number of Blocks to transfer */
  switch (ptr_msc_data->cbw.CB[0]) {
    case SCSI_WRITE10:
      if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_PROTECTED) != 0U) {
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_WP);
        USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_WP);
        return false;
      }
      if (USBD_MSC_CheckCommandDataOut (instance) == false) {
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        return false;
      }
      cnt = ((uint32_t)ptr_msc_data->cbw.CB[7] <<  8) |
            ((uint32_t)ptr_msc_data->cbw.CB[8] <<  0) ;
      EvrUSBD_MSC_Write(instance, lun, ptr_msc_data->block, cnt);
      break;
    case SCSI_WRITE12:
      if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_PROTECTED) != 0U) {
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_WP);
        USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_WP);
        return false;
      }
      if (USBD_MSC_CheckCommandDataOut (instance) == false) {
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        return false;
      }
      cnt = ((uint32_t)ptr_msc_data->cbw.CB[6] << 24) |
            ((uint32_t)ptr_msc_data->cbw.CB[7] << 16) |
            ((uint32_t)ptr_msc_data->cbw.CB[8] <<  8) |
            ((uint32_t)ptr_msc_data->cbw.CB[9] <<  0) ;
      EvrUSBD_MSC_Write(instance, lun, ptr_msc_data->block, cnt);
      break;
    case SCSI_READ10:
      if (USBD_MSC_CheckCommandDataIn (instance) == false) {
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        return false;
      }
      cnt = ((uint32_t)ptr_msc_data->cbw.CB[7] <<  8) |
            ((uint32_t)ptr_msc_data->cbw.CB[8] <<  0) ;
      EvrUSBD_MSC_Read(instance, lun, ptr_msc_data->block, cnt);
      break;
    case SCSI_READ12:
      if (USBD_MSC_CheckCommandDataIn (instance) == false) {
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        return false;
      }
      cnt = ((uint32_t)ptr_msc_data->cbw.CB[6] << 24) |
            ((uint32_t)ptr_msc_data->cbw.CB[7] << 16) |
            ((uint32_t)ptr_msc_data->cbw.CB[8] <<  8) |
            ((uint32_t)ptr_msc_data->cbw.CB[9] <<  0) ;
      EvrUSBD_MSC_Read(instance, lun, ptr_msc_data->block, cnt);
      break;
    default:
      return false;
  }

  ptr_msc_data->length = cnt * ptr_msc_data->lun[lun].block_size;
  max_len = ptr_msc_data->block_buf_size;
  if (ptr_msc_data->length > max_len) {
    ptr_msc_data->bulk_req_len = max_len;
  } else {
    ptr_msc_data->bulk_req_len = ptr_msc_data->length;
  }

  if (ptr_msc_data->cbw.dDataLength == 0U) {            // host requests no data
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
    switch (ptr_msc_data->cbw.CB[0]) {
      case SCSI_READ10:
      case SCSI_READ12:
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        break;
      case SCSI_WRITE10:
      case SCSI_WRITE12:
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        break;
      default:                          // Not read or write command
        break;
    }
#endif
    USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return false;
  }

  if (ptr_msc_data->cbw.dDataLength != ptr_msc_data->length) {
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
    switch (ptr_msc_data->cbw.CB[0]) {
      case SCSI_READ10:
      case SCSI_READ12:
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        break;
      case SCSI_WRITE10:
      case SCSI_WRITE12:
        EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
        break;
      default:                          // Not read or write command
        break;
    }
#endif
    if ((ptr_msc_data->cbw.bmFlags & 0x80U) != 0U) {    // Stall appropriate EP
      USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in),  USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    } else {
      USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    }
    return false;
  }

  return true;
}

/// \brief MSC Memory Read Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_MemoryRead (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         num, cnt;
        uint32_t         max_len;
        uint8_t          device;
        uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  device       = ptr_msc_cfg->dev_num;
  lun          = ptr_msc_data->cbw.bLUN;

  if (ptr_msc_data->block >= ptr_msc_data->lun[lun].block_count) {
    EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    return;
  }
  max_len = ptr_msc_cfg->max_packet_size[usbd_dev_ptr[device]->data_ptr->high_speed];
  if (ptr_msc_data->block_buf_size > max_len) {
    max_len = ptr_msc_data->block_buf_size;
  }
  if (ptr_msc_data->length > max_len) {
    num = max_len;
  } else {
    num = ptr_msc_data->length;
  }

  if ((USBD_MSC_CheckMediaHW(instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  if ((ptr_msc_data->offset == 0U) && (num != 0U)) {
    cnt = (ptr_msc_data->length + (ptr_msc_data->lun[lun].block_size-1U)) / ptr_msc_data->lun[lun].block_size;
    if (cnt > (ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size)) {
      cnt = ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size;
    }
    if (fpUSBD_MSC_LUN_Read [instance] != NULL) {
      EvrUSBD_MSC_OnRead(instance, lun, ptr_msc_data->block, cnt, (const uint8_t *)ptr_msc_data->block_buf);
      if (fpUSBD_MSC_LUN_Read[instance] (lun, ptr_msc_data->block, cnt, ptr_msc_data->block_buf) == false) {
        EvrUSBD_MSC_OnReadFailed(instance, lun, ptr_msc_data->block);
        EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_READ);
        USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_READ);
        return;
      }
    }
  }

  if (num != 0U) {
    (void)USBD_DriverEndpointTransfer (device, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), &ptr_msc_data->block_buf[ptr_msc_data->offset], num);
    ptr_msc_data->offset += num;
    ptr_msc_data->length -= num;
  }

  if (ptr_msc_data->offset == ptr_msc_data->block_buf_size) {
    ptr_msc_data->offset = 0U;
    ptr_msc_data->block += ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size;
  }

  ptr_msc_data->csw.dDataResidue -= num;

  if (num == 0U) { return; }

  if (ptr_msc_data->length == 0U) {
    EvrUSBD_MSC_ReadDone(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
    ptr_msc_data->bulk_stage  = MSC_BS_DATA_IN_LAST;
    ptr_msc_data->csw.bStatus = CSW_CMD_PASSED;
  }
}

/// \brief MSC Memory Write Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_MemoryWrite (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         cnt, max_len;
        uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if ((ptr_msc_data->block >= ptr_msc_data->lun[lun].block_count) ||
     (((ptr_msc_data->offset & (ptr_msc_data->lun[lun].block_size-1U)) + ptr_msc_data->bulk_len) > ptr_msc_data->block_buf_size)) {
    ptr_msc_data->bulk_len = 0U;
    EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    return;
  }

  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    ptr_msc_data->bulk_len = 0U;
    EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  ptr_msc_data->offset += ptr_msc_data->bulk_len;
  ptr_msc_data->length -= ptr_msc_data->bulk_len;

  if (ptr_msc_data->bulk_len != 0U) {
    if ((ptr_msc_data->length == 0U) && (ptr_msc_data->offset != 0U)) {
      cnt = (ptr_msc_data->offset + (ptr_msc_data->lun[lun].block_size-1U)) / ptr_msc_data->lun[lun].block_size;
      if (cnt > (ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size)) {
        cnt = ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size;
      }
      if (fpUSBD_MSC_LUN_Write [instance] != NULL) {
        EvrUSBD_MSC_OnWrite(instance, lun, ptr_msc_data->block, cnt, (const uint8_t *)ptr_msc_data->block_buf);
        if (fpUSBD_MSC_LUN_Write[instance] (lun, ptr_msc_data->block, cnt, ptr_msc_data->block_buf) == false) {
          EvrUSBD_MSC_OnWriteFailed(instance, lun, ptr_msc_data->block);
          EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_WRITE);
          USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_WRITE);
          return;
        }
      }
      ptr_msc_data->offset = 0U;
      ptr_msc_data->block += cnt;
    } else {
      if (ptr_msc_data->offset == ptr_msc_data->block_buf_size) {
        if (fpUSBD_MSC_LUN_Write [instance] != NULL) {
          EvrUSBD_MSC_OnWrite(instance, lun, ptr_msc_data->block, ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size, (const uint8_t *)ptr_msc_data->block_buf);
          if (fpUSBD_MSC_LUN_Write[instance] (lun, ptr_msc_data->block, ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size, ptr_msc_data->block_buf) == false) {
            EvrUSBD_MSC_OnWriteFailed(instance, lun, ptr_msc_data->block);
            EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_WRITE);
            USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_WRITE);
            return;
          }
        }
        ptr_msc_data->offset = 0U;
        ptr_msc_data->block += ptr_msc_data->block_buf_size / ptr_msc_data->lun[lun].block_size;
      }
    }
  }

  ptr_msc_data->csw.dDataResidue -= ptr_msc_data->bulk_len;

  if (ptr_msc_data->length != 0U) {
    max_len = ptr_msc_data->block_buf_size;
    if (ptr_msc_data->length > max_len) {
      ptr_msc_data->bulk_req_len = max_len;
    } else {
      ptr_msc_data->bulk_req_len = ptr_msc_data->length;
    }
    (void)USBD_DriverEndpointTransfer (ptr_msc_cfg->dev_num, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), &ptr_msc_data->block_buf[ptr_msc_data->offset], ptr_msc_data->bulk_req_len);
  }

  if ((ptr_msc_data->length == 0U) || (ptr_msc_data->bulk_stage == MSC_BS_CSW)) {
    EvrUSBD_MSC_WriteDone(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
    USBD_MSC_StatusPassed (instance);
  }
}

/// \brief MSC SCSI Test Unit Ready Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_TestUnitReady (uint8_t instance) {
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  uint8_t lun;

  lun = usbd_msc_ptr[instance]->data_ptr->cbw.bLUN;
#endif

  if (USBD_MSC_CheckCommandNoData (instance) == false) {
    EvrUSBD_MSC_TestUnitReady(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    return;
  }

  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_TestUnitReady(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  EvrUSBD_MSC_TestUnitReady(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);

  USBD_MSC_StatusPassed (instance);
}

/// \brief MSC SCSI Request Sense Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_RequestSense (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        uint32_t         status;
#endif
        uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_RequestSense(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }

  memset((void *)ptr_msc_cfg->bulk_buf, 0, 20U);        // Clear 20 bytes in buffer

  ptr_msc_cfg->bulk_buf[ 0] = 0x70U;            // Response Code
//ptr_msc_cfg->bulk_buf[ 1] = 0x00U;
  switch (ptr_msc_data->lun[lun].request_sense) {
    case USBD_MSC_REQUEST_SENSE_NO_ERROR:
//    ptr_msc_cfg->bulk_buf[ 2] = 0x00U;        // NO SENSE
//    ptr_msc_cfg->bulk_buf[12] = 0x00U;        // Additional Sense Code: No additional code
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_NO_ERROR;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA:
      ptr_msc_cfg->bulk_buf[ 2] = 0x02U;        // NOT READY
      ptr_msc_cfg->bulk_buf[12] = 0x3AU;        // Additional Sense Code: Medium not present
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_WRITE:
      ptr_msc_cfg->bulk_buf[ 2] = 0x03U;        // MEDUIM ERROR
      ptr_msc_cfg->bulk_buf[12] = 0x03U;        // Additional Sense Code: Write fault
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_WRITE;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_READ:
      ptr_msc_cfg->bulk_buf[ 2] = 0x03U;        // MEDUIM ERROR
      ptr_msc_cfg->bulk_buf[12] = 0x11U;        // Additional Sense Code: Unrecovered read error
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_READ;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB:
      ptr_msc_cfg->bulk_buf[ 2] = 0x05U;        // ILLEGAL REQUEST
      ptr_msc_cfg->bulk_buf[12] = 0x24U;        // Additional Sense Code: Invalid field in CDB
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM:
      ptr_msc_cfg->bulk_buf[ 2] = 0x05U;        // ILLEGAL REQUEST
      ptr_msc_cfg->bulk_buf[12] = 0x26U;        // Additional Sense Code: Invalid parm value
      ptr_msc_cfg->bulk_buf[13] = 0x02U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_WP:
      ptr_msc_cfg->bulk_buf[ 2] = 0x07U;        // WRITE PROTECT
      ptr_msc_cfg->bulk_buf[12] = 0x27U;        // Additional Sense Code: Write protect
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_WP;
#endif
      break;
    case USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD:
    default:
      ptr_msc_cfg->bulk_buf[ 2] = 0x05U;        // ILLEGAL REQUEST
      ptr_msc_cfg->bulk_buf[12] = 0x20U;        // Additional Sense Code: Invalid command
//    ptr_msc_cfg->bulk_buf[13] = 0x00U;        // Additional Sense Code Qualifier
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
      status = USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD;
#endif
      break;
  }
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  (void)status;
#endif
  EvrUSBD_MSC_RequestSense(instance, lun, status);

  // Reset sense to no error after host reads current
  ptr_msc_data->lun[lun].request_sense = USBD_MSC_REQUEST_SENSE_NO_ERROR;

  ptr_msc_cfg->bulk_buf[ 7] = 10U;              // Additional Length

  ptr_msc_data->bulk_len = 18U;

  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC SCSI Inquiry Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_Inquiry (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         i, len;
        uint8_t          device, lun;
  const uint8_t         *ptr_str;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_Inquiry(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }

  if ((ptr_msc_data->cbw.CB[1] & 1U) != 0U){// Vital Product Data request (EVPD == 1)
    switch (ptr_msc_data->cbw.CB[2]) {      // Switch on Page Code
      case 0x00:                            // Supported VPD Pages VPD Page
        ptr_msc_cfg->bulk_buf[ 0]  = 0x00U; // Direct Access Device
        ptr_msc_cfg->bulk_buf[ 1]  = 0x00U; // Page Code
        ptr_msc_cfg->bulk_buf[ 2]  = 0x00U; // Reserved
        ptr_msc_cfg->bulk_buf[ 3]  =    2U; // Page Length
        ptr_msc_cfg->bulk_buf[ 4]  = 0x00U; // Supported VPD Page: 0x00
        ptr_msc_cfg->bulk_buf[ 5]  = 0x80U; // Supported VPD Page: 0x80
        ptr_msc_data->bulk_len = 6U;        // Total number of data bytes to send
        break;
      case 0x80:                            // Unit Serial Number VPD Page
        device = ptr_msc_cfg->dev_num;

                                            // Currently we use Serial Number String configured for USB Device
                                            // or if Serial Number String is not configured 20 ASCII SPACE characters
        ptr_msc_cfg->bulk_buf[ 0]  = 0x00U; // Direct Access Device
        ptr_msc_cfg->bulk_buf[ 1]  = 0x80U; // Page Code
        ptr_msc_cfg->bulk_buf[ 2]  = 0x00U; // Reserved
        ptr_msc_cfg->bulk_buf[ 3]  =   21U; // Page Length
                                            // Extract Serial Number from String Descriptor
        len = usbd_desc_ptr[device]->device_descriptor[16];     // Length of Serial Number String
        if (len != 0U) {                    // If Serial Number String is Enabled
                                            // Find Serial Number String in String Descriptor array
          ptr_str = (const uint8_t *)usbd_desc_ptr[device]->string_descriptor;
          for (i = 0U; i < len; i++) {
            ptr_str += ((USB_STRING_DESCRIPTOR *)((uint32_t)ptr_str))->bLength;
          }
          if (((USB_STRING_DESCRIPTOR *)((uint32_t)ptr_str))->bLength > 4U) {
            len = (((uint32_t)((const USB_STRING_DESCRIPTOR *)ptr_str)->bLength) - 2U) / 2U;
          } else {
            len = 0U;
          }
                                            // ptr_str points to Serial Number String
          if (len > 20U) { len = 20U; }     // Limit maximum string size to 20 ASCII characters
          ptr_str += 2U;                    // Skip bLength and bDescriptorType
          for (i = 0U; i < len; i++) {      // 0 .. Serial Number String Size (max 20 bytes)
            if (*ptr_str != 0U) {           // Serial Number String character
              ptr_msc_cfg->bulk_buf[i+4U] = *ptr_str;
              ptr_str += 2U;                // skip empty 0 character, because string descriptor UNICODE characters (2 bytes)
            }
          }
          if (i < 20U) {                    // Fill rest of string with ASCII SPACE character (' ')
            memset((void *)&ptr_msc_cfg->bulk_buf[i+4U], (int32_t)' ', 20U-i);
          }
        } else {                            // If Serial Number String is not Enabled return 20 ASCII SPACEs (' ')
          memset((void *)&ptr_msc_cfg->bulk_buf[4], (int32_t)' ', 20U);
        }
        ptr_msc_data->bulk_len = 24U;       // Total number of data bytes to send
        break;
//    case 0x83:                            // Device Identification VPD Page
//                                          // Prepared Inquiry for Vital Product Data Page Code Device Identification VPD Page
//                                          // for implementation of this page missing are:
//                                          //   - add 0x83 to Supported VPD Pages VPD Page,
//                                          //     and change Supported VPD Pages VPD Page Page Length accordingly (+1)
//                                          //   - acquire T10 Vendor ID
//                                          //   - acquire 24-bit IEEE Company ID
//                                          //   - assign unique serial number to this device
//                                          //   - incorporate 24-bit IEEE Company ID, 24-bit IEEE Company ID
//                                          //     and unique serial number into this Page
//      ptr_msc_cfg->bulk_buf[ 0]  = 0x00U; // Direct Access Device
//      ptr_msc_cfg->bulk_buf[ 1]  = 0x83U; // Page Code
//      ptr_msc_cfg->bulk_buf[ 2]  = 0U;    // Page Length (MSB)
//      ptr_msc_cfg->bulk_buf[ 3]  = 57U-3U;// Page Length (LSB)
//                                          // 1st Identifier
//      ptr_msc_cfg->bulk_buf[ 4]  = 0x02U; // Code Set (IDENTIFIER contains ASCII graphic codes)
//      ptr_msc_cfg->bulk_buf[ 5]  = 0x01U; // IDENTIFIER Type (IDENTIFIER field are a Vendor ID)
//      ptr_msc_cfg->bulk_buf[ 6]  = 0x00U; // Reserved
//      ptr_msc_cfg->bulk_buf[ 7]  = 38U;   // IDENTIFIER Length
//      memcpy(&ptr_msc_cfg->bulk_buf[ 8], "T10_VID_",              8U);    // T10 Vendor ID
//      memcpy(&ptr_msc_cfg->bulk_buf[16], "Product_ID",           10U);    // Product Identifier
//      memcpy(&ptr_msc_cfg->bulk_buf[26], "Product_SN_123456789", 20U);    // Product Serial Number
//      ptr_msc_cfg->bulk_buf[46]  = 0x06U; // Code Set (IDENTIFIER contains binary values)
//      ptr_msc_cfg->bulk_buf[47]  = 0x02U; // IDENTIFIER Type (IEEE Extended Unique Identifier (EUI-64))
//      ptr_msc_cfg->bulk_buf[48]  = 0x00U; // Reserved
//      ptr_msc_cfg->bulk_buf[49]  = 8U;    // IDENTIFIER Length
//      ptr_msc_cfg->bulk_buf[50]  = 0x01U; // IEEE Extended Unique Identifier (EUI-64)
//      ptr_msc_cfg->bulk_buf[51]  = 0x23U;
//      ptr_msc_cfg->bulk_buf[52]  = 0x45U;
//      ptr_msc_cfg->bulk_buf[53]  = 0xFFU;
//      ptr_msc_cfg->bulk_buf[54]  = 0xFFU;
//      ptr_msc_cfg->bulk_buf[55]  = 0xABU;
//      ptr_msc_cfg->bulk_buf[56]  = 0xCDU;
//      ptr_msc_cfg->bulk_buf[57]  = 0xEFU;
//      ptr_msc_data->bulk_len = 58U;       // Total number of data bytes to send
//      break;
      default:                              // Other Page Codes are not supported -> STALL Bulk In Endpoint
        EvrUSBD_MSC_Inquiry(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
        USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
        return;
    }
  } else {
    if (ptr_msc_data->cbw.CB[2] == 0U) {    // Standard Inquiry Data (Page Code == 0)
      ptr_msc_cfg->bulk_buf[ 0]  = 0x00U;   // Direct Access Device
      ptr_msc_cfg->bulk_buf[ 1]  = 0x80U;   // RMB = 1: Removable Medium
      ptr_msc_cfg->bulk_buf[ 2]  = 0x00U;   // Version: device does not claim conformance to any standard
      ptr_msc_cfg->bulk_buf[ 3]  = 0x02U;   // Response Data Format
      ptr_msc_cfg->bulk_buf[ 4]  =   31U;   // Additional Length
      ptr_msc_cfg->bulk_buf[ 5]  = 0x00U;
      ptr_msc_cfg->bulk_buf[ 6]  = 0x00U;
      ptr_msc_cfg->bulk_buf[ 7]  = 0x00U;
      ptr_str = (uint8_t *)ptr_msc_cfg->inquiry_data[lun];
      for (i = 8U; i < 36U; i++) {          // Product Information    +
        if (*ptr_str != 0U) {               // Product Revision Level
          ptr_msc_cfg->bulk_buf[i] = *ptr_str;
          ptr_str++;
        } else {
          ptr_msc_cfg->bulk_buf[i] = (uint8_t)' ';
        }
      }
      ptr_msc_data->bulk_len = 36U;         // Total number of data bytes to send
    } else {                                // Requested Page Code different than 0 with
                                            // EPVD == 0 (illegal request) -> STALL Bulk In Endpoint
      EvrUSBD_MSC_Inquiry(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
      USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
      return;
    }
  }

  EvrUSBD_MSC_Inquiry(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC SCSI Start Stop/Unit Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_StartStopUnit (uint8_t instance) {
  const usbd_msc_data_t *ptr_msc_data;
        uint8_t          lun;

  ptr_msc_data = usbd_msc_ptr[instance]->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (USBD_MSC_CheckCommandNoData (instance) == false) {
    EvrUSBD_MSC_StartStopUnit(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    return;
  }

  if (ptr_msc_data->cbw.CB[3] == 0U) {          // If power condition modifier is 0
    if (fpUSBD_MSC_LUN_StartStop[instance] != NULL) {
      if ((ptr_msc_data->cbw.CB[4] & 1U) != 0U) {
        (void)fpUSBD_MSC_LUN_StartStop[instance] (lun, true);   // Media START
      } else {
        (void)fpUSBD_MSC_LUN_StartStop[instance] (lun, false);  // Media STOP
      }
                                                // Start Stop Unit -> pass
      EvrUSBD_MSC_StartStopUnit(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
      USBD_MSC_StatusPassed (instance);
    } else {
      EvrUSBD_MSC_StartStopUnit(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
      USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
    }
  } else {
                                                // Start Stop Unit -> fail
    EvrUSBD_MSC_StartStopUnit(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
  }
}

/// \brief MSC SCSI Media Removal Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_MediaRemoval (uint8_t instance) {
  const usbd_msc_data_t *ptr_msc_data;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        uint8_t          lun;
#endif

  ptr_msc_data = usbd_msc_ptr[instance]->data_ptr;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  lun          = ptr_msc_data->cbw.bLUN;
#endif

  if (USBD_MSC_CheckCommandNoData (instance) == false) {
    EvrUSBD_MSC_MediaRemoval(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    return;
  }

  if ((ptr_msc_data->cbw.CB[4] & 1U) != 0U) {   // If prevent
                                                // Prevent media removal -> fail
    EvrUSBD_MSC_MediaRemoval(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
    USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_PARAM);
  } else {                                      // If allow */
                                                // Allow media removal -> pass
    EvrUSBD_MSC_MediaRemoval(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
    USBD_MSC_StatusPassed (instance);
  }
}

/// \brief MSC SCSI Mode Sense (6-Byte) Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_ModeSense6 (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        uint8_t          lun;
#endif

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  lun          = ptr_msc_data->cbw.bLUN;
#endif

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }
  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  ptr_msc_cfg->bulk_buf[ 0] = 0x03U;
  ptr_msc_cfg->bulk_buf[ 1] = 0x00U;
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_PROTECTED) != 0U) {
    ptr_msc_cfg->bulk_buf[ 2] = 1U << 7;
  } else {
    ptr_msc_cfg->bulk_buf[ 2] = 0x00U;
  }
  ptr_msc_cfg->bulk_buf[ 3] = 0x00U;

  ptr_msc_data->bulk_len = 4U;

  EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC SCSI SCSI Mode Sense (10-Byte) Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_ModeSense10 (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        uint8_t          lun;
#endif

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  lun          = ptr_msc_data->cbw.bLUN;
#endif

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }
  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  memset((void *)ptr_msc_cfg->bulk_buf, 0, 8U);         // Clear 8 bytes in buffer

//ptr_msc_cfg->bulk_buf[ 0] = 0x00U;
  ptr_msc_cfg->bulk_buf[ 1] = 0x06U;
//ptr_msc_cfg->bulk_buf[ 2] = 0x00U;
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_PROTECTED) != 0U) {
    ptr_msc_cfg->bulk_buf[ 3] = 1U << 7;
  }
//ptr_msc_cfg->bulk_buf[ 4] = 0x00U;
//ptr_msc_cfg->bulk_buf[ 5] = 0x00U;
//ptr_msc_cfg->bulk_buf[ 6] = 0x00U;
//ptr_msc_cfg->bulk_buf[ 7] = 0x00U;

  ptr_msc_data->bulk_len = 8U;

  EvrUSBD_MSC_ModeSense(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC SCSI Read Format Capacity Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_ReadFormatCapacity (uint8_t instance) {
  const  usbd_msc_t      *ptr_msc_cfg;
         usbd_msc_data_t *ptr_msc_data;
  static uint32_t         block_count;
  static uint32_t         block_size;
         uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_ReadFormatCapacities(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }
  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_ReadFormatCapacities(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  // If callback function is invalid
  if (fpUSBD_MSC_LUN_GetMediaCapacity [instance] == NULL) {
    EvrUSBD_MSC_ReadFormatCapacities(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
    return;
  }

  if (fpUSBD_MSC_LUN_GetMediaCapacity [instance] (lun, &block_count, &block_size)) {
    // If get media capacity succeeded
    ptr_msc_data->lun[lun].block_size  = block_size;    // Block size on media
    ptr_msc_data->lun[lun].block_count = block_count;   // Total number of blocks on media
    EvrUSBD_MSC_OnGetMediaCapacity(instance, lun, block_count, block_size);
  } else {
    // If get media capacity failed
    EvrUSBD_MSC_OnGetMediaCapacityFailed(instance, lun);
    EvrUSBD_MSC_ReadFormatCapacities(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_READ);
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_READ);
    return;
  }

  ptr_msc_cfg->bulk_buf[ 0] = 0x00;
  ptr_msc_cfg->bulk_buf[ 1] = 0x00;
  ptr_msc_cfg->bulk_buf[ 2] = 0x00;
  ptr_msc_cfg->bulk_buf[ 3] = 0x08;             // Capacity List Length

  // Block Count
  ptr_msc_cfg->bulk_buf[ 4] = (uint8_t)((ptr_msc_data->lun[lun].block_count >> 24) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 5] = (uint8_t)((ptr_msc_data->lun[lun].block_count >> 16) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 6] = (uint8_t)((ptr_msc_data->lun[lun].block_count >>  8) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 7] = (uint8_t)((ptr_msc_data->lun[lun].block_count >>  0) & 0xFFU);

  // Block Length
  ptr_msc_cfg->bulk_buf[ 8] = 0x02U;            // Descriptor Code: Formatted Media
  ptr_msc_cfg->bulk_buf[ 9] = (uint8_t)((ptr_msc_data->lun[lun].block_size  >> 16) & 0xFFU);
  ptr_msc_cfg->bulk_buf[10] = (uint8_t)((ptr_msc_data->lun[lun].block_size  >>  8) & 0xFFU);
  ptr_msc_cfg->bulk_buf[11] = (uint8_t)((ptr_msc_data->lun[lun].block_size  >>  0) & 0xFFU);

  ptr_msc_data->bulk_len = 12U;

  EvrUSBD_MSC_ReadFormatCapacities(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC SCSI Read Capacity Callback
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_ReadCapacity (uint8_t instance) {
  const  usbd_msc_t      *ptr_msc_cfg;
         usbd_msc_data_t *ptr_msc_data;
  static uint32_t         block_count;
  static uint32_t         block_size;
         uint8_t          lun;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  lun          = ptr_msc_data->cbw.bLUN;

  if (USBD_MSC_CheckCommandDataIn (instance) == false) {
    EvrUSBD_MSC_ReadCapacity(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    return;
  }
  // If media is not present USBD_MSC_CheckMediaHW will prepare and send CSW
  if ((USBD_MSC_CheckMediaHW (instance) & USBD_MSC_MEDIA_READY) == 0U) {
    EvrUSBD_MSC_ReadCapacity(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_NO_MEDIA);
    return;
  }

  // If callback function is invalid
  if (fpUSBD_MSC_LUN_GetMediaCapacity [instance] == NULL) {
    EvrUSBD_MSC_ReadCapacity(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
    return;
  }

  if (fpUSBD_MSC_LUN_GetMediaCapacity [instance] (lun, &block_count, &block_size)) {
    // If get media capacity succeeded
    ptr_msc_data->lun[lun].block_size  = block_size;    // Block size on media
    ptr_msc_data->lun[lun].block_count = block_count;   // Total number of blocks on media
    EvrUSBD_MSC_OnGetMediaCapacity(instance, lun, block_count, block_size);
  } else {
    // If get media capacity failed
    EvrUSBD_MSC_ReadCapacity(instance, lun, USBD_MSC_REQUEST_SENSE_ERR_READ);
    USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), USBD_MSC_REQUEST_SENSE_ERR_READ);
    return;
  }

  // Last Logical Block
  ptr_msc_cfg->bulk_buf[ 0] = (uint8_t)(((ptr_msc_data->lun[lun].block_count - 1U) >> 24) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 1] = (uint8_t)(((ptr_msc_data->lun[lun].block_count - 1U) >> 16) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 2] = (uint8_t)(((ptr_msc_data->lun[lun].block_count - 1U) >>  8) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 3] = (uint8_t)(((ptr_msc_data->lun[lun].block_count - 1U) >>  0) & 0xFFU);

  // Block Length
  ptr_msc_cfg->bulk_buf[ 4] = (uint8_t)((ptr_msc_data->lun[lun].block_size         >> 24) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 5] = (uint8_t)((ptr_msc_data->lun[lun].block_size         >> 16) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 6] = (uint8_t)((ptr_msc_data->lun[lun].block_size         >>  8) & 0xFFU);
  ptr_msc_cfg->bulk_buf[ 7] = (uint8_t)((ptr_msc_data->lun[lun].block_size         >>  0) & 0xFFU);

  ptr_msc_data->bulk_len = 8U;

  EvrUSBD_MSC_ReadCapacity(instance, lun, USBD_MSC_REQUEST_SENSE_NO_ERROR);
  USBD_MSC_DataInTransfer (instance);
}

/// \brief MSC get command block wrapper
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_GetCBW (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         num;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  if (ptr_msc_data->bulk_len != sizeof(ptr_msc_data->cbw)) {
    // If CBW is of invalid size (!= 31 byte) then handle as invalid CBW
    goto invalid_cbw;
  }

  memcpy (&ptr_msc_data->cbw, ptr_msc_cfg->bulk_buf, ptr_msc_data->bulk_len);
  if (ptr_msc_data->cbw.dSignature == MSC_CBW_Signature) {
    // Valid CBW
    ptr_msc_data->csw.dTag = ptr_msc_data->cbw.dTag;
    ptr_msc_data->csw.dDataResidue = ptr_msc_data->cbw.dDataLength;
    if ((ptr_msc_data->cbw.bLUN      >  ptr_msc_data->max_lun) ||
        (ptr_msc_data->cbw.bCBLength <  1U                   ) ||
        (ptr_msc_data->cbw.bCBLength > 16U                   )) {
      USBD_MSC_StatusFailed (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CDB);
    } else {
      switch (ptr_msc_data->cbw.CB[0]) {
        case SCSI_TEST_UNIT_READY:
          USBD_MSC_TestUnitReady (instance);
          break;
        case SCSI_REQUEST_SENSE:
          USBD_MSC_RequestSense (instance);
          break;
        case SCSI_INQUIRY:
          USBD_MSC_Inquiry (instance);
          break;
        case SCSI_START_STOP_UNIT:
          USBD_MSC_StartStopUnit (instance);
          break;
        case SCSI_MEDIA_REMOVAL:
          USBD_MSC_MediaRemoval (instance);
          break;
        case SCSI_MODE_SENSE6:
          USBD_MSC_ModeSense6 (instance);
          break;
        case SCSI_MODE_SENSE10:
          USBD_MSC_ModeSense10 (instance);
          break;
        case SCSI_READ_FORMAT_CAPACITIES:
          USBD_MSC_ReadFormatCapacity (instance);
          break;
        case SCSI_READ_CAPACITY:
          USBD_MSC_ReadCapacity (instance);
          break;
        case SCSI_READ10:
        case SCSI_READ12:
          if (USBD_MSC_RW_Setup (instance) == true)  {
            ptr_msc_data->bulk_stage = MSC_BS_DATA_IN;
            USBD_MSC_MemoryRead (instance);
          }
          break;
        case SCSI_WRITE10:
        case SCSI_WRITE12:
          if (USBD_MSC_RW_Setup (instance) == true)  {
            ptr_msc_data->bulk_stage = MSC_BS_DATA_OUT;
            num = ptr_msc_data->bulk_req_len;
            if (num > ptr_msc_data->block_buf_size) {
              num = ptr_msc_data->block_buf_size;
            }
            (void)USBD_DriverEndpointTransfer (ptr_msc_cfg->dev_num, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), ptr_msc_data->block_buf, num);
          }
          break;
        case SCSI_SYNC_CACHE10:
        case SCSI_SYNC_CACHE16:
          // Synchronize check always passes as we always write data directly
          // so cache is always synchronized
          EvrUSBD_MSC_SyncCache(instance, ptr_msc_data->cbw.bLUN, USBD_MSC_REQUEST_SENSE_NO_ERROR);
          USBD_MSC_StatusPassed (instance);
          break;
        //Not supported commands
        case SCSI_VERIFY10:     // Verify command has been made unsupported as
                                // it's implementation is optional (SPC-4, SBC-3) 
                                // and Command Verifier, Windows and Linux don't use it
        case SCSI_REPORT_ID_INFO:
        case SCSI_FORMAT_UNIT:
        case SCSI_MODE_SELECT6:
        case SCSI_MODE_SELECT10:
        case SCSI_ATA_COMMAND_PASS_THROUGH12:
        case SCSI_SERVICE_ACTION_IN16:
        default:
          EvrUSBD_MSC_UnsupportedCommand(instance, ptr_msc_data->cbw.CB[0]);
          if (ptr_msc_data->cbw.dDataLength != 0U) {
            if ((ptr_msc_data->cbw.bmFlags & 0x80U) != 0U) {
              USBD_MSC_StatusFailed (instance, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in),  USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
            } else {
              USBD_MSC_StatusFailed (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
            }
          } else {
            USBD_MSC_StatusFailed   (instance, 0U, USBD_MSC_REQUEST_SENSE_ERR_ILLEGAL_CMD);
          }
          break;
      }
    }
  } else {
    goto invalid_cbw;
  }
  return;

invalid_cbw:
    // Invalid CBW
    USBD_MSC_StatusPhaseError (instance, USB_ENDPOINT_IN (ptr_msc_cfg->ep_bulk_in));
    USBD_MSC_StatusPhaseError (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out));
}

/// \brief MSC set command status wrapper
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_SetCSW (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  ptr_msc_data->csw.dSignature = MSC_CSW_Signature;
  (void)USBD_DriverEndpointTransfer (ptr_msc_cfg->dev_num, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in), (uint8_t *)&ptr_msc_data->csw, sizeof(ptr_msc_data->csw));
  ptr_msc_data->bulk_stage = MSC_BS_CSW;
}

/// \brief MSC Bulk In Endpoint event handling
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_EpBulkIn (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  switch (ptr_msc_data->bulk_stage) {
    case MSC_BS_DATA_IN:
      switch (ptr_msc_data->cbw.CB[0]) {
        case SCSI_READ10:
        case SCSI_READ12:
          USBD_MSC_MemoryRead(instance);
          break;
        default:                        // Unsupported command
          break;
      }
      break;
    case MSC_BS_DATA_IN_LAST:
      USBD_MSC_SetCSW(instance);
      break;
    case MSC_BS_DATA_IN_LAST_STALL:
      USBD_MSC_SetStallEP(instance, USB_ENDPOINT_IN(ptr_msc_cfg->ep_bulk_in));
      ptr_msc_data->csw.dSignature = MSC_CSW_Signature;
      ptr_msc_data->bulk_stage     = MSC_BS_CSW;
      break;
    case MSC_BS_CSW:
      ptr_msc_data->bulk_req_len = ptr_msc_cfg->max_packet_size[usbd_dev_ptr[ptr_msc_cfg->dev_num]->data_ptr->high_speed];
      (void)USBD_DriverEndpointTransfer (ptr_msc_cfg->dev_num, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out), ptr_msc_cfg->bulk_buf, ptr_msc_data->bulk_req_len);
      ptr_msc_data->bulk_stage   = MSC_BS_CBW;
      break;
    default:                            // Undefined bulk stage
      break;
  }
}

/// \brief MSC Bulk Out Endpoint event handling
/// \param[in]     instance      instance of MSC class.
static void USBD_MSC_EpBulkOut (uint8_t instance) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;

  switch (ptr_msc_data->bulk_stage) {
    case MSC_BS_CBW:
      USBD_MSC_GetCBW(instance);
      break;
    case MSC_BS_DATA_OUT:
      switch (ptr_msc_data->cbw.CB[0]) {
        case SCSI_WRITE10:
        case SCSI_WRITE12:
          USBD_MSC_MemoryWrite (instance);
          break;
        default:                        // Unsupported command
          break;
      }
      break;
    case MSC_BS_CSW:
      break;
    case MSC_BS_RESET:
      // If Bulk-Only Mass Storage Reset command was received on
      // Control Endpoint ignore next bulk transfer if it was not
      // a CBW (as it means it was a unprocessed leftover from
      // transfer before reset happened)
      ptr_msc_data->bulk_stage   = MSC_BS_CBW;
      if (ptr_msc_data->bulk_len == sizeof(ptr_msc_data->cbw)) {
        // If it is a CBW size packet go process it as CBW
        USBD_MSC_GetCBW(instance);
      }
      break;
    default:
      USBD_MSC_StatusPhaseError (instance, USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out));
      break;
  }
}


/************************** Thread functions **********************************/

/// \brief Thread Function: MSC Bulk Endpoint events handling
/// \param[in]     arg                  index of MSC instance
void USBD_MSC_Thread (void *arg) {
  const usbd_msc_t      *ptr_msc_cfg;
        usbd_msc_data_t *ptr_msc_data;
        uint32_t         event;
        uint8_t          device, ep_bulk_out;
        uint8_t          instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_MSC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_msc_cfg  = usbd_msc_ptr[instance];
  ptr_msc_data = ptr_msc_cfg->data_ptr;
  device       = ptr_msc_cfg->dev_num;
  ep_bulk_out  = USB_ENDPOINT_OUT(ptr_msc_cfg->ep_bulk_out);

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      if (((event >> 8) & ARM_USBD_EVENT_OUT) != 0U) {
        ptr_msc_data->bulk_len = USBD_DriverEndpointTransferGetResult (device, ep_bulk_out);
        USBD_MSC_EpBulkOut(instance);
      }
      if (((event >> 8) & ARM_USBD_EVENT_IN)  != 0U) {
        USBD_MSC_EpBulkIn (instance);
      }
    }
  }
}

void USBD_MSC0_Thread (void const *arg) { (void)arg; USBD_MSC_Thread ((void *)0U); }
void USBD_MSC1_Thread (void const *arg) { (void)arg; USBD_MSC_Thread ((void *)1U); }
void USBD_MSC2_Thread (void const *arg) { (void)arg; USBD_MSC_Thread ((void *)2U); }
void USBD_MSC3_Thread (void const *arg) { (void)arg; USBD_MSC_Thread ((void *)3U); }
