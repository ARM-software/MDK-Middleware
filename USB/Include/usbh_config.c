/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config.c
 * Purpose: USB Host (0..3) Configuration
 *----------------------------------------------------------------------------*/

#include "usbh_config.h"

/*------------------------------------------------------------------------------
 *      USB Host Configuration calculated macros
 *----------------------------------------------------------------------------*/

#define USBH_HC_NUM            (USBH0_HC + USBH1_HC + USBH2_HC + USBH3_HC)
#define USBH_DCD_NUM          ((USBH_MSC_NUM > 0) + (USBH_HID_NUM > 0) + (USBH_CDC_NUM > 0) + (USBH_CUSTOM_CLASS_NUM > 0))
#define USBH_DEV_NUM           (USBH_MSC_NUM      +  USBH_HID_NUM      +  USBH_CDC_NUM      +  USBH_CUSTOM_CLASS_NUM     )

/*------------------------------------------------------------------------------
 *      USB Host Macros
 *----------------------------------------------------------------------------*/

#define USBHn_DRIVER_(n)        Driver_USBH##n
#define USBHn_DRIVER(n)         USBHn_DRIVER_(n)

/*------------------------------------------------------------------------------
 *      USB Host externals
 *----------------------------------------------------------------------------*/

#ifdef  USBH0_HC_NUM
extern  ARM_DRIVER_USBH         USBHn_DRIVER(USBH0_HC_NUM);
#endif

#ifdef  USBH1_HC_NUM
extern  ARM_DRIVER_USBH         USBHn_DRIVER(USBH1_HC_NUM);
#endif

#ifdef  USBH2_HC_NUM
extern  ARM_DRIVER_USBH         USBHn_DRIVER(USBH2_HC_NUM);
#endif

#ifdef  USBH3_HC_NUM
extern  ARM_DRIVER_USBH         USBHn_DRIVER(USBH3_HC_NUM);
#endif

extern  void USBH_SignalPortEvent (uint8_t ctrl, uint8_t port, uint32_t event);
extern  void USBH_SignalPipeEvent (uint8_t ctrl, ARM_USBH_EP_HANDLE ep_hndl, uint32_t event);

/*------------------------------------------------------------------------------
 *      USB Host 0 variables declaration/definition
 *----------------------------------------------------------------------------*/

#ifdef  USBH0_HC_NUM

static  const uint8_t           usbh0_power =          (uint8_t)(USBH0_HC_POWER / 2);
static  const uint32_t          usbh0_pipe_num =        USBH0_HC_PIPE_NUM;
static  const uint32_t          usbh0_mem_pool_size =   USBH0_HC_MEM_POOL_SIZE;
static  const uint8_t           usbh0_mem_pool_reloc =  USBH0_HC_MEM_POOL_RELOC;
        USBH_PIPE               usbh0_pipe             [USBH0_HC_PIPE_NUM];
static  uint32_t                usbh0_mem_pool         [USBH0_HC_MEM_POOL_SIZE / 4] __attribute__((section(".driver.usbh0")));
static  ARM_DRIVER_USBH        *usbh0_hcd_ptr  =       &USBHn_DRIVER(USBH0_HC_NUM);

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh0_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh0_core_thread_stack_mem[(USBH0_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const  osThreadAttr_t usbh0_core_thread_attr = {
  "USBH0_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh0_core_thread_cb_mem,
  sizeof(usbh0_core_thread_cb_mem),
  &usbh0_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH0_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH0_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif

#endif

/*------------------------------------------------------------------------------
 *      USB Host 1 variables declaration/definition
 *----------------------------------------------------------------------------*/

#ifdef  USBH1_HC_NUM

static  const uint8_t           usbh1_power =          (uint8_t)(USBH1_HC_POWER / 2);
static  const uint32_t          usbh1_pipe_num =        USBH1_HC_PIPE_NUM;
static  const uint32_t          usbh1_mem_pool_size =   USBH1_HC_MEM_POOL_SIZE;
static  const uint8_t           usbh1_mem_pool_reloc =  USBH1_HC_MEM_POOL_RELOC;
        USBH_PIPE               usbh1_pipe             [USBH1_HC_PIPE_NUM];
static  uint32_t                usbh1_mem_pool         [USBH1_HC_MEM_POOL_SIZE / 4] __attribute__((section(".driver.usbh1")));
static  ARM_DRIVER_USBH        *usbh1_hcd_ptr  =       &USBHn_DRIVER(USBH1_HC_NUM);

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh1_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh1_core_thread_stack_mem[(USBH1_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const  osThreadAttr_t usbh1_core_thread_attr = {
  "USBH1_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh1_core_thread_cb_mem,
  sizeof(usbh1_core_thread_cb_mem),
  &usbh1_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH1_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH1_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif

#endif

/*------------------------------------------------------------------------------
 *      USB Host 2 variables declaration/definition
 *----------------------------------------------------------------------------*/

#ifdef  USBH2_HC_NUM

static  const uint8_t           usbh2_power =          (uint8_t)(USBH2_HC_POWER / 2);
static  const uint32_t          usbh2_pipe_num =        USBH2_HC_PIPE_NUM;
static  const uint32_t          usbh2_mem_pool_size =   USBH2_HC_MEM_POOL_SIZE;
static  const uint8_t           usbh2_mem_pool_reloc =  USBH2_HC_MEM_POOL_RELOC;
        USBH_PIPE               usbh2_pipe             [USBH2_HC_PIPE_NUM];
static  uint32_t                usbh2_mem_pool         [USBH2_HC_MEM_POOL_SIZE / 4] __attribute__((section(".driver.usbh2")));
static  ARM_DRIVER_USBH        *usbh2_hcd_ptr  =       &USBHn_DRIVER(USBH2_HC_NUM);

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh2_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh2_core_thread_stack_mem[(USBH2_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const  osThreadAttr_t usbh2_core_thread_attr = {
  "USBH2_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh2_core_thread_cb_mem,
  sizeof(usbh2_core_thread_cb_mem),
  &usbh2_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH2_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH2_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif

#endif

/*------------------------------------------------------------------------------
 *      USB Host 3 variables declaration/definition
 *----------------------------------------------------------------------------*/

#ifdef  USBH3_HC_NUM

static  const uint8_t           usbh3_power =          (uint8_t)(USBH3_HC_POWER / 2);
static  const uint32_t          usbh3_pipe_num =        USBH3_HC_PIPE_NUM;
static  const uint32_t          usbh3_mem_pool_size =   USBH3_HC_MEM_POOL_SIZE;
static  const uint8_t           usbh3_mem_pool_reloc =  USBH3_HC_MEM_POOL_RELOC;
        USBH_PIPE               usbh3_pipe             [USBH3_HC_PIPE_NUM];
static  uint32_t                usbh3_mem_pool         [USBH3_HC_MEM_POOL_SIZE / 4] __attribute__((section(".driver.usbh3")));
static  ARM_DRIVER_USBH        *usbh3_hcd_ptr  =       &USBHn_DRIVER(USBH3_HC_NUM);

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh3_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh3_core_thread_stack_mem[(USBH3_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
static const  osThreadAttr_t usbh3_core_thread_attr = {
  "USBH3_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh3_core_thread_cb_mem,
  sizeof(usbh3_core_thread_cb_mem),
  &usbh3_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH3_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH3_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif

#endif

/*------------------------------------------------------------------------------
 *      USB Host OS abstraction
 *----------------------------------------------------------------------------*/

#ifdef USB_CMSIS_RTOS2
  #include "usbh_cmsis_rtos2.c"
#else
  #error USB Host component requires CMSIS-RTOS2!
#endif

/*------------------------------------------------------------------------------
 *      USB Host 0..3 Configurations
 *----------------------------------------------------------------------------*/

                                        // Define constants
const   uint8_t usbh_hc_num  =  USBH_HC_NUM;
const   uint8_t usbh_dcd_num =  USBH_DCD_NUM;
const   uint8_t usbh_dev_num =  USBH_DEV_NUM;
const   uint8_t usbh_msc_num =  USBH_MSC_NUM;
const   uint8_t usbh_hid_num =  USBH_HID_NUM;
const   uint8_t usbh_cdc_num =  USBH_CDC_NUM;
const   uint8_t usbh_cls_num =  USBH_CUSTOM_CLASS_NUM;

const   uint8_t          * const usbh_power_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
       &usbh0_power
#endif
#ifdef  RTE_USB_Host_1
     , &usbh1_power
#endif
#ifdef  RTE_USB_Host_2
     , &usbh2_power
#endif
#ifdef  RTE_USB_Host_3
     , &usbh3_power
#endif
};

const   uint8_t          * const usbh_mem_pool_reloc_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
       &usbh0_mem_pool_reloc
#endif
#ifdef  RTE_USB_Host_1
     , &usbh1_mem_pool_reloc
#endif
#ifdef  RTE_USB_Host_2
     , &usbh2_mem_pool_reloc
#endif
#ifdef  RTE_USB_Host_3
     , &usbh3_mem_pool_reloc
#endif
};

        uint32_t       * const usbh_mem_pool_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
        usbh0_mem_pool
#endif
#ifdef  RTE_USB_Host_1
      , usbh1_mem_pool
#endif
#ifdef  RTE_USB_Host_2
      , usbh2_mem_pool
#endif
#ifdef  RTE_USB_Host_3
      , usbh3_mem_pool
#endif
};

const   uint32_t       * const usbh_mem_pool_size_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
       &usbh0_mem_pool_size
#endif
#ifdef  RTE_USB_Host_1
     , &usbh1_mem_pool_size
#endif
#ifdef  RTE_USB_Host_2
     , &usbh2_mem_pool_size
#endif
#ifdef  RTE_USB_Host_3
     , &usbh3_mem_pool_size
#endif
};

        USBH_PIPE        * const usbh_pipe_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
        usbh0_pipe
#endif
#ifdef  RTE_USB_Host_1
      , usbh1_pipe
#endif
#ifdef  RTE_USB_Host_2
      , usbh2_pipe
#endif
#ifdef  RTE_USB_Host_3
      , usbh3_pipe
#endif
};

const   uint32_t         * const usbh_pipe_num_ptr [USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
       &usbh0_pipe_num
#endif
#ifdef  RTE_USB_Host_1
     , &usbh1_pipe_num
#endif
#ifdef  RTE_USB_Host_2
     , &usbh2_pipe_num
#endif
#ifdef  RTE_USB_Host_3
     , &usbh3_pipe_num
#endif
};

                                        // Register Host Controller Drivers
        ARM_DRIVER_USBH ** const usbh_hcd_ptr[USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
       &usbh0_hcd_ptr
#endif
#ifdef  RTE_USB_Host_1
     , &usbh1_hcd_ptr
#endif
#ifdef  RTE_USB_Host_2
     , &usbh2_hcd_ptr
#endif
#ifdef  RTE_USB_Host_3
      , &usbh3_hcd_ptr
#endif
};

#define USBH_DEBOUNCE_IN_MS             100

const   uint16_t usbh_debounce_in_ms[USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
        USBH_DEBOUNCE_IN_MS
#endif
#ifdef  RTE_USB_Host_1
      , USBH_DEBOUNCE_IN_MS
#endif
#ifdef  RTE_USB_Host_2
      , USBH_DEBOUNCE_IN_MS
#endif
#ifdef  RTE_USB_Host_3
      , USBH_DEBOUNCE_IN_MS
#endif
};

void   *usbh_core_thread_id[USBH_HC_NUM];

                                        // Define semaphores
void *usbh_driver_semaphore_id [USBH_HC_NUM];

                                        // Define mutexes
void *usbh_def_pipe_mutex_id [USBH_HC_NUM];

void *usbh_debounce_timer_id[USBH_HC_NUM];

#ifdef  RTE_USB_Host_0
        void USBH0_SignalPortEvent      (uint8_t port, uint32_t event);
        void USBH0_SignalPortEvent      (uint8_t port, uint32_t event)                      { USBH_SignalPortEvent (0, port, event);      }
        void USBH0_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event);
        void USBH0_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)    { USBH_SignalPipeEvent (0, pipe_hndl, event); }
#endif
#ifdef  RTE_USB_Host_1
        void USBH1_SignalPortEvent      (uint8_t port, uint32_t event);
        void USBH1_SignalPortEvent      (uint8_t port, uint32_t event)                      { USBH_SignalPortEvent (1, port, event);      }
        void USBH1_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event);
        void USBH1_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)    { USBH_SignalPipeEvent (1, pipe_hndl, event); }
#endif
#ifdef  RTE_USB_Host_2
        void USBH2_SignalPortEvent      (uint8_t port, uint32_t event);
        void USBH2_SignalPortEvent      (uint8_t port, uint32_t event)                      { USBH_SignalPortEvent (2, port, event);      }
        void USBH2_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event);
        void USBH2_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)    { USBH_SignalPipeEvent (2, pipe_hndl, event); }
#endif
#ifdef  RTE_USB_Host_3
        void USBH3_SignalPortEvent      (uint8_t port, uint32_t event);
        void USBH3_SignalPortEvent      (uint8_t port, uint32_t event)                      { USBH_SignalPortEvent (3, port, event);      }
        void USBH3_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event);
        void USBH3_SignalPipeEvent      (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)    { USBH_SignalPipeEvent (3, pipe_hndl, event); }
#endif

#if    (USBH_HC_NUM)
        ARM_USBH_SignalPortEvent_t const usbh_cb_port_event_ptr[USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
        USBH0_SignalPortEvent
#endif
#ifdef  RTE_USB_Host_1
      , USBH1_SignalPortEvent
#endif
#ifdef  RTE_USB_Host_2
      , USBH2_SignalPortEvent
#endif
#ifdef  RTE_USB_Host_3
      , USBH3_SignalPortEvent
#endif
};

        ARM_USBH_SignalPipeEvent_t const usbh_cb_pipe_event_ptr[USBH_HC_NUM] = {
#ifdef  RTE_USB_Host_0
        USBH0_SignalPipeEvent
#endif
#ifdef  RTE_USB_Host_1
      , USBH1_SignalPipeEvent
#endif
#ifdef  RTE_USB_Host_2
      , USBH2_SignalPipeEvent
#endif
#ifdef  RTE_USB_Host_3
      , USBH3_SignalPipeEvent
#endif
};

        ARM_DRIVER_VERSION      usbh_drv_version [USBH_HC_NUM];
        ARM_USBH_CAPABILITIES   usbh_capabilities[USBH_HC_NUM];

        USBH_HC                 usbh_hc  [USBH_HC_NUM];
#endif

        USBH_DEV                usbh_dev [USBH_DEV_NUM];

#if    (USBH_MSC_NUM != 0)
        USBH_MSC                usbh_msc [USBH_MSC_NUM];
#else
uint8_t   USBH_MSC_Configure            (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) { (void)device; (void)ptr_dev_desc; (void)ptr_cfg_desc; return 254U; }
usbStatus USBH_MSC_Unconfigure          (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_MSC_Initialize_Lib       (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_MSC_Uninitialize_Lib     (uint8_t instance) { (void)instance; return usbDeviceError; }
#endif

#if    (USBH_HID_NUM != 0)
        USBH_HID                usbh_hid [USBH_HID_NUM];
#else
uint8_t   USBH_HID_Configure            (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) { (void)device; (void)ptr_dev_desc; (void)ptr_cfg_desc; return 254U; }
usbStatus USBH_HID_Unconfigure          (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_HID_Initialize_Lib       (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_HID_Uninitialize_Lib     (uint8_t instance) { (void)instance; return usbDeviceError; }
#endif

#if    (USBH_CDC_NUM != 0)
        USBH_CDC                usbh_cdc [USBH_CDC_NUM];
#else
uint8_t   USBH_CDC_Configure            (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) { (void)device; (void)ptr_dev_desc; (void)ptr_cfg_desc; return 254U; }
usbStatus USBH_CDC_Unconfigure          (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_CDC_Initialize_Lib       (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_CDC_Uninitialize_Lib     (uint8_t instance) { (void)instance; return usbDeviceError; }
#endif

#if    (USBH_CUSTOM_CLASS_NUM == 0)
uint8_t   USBH_CustomClass_Configure    (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) { (void)device; (void)ptr_dev_desc; (void)ptr_cfg_desc; return 254U; }
usbStatus USBH_CustomClass_Unconfigure  (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_CustomClass_Initialize   (uint8_t instance) { (void)instance; return usbDeviceError; }
usbStatus USBH_CustomClass_Uninitialize (uint8_t instance) { (void)instance; return usbDeviceError; }
#endif
