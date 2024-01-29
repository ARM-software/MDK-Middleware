/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2013-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USART_PL2303.c
 * Purpose: CMSIS USART driver for USB Host with Prolific USB-to-UART Adapter
 * Rev.:    V1.10
 *----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
 * Driver:       Driver_USARTn
 * Project:      USART Driver for Prolific PL2303 USB to serial RS232 adapter
 * Settings:     USART_PL2303_DRV_NUM sets the driver number
 *               (default number value is 9)
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.10
 *    - Corrected compiler warnings
 *  Version 1.9
 *    - Corrected CMSIS-RTOS1 compatibility mode compilation error
 *  Version 1.8
 *    - Corrected CMSIS-RTOS2 compilation error
 *  Version 1.7
 *    - Corrected Receive and Send functions so they can now be called from 
 *      Received and Sent callbacks
 *    - Corrected Receive function so ARM_USART_EVENT_RECEIVE_COMPLETE event 
 *      is signaled after requested number of bytes was received
 *  Version 1.6
 *    - OS headers included depending on RTE settings
 *  Version 1.5
 *    - Corrected USBH_PL2303_GetRxCount function
 *  Version 1.4
 *    - Added support for CMSIS-RTOS2
 *  Version 1.3
 *    - Updated with custom class API changes
 *  Version 1.2
 *    - Improved compatibility with ARM Compiler 6
 *    - Corrected termination of USBH_PL2303_Status_Thread thread when adapter 
 *      is disconnected
 *  Version 1.0
 *    - Initial release
 */

#include <stdint.h>
#include <string.h>

#include "rl_usb.h"

#include "Driver_USART.h"

/************************** Constant and Macro Definitions ********************/

// Driver number used for Driver Control Block name
#ifndef USART_PL2303_DRV_NUM
#define USART_PL2303_DRV_NUM            9
#endif

#define USART_PL2303_DRV_(n)            Driver_USART##n
#define USART_PL2303_DRV(n)             USART_PL2303_DRV_(n)

// Send, Receive and Status thread settings
#ifndef USBH_PL2303_SEND_THREAD_STACK_SIZE
#define USBH_PL2303_SEND_THREAD_STACK_SIZE     (512U)
#endif
#ifndef USBH_PL2303_SEND_THREAD_PRIORITY
#define USBH_PL2303_SEND_THREAD_PRIORITY       (osPriorityNormal)
#endif
#ifndef USBH_PL2303_RECEIVE_THREAD_STACK_SIZE
#define USBH_PL2303_RECEIVE_THREAD_STACK_SIZE  (512U)
#endif
#ifndef USBH_PL2303_RECEIVE_THREAD_PRIORITY
#define USBH_PL2303_RECEIVE_THREAD_PRIORITY    (osPriorityNormal)
#endif
#ifndef USBH_PL2303_STATUS_THREAD_STACK_SIZE
#define USBH_PL2303_STATUS_THREAD_STACK_SIZE   (512U)
#endif
#ifndef USBH_PL2303_STATUS_THREAD_PRIORITY
#define USBH_PL2303_STATUS_THREAD_PRIORITY     (osPriorityNormal)
#endif

// Indexes of Pipes for PL2303 device
#define PL2303_STATUS_PIPE_INDEX               (0U)
#define PL2303_SEND_PIPE_INDEX                 (1U)
#define PL2303_RECEIVE_PIPE_INDEX              (2U)

// Thread flags
#define USBH_THREAD_EVENT_FLAG                 (1U)
#define USBH_THREAD_TERMINATE_FLAG             (2U)

/************************** Variable and Memory Definitions *******************/

extern uint8_t  USBH_CC_Device;                // Device used for USB transfers
extern USBH_PIPE_HANDLE USBH_CC_PipeHandle[];  // Pipe Handles

static volatile void    *usbh_receive_thread_id = NULL; // Receive (Bulk In) thread ID
static volatile void    *usbh_send_thread_id    = NULL; // Send (Bulk Out) thread ID
static volatile void    *usbh_status_thread_id  = NULL; // Receive status (Interrupt In) thread ID
static volatile bool     usbh_status_active;            // USB read of status activity state

static          uint8_t  usbh_receive_buf[512] __ALIGNED(4);    // USB receive buffer of size maximum endpoint size (512 bytes)
static          uint8_t *usbh_receive_buf_in    = NULL; // Pointer to first free byte in USB receive buffer
static          uint8_t *usbh_receive_buf_out   = NULL; // Pointer to first unread byte in USB receive buffer

static ARM_USART_SignalEvent_t usart_cb_event;          // Pointer to USART Signal Function

static          bool     usart_initialized = false;      // USART initialized flag
static          bool     usart_control_tx;              // USART Tx enable
static          bool     usart_control_rx;              // USART Rx enable
static          bool     usart_flow_control_rts;        // USART flow control on RTS line enable
static          bool     usart_flow_control_cts;        // USART flow control on CTS line enable
static          bool     usart_send_busy;               // Send in progress
static const    uint8_t *usart_send_data;               // Pointer to memory containing data to send
static          uint32_t usart_send_num;                // Number of data bytes to send
static          bool     usart_receive_busy;            // Reception in progress
static          uint8_t *usart_receive_data;            // Pointer to memory for data reception
static          uint32_t usart_receive_num;             // Number of data bytes left to receive
static          uint32_t usart_receive_rq_num;          // Number of data bytes requested to receive
static          uint16_t usart_ctrl_lines;              // Modem lines control
static          uint16_t usart_status;                  // Modem lines and error status
static          CDC_LINE_CODING usart_line_coding =  { 9600U, 0U, 0U , 8U };    // Initial line coding

/************************** Global Function Prototypes ************************/

#ifdef USB_CMSIS_RTOS2
static void USBH_PL2303_Send_Thread         (void *arg);
static void USBH_PL2303_Receive_Thread      (void *arg);
static void USBH_PL2303_Status_Thread       (void *arg);
#else
       void USBH_PL2303_Send_Thread         (void const *arg);
       void USBH_PL2303_Receive_Thread      (void const *arg);
       void USBH_PL2303_Status_Thread       (void const *arg);
#endif

uint32_t    USBH_PL2303_GetTxCount          (void);
uint32_t    USBH_PL2303_GetRxCount          (void);
usbStatus   USBH_PL2303_SetLineCoding       (const CDC_LINE_CODING *line_coding);
usbStatus   USBH_PL2303_GetLineCoding       (      CDC_LINE_CODING *line_coding);
usbStatus   USBH_PL2303_SetControlLineState (uint16_t state);
uint16_t    USBH_PL2303_GetStatus           (void);
usbStatus   USBH_PL2303_SendBreak           (uint16_t duration);

/************************** Internal Function Prototypes **********************/

static void ARM_USART_Sent                  (void);
static void ARM_USART_Received              (void);

/************************** Internal Functions ********************************/

/*------------------------- Internal Threads ---------------------------------*/

/// \brief User thread for handling data send (to Bulk Out endpoint)
/// \param[in]     arg                  unused.
#ifdef USB_CMSIS_RTOS2
static void USBH_PL2303_Send_Thread (void *arg) {
#else
       void USBH_PL2303_Send_Thread (void const *arg) {
#endif
#ifndef RTE_CMSIS_RTOS2
  osEvent   evt;
#endif
  uint32_t  flags;
  usbStatus status;

  (void)(arg);

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

    status = USBH_PipeSend (USBH_CC_PipeHandle[PL2303_SEND_PIPE_INDEX], usart_send_data, usart_send_num);

    if (status == usbOK) {
      usart_send_busy = false;
      ARM_USART_Sent();
    } else {
      // If USB transfer failed
      break;
    }
  }

  usart_send_busy     = false;
  usbh_send_thread_id = NULL;
  (void)osThreadTerminate (osThreadGetId());
}
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t        usbh_pl2303_send_thread_cb_mem                                                    __SECTION(.bss.os.thread.cb);
static uint64_t             usbh_pl2303_send_thread_stack_mem[(USBH_PL2303_SEND_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const osThreadAttr_t usbh_pl2303_send_thread_attr = {
  "USBH_PL2303_Send_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &usbh_pl2303_send_thread_cb_mem,
  sizeof(osRtxThread_t),
 &usbh_pl2303_send_thread_stack_mem[0U],
#else
  NULL,
  0U,
  NULL,
#endif
  (((USBH_PL2303_SEND_THREAD_STACK_SIZE + 7U) / 8U) * 8U),
  USBH_PL2303_SEND_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBH_PL2303_Send_Thread;
osThreadDef(USBH_PL2303_Send_Thread, USBH_PL2303_SEND_THREAD_PRIORITY, 1U, USBH_PL2303_SEND_THREAD_STACK_SIZE);
#endif

/// \brief User thread for handling data reception (from Bulk In endpoint)
/// \param[in]     arg                  unused.
#ifdef USB_CMSIS_RTOS2
static void USBH_PL2303_Receive_Thread (void *arg) {
#else
       void USBH_PL2303_Receive_Thread (void const *arg) {
#endif
#ifndef RTE_CMSIS_RTOS2
  osEvent   evt;
#endif
  uint32_t  flags, buf_len, cpy_len;
  usbStatus status;
  uint8_t   exit;

  (void)(arg);

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
    while (usart_receive_num != 0U) {

      // Check if there is already data received on USB
      if (usbh_receive_buf_in > usbh_receive_buf_out) {
        buf_len = (uint32_t)(usbh_receive_buf_in - usbh_receive_buf_out);
      } else {
        buf_len = 0U;
      }
      if (buf_len > usart_receive_num) {
        // If already available more data on USB then requested on UART,
        // limit it to UART requested
        cpy_len = usart_receive_num;
      } else {
        cpy_len = buf_len;
      }

      if (cpy_len != 0U) {
        // Copy available data from USB to UART requested data buffer
        memcpy(usart_receive_data, (void *)usbh_receive_buf_out, cpy_len);
        usbh_receive_buf_out += cpy_len;
        usart_receive_data   += cpy_len;
        usart_receive_num    -= cpy_len;
      }

      if (usart_receive_num == 0U) {
        // If all requested data was received
        usart_receive_busy = false;
        ARM_USART_Received();
        break;
      }

      if (usbh_receive_buf_in == usbh_receive_buf_out) {
        // If all data was not yet received and USB buffer is empty
        // Start new USB transfer
        usbh_receive_buf_in  = (uint8_t *)usbh_receive_buf;
        usbh_receive_buf_out = (uint8_t *)usbh_receive_buf;
        do {
          status = USBH_PipeReceive (USBH_CC_PipeHandle[PL2303_RECEIVE_PIPE_INDEX], (uint8_t *)usbh_receive_buf, 512U);
          if (status == usbTransferError) {
            // If USB transfer failed to start
            exit = 1U;
            break;
          }
          usbh_receive_buf_in += USBH_PipeReceiveGetResult (USBH_CC_PipeHandle[PL2303_RECEIVE_PIPE_INDEX]);
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

  usart_receive_busy     = false;
  usbh_receive_buf_in    = (uint8_t *)usbh_receive_buf;
  usbh_receive_buf_out   = (uint8_t *)usbh_receive_buf;
  usbh_receive_thread_id = NULL;
  (void)osThreadTerminate (osThreadGetId());
}
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t        usbh_pl2303_receive_thread_cb_mem                                                       __SECTION(.bss.os.thread.cb);
static uint64_t             usbh_pl2303_receive_thread_stack_mem[(USBH_PL2303_RECEIVE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const osThreadAttr_t usbh_pl2303_receive_thread_attr = {
  "USBH_PL2303_Receive_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &usbh_pl2303_receive_thread_cb_mem,
  sizeof(osRtxThread_t),
 &usbh_pl2303_receive_thread_stack_mem[0U],
#else
  NULL,
  0U,
  NULL,
#endif
  (((USBH_PL2303_RECEIVE_THREAD_STACK_SIZE + 7U) / 8U) * 8U),
  USBH_PL2303_RECEIVE_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBH_PL2303_Receive_Thread;
osThreadDef(USBH_PL2303_Receive_Thread, USBH_PL2303_RECEIVE_THREAD_PRIORITY, 1U, USBH_PL2303_RECEIVE_THREAD_STACK_SIZE);
#endif

/// \brief User thread for handling modem status and error feedback from PL2303
///        device (from Interrupt In endpoint)
/// \param[in]     arg              unused.
#ifdef USB_CMSIS_RTOS2
static void USBH_PL2303_Status_Thread (void *arg) {
#else
       void USBH_PL2303_Status_Thread (void const *arg) {
#endif
  usbStatus status;
  uint8_t   buf[10];

  (void)(arg);

  usbh_status_active = true;

  for (;;) {
    status = USBH_PipeReceive (USBH_CC_PipeHandle[PL2303_STATUS_PIPE_INDEX], buf, 10U);
    if (status == usbOK) {
      // Data has been received on Interrupt In Pipe
      if ((buf[0] == 0xA1U)                         &&
          (buf[1] == CDC_NOTIFICATION_SERIAL_STATE) &&
          (buf[2] == 0x00U)                         &&
          (buf[3] == 0x00U)                         &&
          (buf[4] == 0x00U)                         &&
          (buf[5] == 0x00U)                         &&
          (buf[6] == 0x02U)                         &&
          (buf[7] == 0x00U)) {
        // If this is Notification store new modem line and error status
        usart_status = (uint16_t)(((uint16_t)(buf[9]) << 8) | (uint16_t)(buf[8]));
      }
    } else if (status != usbTimeout) {
      usbh_status_active    = false;
      usbh_status_thread_id = NULL;
      (void)osThreadTerminate (osThreadGetId());
      break;
    }
  }
}
#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t        usbh_pl2303_status_thread_cb_mem                                                      __SECTION(.bss.os.thread.cb);
static uint64_t             usbh_pl2303_status_thread_stack_mem[(USBH_PL2303_STATUS_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const osThreadAttr_t usbh_pl2303_status_thread_attr = {
  "USBH_PL2303_Status_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &usbh_pl2303_status_thread_cb_mem,
  sizeof(osRtxThread_t),
 &usbh_pl2303_status_thread_stack_mem[0U],
#else
  NULL,
  0U,
  NULL,
#endif
  (((USBH_PL2303_STATUS_THREAD_STACK_SIZE + 7U) / 8U) * 8U),
  USBH_PL2303_STATUS_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBH_PL2303_Status_Thread;
osThreadDef(USBH_PL2303_Status_Thread, USBH_PL2303_STATUS_THREAD_PRIORITY, 1U, USBH_PL2303_STATUS_THREAD_STACK_SIZE);
#endif

/***-------------- PL2303 Specific Functions -------------------------------***/

/// \brief Get result of send data to PL2303 device
/// \return                             number of successfully sent data bytes.
uint32_t USBH_PL2303_GetTxCount (void) {
  return (USBH_PipeSendGetResult (USBH_CC_PipeHandle[PL2303_SEND_PIPE_INDEX]));
}

/// \brief Get result of receive data from PL2303 device
/// \return                             number of successfully received data bytes.
uint32_t USBH_PL2303_GetRxCount (void) {
  return (usart_receive_rq_num - usart_receive_num);
}
/// \brief Change communication settings of PL2303 device
/// \param[in]     line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_PL2303_SetLineCoding (const CDC_LINE_CODING *line_coding) {
  USB_SETUP_PACKET setup_packet;

  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.bmRequestType.Type     = USB_REQUEST_CLASS;
  setup_packet.bmRequestType.Recipient= USB_REQUEST_TO_INTERFACE;
  setup_packet.bRequest               = CDC_SET_LINE_CODING;
  setup_packet.wValue                 = U16_LE(0U);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(sizeof(CDC_LINE_CODING));

  return (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, (uint8_t *)((uint32_t)line_coding), sizeof(CDC_LINE_CODING)));
}

/// \brief Retrieve communication settings of PL2303 device
/// \param[out]    line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_PL2303_GetLineCoding (CDC_LINE_CODING *line_coding) {

  *line_coding = usart_line_coding;

  return usbOK;
}

/// \brief Set control line states of PL2303 device
/// \param[in]     state                control line settings bitmap.
///                                       - bit 0: DTR state
///                                       - bit 1: RTS state
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_PL2303_SetControlLineState (uint16_t state) {
  USB_SETUP_PACKET setup_packet;

  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.bmRequestType.Type     = USB_REQUEST_CLASS;
  setup_packet.bmRequestType.Recipient= USB_REQUEST_TO_INTERFACE;
  setup_packet.bRequest               = CDC_SET_CONTROL_LINE_STATE;
  setup_packet.wValue                 = U16_LE(state);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(0U);

  return (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U));
}

/// \brief Get PL2303 device modem line and error status
/// \return                             error status and line states:
///                                       - bit 6: bOverRun
///                                       - bit 5: bParity
///                                       - bit 4: bFraming
///                                       - bit 3: bRingSignal
///                                       - bit 2: bBreak
///                                       - bit 1: bTxCarrier (DSR line state)
///                                       - bit 0: bRxCarrier (DCD line state)
uint16_t USBH_PL2303_GetStatus (void) {
  return usart_status;
}

/// \brief Send break on PL2303 device
/// \param[in]     duration             duration of break (in milliseconds)
///                                       - value 0xFFFF: indefinite
///                                       - value      0: immediate
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_PL2303_SendBreak (uint16_t duration) {
  USB_SETUP_PACKET setup_packet;

  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.bmRequestType.Type     = USB_REQUEST_CLASS;
  setup_packet.bmRequestType.Recipient= USB_REQUEST_TO_INTERFACE;
  setup_packet.bRequest               = CDC_SEND_BREAK;
  setup_packet.wValue                 = U16_LE(duration);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(0U);

  return (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U));
}

/************************** USART Driver **************************************/

#define ARM_USART_DRV_VERSION           ARM_DRIVER_VERSION_MAJOR_MINOR(1,10)

// Driver Version
static const ARM_DRIVER_VERSION         usart_driver_version = { ARM_USART_API_VERSION, ARM_USART_DRV_VERSION };

// USB to serial RS232 adapter Capabilities
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

/// \fn          int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t cb_event)
/// \brief       Initialize USART Interface.
/// \param[in]   cb_event  Pointer to ARM_USART_SignalEvent
/// \return      execution status
static int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t cb_event) {
  usart_cb_event    = cb_event;
  usart_initialized = true;

  return ARM_DRIVER_OK;
}

/// \fn          int32_t ARM_USART_Uninitialize (void)
/// \brief       De-initialize USART Interface.
/// \return      execution status
static int32_t ARM_USART_Uninitialize (void) {

  usart_initialized = false;

  return ARM_DRIVER_OK;
}

/// \fn          int32_t ARM_USART_PowerControl (ARM_POWER_STATE state)
/// \brief       Control USART Interface Power.
/// \param[in]   state  Power state
/// \return      execution status
static int32_t ARM_USART_PowerControl (ARM_POWER_STATE state) {
  switch (state) {
    case ARM_POWER_OFF:
      if (usbh_status_active) {
        (void)USBH_PipeAbort (USBH_CC_PipeHandle[PL2303_STATUS_PIPE_INDEX]);
      }
      while (usbh_status_thread_id != NULL) { (void)osDelay(10U); }
      break;

    case ARM_POWER_FULL:
      if (!usart_initialized) { return ARM_DRIVER_ERROR; }
      if (USBH_CustomClass_GetStatus (0U) != usbOK) {   // If device is not connected or not enumerated
        return ARM_DRIVER_ERROR;
      }
      usbh_status_active = false;

      // Start Interrupt In Pipe handling thread
#ifdef USB_CMSIS_RTOS2
      usbh_status_thread_id = osThreadNew (USBH_PL2303_Status_Thread, NULL, &usbh_pl2303_status_thread_attr);
#else
      usbh_status_thread_id = osThreadCreate (osThread(USBH_PL2303_Status_Thread), NULL);
#endif
      if (usbh_status_thread_id == NULL) {
        return ARM_DRIVER_ERROR;
      }
      while (!usbh_status_active) { (void)osDelay(10U); }
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/// \fn          int32_t ARM_USART_Send (const void *data, uint32_t num)
/// \brief       Start sending data to USART transmitter.
/// \param[in]   data  Pointer to buffer with data to send to USART transmitter
/// \param[in]   num   Number of data items to send
/// \return      execution status
static int32_t ARM_USART_Send (const void *data, uint32_t num) {

  if (!usart_control_tx) {              // If Tx disabled
    return ARM_DRIVER_ERROR;  
  }

  if (usart_send_busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  usart_send_busy = true;

  usart_send_data = (const uint8_t *)data;
  usart_send_num  = num;

  // Start send thread if it is not already running
  if (usbh_send_thread_id == NULL) {
#ifdef USB_CMSIS_RTOS2
    usbh_send_thread_id = osThreadNew (USBH_PL2303_Send_Thread, NULL, &usbh_pl2303_send_thread_attr);
#else
    usbh_send_thread_id = osThreadCreate (osThread(USBH_PL2303_Send_Thread), NULL);
#endif
    if (usbh_send_thread_id == NULL) {
      usart_send_busy = false;
      return ARM_DRIVER_ERROR;
    }
  }

  // Here USB send thread is active, send event to USB send thread
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet((osThreadId_t)((uint32_t)usbh_send_thread_id), USBH_THREAD_EVENT_FLAG);
#else
  (void)osSignalSet     ((osThreadId)  ((uint32_t)usbh_send_thread_id), USBH_THREAD_EVENT_FLAG);
#endif

  return ARM_DRIVER_OK;
}

/// \fn          void ARM_USART_Sent (void)
/// \brief       All data was sent on USART transmitter.
static void ARM_USART_Sent (void) {

  if (usart_cb_event != 0U) {
    usart_cb_event (ARM_USART_EVENT_SEND_COMPLETE);
  }
}

/// \fn          int32_t ARM_USART_Receive (void *data, uint32_t num)
/// \brief       Start receiving data from USART receiver.
/// \param[out]  data  Pointer to buffer for data to receive from USART receiver
/// \param[in]   num   Number of data items to receive
/// \return      execution status
static int32_t ARM_USART_Receive (void *data, uint32_t num) {

  if (!usart_control_rx) {              // If Rx disabled
    return ARM_DRIVER_ERROR;
  }

  if (usart_receive_busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  usart_receive_busy = true;

  usart_receive_data   = data;
  usart_receive_num    = num;
  usart_receive_rq_num = num;

  // Start reception thread if it is not already running
  if (usbh_receive_thread_id == NULL) {
#ifdef USB_CMSIS_RTOS2
    usbh_receive_thread_id = osThreadNew (USBH_PL2303_Receive_Thread, NULL, &usbh_pl2303_receive_thread_attr);
#else
    usbh_receive_thread_id = osThreadCreate (osThread(USBH_PL2303_Receive_Thread), NULL);
#endif
    if (usbh_receive_thread_id == NULL) {
      usart_receive_busy = false;
      return ARM_DRIVER_ERROR;
    }
  }

  // Here USB receive thread is active, send event to USB receive thread
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet((osThreadId_t)((uint32_t)usbh_receive_thread_id), USBH_THREAD_EVENT_FLAG);
#else
  (void)osSignalSet     ((osThreadId)  ((uint32_t)usbh_receive_thread_id), USBH_THREAD_EVENT_FLAG);
#endif

  return ARM_DRIVER_OK;
}

/// \fn          void ARM_USART_Received (void)
/// \brief       All data was received on USART receiver.
static void ARM_USART_Received (void) {

  if (usart_cb_event != 0U) {
    usart_cb_event (ARM_USART_EVENT_RECEIVE_COMPLETE);
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

/// \fn          uint32_t ARM_USART_GetTxCount (void)
/// \brief       Get transmitted data count.
/// \return      number of data items transmitted
static uint32_t ARM_USART_GetTxCount (void) {
  return (USBH_PL2303_GetTxCount());
}

/// \fn          uint32_t ARM_USART_GetRxCount (void)
/// \brief       Get received data count.
/// \return      number of data items received
static uint32_t ARM_USART_GetRxCount (void) {
  return (USBH_PL2303_GetRxCount());
}

/// \fn          int32_t ARM_USART_Control (uint32_t control, uint32_t arg)
/// \brief       Control USART Interface.
/// \param[in]   control  Operation
/// \param[in]   arg      Argument of operation (optional)
/// \return      common execution status and driver specific usart_execution status
static int32_t ARM_USART_Control (uint32_t control, uint32_t arg) {
  CDC_LINE_CODING  line_coding;

  if (!usart_initialized) { return ARM_DRIVER_ERROR; }

  line_coding = usart_line_coding;

  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS:
      break;

    case ARM_USART_CONTROL_TX:                  // Control Tx
      if (arg != 0U) {
        usart_control_tx = true;                // Tx enabled
      } else {
        usart_control_tx = false;               // Tx disable
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_RX:                  // Control Rx
      if (arg != 0U) { 
        usart_control_rx = true;                // Rx enabled
      } else {
        usart_control_rx = false;               // Rx disable
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_BREAK:               // Control break
      if (arg) {                                // Break enabled
        if (USBH_PL2303_SendBreak (65535U) != usbOK) {
          return ARM_DRIVER_ERROR;
        }
      } else {                                  // Break disable
        if (USBH_PL2303_SendBreak (0U) != usbOK) {
          return ARM_DRIVER_ERROR;
        }
      }
      return ARM_DRIVER_OK;

    case ARM_USART_ABORT_SEND:                  // Abort send
      if (usbh_send_thread_id != NULL) {
#ifdef USB_CMSIS_RTOS2
        (void)osThreadFlagsSet((osThreadId_t)((uint32_t)usbh_send_thread_id), USBH_THREAD_TERMINATE_FLAG);
#else
        (void)osSignalSet     ((osThreadId)  ((uint32_t)usbh_send_thread_id), USBH_THREAD_TERMINATE_FLAG);
#endif
        (void)USBH_PipeAbort  (USBH_CC_PipeHandle[PL2303_SEND_PIPE_INDEX]);
      }
      while (usbh_send_thread_id != NULL) { (void)osDelay(10U); }
      return ARM_DRIVER_OK;

    case ARM_USART_ABORT_RECEIVE:               // Abort receive
      if (usbh_receive_thread_id != NULL) {
#ifdef USB_CMSIS_RTOS2
        (void)osThreadFlagsSet((osThreadId_t)((uint32_t)usbh_receive_thread_id), USBH_THREAD_TERMINATE_FLAG);
#else
        (void)osSignalSet     ((osThreadId)  ((uint32_t)usbh_receive_thread_id), USBH_THREAD_TERMINATE_FLAG);
#endif
        (void)USBH_PipeAbort  (USBH_CC_PipeHandle[PL2303_RECEIVE_PIPE_INDEX]);
      }
      while (usbh_receive_thread_id != NULL) { (void)osDelay(10U); }
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
      usart_flow_control_rts = false;
      usart_flow_control_cts = false;
      break;
    case ARM_USART_FLOW_CONTROL_RTS:
      usart_flow_control_rts = true;
      break;
    case ARM_USART_FLOW_CONTROL_CTS:
      usart_flow_control_cts = true;
      break;
    case ARM_USART_FLOW_CONTROL_RTS_CTS:
      usart_flow_control_rts = true;
      usart_flow_control_cts = true;
      break;
    default:
     return ARM_USART_ERROR_FLOW_CONTROL;
  }

  line_coding.dwDTERate = arg;          // USART Baudrate

  if (USBH_PL2303_SetLineCoding (&line_coding) != usbOK) {
    return ARM_DRIVER_ERROR;
  }

  usart_line_coding = line_coding;

  return ARM_DRIVER_OK;
}

/// \fn          ARM_USART_STATUS ARM_USART_GetStatus (void)
/// \brief       Get USART status.
/// \return      USART status ARM_USART_STATUS
static ARM_USART_STATUS ARM_USART_GetStatus (void) {
  ARM_USART_STATUS status;

  status.tx_busy          =  usart_send_busy;           // Transmitter busy flag
  status.rx_busy          =  usart_receive_busy;        // Receiver busy flag
  status.tx_underflow     =  0U;                        // Transmit data underflow detected (cleared on start of next send operation)
  status.rx_overflow      = (usart_status >> 6) & 1U;   // Receive data overflow detected (cleared on start of next receive operation)
  status.rx_break         = (usart_status >> 2) & 1U;   // Break detected on receive (cleared on start of next receive operation)
  status.rx_framing_error = (usart_status >> 4) & 1U;   // Framing error detected on receive (cleared on start of next receive operation)
  status.rx_parity_error  = (usart_status >> 5) & 1U;   // Parity error detected on receive (cleared on start of next receive operation)

  return status;
}

/// \fn          int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL control)
/// \brief       Set USART Modem Control line state.
/// \param[in]   control  ARM_USART_MODEM_CONTROL
/// \return      execution status
static int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL control) {
  uint16_t  ctrl_lines;

  ctrl_lines = usart_ctrl_lines;

  if (usart_flow_control_rts) {         // If flow control on RTS line enabled
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

  if (USBH_PL2303_SetControlLineState (ctrl_lines) != usbOK) {
    return ARM_DRIVER_ERROR;
  }

  usart_ctrl_lines = ctrl_lines;

  return ARM_DRIVER_OK;
}

/// \fn          ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (void)
/// \brief       Get USART Modem Status lines state.
/// \return      modem status ARM_USART_MODEM_STATUS
static ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (void) {
  ARM_USART_MODEM_STATUS modem_status;

  modem_status.cts =  usart_flow_control_cts;   // CTS state: 1=Active, 0=Inactive
  modem_status.dsr = (usart_status >> 1) & 1U;  // DSR state: 1=Active, 0=Inactive
  modem_status.dcd = (usart_status     ) & 1U;  // DCD state: 1=Active, 0=Inactive
  modem_status.ri  = (usart_status >> 3) & 1U;  // RI  state: 1=Active, 0=Inactive

  return modem_status;
}

// USB to serial RS232 adapter Driver Control Block
extern 
ARM_DRIVER_USART USART_PL2303_DRV(USART_PL2303_DRV_NUM);
ARM_DRIVER_USART USART_PL2303_DRV(USART_PL2303_DRV_NUM) = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART_Initialize,
    ARM_USART_Uninitialize,
    ARM_USART_PowerControl,
    ARM_USART_Send,
    ARM_USART_Receive,
    ARM_USART_Transfer,
    ARM_USART_GetTxCount,
    ARM_USART_GetRxCount,
    ARM_USART_Control,
    ARM_USART_GetStatus,
    ARM_USART_SetModemControl,
    ARM_USART_GetModemStatus
};
