# Resource Requirements {#usb_resource_requirements}

This section describes the resource requirements of the USB component.

## USB Device Resource Requirements {#usbd_res_req}

The following section documents the requirements for the **USB Device** component. The actual requirements depend on the
components used in the application and the configuration of these components.

### Stack Requirements {#usbd_stack_req}

The USB Device Core receives events sent from the interrupt service routine (ISR) of the **USB Device Driver**.
The stack requirements for the ISR are typically less than 512 Bytes. The total stack space required for ISR depends
on the interrupt nesting and therefore on the priority settings of these ISR.

| Option (under section Stack Configuration)                        | Increase Value by
| :---------------------------------------------------------------- | :----------------------
| Stack Size (in Bytes)                                             | + 512 for USB Device Driver

\note
The stack size requirements depend on the maximum nesting of exception and ISR execution and therefore on the priority settings
of the various interrupt and exception sources.

User code that calls API functions of the USB Device Component should have a minimum of 512 Bytes of stack space available.
Since API functions are frequently called from threads, the thread stack size should be at least 512 Bytes (see below).

### CMSIS-RTOS2 Requirements {#usbd_rtos_req}

Currently, the USB Device component works in two different RTOS configurations:

-# **CMSIS-RTOS2 RTX5**: This mode uses Keil RTX5 natively. In the Manage Run-Time Environment window, the component
   **CMSIS:RTOS2 (API):Keil RTX5** must be selected. The RTOS resources are static, configured by the system. There is
   **no need for additional user configuration**. This is the suggested option for new development.
-# **CMSIS-RTOS2 with a generic RTOS**: this mode uses a generic RTOS (such as FreeRTOS). The RTOS
   resources (thread, mutex and semaphore control blocks and thread stacks) are dynamically allocated. The user needs to
   provide memory pools big enough for the allocations in the specific RTOS configuration.

The USB Device component uses threads. Each instance of a component starts its own threads, for example two HID
instances start two threads with the name: USBD_HID0_Thread, USBD_HID1_Thread. The thread execution starts with
\ref USBD_Initialize.

The following table lists the components, number of threads, thread name, default thread priority, default stack size
and the configuration file with **OS Resource Settings** for stack size adjustments.

| Component                   | Number of Threads | Thread Name                                 | Default Priority         | Default Stack Size | Configuration File
| :---------------------------| :-----------------| :-------------------------------------------| :----------------------: | :----------------: | :------------------------
| **USB:Device**              | 1                 | USBDn_Core_Thread                           | \c osPriorityAboveNormal | 1024 Bytes         | USBD_Config_n.h
| **USB:Device:ADC**          | 1                 | USBD_ADCn_Thread                            | \c osPriorityAboveNormal | 512 Bytes          | USBD_Config_ADC_n.h
| **USB:Device:CDC**          | 2                 | USBD_CDCn_Bulk_Thread, USBD_CDCn_Int_Thread | \c osPriorityAboveNormal | 512 Bytes          | USBD_Config_CDC_n.h
| **USB:Device:Custom Class** | 1...15            | USBD_CustomClassn_EPm_Thread                | \c osPriorityAboveNormal | 512 Bytes          | USBD_Config_CustomClass_n.h
| **USB:Device:HID**          | 1                 | USBD_HIDn_Thread                            | \c osPriorityAboveNormal | 512 Bytes          | USBD_Config_HID_n.h
| **USB:Device:MSC**          | 1                 | USBD_MSCn_Thread                            | \c osPriorityAboveNormal | 512 Bytes          | USBD_Config_MSC_n.h
n is instance number, m is Endpoint number 1...15

\note
- For **USB:Device:Custom Class** each endpoint In/Out pair executes in a separate thread. The number of threads depends on
  the number of endpoints enabled in the configuration file USBD_Config_CustomClass_n.c.
- The **Default Stack Size** in the table above can be changed in the USB configuration files (for example
  USBD_Config_n.h and USBD_Config_DeviceClass_n.h). At the end of each configuration file, you will find
  **OS Resource Settings** and below that the value for the **Thread stack size**.

### RTX v5 Requirements {#usbd_req_rtx5}

The USB Device component allocates all necessary resources statically so no changes to RTX settings are necessary.

### Memory Requirements {#usbd_mem_req}

The following table shows the memory requirements for read-only (Flash) and read-write (RAM) memory.
These numbers may be used to estimate the total size requirements of the application, but the actual numbers depend
on compiler optimizations and target processor and may be therefore slightly different.

| Component                   | RO Memory [Bytes] | RW Memory (Data) [Bytes]                                                                                 |
| :-------------------------- | :---------------: | :--------------------------------------------------------------------------------------------------------|
| **USB:Core** (Device)       |      6.0 k        | < 470                                                                                                    |
| **USB:Device:ADC**          |      2.5 k        | < 110 + Audio Subframe Sizes * Buffer Sizes (configured in the USBD_Config_ADC_n.h) |
| **USB:Device:HID**          |      2.7 k        | < 130 + Maximum Report Sizes                       (configured in the USBD_Config_HID_n.h) |
| **USB:Device:MSC**          |      4.1 k        | < 160 + Maximum Bulk Buffer Size                   (configured in the USBD_Config_MSC_n.h) |
| **USB:Device:CDC ACM**      |      3.2 k        | < 220 + Maximum Send/Receive Buffer Sizes          (configured in the USBD_Config_CDC_n.h) |
| **USB:Device:Custom Class** |      2.0 k        | < 80                                                                                  |
| **USB Device Driver**       |     1 .. 4 k      | 0.5 .. 4 k                                                                            |

## USB Host Resource Requirements {#usbh_res_req}

The following section documents the requirements for the **USB Host** component. The actual requirements depend on the
components used in the application and the configuration of these components.

### Stack Requirements {#usbh_stack_req}

The USB Host Core receives events sent from the interrupt service routine (ISR) of the USB Host Driver.
The stack requirements for the ISR are typically less than 512 Bytes. The total stack space required for ISR depends
on the interrupt nesting and therefore on the priority settings of these ISR.

| Option (under section Stack Configuration)                        | Increase Value by
| :---------------------------------------------------------------- | :----------------------
| Stack Size (in Bytes)                                             | + 512 for USB Host Driver

\note
The stack size requirements depend on the maximum nesting of exception and ISR execution and therefore on the priority settings
of the various interrupt and exception sources.

User code that calls API functions of the USB Host Component should have a minimum of 512 Bytes of stack space available.
Since API functions are frequently called from threads, the thread stack size should be at least 512 Bytes.

### CMSIS-RTOS2 Requirements {#usbh_rtos_req}

Currently, the USB Host component works in two different RTOS configurations:

-# **CMSIS-RTOS2 RTX5**: This mode uses Keil RTX5 natively. In the Manage Run-Time Environment window, the component
   **CMSIS:RTOS2 (API):Keil RTX5** must be selected. The RTOS resources are static, configured by the system. There is
   **no need for additional user configuration**. This is the suggested option for new development.
-# **CMSIS-RTOS2 with a generic RTOS**: this mode uses a generic RTOS (such as FreeRTOS). The RTOS
   resources (thread, mutex and semaphore control blocks and thread stacks) are dynamically allocated. The user needs to
   provide memory pools big enough for the allocations in the specific RTOS configuration.

The USB Host component uses threads. Each USB Host instance starts its own thread (USBHn_Core_Thread).
The thread execution starts with \ref USBH_Initialize. Additionally each instance of HID and CDC class also start their own
thread (USBH_HIDn_IntIn_Thread or USBH_CDCn_IntIn_Thread) when HID or CDC device is connected.

The following table lists the components, number of threads, thread name, default thread priority, default stack size
and the configuration file with **OS Resource Settings** for stack size adjustments.

| Component                   | Number of Threads | Thread Name            | Default Priority         | Default Stack Size | Configuration File
| :---------------------------| :-----------------| :----------------------| :----------------------: | :----------------: | :------------------------
| **USB:Host**                | 1                 | USBHn_Core_Thread      | \c osPriorityAboveNormal | 2048 Bytes         | USBH_Config_n.h
| **USB:Host:CDC**            | 1                 | USBH_CDCn_IntIn_Thread | \c osPriorityAboveNormal |  512 Bytes         | USBH_Config_CDC.h
| **USB:Host:HID**            | 1                 | USBH_HIDn_IntIn_Thread | \c osPriorityAboveNormal |  512 Bytes         | USBH_Config_HID.h
n is instance number

\note
- The **Default Stack Size** in the table above can be changed in the USB configuration files (for example
  USBH_Config_n.h and USBH_Config_DeviceClass.h). At the end of each configuration file, you will find
  **OS Resource Settings** and below that the value for the **Thread stack size**.
- If File System is used retargeted to USB Host Mass Storage the stack size for thread calling File System functions should
  be at least 2560 Bytes.

### RTX v5 Requirements {#usbh_req_rtx5}

The USB Host component allocates all necessary resources statically so no changes to RTX settings are necessary.

### Memory Requirements {#usbh_mem_req}

The following table shows the memory requirements for read-only (Flash) and read-write (RAM) memory.
These numbers may be used to estimate the total size requirements of the application, but the actual numbers depend
on compiler optimizations and target processor and may be therefore slightly different.

| Component                   | RO Memory [Bytes] | RW Memory (Data) [Bytes]
| :---------------------------| :---------------: | :-----------------------:
| **USB:Core** (Host)         |     < 9.5 k       | < 370 + Memory Pool Size (configured in the USBH_Config_n.h)
| **USB:Host:HID**            |     < 1.3 k       | < 70
| **USB:Host:MSC**            |     < 3.4 k       | < 40
| **USB:Host:CDC**            |     < 1.0 k       | < 30
| **USB Host OHCI** support   |     < 3.5 k       | < 0.5 k
| **USB Host EHCI** support   |     < 5.0 k       | < 4.5 k
