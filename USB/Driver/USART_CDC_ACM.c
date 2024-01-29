/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host:CDC
 * Copyright (c) 2013-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USART_CDC_ACM.c
 * Purpose: CMSIS USART driver for USB Host with USB CDC ACM (VirtualCOM)
 * Rev.:    V1.10
 *----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
 * Driver:       Driver_USARTn..Driver_USARTm
 * Project:      USART Driver for USB Host CDC ACM compliant device
 * Settings:     USART_CDC_DRV_INSTANCE_NUM sets number of driver instances
 *               USART_CDC_DRV0_NUM sets the first instance driver number
 *               (default first instance number is 9)
 *               USART_CDC_DRV1_NUM sets the second instance driver number
 *               (default first instance number is 10)
 *               USART_CDC_DRV2_NUM sets the third instance driver number
 *               (default first instance number is 11)
 *               USART_CDC_DRV3_NUM sets the fourth instance driver number
 *               (default first instance number is 12)
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.10
 *    - Corrected compiler warnings
 *  Version 1.9
 *    - Corrected CMSIS-RTOS1 compatibility mode compilation error
 *  Version 1.8
 *    - Corrected CMSIS-RTOS2 compilation error
 *  Version 1.7
 *    - Corrected Receive function (invalid data in receive buffer)
 *  Version 1.6
 *    - Corrected Receive and Send functions so they can now be called from 
 *      Received and Sent callbacks
 *    - Corrected Receive function so ARM_USART_EVENT_RECEIVE_COMPLETE event 
 *      is signaled after requested number of bytes was received
 *  Version 1.5
 *    - OS headers included depending on RTE settings
 *  Version 1.4
 *    - Added support for CMSIS-RTOS2
 *  Version 1.3
 *    - Updated with custom class API changes
 *  Version 1.2
 *    - Improved compatibility with ARM Compiler 6
 *  Version 1.0
 *    - Initial release
 */

#include <stdint.h>
#include <string.h>

#include "rl_usb.h"

#include "Driver_USART.h"

/************************** Constant and Macro Definitions ********************/

// Number of instances (default 1, maximum 4)
#ifndef USART_CDC_DRV_INSTANCE_NUM
#define USART_CDC_DRV_INSTANCE_NUM     (1U)
#endif

// Driver number used for Driver Control Block instance names
#ifndef USART_CDC_DRV0_NUM
#define USART_CDC_DRV0_NUM              9
#endif
#if    (USART_CDC_DRV_INSTANCE_NUM > 1U)
#ifndef USART_CDC_DRV1_NUM
#define USART_CDC_DRV1_NUM              10
#endif
#endif
#if    (USART_CDC_DRV_INSTANCE_NUM > 2U)
#ifndef USART_CDC_DRV2_NUM
#define USART_CDC_DRV2_NUM              11
#endif
#endif
#if    (USART_CDC_DRV_INSTANCE_NUM > 3U)
#ifndef USART_CDC_DRV3_NUM
#define USART_CDC_DRV3_NUM              12
#endif
#endif

#define USART_CDC_DRV_(n)               Driver_USART##n
#define USART_CDC_DRV(n)                USART_CDC_DRV_(n)

// Send, Receive and Status thread settings
#ifndef USART_CDC_SEND_THREAD_STACK_SIZE
#define USART_CDC_SEND_THREAD_STACK_SIZE       (512U)
#endif
#ifndef USART_CDC_SEND_THREAD_PRIORITY
#define USART_CDC_SEND_THREAD_PRIORITY         (osPriorityNormal)
#endif
#ifndef USART_CDC_RECEIVE_THREAD_STACK_SIZE
#define USART_CDC_RECEIVE_THREAD_STACK_SIZE    (512U)
#endif
#ifndef USART_CDC_RECEIVE_THREAD_PRIORITY
#define USART_CDC_RECEIVE_THREAD_PRIORITY      (osPriorityNormal)
#endif

// Thread flags
#define USBH_THREAD_EVENT_FLAG                 (1U)
#define USBH_THREAD_TERMINATE_FLAG             (2U)

/************************** Variable and Memory Definitions *******************/

typedef struct {
  volatile void          *receive_thread_id;  ///< bulk in pipe thread ID
  volatile void          *send_thread_id;     ///< bulk out pipe thread ID
  ARM_USART_SignalEvent_t cb_event;           ///< USART Driver callback function
  const    uint8_t       *send_data;          ///< pointer to memory containing data to send
           uint32_t       send_num;           ///< number of data bytes to send
           uint8_t       *receive_buf;        ///< pointer to receive buffer
           uint8_t       *receive_buf_in;     ///< pointer to first free byte in receive buffer
           uint8_t       *receive_buf_out;    ///< pointer to first unread byte in receive buffer
           uint8_t       *receive_data;       ///< pointer to memory for data reception
           uint32_t       receive_num;        ///< number of data bytes left to receive
           uint32_t       receive_rq_num;     ///< number of data bytes requested to receive
           uint16_t       ctrl_lines;         ///< modem lines control
           uint16_t       status;             ///< modem lines and error status
           bool           initialized;        ///< UART instance initialized
           uint8_t        cdc_instance;       ///< CDC instance
           bool           control_tx;         ///< UART Tx enable
           bool           control_rx;         ///< UART Rx enable
           bool           flow_control_rts;   ///< UART flow control on RTS line enable
           bool           flow_control_cts;   ///< UART flow control on CTS line enable
           bool           send_busy;          ///< send in progress
           bool           receive_busy;       ///< reception in progress
  CDC_LINE_CODING         line_coding;        ///< initial line coding
           uint8_t        pad0;               ///< explicit padding
} USART_CDC_t;

static uint8_t     usbh_receive_buf[USART_CDC_DRV_INSTANCE_NUM][512] __ALIGNED(4);

static USART_CDC_t usart_cdc       [USART_CDC_DRV_INSTANCE_NUM];

/************************** Global Function Prototypes ************************/

#ifdef USB_CMSIS_RTOS2
static void USART_CDC_Send_Thread    (void *arg);
static void USART_CDC_Receive_Thread (void *arg);
#else
       void USART_CDC_Send_Thread    (void const *arg);
       void USART_CDC_Receive_Thread (void const *arg);
#endif

/************************** Internal Function Prototypes **********************/

static void ARM_USARTx_Sent            (uint8_t instance);
static void ARM_USARTx_Received        (uint8_t instance);

/************************** Internal Functions ********************************/

/// \brief Check device instance pointers and variables
/// \param[in]     instance             index of CDC instance.
/// \return        true                 requested instance exists.
/// \return        false                requested instance does not exist.
static bool CheckInstance (uint8_t instance) {

  if (instance >= USART_CDC_DRV_INSTANCE_NUM)   { return false; }
  if (usart_cdc[instance].initialized == false) { return false; }

  return true;
}

/*------------------------- Internal Threads ---------------------------------*/

/// \brief User thread for handling data send (to Bulk Out endpoint)
/// \param[in]     arg                  index of CDC instance.
#ifdef USB_CMSIS_RTOS2
static void USART_CDC_Send_Thread (void *arg) {
#else
       void USART_CDC_Send_Thread (void const *arg) {
#endif
#ifndef USB_CMSIS_RTOS2
  osEvent      evt;
#endif
  USART_CDC_t *ptr_usart_cdc;
  uint32_t     flags;
  uint8_t      instance;
  usbStatus    status;

  instance      = (uint8_t)((uint32_t)arg);
  ptr_usart_cdc = &usart_cdc[instance];

  for (;;) {
#ifdef USB_CMSIS_RTOS2
    flags = osThreadFlagsWait (3U, osFlagsWaitAny, osWaitForever);
    if ((flags & 0x80000000U) != 0U) {
      flags = USBH_THREAD_TERMINATE_FLAG;
    }
#else
    evt   = osSignalWait(0U, osWaitForever);
    if (evt.status == osEventSignal) {
      flags = evt.value.v;
    } else {
      flags = USBH_THREAD_TERMINATE_FLAG;
    }
#endif
    if ((flags & USBH_THREAD_TERMINATE_FLAG) != 0U) {
      // If flag is thread termination or if error during get flag
      break;
    }

    status = USBH_CDC_ACM_Send (instance, ptr_usart_cdc->send_data, ptr_usart_cdc->send_num);

    if (status == usbOK) {
      ptr_usart_cdc->send_busy = false;
      ARM_USARTx_Sent(instance);
    } else {
      // If USB transfer failed
      break;
    }
  }

  ptr_usart_cdc->send_busy      = false;
  ptr_usart_cdc->send_thread_id = NULL;
  (void)osThreadTerminate (osThreadGetId());
}
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usart_cdc_send_thread_cb_mem   [USART_CDC_DRV_INSTANCE_NUM]                                               __SECTION(.bss.os.thread.cb);
static uint64_t       usart_cdc_send_thread_stack_mem[USART_CDC_DRV_INSTANCE_NUM][(USART_CDC_SEND_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static osThreadAttr_t usart_cdc_send_thread_attr = {
  NULL,
  0U,
  NULL,
  0U,
  NULL,
  (((USART_CDC_SEND_THREAD_STACK_SIZE + 7U) / 8U) * 8U),
  USART_CDC_SEND_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USART_CDC_Send_Thread;
osThreadDef(USART_CDC_Send_Thread, USART_CDC_SEND_THREAD_PRIORITY, USART_CDC_DRV_INSTANCE_NUM, USART_CDC_SEND_THREAD_STACK_SIZE);
#endif

/// \brief User thread for handling data reception (from Bulk In endpoint)
/// \param[in]     arg                  index of CDC instance.
#ifdef USB_CMSIS_RTOS2
static void USART_CDC_Receive_Thread (void *arg) {
#else
       void USART_CDC_Receive_Thread (void const *arg) {
#endif
#ifndef USB_CMSIS_RTOS2
  osEvent      evt;
#endif
  USART_CDC_t *ptr_usart_cdc;
  uint32_t     flags, buf_len, cpy_len;
  uint8_t      instance;
  usbStatus    status;
  uint8_t      exit;

  instance      = (uint8_t)((uint32_t)arg);
  ptr_usart_cdc = &usart_cdc[instance];

  exit = 0U;

  for (;;) {
#ifdef USB_CMSIS_RTOS2
    flags = osThreadFlagsWait (3U, osFlagsWaitAny, osWaitForever);
    if ((flags & 0x80000000U) != 0U) {
      flags = USBH_THREAD_TERMINATE_FLAG;
    }
#else
    evt   = osSignalWait(0U, osWaitForever);
    if (evt.status == osEventSignal) {
      flags = evt.value.v;
    } else {
      flags = USBH_THREAD_TERMINATE_FLAG;
    }
#endif
    if ((flags & USBH_THREAD_TERMINATE_FLAG) != 0U) {
      // If flag is thread termination or if error during get flag
      break;
    }

    // Here event was received from Receive function
    while (ptr_usart_cdc->receive_num != 0U) {

      // Check if there is already data received on USB
      if (ptr_usart_cdc->receive_buf_in > ptr_usart_cdc->receive_buf_out) {
        buf_len = (uint32_t)(ptr_usart_cdc->receive_buf_in - ptr_usart_cdc->receive_buf_out);
      } else {
        buf_len = 0U;
      }
      if (buf_len > ptr_usart_cdc->receive_num) {
        // If already available more data on USB then requested on UART,
        // limit it to UART requested
        cpy_len = ptr_usart_cdc->receive_num;
      } else {
        cpy_len = buf_len;
      }

      if (cpy_len != 0U) {
        // Copy available data from USB to UART requested data buffer
        memcpy(ptr_usart_cdc->receive_data, (const void *)ptr_usart_cdc->receive_buf_out, cpy_len);
        ptr_usart_cdc->receive_buf_out += cpy_len;
        ptr_usart_cdc->receive_data    += cpy_len;
        ptr_usart_cdc->receive_num     -= cpy_len;
      }

      if (ptr_usart_cdc->receive_num == 0U) {
        // If all requested data was received
        ptr_usart_cdc->receive_busy = false;
        ARM_USARTx_Received(instance);
        break;
      }

      if (ptr_usart_cdc->receive_buf_in == ptr_usart_cdc->receive_buf_out) {
        // If all data was not yet received and USB buffer is empty
        // Start new USB transfer
        ptr_usart_cdc->receive_buf_in  = ptr_usart_cdc->receive_buf;
        ptr_usart_cdc->receive_buf_out = ptr_usart_cdc->receive_buf;
        do {
          status = USBH_CDC_ACM_Receive (instance, (uint8_t *)ptr_usart_cdc->receive_buf, 512U);
          if (status == usbTransferError) {
            // If USB transfer failed to start
            exit = 1U;
            break;
          }
          ptr_usart_cdc->receive_buf_in += USBH_CDC_ACM_GetRxCount (instance);
        } while (status == usbTimeout);
      }
      if (exit != 0U) {
        break;
      }
    }
    if (exit != 0U) {
      break;
    }
  }

  ptr_usart_cdc->receive_busy      = false;
  ptr_usart_cdc->receive_buf_in    = ptr_usart_cdc->receive_buf;
  ptr_usart_cdc->receive_buf_out   = ptr_usart_cdc->receive_buf;
  ptr_usart_cdc->receive_thread_id = NULL;
  (void)osThreadTerminate (osThreadGetId());
}
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usart_cdc_receive_thread_cb_mem   [USART_CDC_DRV_INSTANCE_NUM]                                                  __SECTION(.bss.os.thread.cb);
static uint64_t       usart_cdc_receive_thread_stack_mem[USART_CDC_DRV_INSTANCE_NUM][(USART_CDC_RECEIVE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static osThreadAttr_t usart_cdc_receive_thread_attr = {
  NULL,
  0U,
  NULL,
  0U,
  NULL,
  (((USART_CDC_RECEIVE_THREAD_STACK_SIZE + 7U) / 8U) * 8U),
  USART_CDC_RECEIVE_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USART_CDC_Receive_Thread;
osThreadDef(USART_CDC_Receive_Thread, USART_CDC_RECEIVE_THREAD_PRIORITY, USART_CDC_DRV_INSTANCE_NUM, USART_CDC_RECEIVE_THREAD_STACK_SIZE);
#endif

/***-------------- USBH CDC Specific Functions -----------------------------***/

/// \brief Callback function called when Communication Device Class device
///        modem line or error status changes
/// \param[in]     instance             index of CDC instance.
/// \return                             error status and line states:
///                                       - bit 6: bOverRun
///                                       - bit 5: bParity
///                                       - bit 4: bFraming
///                                       - bit 3: bRingSignal
///                                       - bit 2: bBreak
///                                       - bit 1: bTxCarrier (DSR line state)
///                                       - bit 0: bRxCarrier (DCD line state)
void USBH_CDC_ACM_Notify (uint8_t instance, uint16_t status) {
  usart_cdc[instance].status = status;
}

/************************** USART Driver **************************************/

#define ARM_USART_DRV_VERSION           ARM_DRIVER_VERSION_MAJOR_MINOR(1,10)

// Driver Version
static const ARM_DRIVER_VERSION         usart_driver_version = { ARM_USART_API_VERSION, ARM_USART_DRV_VERSION };

// USART Capabilities
static const ARM_USART_CAPABILITIES     usart_capabilities = {
  1U,                                   ///< supports UART (Asynchronous) mode
  0U,                                   ///< supports Synchronous Master mode
  0U,                                   ///< supports Synchronous Slave mode
  0U,                                   ///< supports UART Single-wire mode
  0U,                                   ///< supports UART IrDA mode
  0U,                                   ///< supports UART Smart Card mode
  0U,                                   ///< Smart Card Clock generator available
  1U,                                   ///< RTS Flow Control available
  1U,                                   ///< CTS Flow Control available
  0U,                                   ///< Transmit completed event: ARM_USART_EVENT_TX_COMPLETE
  0U,                                   ///< Signal receive character timeout event: ARM_USART_EVENT_RX_TIMEOUT
  1U,                                   ///< RTS Line: 0=not available, 1=available
  1U,                                   ///< CTS Line: 0=not available, 1=available
  1U,                                   ///< DTR Line: 0=not available, 1=available
  1U,                                   ///< DSR Line: 0=not available, 1=available
  1U,                                   ///< DCD Line: 0=not available, 1=available
  1U,                                   ///< RI Line: 0=not available, 1=available
  0U,                                   ///< Signal CTS change event: ARM_USART_EVENT_CTS
  0U,                                   ///< Signal DSR change event: ARM_USART_EVENT_DSR
  0U,                                   ///< Signal DCD change event: ARM_USART_EVENT_DCD
  0U,                                   ///< Signal RI change event: ARM_USART_EVENT_RI
  0U                                    ///< Reserved (must be zero)
};

/// \fn          ARM_DRIVER_VERSION ARM_USART_GetVersion (void)
/// \brief       Get driver version.
/// \return      ARM_DRIVER_VERSION
static ARM_DRIVER_VERSION ARM_USART_GetVersion (void) {
  return usart_driver_version;
}

/// \fn          ARM_USART_CAPABILITIES ARM_USART_GetCapabilities (void)
/// \brief       Get driver capabilities
/// \return      ARM_USART_CAPABILITIES
static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities (void) {
  return usart_capabilities;
}

/// \fn          int32_t ARM_USARTx_Initialize (uint8_t instance, ARM_USART_SignalEvent_t cb_event)
/// \brief       Initialize USART Interface.
/// \param[in]   instance  Index of CDC instance
/// \param[in]   cb_event  Pointer to ARM_USART_SignalEvent
/// \return      execution status
static int32_t ARM_USARTx_Initialize (uint8_t instance, ARM_USART_SignalEvent_t cb_event) {

  if (instance >= USART_CDC_DRV_INSTANCE_NUM) { return ARM_DRIVER_ERROR; }

  memset((void *)&usart_cdc[instance], 0, sizeof(USART_CDC_t));

  usart_cdc[instance].cb_event     = cb_event;
  usart_cdc[instance].cdc_instance = instance;
  usart_cdc[instance].receive_buf  = (uint8_t *)&usbh_receive_buf[instance][0];
  usart_cdc[instance].initialized  = true;

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_Initialize (ARM_USART_SignalEvent_t cb_event) { return ARM_USARTx_Initialize (0U, cb_event); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_Initialize (ARM_USART_SignalEvent_t cb_event) { return ARM_USARTx_Initialize (1U, cb_event); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_Initialize (ARM_USART_SignalEvent_t cb_event) { return ARM_USARTx_Initialize (2U, cb_event); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_Initialize (ARM_USART_SignalEvent_t cb_event) { return ARM_USARTx_Initialize (3U, cb_event); }
#endif

/// \fn          int32_t ARM_USARTx_Uninitialize (uint8_t instance)
/// \brief       De-initialize USART Interface.
/// \param[in]   instance  Index of CDC instance
/// \return      execution status
static int32_t ARM_USARTx_Uninitialize (uint8_t instance) {

  if (CheckInstance (instance) == false) { return ARM_DRIVER_ERROR; }

  memset((void *)&usart_cdc[instance], 0, sizeof(USART_CDC_t));
  (void)usart_cdc[instance].pad0;

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_Uninitialize (void) { return ARM_USARTx_Uninitialize (0U); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_Uninitialize (void) { return ARM_USARTx_Uninitialize (1U); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_Uninitialize (void) { return ARM_USARTx_Uninitialize (2U); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_Uninitialize (void) { return ARM_USARTx_Uninitialize (3U); }
#endif

/// \fn          int32_t ARM_USARTx_PowerControl (uint8_t instance, ARM_POWER_STATE state)
/// \brief       Control USART Interface Power.
/// \param[in]   instance  Index of CDC instance
/// \param[in]   state     Power state
/// \return      execution status
static int32_t ARM_USARTx_PowerControl (uint8_t instance, ARM_POWER_STATE state) {
  switch (state) {
    case ARM_POWER_OFF:
      break;

    case ARM_POWER_FULL:
      if (usart_cdc[instance].initialized == false) { return ARM_DRIVER_ERROR; }
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_PowerControl (ARM_POWER_STATE state) { return (ARM_USARTx_PowerControl (0U, state)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_PowerControl (ARM_POWER_STATE state) { return (ARM_USARTx_PowerControl (1U, state)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_PowerControl (ARM_POWER_STATE state) { return (ARM_USARTx_PowerControl (2U, state)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_PowerControl (ARM_POWER_STATE state) { return (ARM_USARTx_PowerControl (3U, state)); }
#endif

/// \fn          int32_t ARM_USARTx_Send (uint8_t instance, const void *data, uint32_t num)
/// \brief       Start sending data to USART transmitter.
/// \param[in]   instance  Index of CDC instance
/// \param[in]   data      Pointer to buffer with data to send to USART transmitter
/// \param[in]   num       Number of data items to send
/// \return      execution status
static int32_t ARM_USARTx_Send (uint8_t instance, const void *data, uint32_t num) {
  USART_CDC_t *ptr_usart_cdc;

  if (CheckInstance (instance) == false)       { return ARM_DRIVER_ERROR; }
  if (usart_cdc[instance].control_tx == false) { return ARM_DRIVER_ERROR; }

  ptr_usart_cdc = &usart_cdc[instance];

  if (ptr_usart_cdc->send_busy == true)        { return ARM_DRIVER_ERROR_BUSY; }
  ptr_usart_cdc->send_busy    = true;

  ptr_usart_cdc->send_data    = (const uint8_t *)data;
  ptr_usart_cdc->send_num     = num;

  // Start send thread if it is not already running
  if (ptr_usart_cdc->send_thread_id == NULL) {
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
    usart_cdc_send_thread_attr.cb_mem    = &usart_cdc_send_thread_cb_mem   [instance];
    usart_cdc_send_thread_attr.cb_size   = sizeof(osRtxThread_t);
    usart_cdc_send_thread_attr.stack_mem = &usart_cdc_send_thread_stack_mem[instance][0];
#endif
    ptr_usart_cdc->send_thread_id = osThreadNew (USART_CDC_Send_Thread, (void *)((uint32_t)instance), &usart_cdc_send_thread_attr);
#else
    ptr_usart_cdc->send_thread_id = osThreadCreate (osThread(USART_CDC_Send_Thread), (void *)((uint32_t)instance));
#endif
    if (ptr_usart_cdc->send_thread_id == NULL) {
      ptr_usart_cdc->send_busy = false;
      return ARM_DRIVER_ERROR;
    }
  }

  // Here USB send thread is active, send event to USB send thread
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet((osThreadId_t)((uint32_t)ptr_usart_cdc->send_thread_id), USBH_THREAD_EVENT_FLAG);
#else
  (void)osSignalSet     ((osThreadId)  ((uint32_t)ptr_usart_cdc->send_thread_id), USBH_THREAD_EVENT_FLAG);
#endif

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_Send (const void *data, uint32_t num) { return (ARM_USARTx_Send (0U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_Send (const void *data, uint32_t num) { return (ARM_USARTx_Send (1U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_Send (const void *data, uint32_t num) { return (ARM_USARTx_Send (2U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_Send (const void *data, uint32_t num) { return (ARM_USARTx_Send (3U, data, num)); }
#endif

/// \fn          void ARM_USARTx_Sent (uint8_t instance)
/// \brief       All data was sent on USART transmitter.
/// \param[in]   instance  Index of CDC instance
static void ARM_USARTx_Sent (uint8_t instance) {
  if (usart_cdc[instance].cb_event != 0U) {
    usart_cdc[instance].cb_event (ARM_USART_EVENT_SEND_COMPLETE);
  }
}

/// \fn          int32_t ARM_USARTx_Receive (uint8_t instance, void *data, uint32_t num)
/// \brief       Start receiving data from USART receiver.
/// \param[in]   instance  Index of CDC instance
/// \param[out]  data      Pointer to buffer for data to receive from USART receiver
/// \param[in]   num       Number of data items to receive
/// \return      execution status
static int32_t ARM_USARTx_Receive (uint8_t instance, void *data, uint32_t num) {
  USART_CDC_t *ptr_usart_cdc;

  if (CheckInstance (instance) == false)       { return ARM_DRIVER_ERROR; }
  if (usart_cdc[instance].control_rx == false) { return ARM_DRIVER_ERROR; }

  ptr_usart_cdc = &usart_cdc[instance];

  if (ptr_usart_cdc->receive_busy == true)     { return ARM_DRIVER_ERROR_BUSY; }
  ptr_usart_cdc->receive_busy    = true;

  ptr_usart_cdc->receive_data    = data;
  ptr_usart_cdc->receive_num     = num;
  ptr_usart_cdc->receive_rq_num  = num;

  // Start reception thread if it is not already running
  if (ptr_usart_cdc->receive_thread_id == NULL) {
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
    usart_cdc_receive_thread_attr.cb_mem    = &usart_cdc_receive_thread_cb_mem   [instance];
    usart_cdc_receive_thread_attr.cb_size   = sizeof(osRtxThread_t);
    usart_cdc_receive_thread_attr.stack_mem = &usart_cdc_receive_thread_stack_mem[instance][0];
#endif
    ptr_usart_cdc->receive_thread_id = osThreadNew (USART_CDC_Receive_Thread, (void *)((uint32_t)instance), &usart_cdc_receive_thread_attr);
#else
    ptr_usart_cdc->receive_thread_id = osThreadCreate (osThread(USART_CDC_Receive_Thread), (void *)((uint32_t)instance));
#endif
    if (ptr_usart_cdc->receive_thread_id == NULL) {
      ptr_usart_cdc->receive_busy = false;
      return ARM_DRIVER_ERROR;
    }
  }

  // Here USB receive thread is active, send event to USB receive thread
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet((osThreadId_t)((uint32_t)ptr_usart_cdc->receive_thread_id), USBH_THREAD_EVENT_FLAG);
#else
  (void)osSignalSet     ((osThreadId)  ((uint32_t)ptr_usart_cdc->receive_thread_id), USBH_THREAD_EVENT_FLAG);
#endif

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_Receive (void *data, uint32_t num) { return (ARM_USARTx_Receive (0U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_Receive (void *data, uint32_t num) { return (ARM_USARTx_Receive (1U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_Receive (void *data, uint32_t num) { return (ARM_USARTx_Receive (2U, data, num)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_Receive (void *data, uint32_t num) { return (ARM_USARTx_Receive (3U, data, num)); }
#endif

/// \fn          void ARM_USARTx_Received (uint8_t instance)
/// \brief       All data was received on USART receiver.
/// \param[in]   instance  Index of CDC instance
static void ARM_USARTx_Received (uint8_t instance) {
  if (usart_cdc[instance].cb_event != 0U) {
    usart_cdc[instance].cb_event (ARM_USART_EVENT_RECEIVE_COMPLETE);
  }
}

/// \fn          int32_t ARM_USART_Transfer (const void *data_out,
///                                                void *data_in,
///                                          uint32_t    num)
/// \brief       Start sending/receiving data to/from USART transmitter/receiver.
/// \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
/// \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
/// \param[in]   num       Number of data items to transfer
/// \return      execution status
static int32_t ARM_USART_Transfer (const void *data_out, void *data_in, uint32_t num) {
  // Not supported in asynchronous mode
  (void)data_out;
  (void)data_in;
  (void)num;
  return ARM_DRIVER_ERROR;
}

/// \fn          uint32_t ARM_USARTx_GetTxCount (uint8_t instance)
/// \brief       Get transmitted data count.
/// \param[in]   instance  Index of CDC instance
/// \return      number of data items transmitted
static uint32_t ARM_USARTx_GetTxCount (uint8_t instance) {

  if (CheckInstance (instance) == false) { return 0U; }

  return (USBH_CDC_ACM_GetTxCount(instance));
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static uint32_t ARM_USART0_GetTxCount (void) { return (ARM_USARTx_GetTxCount (0U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static uint32_t ARM_USART1_GetTxCount (void) { return (ARM_USARTx_GetTxCount (1U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static uint32_t ARM_USART2_GetTxCount (void) { return (ARM_USARTx_GetTxCount (2U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static uint32_t ARM_USART3_GetTxCount (void) { return (ARM_USARTx_GetTxCount (3U)); }
#endif

/// \fn          uint32_t ARM_USARTx_GetRxCount (uint8_t instance)
/// \brief       Get received data count.
/// \param[in]   instance  Index of CDC instance
/// \return      number of data items received
static uint32_t ARM_USARTx_GetRxCount (uint8_t instance) {

  if (CheckInstance (instance) == false) { return 0U; }

  return (usart_cdc[instance].receive_rq_num - usart_cdc[instance].receive_num);
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static uint32_t ARM_USART0_GetRxCount (void) { return (ARM_USARTx_GetRxCount (0U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static uint32_t ARM_USART1_GetRxCount (void) { return (ARM_USARTx_GetRxCount (1U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static uint32_t ARM_USART2_GetRxCount (void) { return (ARM_USARTx_GetRxCount (2U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static uint32_t ARM_USART3_GetRxCount (void) { return (ARM_USARTx_GetRxCount (3U)); }
#endif

/// \fn          int32_t ARM_USARTx_Control (uint8_t instance, uint32_t control, uint32_t arg)
/// \brief       Control USART Interface.
/// \param[in]   instance  Index of CDC instance
/// \param[in]   control   Operation
/// \param[in]   arg       Argument of operation (optional)
/// \return      common execution status and driver specific usart_execution status
static int32_t ARM_USARTx_Control (uint8_t instance, uint32_t control, uint32_t arg) {
  USART_CDC_t       *ptr_usart_cdc;
  CDC_LINE_CODING    line_coding;

  if (CheckInstance (instance) == false) { return ARM_DRIVER_ERROR; }

  ptr_usart_cdc = &usart_cdc[instance];
  line_coding   = ptr_usart_cdc->line_coding;

  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS:
      break;

    case ARM_USART_CONTROL_TX:                  // Control Tx
      if (arg != 0U) {
        ptr_usart_cdc->control_tx = true;       // Tx enabled
      } else {
        ptr_usart_cdc->control_tx = false;      // Tx disable
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_RX:                  // Control Rx
      if (arg != 0U) {
        ptr_usart_cdc->control_rx = true;       // Rx enabled
      } else {
        ptr_usart_cdc->control_rx = false;      // Rx disable
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_BREAK:               // Control break
      if (arg != 0U) {                          // Break enabled
        if (USBH_CDC_ACM_SendBreak (instance, 65535U) != usbOK) {
          return ARM_DRIVER_ERROR;
        }
      } else {                                  // Break disable
        if (USBH_CDC_ACM_SendBreak (instance, 0U) != usbOK) {
          return ARM_DRIVER_ERROR;
        }
      }
      return ARM_DRIVER_OK;

    case ARM_USART_ABORT_SEND:                  // Abort send
      if (ptr_usart_cdc->send_thread_id != NULL) {
#ifdef USB_CMSIS_RTOS2
        (void)osThreadFlagsSet((osThreadId_t)((uint32_t)ptr_usart_cdc->send_thread_id), USBH_THREAD_TERMINATE_FLAG);
#else
        (void)osSignalSet     ((osThreadId)  ((uint32_t)ptr_usart_cdc->send_thread_id), USBH_THREAD_TERMINATE_FLAG);
#endif
      }
      while (ptr_usart_cdc->send_thread_id != NULL) { (void)osDelay(10U); }
      return ARM_DRIVER_OK;

    case ARM_USART_ABORT_RECEIVE:               // Abort receive
      if (ptr_usart_cdc->receive_thread_id != NULL) {
#ifdef USB_CMSIS_RTOS2
        (void)osThreadFlagsSet((osThreadId_t)((uint32_t)ptr_usart_cdc->receive_thread_id), USBH_THREAD_TERMINATE_FLAG);
#else
        (void)osSignalSet     ((osThreadId)  ((uint32_t)ptr_usart_cdc->receive_thread_id), USBH_THREAD_TERMINATE_FLAG);
#endif
      }
      while (ptr_usart_cdc->receive_thread_id != NULL) { (void)osDelay(10U); }
      return ARM_DRIVER_OK;

//  case ARM_USART_MODE_SYNCHRONOUS_MASTER:
//  case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
//  case ARM_USART_MODE_SINGLE_WIRE:
//  case ARM_USART_MODE_IRDA:
//  case ARM_USART_MODE_SMART_CARD:
//  case ARM_USART_SET_DEFAULT_TX_VALUE:        // Default TX value
//  case ARM_USART_SET_IRDA_PULSE:              // IrDA pulse
//  case ARM_USART_SET_SMART_CARD_GUARD_TIME:   // SmartCard guard time
//  case ARM_USART_SET_SMART_CARD_CLOCK:        // SmartCard guard time
//  case ARM_USART_CONTROL_SMART_CARD_NACK:     // SmartCard NACK
//  case ARM_USART_ABORT_TRANSFER:              // Abort transfer

    default:                                    // Undefined command
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  switch (control & ARM_USART_DATA_BITS_Msk) {  // USART Data bits
    case ARM_USART_DATA_BITS_5:
      line_coding.bDataBits = 5U;
      break;
    case ARM_USART_DATA_BITS_6:
      line_coding.bDataBits = 6U;
      break;
    case ARM_USART_DATA_BITS_7:
      line_coding.bDataBits = 7U;
      break;
    case ARM_USART_DATA_BITS_8:
      line_coding.bDataBits = 8U;
      break;
    default:
      return ARM_USART_ERROR_DATA_BITS;
  }

  switch (control & ARM_USART_PARITY_Msk) {     // USART Parity
    case ARM_USART_PARITY_NONE:
      line_coding.bParityType = 0U;
      break;
    case ARM_USART_PARITY_ODD:
      line_coding.bParityType = 1U;
      break;
    case ARM_USART_PARITY_EVEN:
      line_coding.bParityType = 2U;
      break;
    default:
      return ARM_USART_ERROR_PARITY;
  }

  switch (control & ARM_USART_STOP_BITS_Msk) {  // USART Stop bits
    case ARM_USART_STOP_BITS_1:
      line_coding.bCharFormat = 0U;
      break;
    case ARM_USART_STOP_BITS_1_5:
      line_coding.bCharFormat = 1U;
      break;
    case ARM_USART_STOP_BITS_2:
      line_coding.bCharFormat = 2U;
      break;
    default:
      return ARM_USART_ERROR_STOP_BITS;
  }

  switch (control & ARM_USART_FLOW_CONTROL_Msk) { // USART Flow control
    case ARM_USART_FLOW_CONTROL_NONE:
      ptr_usart_cdc->flow_control_rts = false;
      ptr_usart_cdc->flow_control_cts = false;
      break;
    case ARM_USART_FLOW_CONTROL_RTS:
      ptr_usart_cdc->flow_control_rts = true;
      break;
    case ARM_USART_FLOW_CONTROL_CTS:
      ptr_usart_cdc->flow_control_cts = true;
      break;
    case ARM_USART_FLOW_CONTROL_RTS_CTS:
      ptr_usart_cdc->flow_control_rts = true;
      ptr_usart_cdc->flow_control_cts = true;
      break;
    default:
      return ARM_USART_ERROR_FLOW_CONTROL;
  }

  line_coding.dwDTERate = arg;          // USART Baudrate

  if (USBH_CDC_ACM_SetLineCoding (instance, &line_coding) != usbOK) {
    return ARM_DRIVER_ERROR;
  }

  ptr_usart_cdc->line_coding = line_coding;

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_Control (uint32_t control, uint32_t arg) { return (ARM_USARTx_Control (0U, control, arg)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_Control (uint32_t control, uint32_t arg) { return (ARM_USARTx_Control (1U, control, arg)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_Control (uint32_t control, uint32_t arg) { return (ARM_USARTx_Control (2U, control, arg)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_Control (uint32_t control, uint32_t arg) { return (ARM_USARTx_Control (3U, control, arg)); }
#endif

/// \fn          ARM_USART_STATUS ARM_USARTx_GetStatus (uint8_t instance)
/// \brief       Get USART status.
/// \param[in]   instance  Index of CDC instance
/// \return      USART status ARM_USART_STATUS
static ARM_USART_STATUS ARM_USARTx_GetStatus (uint8_t instance) {
  USART_CDC_t        *ptr_usart_cdc;
  ARM_USART_STATUS    status = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  uint16_t            usart_status;

  if (CheckInstance (instance) == false) { return status; }

  ptr_usart_cdc = &usart_cdc[instance];
  usart_status  = ptr_usart_cdc->status;

  status.tx_busy          =  ptr_usart_cdc->send_busy;      // Transmitter busy flag
  status.rx_busy          =  ptr_usart_cdc->receive_busy;   // Receiver busy flag
  status.tx_underflow     =  0U;                            // Transmit data underflow detected (cleared on start of next send operation)
  status.rx_overflow      = (usart_status >> 6) & 1U;       // Receive data overflow detected (cleared on start of next receive operation)
  status.rx_break         = (usart_status >> 2) & 1U;       // Break detected on receive (cleared on start of next receive operation)
  status.rx_framing_error = (usart_status >> 4) & 1U;       // Framing error detected on receive (cleared on start of next receive operation)
  status.rx_parity_error  = (usart_status >> 5) & 1U;       // Parity error detected on receive (cleared on start of next receive operation)

  return status;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static ARM_USART_STATUS ARM_USART0_GetStatus (void) { return (ARM_USARTx_GetStatus (0U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static ARM_USART_STATUS ARM_USART1_GetStatus (void) { return (ARM_USARTx_GetStatus (1U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static ARM_USART_STATUS ARM_USART2_GetStatus (void) { return (ARM_USARTx_GetStatus (2U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static ARM_USART_STATUS ARM_USART3_GetStatus (void) { return (ARM_USARTx_GetStatus (3U)); }
#endif

/// \fn          int32_t ARM_USARTx_SetModemControl (uint8_t instance, ARM_USART_MODEM_CONTROL control)
/// \brief       Set USART Modem Control line state.
/// \param[in]   instance  Index of CDC instance
/// \param[in]   control   ARM_USART_MODEM_CONTROL
/// \return      execution status
static int32_t ARM_USARTx_SetModemControl (uint8_t instance, ARM_USART_MODEM_CONTROL control) {
  USART_CDC_t *ptr_usart_cdc;
  uint16_t     ctrl_lines;

  if (CheckInstance (instance) == false) { return ARM_DRIVER_ERROR; }

  ptr_usart_cdc = &usart_cdc[instance];
  ctrl_lines    = ptr_usart_cdc->ctrl_lines;

  if (ptr_usart_cdc->flow_control_rts == true) {// If flow control on RTS line enabled
    if (control == ARM_USART_RTS_CLEAR) {       // Deactivate RTS
      ctrl_lines &= ~(1U << 1);
    }
    if (control == ARM_USART_RTS_SET) {         // Activate RTS
      ctrl_lines |=  (1U << 1);
    }
  }
  if (control == ARM_USART_DTR_CLEAR) {         // Deactivate DTR
    ctrl_lines &= ~1U;
  }
  if (control == ARM_USART_DTR_SET) {           // Activate DTR
    ctrl_lines |=  1U;
  }

  if (USBH_CDC_ACM_SetControlLineState (instance, ctrl_lines) != usbOK) {
    return ARM_DRIVER_ERROR;
  }

  ptr_usart_cdc->ctrl_lines = ctrl_lines;

  return ARM_DRIVER_OK;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static int32_t ARM_USART0_SetModemControl (ARM_USART_MODEM_CONTROL control) { return (ARM_USARTx_SetModemControl (0U, control)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static int32_t ARM_USART1_SetModemControl (ARM_USART_MODEM_CONTROL control) { return (ARM_USARTx_SetModemControl (1U, control)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static int32_t ARM_USART2_SetModemControl (ARM_USART_MODEM_CONTROL control) { return (ARM_USARTx_SetModemControl (2U, control)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static int32_t ARM_USART3_SetModemControl (ARM_USART_MODEM_CONTROL control) { return (ARM_USARTx_SetModemControl (3U, control)); }
#endif

/// \fn          ARM_USART_MODEM_STATUS ARM_USARTx_GetModemStatus (uint8_t instance)
/// \brief       Get USART Modem Status lines state.
/// \param[in]   instance  Index of CDC instance
/// \return      modem status ARM_USART_MODEM_STATUS
static ARM_USART_MODEM_STATUS ARM_USARTx_GetModemStatus (uint8_t instance) {
  USART_CDC_t            *ptr_usart_cdc;
  ARM_USART_MODEM_STATUS  modem_status = { 0U, 0U, 0U, 0U, 0U };
  uint16_t                usart_status;

  if (CheckInstance (instance) == false) { return modem_status; }

  ptr_usart_cdc = &usart_cdc[instance];
  usart_status  = usart_cdc[instance].status;

  modem_status.cts =  ptr_usart_cdc->flow_control_cts;  // CTS state: 1=Active, 0=Inactive
  modem_status.dsr = (usart_status >> 1) & 1U;  // DSR state: 1=Active, 0=Inactive
  modem_status.dcd = (usart_status     ) & 1U;  // DCD state: 1=Active, 0=Inactive
  modem_status.ri  = (usart_status >> 3) & 1U;  // RI  state: 1=Active, 0=Inactive

  return modem_status;
}
#if   (USART_CDC_DRV_INSTANCE_NUM > 0U)
static ARM_USART_MODEM_STATUS ARM_USART0_GetModemStatus (void) { return (ARM_USARTx_GetModemStatus(0U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 1U)
static ARM_USART_MODEM_STATUS ARM_USART1_GetModemStatus (void) { return (ARM_USARTx_GetModemStatus(1U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 2U)
static ARM_USART_MODEM_STATUS ARM_USART2_GetModemStatus (void) { return (ARM_USARTx_GetModemStatus(2U)); }
#endif
#if   (USART_CDC_DRV_INSTANCE_NUM > 3U)
static ARM_USART_MODEM_STATUS ARM_USART3_GetModemStatus (void) { return (ARM_USARTx_GetModemStatus(3U)); }
#endif

#if (USART_CDC_DRV_INSTANCE_NUM > 0U)
// USART CDC Driver Control Instance 1st Block
extern 
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV0_NUM);
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV0_NUM) = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART0_Initialize,
    ARM_USART0_Uninitialize,
    ARM_USART0_PowerControl,
    ARM_USART0_Send,
    ARM_USART0_Receive,
    ARM_USART_Transfer,
    ARM_USART0_GetTxCount,
    ARM_USART0_GetRxCount,
    ARM_USART0_Control,
    ARM_USART0_GetStatus,
    ARM_USART0_SetModemControl,
    ARM_USART0_GetModemStatus
};
#endif
#if (USART_CDC_DRV_INSTANCE_NUM > 1U)
// USART CDC Driver Control Instance 2nd Block
extern 
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV1_NUM);
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV1_NUM) = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART1_Initialize,
    ARM_USART1_Uninitialize,
    ARM_USART1_PowerControl,
    ARM_USART1_Send,
    ARM_USART1_Receive,
    ARM_USART_Transfer,
    ARM_USART1_GetTxCount,
    ARM_USART1_GetRxCount,
    ARM_USART1_Control,
    ARM_USART1_GetStatus,
    ARM_USART1_SetModemControl,
    ARM_USART1_GetModemStatus
};
#endif
#if (USART_CDC_DRV_INSTANCE_NUM > 2U)
// USART CDC Driver Control Instance 3rd Block
extern 
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV2_NUM);
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV2_NUM) = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART2_Initialize,
    ARM_USART2_Uninitialize,
    ARM_USART2_PowerControl,
    ARM_USART2_Send,
    ARM_USART2_Receive,
    ARM_USART_Transfer,
    ARM_USART2_GetTxCount,
    ARM_USART2_GetRxCount,
    ARM_USART2_Control,
    ARM_USART2_GetStatus,
    ARM_USART2_SetModemControl,
    ARM_USART2_GetModemStatus
};
#endif
#if (USART_CDC_DRV_INSTANCE_NUM > 3U)
// USART CDC Driver Control Instance 4th Block
extern 
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV3_NUM);
ARM_DRIVER_USART USART_CDC_DRV(USART_CDC_DRV3_NUM) = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART3_Initialize,
    ARM_USART3_Uninitialize,
    ARM_USART3_PowerControl,
    ARM_USART3_Send,
    ARM_USART3_Receive,
    ARM_USART_Transfer,
    ARM_USART3_GetTxCount,
    ARM_USART3_GetRxCount,
    ARM_USART3_Control,
    ARM_USART3_GetStatus,
    ARM_USART3_SetModemControl,
    ARM_USART3_GetModemStatus
};
#endif
