/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_msc.c
 * Purpose: USB Host - Mass Storage device Class (MSC) module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_msc.h"

#include <string.h>

#include "rl_usbh_lib.h"
#include "rl_fs.h"
#include "rl_fs_lib.h"
#include "usb_lib_rte.h"
#include "usbh_lib_config_def.h"
#include "usbh_lib_core.h"
#include "usbh_evr.h"


/************************** Local function declarations ***********************/

static   usbStatus CheckInstance                      (uint8_t instance);
//static usbStatus USBH_MSC_ClearStall                (uint8_t instance, uint8_t ep_addr);
//static usbStatus USBH_MSC_BulkOnlyMassStorageReset  (uint8_t instance);
static   usbStatus USBH_MSC_GetMaxLUN                 (uint8_t instance, uint8_t *ptr_max_lun);
static   usbStatus USBH_MSC_SCSI_TestUnitReady        (uint8_t instance, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_SCSI_RequestSense         (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_SCSI_Inquiry              (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat);
//static usbStatus USBH_MSC_SCSI_ReadFormatCapacities (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_SCSI_ReadCapacity         (uint8_t instance, uint32_t lba, uint8_t *ptr_data, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_SCSI_Read10               (uint8_t instance, uint32_t block_addr, uint32_t block_num, uint8_t *ptr_data, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_SCSI_Write10              (uint8_t instance, uint32_t block_addr, uint32_t block_num, const uint8_t *ptr_data, uint32_t *ptr_stat);
static   usbStatus USBH_MSC_Recover                   (uint8_t instance);


/************************** User accessible functions *************************/

/// \brief Callback function called when Mass Storage Device was enumerated and is ready
/// \param[in]     instance             instance of MSC Device.
/// \return                             none.
__WEAK void USBH_MSC_Initialize (uint8_t instance) { (void)instance; }

/// \brief Callback function called when Mass Storage Device was disconnected
/// \param[in]     instance             instance of MSC Device.
/// \return                             none.
__WEAK void USBH_MSC_Uninitialize (uint8_t instance) { (void)instance; }

/// \brief Get Device instance of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
uint8_t USBH_MSC_GetDevice (uint8_t instance) {
  uint8_t device;

  if (CheckInstance (instance) == usbOK) {
    if (usbh_msc[instance].ptr_dev == usbh_dev) {
      return 0U;
    } else {
      device = (uint8_t)(((uint32_t)usbh_msc[instance].ptr_dev - (uint32_t)usbh_dev) / sizeof (USBH_DEV));
    }
  } else {
    device = 0xFFU;
  }

  EvrUSBH_MSC_GetDevice(instance, device);
  return device;
}

/// \brief Get status of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MSC_GetDeviceStatus (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        usbStatus status;

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_dev = usbh_msc[instance].ptr_dev;

    if ((ptr_dev->state.configured == 0U) || (ptr_dev->state.initialized == 0U)) {
      status = usbDeviceError;
    }
  }

  EvrUSBH_MSC_GetStatus(instance, status);
  return status;
}

/// \brief Read requested number of blocks from Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[in]     lba                  logical block address of first block to read.
/// \param[in]     cnt                  number of contiguous blocks to read.
/// \param[out]    buf                  data buffer in which to read data.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MSC_Read (uint8_t instance, uint32_t lba, uint32_t cnt, uint8_t *buf) {
  uint32_t  stat;
  uint8_t   retry;
  usbStatus status;

  EvrUSBH_MSC_Read(instance, lba, cnt);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (buf != NULL) {
      for (retry = 0U; retry < 3U; retry ++) {
        status = USBH_MSC_SCSI_Read10       (instance, lba, cnt, buf, &stat);
        if ((status == usbOK) && (stat == 0U)) {
          break;
        }
        (void)USBH_Delay(1000U);
        status = CheckInstance (instance);
        if (status != usbOK) {          // If device got disconnected
          status = usbDeviceError;
          break;
        }
        if (status == usbTransferError) {
          status = USBH_RecoverDevice       (usbh_msc[instance].ptr_dev);
          if (status != usbOK) {
            break;
          }
          status = USBH_MSC_Recover         (instance);
          if (status != usbOK) {
            break;
          }
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ReadFailed(instance, lba, cnt, status);
  } else {
    EvrUSBH_MSC_ReadDone(instance, lba, cnt);
  }
#endif
  return status;
}

/// \brief Write requested number of blocks to Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[in]     lba                  logical address of first block to write.
/// \param[in]     cnt                  number of contiguous blocks to write.
/// \param[in]     buf                  data buffer containing data to write.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MSC_Write (uint8_t instance, uint32_t lba, uint32_t cnt, const uint8_t *buf) {
  uint32_t  stat;
  uint8_t   retry;
  usbStatus status;

  EvrUSBH_MSC_Write(instance, lba, cnt);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (buf != NULL) {
      for (retry = 0U; retry < 3U; retry ++) {
        status = USBH_MSC_SCSI_Write10      (instance, lba, cnt, buf, &stat);
        if ((status == usbOK) && (stat == 0U)) {
          break;
        }
        (void)USBH_Delay(1000U);
        status = CheckInstance (instance);
        if (status != usbOK) {          // If device got disconnected
          status = usbDeviceError;
          break;
        }
        if (status == usbTransferError) {
          status = USBH_RecoverDevice       (usbh_msc[instance].ptr_dev);
          if (status != usbOK) {
            break;
          }
          status = USBH_MSC_Recover         (instance);
          if (status != usbOK) {
            break;
          }
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_WriteFailed(instance, lba, cnt, status);
  } else {
    EvrUSBH_MSC_WriteDone(instance, lba, cnt);
  }
#endif
  return status;
}

/// \brief Read capacity of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[out]    block_count          pointer to where total number of blocks available will be read.
/// \param[out]    block_size           pointer to where block size will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MSC_ReadCapacity (uint8_t instance, uint32_t *block_count, uint32_t *block_size) {
  const USBH_MSC *ptr_msc;
        usbStatus status;

  EvrUSBH_MSC_ReadCapacity(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if ((block_count != NULL) && (block_size != NULL)) {
      ptr_msc = &usbh_msc[instance];

      *block_count = ptr_msc->block_count;
      *block_size  = ptr_msc->block_size;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ReadCapacityFailed(instance, status);
  } else {
    EvrUSBH_MSC_ReadCapacityDone(instance, *block_count, *block_size);
  }
#endif
  return status;
}


/************************** Library accessible functions **********************/

/// \brief Analyze device configuration and configure resources
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured MSC instance or configuration failed
///                                       - value <= 127 : index of configured MSC instance
///                                       - value 255 :    configuration failed
__WEAK uint8_t USBH_MSC_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) {
        USBH_DEV                 *ptr_dev;
        USBH_MSC                 *ptr_msc;
  const USB_INTERFACE_DESCRIPTOR *ptr_if_desc;
  const USB_ENDPOINT_DESCRIPTOR  *ptr_ep_desc;
  const uint8_t                  *ptr_desc;
        USBH_PIPE_HANDLE          pipe_hndl;
        uint8_t                   num;
        uint8_t                   ret, idx;
        usbStatus                 status;

  status = usbOK;
  ret    = 255U;

  if (device == 255U) {
    goto exit;
  }
  if (ptr_dev_desc == NULL) {
    goto exit;
  }
  if (ptr_cfg_desc == NULL) {
    goto exit;
  }

  ptr_dev = &usbh_dev[device];

  // Find first free mass storage instance block
  for (idx = 0U; idx < usbh_msc_num; idx++) {
    if (usbh_msc[idx].ptr_dev == NULL) {
      break;                            // Free block found
    }
  }
  if (idx == usbh_msc_num) {
    goto exit;                          // Free block not found
  }

  EvrUSBH_MSC_Configure(idx);

  ptr_msc = &usbh_msc[idx];
  memset ((void *)ptr_msc, 0, sizeof (USBH_MSC));

  ptr_msc->ptr_dev = (USBH_DEV *)ptr_dev;

  ptr_desc    = (const uint8_t *)ptr_cfg_desc;
  ptr_desc   += ptr_cfg_desc->bLength;
  ptr_if_desc = (const USB_INTERFACE_DESCRIPTOR *)ptr_desc;
  num = ptr_if_desc->bNumEndpoints;     // Number of endpoints

  switch (ptr_if_desc->bInterfaceClass) {
    case USB_DEVICE_CLASS_STORAGE:
      switch (ptr_if_desc->bInterfaceSubClass) {
        case MSC_SUBCLASS_SCSI:         // SCSI transparent command set
          if (ptr_if_desc->bInterfaceProtocol == MSC_PROTOCOL_BULK_ONLY) {
            // Create Pipes
            ptr_desc    = (const uint8_t *)ptr_if_desc;
            ptr_desc   += ptr_if_desc->bLength;
            ptr_ep_desc = (const USB_ENDPOINT_DESCRIPTOR *)ptr_desc;
            while (num-- != 0U) {
              if ((ptr_ep_desc->bmAttributes & 3U) == USB_ENDPOINT_TYPE_BULK) { // Bulk Endpoint
                pipe_hndl = USBH_PipeCreate (device, ptr_ep_desc->bEndpointAddress, ptr_ep_desc->bmAttributes & USB_ENDPOINT_TYPE_MASK, ptr_ep_desc->wMaxPacketSize & 0x7FFU, ptr_ep_desc->bInterval);
                if (pipe_hndl == 0U) {
                  // If creation of pipe has failed delete previously created pipes
                  if (ptr_msc->bulk_in_pipe_hndl  != 0U) {
                    status = USBH_PipeDelete (ptr_msc->bulk_in_pipe_hndl);
                    if (status != usbOK) {
                      goto exit;
                    }
                  }
                  if (ptr_msc->bulk_out_pipe_hndl != 0U) {
                    status = USBH_PipeDelete (ptr_msc->bulk_out_pipe_hndl);
                    if (status != usbOK) {
                      goto exit;
                    }
                  }
                  memset ((void *)ptr_msc, 0, sizeof (USBH_MSC));
                  goto exit;
                }
                if ((ptr_ep_desc->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) != 0U) {
                  ptr_msc->bulk_in_pipe_hndl  = pipe_hndl;
                } else {
                  ptr_msc->bulk_out_pipe_hndl = pipe_hndl;
                }
              }
              ptr_ep_desc++;
            }
          }
          break;
        default:                        // Unhandled interface sub class
          break;
      }
      break;
    default:                            // Unhandled interface class
      break;
  }

  if ((ptr_msc->bulk_in_pipe_hndl  != 0U) &&
      (ptr_msc->bulk_out_pipe_hndl != 0U)) {
    ret = idx;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ConfigureFailed(status);
  }
#endif
  return ret;
}

/// \brief De-configure resources
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_MSC_Unconfigure (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        USBH_MSC *ptr_msc;
        usbStatus status, first_err_status;

  EvrUSBH_MSC_Unconfigure(instance);

  first_err_status = CheckInstance (instance);
  if (first_err_status == usbOK) {
    ptr_msc = &usbh_msc[instance];
    ptr_dev =  ptr_msc->ptr_dev;

    if (ptr_dev->state.configured != 0U) {
      if (ptr_msc->bulk_out_pipe_hndl != 0U) {
        status = USBH_PipeAbort  (ptr_msc->bulk_out_pipe_hndl);
        if (status != usbOK) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_msc->bulk_out_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      if (ptr_msc->bulk_in_pipe_hndl  != 0U) { 
        status = USBH_PipeAbort  (ptr_msc->bulk_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_msc->bulk_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
    }
    if (first_err_status == usbOK) {
      memset ((void *)ptr_msc, 0, sizeof (USBH_MSC));
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (first_err_status != usbOK) {
    EvrUSBH_MSC_UnconfigureFailed(instance, first_err_status);
  }
#endif
  return first_err_status;
}

/// \brief Initialize Mass Storage Class device instance
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_MSC_Initialize_Lib (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        USBH_MSC *ptr_msc;
        uint8_t  *data;
        uint32_t  stat;
        uint8_t   count;
        uint8_t   ctrl;
        usbStatus status;
        usbStatus mstatus;

  EvrUSBH_MSC_Initialize(instance);

  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc = &usbh_msc[instance];
  ptr_dev =  ptr_msc->ptr_dev;
  ctrl    =  ptr_dev->ctrl;

  // Get Max LUN
  status  = USBH_MSC_GetMaxLUN            (instance, &ptr_msc->max_lun);
  if ((status != usbOK) && (status != usbTransferStall)) {
    goto exit;
  }

  for (count = 0U; count < 5U; count ++) {
    // 5 * Test Unit Ready
    status  = USBH_MSC_SCSI_TestUnitReady (instance, &stat);
    if (status != usbOK) {
      goto exit;
    }
    if (stat != 0U) {
      // Request Sense if Test Unit Ready failed
      (void)USBH_Delay (1250U);
      mstatus = USBH_MemoryAllocate       (ctrl,    (uint8_t **)&data, 18U);
      if (mstatus != usbOK) {
        status = mstatus;
        goto exit;
      }
      status  = USBH_MSC_SCSI_RequestSense(instance,(uint8_t  *) data, &stat);
      mstatus = USBH_MemoryFree           (ctrl,    (uint8_t  *) data);
      if ((status == usbOK) && (mstatus != usbOK)) {
        status = mstatus;
      }
      if (status != usbOK) {
        goto exit;
      }
    }
  }

  // Inquiry
  mstatus = USBH_MemoryAllocate           (ctrl,    (uint8_t **)&data, 36U);
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status  = USBH_MSC_SCSI_Inquiry         (instance,(uint8_t  *) data, &stat);
  mstatus = USBH_MemoryFree               (ctrl,    (uint8_t  *) data);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Test Unit Ready
  status  = USBH_MSC_SCSI_TestUnitReady   (instance, &stat);
  if (status != usbOK) {
    goto exit;
  }
  if (stat != 0U) {
    // Request Sense if Test Unit Ready failed
    (void)USBH_Delay (1000U);
    mstatus = USBH_MemoryAllocate         (ctrl,    (uint8_t **)&data, 18U);
    if (mstatus != usbOK) {
      status = mstatus;
      goto exit;
    }
    status  = USBH_MSC_SCSI_RequestSense  (instance,(uint8_t  *) data, &stat);
    mstatus = USBH_MemoryFree             (ctrl,    (uint8_t  *) data);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
    if (status != usbOK) {
      goto exit;
    }
  }

  for (count = 0U; count < 2U; count ++) {
    // Read Capacity
    mstatus = USBH_MemoryAllocate         (ctrl,    (uint8_t **)&data, 8U);
    if (mstatus != usbOK) {
      status = mstatus;
      goto exit;
    }
    status  = USBH_MSC_SCSI_ReadCapacity  (instance, 0U, (uint8_t *)data, &stat);
    if ((status == usbOK) && (stat == 0U)) {
      ptr_msc->block_count = U32_BE(__UNALIGNED_UINT32_READ(data));
      ptr_msc->block_size  = U32_BE(__UNALIGNED_UINT32_READ(data+4U));
    }
    mstatus = USBH_MemoryFree             (ctrl,    (uint8_t  *) data);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
    if (status != usbOK) {
      goto exit;
    }

    // Test Unit Ready
    status  = USBH_MSC_SCSI_TestUnitReady (instance, &stat);
    if (status != usbOK) {
      goto exit;
    }
    if (stat != 0U) {
      // Request Sense if Test Unit Ready failed
      (void)USBH_Delay (1000U);
      mstatus = USBH_MemoryAllocate       (ctrl,    (uint8_t **)&data, 18U);
      if (mstatus != usbOK) {
        status = mstatus;
        goto exit;
      }
      status  = USBH_MSC_SCSI_RequestSense(instance,(uint8_t  *) data, &stat);
      mstatus = USBH_MemoryFree           (ctrl,    (uint8_t  *) data);
      if ((status == usbOK) && (mstatus != usbOK)) {
        status = mstatus;
      }
      if (status != usbOK) {
        goto exit;
      }
    }

    // Read10 added as some sticks do not fail during enumeration but fail
    // during first Read10 command and at that point it is too late as stick
    // is already mounted, so this is a dummy read just to enable that if
    // first Read10 fails that USB is reset and enumeration is retried again
    mstatus = USBH_MemoryAllocate         (ctrl,    (uint8_t **)&data, 512U);
    if (mstatus != usbOK) {
      status = mstatus;
      goto exit;
    }
    status  = USBH_MSC_SCSI_Read10        (instance, 0U, 1U, (uint8_t *)data, &stat);
    mstatus = USBH_MemoryFree             (ctrl,    (uint8_t  *) data);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
    if (status != usbOK) {
      goto exit;
    }

    // Test Unit Ready
    status  = USBH_MSC_SCSI_TestUnitReady (instance, &stat);
    if (status != usbOK) {
      goto exit;
    }
    if (stat != 0U) {
      // Request Sense if Test Unit Ready failed
      (void)USBH_Delay (1000U);
      mstatus = USBH_MemoryAllocate       (ctrl,    (uint8_t **)&data, 18U);
      if (mstatus != usbOK) {
        status = mstatus;
        goto exit;
      }
      status  = USBH_MSC_SCSI_RequestSense(instance,(uint8_t  *) data, &stat);
      mstatus = USBH_MemoryFree           (ctrl,    (uint8_t  *) data);
      if ((status == usbOK) && (mstatus != usbOK)) {
        status = mstatus;
      }
      if (status != usbOK) {
        goto exit;
      }
    }
  }

  EvrUSBH_MSC_OnInitialize(instance);
  USBH_MSC_Initialize (instance);       // User level callback

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_InitializeFailed(instance, status);
  }
#endif
  return status;
}

/// \brief De-initialize Mass Storage Class device instance
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_MSC_Uninitialize_Lib (uint8_t instance) {

  EvrUSBH_MSC_Uninitialize(instance);
  EvrUSBH_MSC_OnUninitialize(instance);
  USBH_MSC_Uninitialize (instance);     // User level callback

  return usbOK;
}

/************************** File System accessible functions ******************/

/// \brief File System retarget to initialize Mass Storage Device hardware and media
/// \param[in]     instance             instance of MSC device.
/// \param[in]     mode                 hardware (DM_IO) or media (DM_MEDIA).
/// \return        true                 initialization succeeded.
/// \return        false                initialization failed.
bool FS_USBH_MSC_Initialize (uint8_t instance, uint32_t mode) {

  if (mode == DM_IO) {
    return true;
  }

  if (CheckInstance (instance) == usbOK) {
    if (mode == DM_MEDIA) {
      if (USBH_MSC_GetDeviceStatus (instance) == usbOK) {
        usbh_msc[instance].media_ok = true;
        return true;
      }
    }
  }

  return false;
}

/// \brief File System retarget to de-initialize Mass Storage Device hardware and media
/// \param[in]     instance             instance of MSC device.
/// \param[in]     mode                 hardware (DM_IO) or media (DM_MEDIA).
/// \return        true                 de-initialization succeeded.
/// \return        false                de-initialization failed.
bool FS_USBH_MSC_Uninitialize (uint8_t instance, uint32_t mode) {
  uint32_t stat;

  (void)USBH_Delay(200U);               // Allow disconnect to be processed by engine first

  if (CheckInstance (instance) == usbOK) {
    if (USBH_MSC_SCSI_TestUnitReady (instance, &stat) == usbOK) {
      if (mode == DM_IO) {
        return true;
      }
      if (mode == DM_MEDIA) {
        return true;
      }
    }
  }

  return false;
}

/// \brief File System retarget to read single/multiple sectors from Mass Storage Device
/// \param[in]     instance             instance of MSC device.
/// \param[in]     sector_addr          address of first sector that read will start from.
/// \param[in]     sector_num           number of sector to be read.
/// \param[out]    buf                  pointer to where data will be read.
/// \return        true                 read succeeded.
/// \return        false                read failed.
bool FS_USBH_MSC_ReadSectors (uint8_t instance, uint32_t sector_addr, uint32_t sector_num, uint8_t *buf) {

  if (CheckInstance (instance) == usbOK) {
    if (buf != NULL) {
      if (USBH_MSC_Read (instance, sector_addr, sector_num, buf) == usbOK) {
        return true;
      }
    }
  }

  return false;
}

/// \brief File System retarget to write single/multiple sectors to Mass Storage Device
/// \param[in]     instance             instance of MSC device.
/// \param[in]     sector_addr          address of first sector that will be written.
/// \param[in]     sector_num           number of sector to be written.
/// \param[in]     buf                  pointer to data which will be written.
/// \return        true                 write succeeded.
/// \return        false                write failed.
bool FS_USBH_MSC_WriteSectors (uint8_t instance, uint32_t sector_addr, uint32_t sector_num, const uint8_t *buf) {

  if (CheckInstance (instance) == usbOK) {
    if (buf != NULL) {
      if (USBH_MSC_Write (instance, sector_addr, sector_num, buf) == usbOK) {
        return true;
      }
    }
  }

  return false;
}

/// \brief File System retarget to read Mass Storage Device configuration
/// \param[in]     instance             instance of MSC device.
/// \param[out]    info                 pointer to fsMediaInfo structure.
/// \return        true                 read info succeeded.
/// \return        false                read info failed.
bool FS_USBH_MSC_ReadInfo (uint8_t instance, fsMediaInfo *info) {
  uint32_t blen;

  if (CheckInstance (instance) == usbOK) {
    if (info != NULL) {
      if (USBH_MSC_ReadCapacity (instance, &info->block_cnt, &blen) == usbOK) {
        // Mass Storage Device configuration was read
        info->write_blen = (uint16_t)blen;
        info->read_blen  = (uint16_t)blen;
        return true;
      }
    }
  }

  return false;
}

/// \brief File File System retarget to Device Control system call
/// \param[in]     instance             instance of MSC device.
/// \param[in]     code                 device control code (fsDevCtrlCodeCheckMedia).
/// \param[in]     p                    device control return information.
/// \return                             device control call function result
fsStatus FS_USBH_MSC_DeviceCtrl (uint8_t instance, fsDevCtrlCode code, void *p) {
  // Device Control system call

  if (CheckInstance (instance) == usbOK) {
    if (code == fsDevCtrlCodeCheckMedia) {      // If Check Media control code
      if (usbh_msc[instance].media_ok == false) {
        // Allow to initialize the media first
        if (p != NULL) {
          *(uint32_t *)p = FS_MEDIA_INSERTED;
        }
        return fsOK;
      }
      if (USBH_MSC_GetDeviceStatus (instance) == usbOK) {
        // Check if media (USB Memory) is ready
        if (p != NULL) {
          *(uint32_t *)p = FS_MEDIA_INSERTED;
        }
        return fsOK;
      }
      if (p != NULL) {
        *(uint32_t *)p = 0U;
      }
    } else {
      return fsUnsupported;
    }
  }

  return fsError;
}


/************************** Local functions ***********************************/

/// \brief Check device instance pointers and variables
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckInstance (uint8_t instance) {

  if (instance >= usbh_msc_num)                        { return usbClassErrorMSC;   }
  if (usbh_msc[instance].ptr_dev == NULL)              { return usbDeviceError;     }
  if (usbh_msc[instance].ptr_dev->state.in_use == 0U)  { return usbDeviceError;     }
  if (usbh_msc[instance].ptr_dev->ctrl >= usbh_hc_num) { return usbControllerError; }

  return usbOK;
}


/***-------------- Class Specific Requests on Default Pipe -----------------***/

#if 0
/// \brief Clear STALL Condition on an Endpoint (Standard Device Request - CLEAR_FEATURE)
/// \param[in]     instance             index of MSC instance.
/// \param[in]     ep_addr              endpoint address (with direction).
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_ClearStall (uint8_t instance, uint8_t ep_addr) {
  usbStatus status;

  (void)instance;

  status = CheckInstance (instance);
  if (status == usbOK) {
    status = USBH_DeviceRequest_ClearFeature (usbh_msc[instance].ptr_dev->ctrl, USB_REQUEST_TO_ENDPOINT, ep_addr, USB_FEATURE_ENDPOINT_STALL);
  }

  return status;
}

/// \brief Bulk-Only Mass Storage Reset
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_BulkOnlyMassStorageReset (uint8_t instance) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_MSC_BomReset(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    device = USBH_MSC_GetDevice (instance);
    if (device != 0xFFU) {
      PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, 0xFFU, 0U, 0U, 0U)
      status = USBH_ControlTransfer (device, &setup_packet, NULL, 0U);
    } else {
      status = usbDeviceError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_BomResetFailed(instance, status);
  }
#endif
  return status;
}
#endif

/// \brief Get Max LUN Device Request
/// \param[in]     instance             index of MSC instance.
/// \param[out]    ptr_max_lun          pointer to where Maximum number of LUN will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_GetMaxLUN (uint8_t instance, uint8_t *ptr_max_lun) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_MSC_GetMaxLun(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (ptr_max_lun != NULL) {
      device = USBH_MSC_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, 0xFEU, 0U, 0U, 1U)
        status = USBH_ControlTransfer (device, &setup_packet, ptr_max_lun, 1U);
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_GetMaxLunFailed(instance, status);
  }
#endif
  return status;
}

/***-------------- Class Specific Requests on Bulk Pipes -------------------***/

/***----------------------- SCSI Commands ----------------------------------***/

/// \brief USB Mass Storage Class - SCSI - Test Unit Ready
/// \param[in]     instance             index of MSC instance.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_TestUnitReady (uint8_t instance, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiTestUnitReady(instance);

  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, 0U, 0U, 0U, 6U, SCSI_TEST_UNIT_READY, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
//  Some USB Flash disks return wrong data residue, so we ignore data residue
//  if (U32_LE(ptr_csw->dDataResidue) != 0U) {
//    status = usbClassErrorMSC;
//    goto mem_free_and_exit;
//  }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiTestUnitReadyFailed(instance, status);
  }
#endif
  return status;
}

/// \brief USB Mass Storage Class - SCSI - Request Sense
/// \param[in]     instance             index of MSC instance.
/// \param[out]    ptr_data             pointer to where sense data will be read.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_RequestSense (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiRequestSense(instance);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, 18U, (1U << 7), 0U, 6U, SCSI_REQUEST_SENSE, 0U, 0U, 0U, 18U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive Command Data
  (void)USBH_Delay (1200U);
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,          ptr_data, 18);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
//  Some USB Flash disks (non-compliant cheap ones) return wrong data residue, so we ignore data residue
//  if (U32_LE(ptr_csw->dDataResidue) != 0U) {
//    status = usbClassErrorMSC;
//    goto mem_free_and_exit;
//  }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiRequestSenseFailed(instance, status);
  }
#endif
  return status;
}

/// \brief USB Mass Storage Class - SCSI - Inquiry
/// \param[in]     instance             index of MSC instance.
/// \param[out]    ptr_data             pointer to where inquiry data will be read.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_Inquiry (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiInquiry(instance);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, 36U, (1U << 7), 0U, 6U, SCSI_INQUIRY, 0U, 0U, 0U, 36U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive Command Data
  (void)USBH_Delay (120U);
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,          ptr_data, 36U);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
//  Some USB Flash disks return wrong data residue, so we ignore data residue
//  if (U32_LE(ptr_csw->dDataResidue) != 0U) {
//    status = usbClassErrorMSC;
//    goto mem_free_and_exit;
//  }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiInquiryFailed(instance, status);
  }
#endif
  return status;
}

#if 0
/// \brief USB Mass Storage Class - SCSI - Read Format Capacities
/// \param[in]     instance             index of MSC instance.
/// \param[out]    ptr_data             pointer to where format capacities data will be read.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_ReadFormatCapacities (uint8_t instance, uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiReadFormatCapacities(instance);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, 8U, 1U << 7, 0U, 10U, SCSI_READ_FORMAT_CAPACITIES, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 8U, 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive Command Data
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,          ptr_data, 8U);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dDataResidue) != 0U) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiReadFormatCapacitiesFailed(instance, status);
  }
#endif
  return status;
}
#endif

/// \brief USB Mass Storage Class - SCSI - Read Capacity
/// \param[in]     instance             index of MSC instance.
/// \param[in]     lba                  logical block address.
/// \param[out]    ptr_data             pointer to where format capacity data will be read.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_ReadCapacity (uint8_t instance, uint32_t lba, uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiReadCapacity(instance);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, 8U, (1U << 7), 0U, 10U, SCSI_READ_CAPACITY, 0U, (uint8_t)(lba >> 24), (uint8_t)(lba >> 16), (uint8_t)(lba >> 8), (uint8_t)(lba), 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive Command Data
  (void)USBH_Delay (250U);
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,          ptr_data, 8);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
//  Some USB Flash disks (XPLORE USB 3.0 32GB) return wrong data residue, so we ignore data residue
//  if (U32_LE(ptr_csw->dDataResidue) != 0U) {
//    status = usbClassErrorMSC;
//    goto mem_free_and_exit;
//  }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiReadCapacityFailed(instance, status);
  }
#endif
  return status;
}

/// \brief USB Mass Storage Class - SCSI - Read(10)
/// \param[in]     instance             index of MSC instance.
/// \param[in]     block_addr           address of first block to be read.
/// \param[in]     block_num            number of blocks to be read.
/// \param[out]    ptr_data             pointer to where data will be read.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_Read10 (uint8_t instance, uint32_t block_addr, uint32_t block_num, uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiRead10(instance, block_addr, block_num);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, (block_num << 9), (1U << 7), 0U, 10U, SCSI_READ10, 0U, (uint8_t)(block_addr >> 24), (uint8_t)(block_addr >> 16), (uint8_t)(block_addr >> 8), (uint8_t)(block_addr), 0U, (uint8_t)(block_num >> 8), (uint8_t)(block_num), 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Receive Data
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,          ptr_data, block_num << 9);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dDataResidue) != 0U) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiRead10Failed(instance, block_addr, block_num, status);
  }
#endif
  return status;
}

/// \brief USB Mass Storage Class - SCSI - Write(10)
/// \param[in]     instance             index of MSC instance.
/// \param[in]     block_addr           address of first block to be written.
/// \param[in]     block_num            number of blocks to be written.
/// \param[in]     ptr_data             pointer to data to be written.
/// \param[out]    ptr_stat             pointer to command status response.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_SCSI_Write10 (uint8_t instance, uint32_t block_addr, uint32_t block_num, const uint8_t *ptr_data, uint32_t *ptr_stat) {
  USBH_MSC         *ptr_msc;
  MSC_CBW          *ptr_cbw;
  MSC_CSW          *ptr_csw;
  uint8_t          *ptr_mem;
  USBH_PIPE_HANDLE  bulk_out_pipe_hndl;
  USBH_PIPE_HANDLE  bulk_in_pipe_hndl;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_MSC_ScsiWrite10(instance, block_addr, block_num);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  if (ptr_stat == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc            = &usbh_msc[instance];
  ctrl               =  ptr_msc->ptr_dev->ctrl;
  bulk_out_pipe_hndl =  ptr_msc->bulk_out_pipe_hndl;
  bulk_in_pipe_hndl  =  ptr_msc->bulk_in_pipe_hndl;

  // Send CBW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 31U); ptr_cbw = (MSC_CBW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  ptr_msc->tag++;
  PREPARE_MSC_CBW                       (ptr_cbw, MSC_CBW_Signature, ptr_msc->tag, (block_num << 9), 0U, 0U, 10U, SCSI_WRITE10, 0U, (uint8_t)(block_addr >> 24), (uint8_t)(block_addr >> 16), (uint8_t)(block_addr >> 8), (uint8_t)(block_addr), 0U, (uint8_t)(block_num >> 8), (uint8_t)(block_num), 0U, 0U, 0U, 0U, 0U, 0U, 0U)
  status  = USBH_PipeSend               (bulk_out_pipe_hndl,        (uint8_t  *) ptr_cbw, 31U);
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_cbw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }
  if (status != usbOK) {
    goto exit;
  }

  // Send Data
  status = USBH_PipeSend                (bulk_out_pipe_hndl,         ptr_data, block_num << 9);
  if (status != usbOK) {
    goto exit;
  }

  // Receive CSW
  mstatus = USBH_MemoryAllocate         (ctrl,                      (uint8_t **)&ptr_mem, 13U); ptr_csw = (MSC_CSW *)ptr_mem;
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }
  status = USBH_PipeReceive             (bulk_in_pipe_hndl,         (uint8_t  *) ptr_csw, 13U);
  if (status == usbOK) {
    if (U32_LE(ptr_csw->dSignature) != MSC_CSW_Signature) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dTag) != ptr_msc->tag) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    if (U32_LE(ptr_csw->dDataResidue) != 0U) {
      status = usbClassErrorMSC;
      goto mem_free_and_exit;
    }
    *ptr_stat = ptr_csw->bStatus;
  }

mem_free_and_exit:
  mstatus = USBH_MemoryFree             (ctrl,                      (uint8_t  *) ptr_csw);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_ScsiWrite10Failed(instance, block_addr, block_num, status);
  }
#endif
  return status;
}

/// \brief Recover Mass Storage Device instance
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_MSC_Recover (uint8_t instance) {
  const USBH_DEV  *ptr_dev;
  const USBH_MSC  *ptr_msc;
        uint8_t   *data;
        uint32_t   stat;
        uint8_t    count;
        uint8_t    ctrl;
        usbStatus  status;
        usbStatus  mstatus;

  EvrUSBH_MSC_Recover(instance);

  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_msc  = &usbh_msc[instance];
  ptr_dev  =  ptr_msc->ptr_dev;
  ctrl     =  ptr_dev->ctrl;

  if (ptr_msc->bulk_in_pipe_hndl != 0U) {
    (void)USBH_PipeUpdate (ptr_msc->bulk_in_pipe_hndl);
    (void)USBH_PipeReset  (ptr_msc->bulk_in_pipe_hndl);
  }

  if (ptr_msc->bulk_out_pipe_hndl != 0U) {
    (void)USBH_PipeUpdate (ptr_msc->bulk_out_pipe_hndl);
    (void)USBH_PipeReset  (ptr_msc->bulk_out_pipe_hndl);
  }

  for (count = 0U; count < 5U; count ++) {
    (void)USBH_Delay (250U);
    // 5 * Test Unit Ready
    status = USBH_MSC_SCSI_TestUnitReady(instance, &stat);
    if (status != usbOK) {
      goto exit;
    }

    if (stat != 0U) {
      // Request Sense if Test Unit Ready Failed
      (void)USBH_Delay (1000U);
      mstatus = USBH_MemoryAllocate     (ctrl, (uint8_t **)&data, 18U);
      if (mstatus != usbOK) {
        status = mstatus;
        goto exit;
      }
      status  = USBH_MSC_SCSI_RequestSense(instance, (uint8_t *)data, &stat);
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) data);
      if ((status == usbOK) && (mstatus != usbOK)) {
        status = mstatus;
      }
      if (status != usbOK) {
        goto exit;
      }
    }
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_MSC_RecoverFailed(instance, status);
  }
#endif
  return status;
}
