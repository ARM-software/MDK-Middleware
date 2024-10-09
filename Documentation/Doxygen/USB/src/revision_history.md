# Revision History {#rev_hist}

<table class="cmtable" summary="Revision History">
    <tr>
      <th>Version</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>V8.0.0</td>
      <td>
        - removed CORE component variants
        - changed source config files to headers
        - removed support for legacy Arm Compiler 5
        - removed support for CMSIS-RTOS version 1
        - USB Host: removed OHCI and EHCI drivers (moved to CMSIS-Driver pack)
        - USB Host: increased default Core Thread stack size (from 1024 to 2048)
        - USB Device: increased default Core Thread stack size (from 512 to 1024)
      </td>
    </tr>
    <tr>
      <td>V6.17.0</td>
      <td>
        - USB Device/Host: using Event Recorder component from CMSIS-View pack (instead of deprecated ARM_Compiler pack)
        - USB Device/Host: updated main Config files (clean-up)
        - USB Device/Host: fixed issue with Arm Compiler 5 (Warning: L6418W: Tagging symbol not recognized)
        - USB Device: increased maximum number of HID instances from 4 to 8
        - USB Device: corrected Control Transfer data stage ZLP termination (unnecessary ZLP was sent if requested data length was equal to available data length)
      </td>
    </tr>
    <tr>
      <td>V6.16.1</td>
      <td>
        - USB Device/Host: improved code robustness
      </td>
    </tr>
    <tr>
      <td>V6.16.0</td>
      <td>
        - USB Host: libraries built with Arm Compiler 6
        - USB Device: libraries built with Arm Compiler 6
        - USB Device: fixed Control Transfer data stage ZLP termination (unnecessary ZLP not sent if all requested data was transferred)
      </td>
    </tr>
    <tr>
      <td>V6.15.4</td>
      <td>
        - USB Host: Improved enumeration process
        - USB Host: Improved handling for HID keyboard device including Report ID in the Input Report
        - USB Host: Fixed USBH_HID_Read function to return all data received on the Interrupt IN pipe
        - USB Device: Enabled automatic driver installation on Microsoft Windows 8.1 and later for full-speed devices
        - USB Device: Fixed Mass Storage device mounting as read-only on MAC OS
      </td>
    </tr>
    <tr>
      <td>V6.15.0</td>
      <td>
        - USB Host: Added support for Isochronous transfers (only for EHCI full-speed)
        - USB Host: Fixed sporadic crash when disconnecting device from port 1
        - USB Host: Fixed OHCI driver causing HardFault during debugging
        - USB Device: ClearEndpointFeature request propagated to low-level driver also if endpoint was not in halt state
      </td>
    </tr>
    <tr>
      <td>V6.14.1</td>
      <td>
        - USB Host: Corrected enumeration of a CDC device if it is a part of a composite device
        - USB Device: Corrected RNDIS (incomplete type is not allowed)
      </td>
    </tr>
    <tr>
      <td>V6.14.0</td>
      <td>
        - USB Host: Added abort functionality to CDC ACM transfers
      </td>
    </tr>
    <tr>
      <td>V6.13.7</td>
      <td>
        - USB Device/Host: Added thread name for all threads created by USB stack
        - USB Device/Host: Corrected compiler warnings
        - USB Host: Corrected USBH_Device_GetController/Port/Speed/Address/VID/PID functions to work in device enumeration Initialize callbacks
      </td>
    </tr>
    <tr>
      <td>V6.13.6</td>
      <td>
        - USB Device: Corrected delay on USB out transfer when multiple USB devices are used
        - USB Device: Corrected handling if endpoint configuration in driver fails
        - USB Device: Corrected MSC class support for sector size different than 512
        - USB Device: Corrected MAC_str_to_addr function in Ethernet RNDIS driver
        - USB Device: Corrected MAC_str_to_addr function in USBD_User_CDC_ACM_RNDIS_ETH.c template
        - USB Device: Corrected MAC_str_to_addr function in USBD_User_CDC_ACM_RNDIS_VETH.c template
        - USB Device: Improved USB input buffers alignment (aligned to 32 bytes, support cache up to 32 bytes per cache line)
        - USB Host: Corrected CMSIS-RTOS1 compatibility mode compilation error in USART_CDC_ACM.c template
        - USB Host: Corrected CMSIS-RTOS1 compatibility mode compilation error in USART_PL2303.c template
      </td>
    </tr>
    <tr>
      <td>V6.13.0</td>
      <td>
        - USB Device/Host: Added debug configuration for debugging with Event Recorder
        - USB Device/Host: Corrected compilation warnings if GNU extensions is enabled in C/C++ settings for ARM Compiler 5
        - USB Device: Added support for USB Device CDC ACM using RNDIS protocol
        - USB Device: Renamed CDC ACM function USBD_CDC_ACM_Notify to USBD_CDC_ACM_Notify_SerialState
        - USB Device: Updated configuration with capability to specify Control Transfer Buffer Size
        - USB Device: Updated CDC configuration for RNDIS
        - USB Device: Increased number of available CDC instances to 8
        - USB Host: Corrected EHCI driver for handling transfers larger than 16kB
        - USB Host: Corrected CMSIS-RTOS2 compilation error in USART_CDC_ACM.c template
        - USB Host: Corrected CMSIS-RTOS2 compilation error in USART_PL2303.c template
        - USB Host: Corrected Component Viewer display when ARM Compiler 6 is used
        - USB Host: Added unsupported state to mounted device in Component Viewer if no driver is available for device
      </td>
    </tr>
    <tr>
      <td>V6.12.8</td>
      <td>
        - USB Device/Host: Corrected Initialize and Uninitialize functions so they can be called on already initialized/uninitialized device/controller
        - USB Device: Corrected HID descriptors offset calculation
        - USB Host: Corrected EHCI driver USB Controller power off procedure
        - USB Host: Corrected OHCI driver transfers after endpoint returns stall
        - USB Host: Improved USB Flash disk enumeration procedure (some USB Flash disks return wrong data residue on RequestSense command)
        - USB Host: Improved recovery after USB Flash disk is disconnect while reading is in progress
      </td>
    </tr>
    <tr>
      <td>V6.12.4</td>
      <td>
        - USB Device/Host: Corrected timer, semaphore and mutex section location (for debugging)
        - USB Device/Host: Improved robustness by adding 2 retries for failed driver operations
        - USB Device: Corrected USBD_MSCn_GetMaxLUN callback return type from bool to uint8_t
        - USB Host: Corrected USART_CDC_ACM.c CMSIS UART driver receive function returning invalid data in receive buffer
        - USB Host: Improved USB Flash disk unmount procedure
      </td>
    </tr>
    <tr>
      <td>V6.12.0</td>
      <td>
        - USB Device/Host: Improved RTOS2 wrapper (reduced number of osKernelGetTickFreq() calls to improve performance)
        - USB Device: Added function for setting Serial Number String at run-time
        - USB Device: Added Microsoft OS Descriptors (automatic driver installation on Win 8 and later)
        - USB Device: Added USBD_CDCn_ACM_Data_Received and USBD_CDCn_ACM_Data_Sent callback functions to USBD_User_CDC_ACM.c template
        - USB Device: Added USBD_MSCn_GetMaxLUN callback for runtime change of maximum LUN
        - USB Device: Improved custom class configuration, extended to 8 endpoints of selectable type and direction per interface
        - USB Device: Improved custom class interface association setting, usage made optional
        - USB Device: Improved USBD_MSC.c template to be able to switch owner to FS if device is not connected on the USB
        - USB Device: Improved descriptor positioning in code memory, aligned to 4 bytes so DMA can be used
        - USB Device: Improved CDC with aligned transfers (USB driver can use DMA for data transfers)
        - USB Device: Corrected crc_mode parameter value in USBD_CDCn_NCM_SetCrcMode callbacks
        - USB Host: Improved USB Flash disk enumeration procedure (some USB Flash disks return wrong data residue on TestUnitReady and Inquiry commands)
        - USB Host: Improved memory management
        - USB Host: Improved pipe transfer handling (corrected possible problem of simultaneous access on different pipes)
        - USB Host: Improved disconnect procedure
        - USB Host: Corrected USART_PL2303.c and USART_CDC_ACM.c CMSIS UART driver to allow send/receive restart form callbacks, 
                    and corrected receive function so ARM_USART_EVENT_RECEIVE_COMPLETE event is signaled after requested number of bytes was received
        - USB Host: Corrected functionality when Memory Pool Relocation is used
        - USB Host: Corrected Default Pipe deletion in uninitialization
        - USB Host: Corrected OHCI driver number of bytes transferred
        - USB Host: Corrected OHCI driver USB Flash Disk recognition
        - USB Host: Corrected OHCI driver causing HardFault if two USB Memory Sticks are connected on two root hub ports
        - USB Host: Corrected OHCI driver causing HID device to stop working when another HID device gets disconnected
        - USB Host: Corrected OHCI driver disconnect on one root hub port causing malfunction of device attached to the other root hub port
        - USB Host: Corrected EHCI driver asynchronous simultaneous transfers functionality
        - USB Host: Corrected EHCI driver USB Controller power off procedure
      </td>
    </tr>
    <tr>
      <td>V6.11.0</td>
      <td>
        - USB Device/Host: Added support for ARM Compiler 6
        - USB Device/Host: Updated source code for MISRA compliance
        - USB Device/Host: Updated resource requirements
        - USB Device: Added Mass Storage Class support for multiple Logical Units (up to 4)
        - USB Device: Updated custom device/class handling (buffer and length are initialized to 0 when SETUP packet is received)
        - USB Device: Corrected USBD_User_Device_SerNum_n.c template (corrected handling of requests shorter than serial number string descriptor size)
        - USB Device: Corrected Mass Storage Class was stalling bulk out endpoint if all data for write was received but media has become unavailable
        - USB Device: Corrected Audio Device in case only microphone is used
        - USB Device: Updated Audio Device USBD_User_ADC_Audio.c template
        - USB Device: Corrected string descriptors if ARM Compiler 5 is used with Optimization Level 0
        - USB Host: Corrected USBH_PL2303_GetRxCount function in USART_PL2303.c template
        - USB Host: Corrected enumeration of USB HID devices which stall SetIdle request
      </td>
    </tr>
    <tr>
      <td>V6.10.0</td>
      <td>
        - USB Device/Host: Library made RTOS agnostic, all hooks externally (compile time) available
        - USB Device/Host: Added support for CMSIS-RTOS2
        - USB Device/Host: Added version information in Component Viewer
        - USB Device: Added USBD_GetVersion function for retrieving version of USB Device library
        - USB Device: Updated USBD_User_CDC_ACM.c template with default SetLineCoding/GetLineCoding handling
        - USB Device: Improved display of memory size for Mass Storage class
        - USB Device: Improved Mass Storage read performance through multiple transfers to host
        - USB Device: Corrected HID deadlock if device is disconnected in the middle of interrupt transfer
        - USB Device: Corrected CDC ZLP termination of bulk transfer if bulk transfer is multiple of maximum packet size
        - USB Host: Corrected deadlock in case that USB Flash Disk is disconnected while there was active read or write to it
        - USB Host: Corrected enumeration of USB Flash Disks which stall GetMaxLUN request
        - USB Host: Corrected memory size information for Mass Storage class in Component Viewer
        - USB Host: Changed first parameter of USBH_..._Configure functions from const USBH_DEV *ptr_dev to uint8_t device
        - USB Host: Changed USBH_Port_Notify function to USBH_Notify with added 3-rd parameter uint8_t device for device notifications
        - USB Host: Changed first parameter of USBH_ControlTransfer and USBH_DeviceRequest_... functions from uint8_t ctrl to uint8_t device
        - USB Host: Removed USBH_DefaultPipe... functions and all default pipe communication provided through USBH_ControlTransfer function
        - USB Host: Removed internal structures from main rl_usb.h file
        - USB Host: Added USBH_GetVersion function for retrieving version of USB Host library
        - USB Host: Added USBH_Device_GetStringDescriptor function for retrieving string descriptor
        - USB Host: Updated USBH_Pipe... functions see function reference documentation for details
        - USB Host: Updated USBH_PL2303.c and USBH_User_CustomClass.c templates to fit API changes
        - USB Host: Updated USART_PL2303.c and USART_CDC_ACM.c drivers to fit API changes
        - USB Host: Improved display of memory size for Mass Storage class in Component Viewer
      </td>
    </tr>
    <tr>
      <td>V6.9.0</td>
      <td>
        - USB Device/Host: Added debug library variants with event recording
        - USB Device: Added capability to disable string for Custom Class interfaces
        - USB Device: Corrected unbalanced mutex release when using USBD_HID_GetReportTrigger (lock mechanism changed from mutex to semaphore)
        - USB Device: Corrected string descriptors
        - USB Device: Corrected initial UART reception size in CDC ACM UART template
        - USB Device: Corrected Audio Class template as volume or mute control changes on Host side could cause device to stop responding
        - USB Host: Improved USB memory stick enumeration procedure to work for sticks that require more time after initial reset
      </td>
    </tr>
    <tr>
      <td>V6.8.0</td>
      <td>
        - USB Device/Host: Improved compatibility with ARM Compiler 6
        - USB Device: Corrected Audio Class functionality for 24-bit resolution with 3 bytes per sample setting
        - USB Device: Updated Audio Class API (old API is still functional)
        - USB Device: Added state retrieval function (USBD_GetState)
        - USB Device: Added user notifications for device events
        - USB Device: Added setting for Interface Association for Custom Class
        - USB Device: Improved Audio Class template
        - USB Host: Corrected device causing HardFault if USB Memory Stick gets disconnected while device was reading it
        - USB Host: Corrected EHCI Relocate Memory Pool compilation errors
        - USB Host: Corrected enumeration if SetIdle on HID device stalls
        - USB Host: Corrected interrupt pipe creation and deletion for OHCI and EHCI controllers
        - USB Host: Extended API with additional functions to get more device information
        - USB Host: Added notification function (USBH_Port_Notify)
        - USB Host: Added class driver notification functions for each supported class (..._Initialize and ..._Uninitialize)
        - USB Host: Added maximum power consumption configuration to configuration file
        - USB Host: Added len parameter to USBH_HID_ParseReportDescriptor function
        - USB Host: Renamed all get status functions from ..._GetDeviceStatus to ..._GetStatus
        - USB Host: Improved EHCI structures positioning in RAM
      </td>
    </tr>
    <tr>
      <td>V6.7.0</td>
      <td>
        - Corrected USB Host Mass Storage class finit failing if no USB MSC device is connected
        - Corrected USB Host language_id parameter for USBH_DeviceRequest_GetDescriptor() and USBH_DeviceRequest_SetDescriptor() functions changed from uint8_t to uint16_t
        - Improved USB Host keyboard functionality (cleaned up translation to ASCII and routing of all non-ASCII translated keys to user)
        - Corrected USB Device USBD_EndpointRead and USBD_EndpointWrite functions return usbDriverBusy if low level hardware is busy
        - Corrected USB Device Custom Device handling for Device to Host request with wLength of 0, no Status OUT stage was generated
        - Corrected USB Device Custom Class calling multiple interface callbacks for class request to single interface
        - Corrected USB Device Custom Class not working if endpoints higher than 7 are configured
        - Corrected USB Device Audio Class race condition (user function directly started USB transfer)
        - Corrected USB Device CDC ACM Class race condition (user function directly started USB transfer)
        - Corrected USB Device CDC ACM Class functionality for Maximum Communication Device Receive Buffer Size equal to two OUT endpoint maximum packet sizes
        - Corrected USB Device Mass Storage Class hang in case of Bulk-Only Mass Storage Reset was issued during Write 10 operation
        - Corrected USB Device HID to be thread safe
        - Improved robustness of USB Device Core by adding retries to low-level driver accesses
        - Added USB Device CDC ACM <-> UART Bridge template
        - Added USB Device Audio Class callback USBD_ADCn_ReceivedSamples called when receive buffer is half-full
        - Changed USB Device Audio Class USB sending starts when there is half of USB buffer of data pending
        - Removed __packed specifier form structures for GCC compliance
        - Removed anonymous unions and structures for GCC compliance
      </td>
    </tr>
    <tr>
      <td>V6.6.10</td>
      <td>
        - Corrected USB Device USBD_Configured function return value, it wrongly returned error status if instance did not exist instead of false value
        - Improved USB Host CDC and PL2303 templates
      </td>
    </tr>
    <tr>
      <td>V6.6.9</td>
      <td>
        - Corrected USB Device Audio Class corner case of sending Isochronous IN data beyond intermediate buffer
      </td>
    </tr>
    <tr>
      <td>V6.6.8</td>
      <td>
        - Corrected USB Device core handling of simultaneous signals OUT ZLP and SETUP
      </td>
    </tr>
    <tr>
      <td>V6.6.7</td>
      <td>
        - Corrected USB Device CDC NCM bulk transfer termination of multiple maximum packet size with ZLP
      </td>
    </tr>
    <tr>
      <td>V6.6.6</td>
      <td>
        - renamed CoreThread to Core_Thread
        - removed msc structures from rl_usb.h
      </td>
    </tr>
    <tr>
      <td>V6.6.5</td>
      <td>
        - corrected USB Device functionality so Device level custom handling is always enabled
        - fixed capability to send ZLP on Control Endpoint 0 request
        - fixed sending ZLP on Control Endpoint 0 request if sent data is multiple of maximum packet size and less then requested
        - added template for USB Device device level Control Endpoint 0 request handling demonstrating serial number provided at runtime
        - removed section about migration from RL-USB. If you need to migrate from previous versions, please install Middleware Pack 6.5.0.
      </td>
    </tr>
    <tr>
      <td>V6.6.4</td>
      <td>
        - corrected USB Host functionality not to do PING after there was error in transfer
      </td>
    </tr>
    <tr>
      <td>V6.6.3</td>
      <td>
        - corrected USB Host functionality if relocate memory pool is not used
      </td>
    </tr>
    <tr>
      <td>V6.6.2</td>
      <td>
        - added USB Host force disconnect of active device on USBH_Uninitialize
      </td>
    </tr>
    <tr>
      <td>V6.6.1</td>
      <td>
        - added support for double buffering for USB Device CDC Network Control Model (NCM) class
        - changed behaviour of USBD_HID_GetReportTrigger to be non blocking and return usbDriverBusy
          if previous request has not yet finished
      </td>
    </tr>
    <tr>
      <td>V6.6.0</td>
      <td>
        - added support for USB Device CDC Network Control Model (NCM) class
      </td>
    </tr>
    <tr>
      <td>V6.5.0</td>
      <td>
        - cleared USB Device device state after disconnect, so after 
          disconnect USBD_Configured function will return false
        - prevented race condition in USB Device CDC that could happen in
          situation when any USB Device CDC user function was called and
          while it was being executed an USB interrupt happened it could
          corrupt information about current state of transmission and
          reception thus leaving CDC communication in inoperable state
        - improved USB Host port lock handling during enumeration
      </td>
    </tr>
    <tr>
      <td>V6.4.0</td>
      <td>
        - added USB Device device level endpoint 0 request handling
        - added template for USB Device device level endpoint 0 request handling
        - added example code into USB Device Custom Class template
        - corrected USB Device Custom Class endpoint 0 device request handling
        - corrected USB Host send/receive timeout functionality
        - corrected USB Host CDC device disconnect handling
      </td>
    </tr>
    <tr>
      <td>V6.3.0</td>
      <td>
        - added template for implementing Audio Class Device (ADC)
        - added template for implementing ADC Headphone
        - corrected USB Device ADC meaning of samples to per channel basis
        - corrected USB Host handling of stall response on GetMaxLUN
        - corrected USB Device CDC DataReceived functionality
        - corrected USB Device MSC SCSI command TestUnitReady response on no media
        - improved USB Device MSC SCSI command RequestSense functionality
        - improved USBD_User_MSC.c template for media ownership control, media connect/disconnect
          and removed direct access to MSC data variables with additional callback functions
        - added template for USB Device MSC media ownership control (.c and .h files)
        - corrected USB Host behavior on connection of non boot subclass device
        - corrected USB Device Audio Class interface string number
        - corrected USB Device Audio Class control endpoint request status stage response
        - added support for USB Device Mass Storage Inquiry - Vital Product Data Pages: 0 and 80h
        - corrected USB Device Mass Storage class behaviour for read request of non-existing sector
        - corrected USB Device behaviour for slower code execution
        - added setting for overriding HID Report Descriptor to USBD_Config_HID.h file
        - added template for implementing HID Mouse (shows how to override HID Report Descriptor)
        - corrected USB Host OHCI and EHCI structure positioning in RAM
        - corrected USB Host OHCI and EHCI larger transfer handling
        - corrected USB Device USBD_HID_GetReportTrigger lock-up if called too often
      </td>
    </tr>
    <tr>
      <td>V6.2.0</td>
      <td>
        - reimplemented USB Device Audio Class support
        - added USB Host CDC support
        - added USART USB Host CDC Driver
        - added USB Host Custom Class Template
        - added USB Host Prolific PL2303 Custom Class Template
        - added USART Prolific PL2303 Driver (USB to serial RS232 adapter)
        - added USB Device Mass Storage CheckMedia callback
        - changed USB Device CDC DataReceived and DataSent callback functions to callback style functions with instance in function name
        - moved user accessible functions and structures from rl_usbd.h and rl_usbh.h file to main rl_usb.h file
        - changed USB Host library functions for usage with custom class
        - USB Host library adapted to use USB Host CMSIS Driver v2.01
        - fixed problem with simultaneous same device types on different controllers
        - corrected USB Device Mass Storage handling if media removed while write/read in progress
      </td>
    </tr>
    <tr>
      <td>V6.1.0</td>
      <td>
        - corrected USB Host OHCI driver interrupt endpoint handling
      </td>
    </tr>
    <tr>
      <td>V6.0.0</td>
      <td> 
        - adapted to CMSIS-Driver V2 interface
        - added Custom Class support for USB Host
      </td>
    </tr>
    <tr>
      <td>V5.1.5</td>
      <td>
        - corrected USB Device Custom Class event handling, build issue and template
        - corrected USB Host polling rate when Interrupt Endpoint requests "bInterval = 1"
      </td>
    </tr>
     <tr>
      <td>V5.1.4</td>
      <td>
        - updated user API
        - added configuration options for OS resources
        - added OHCI and EHCI drivers for USB Host
        - added multiple packet read for USB Device
        - corrected issue on USB Host when using addresses over 127
      </td>
    </tr>
    <tr>
      <td>V5.1</td>
      <td>
        - updated USB Device configuration 
        - added Custom Class support for USB Device
      </td>
    </tr>
    <tr>
      <td>V5.0</td>
      <td>
        - Initial release for MDK Version 5
        - Explains modifications compared to previous releases
      </td>
    </tr>
 </table>
