# Supported USB Classes {#USB_Classes}

This page gives an in-depth description of the USB Device Classes that are covered in the USB Middleware Component:

- \ref ADC "Audio Device Class (ADC)" is used to exchange streaming audio data between the USB Host and the USB Device.
  The USB Component supports the ADC Class for USB Device applications only.
- \ref CDC "Communication Device Class (CDC)" implements virtual communication ports. The USB Component supports the CDC
  Class for USB Device (ACM and NCM) and USB Host (ACM only) applications.
- \ref HID "Human Interface Device (HID)" is typically used to implement a keyboard, joystick or mouse. However, the
  HID class can be also used for simple data exchange. The USB Component supports the HID Class for USB Device and USB Host
  applications.
- \ref MSC "Mass Storage Class (MSC)" devices are not limited to an USB memory stick. Various devices can be connected
  to an USB Host using MSC: SD cards, internal or external Flash memory and even a simple RAM disk. The USB Component
  supports the MSC Class for USB Device and USB Host applications.
- \ref CustomClass "Custom Class" devices can be used to implement any USB Device Class. The USB Component supports the
  Custom Class for USB Device and USB Host applications.

## ADC: Audio Device Class {#ADC}

USB peripherals that support the Audio Device Class send or receive audio, voice, and other sound-related functionality (such
as control data for the audio equipment). These audio functions often come with other functions such as video or storage.
Together, they make up a composite device (e.g. a DVD-ROM player that can provide video, audio, data storage and transport
control functionality). Audio class devices use isochronous data transfer for audio streams. Digitally encoded music data can
be delivered using bulk transfers.

A detailed description about ADC is provided by the
[USB Implementers Forum (USB-IF)](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50).

\note
- The ADC implementation in the USB Component supports:
  - A subset of the Audio Device Class Release v1.0 specification
  - One streaming OUT and/or one streaming IN interface, each configurable in 1 channel (mono) or 2 channels (stereo) mode
  - Asynchronous synchronization type
  - Full-speed isochronous transfer type (only with one isochronous OUT and/or one isochronous IN endpoint)
  - Type I PCM data Format
  - One sampling frequency for each streaming interface
  - Sample resolutions of 8 bits, 16 bits, 24 bits or 32 bits per sample
  - Feature unit with mute and volume controls
- The \ref mainpage "USB Component" supports the ADC Class for \ref USB_Device applications only.
- Further information on how to use ADC is given in the Reference section: \ref usbd_adcFunctions

## CDC: Communication Device Class {#CDC}

The **Communication Device Class (CDC)** supports a wide range of devices that can perform telecommunications and
networking functions. Examples for communications equipment are:

- *Telecommunications* devices, such as analog phones and modems, ISDN terminal adapters, digital phones, as well as
  COM-port devices
- *Networking* devices, such as ADSL and cable modems, as well as Ethernet adapters and hubs

A detailed description about CDC is provided by the
[USB Implementers Forum (USB-IF)](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50).

### CDC Class Features {#CDC_Features}

A communications device has three basic tasks:

- Device management (controlling an configuring a specific device and notifying the USB Host of certain events)
- Call management (establishing and terminating telephone calls or other connections)
- Data transmission (sending and receiving application data)

The CDC implementation in the USB Component features:

- Emulation of a Virtual COM-port using the \ref usbd_cdcFunctions_acm "ACM (Abstract Control Model)" subclass of CDC.
- Emulation of network connectivity using the [RDNIS](https://en.wikipedia.org/wiki/RNDIS) protocol using the
  \ref usbd_cdcFunctions_acm "ACM (Abstract Control Model)" subclass of CDC. This enables network connections over USB
  between a Windows host PC and an embedded device, as well as USB Device RNDIS to Ethernet Bridge applications.
- Emulation of an Ethernet adapter using the \ref usbd_cdcFunctions_ncm "NCM (Network Control Model)" subclass of CDC (only
  available for \ref USB_Device). Using CDC (NCM), you can create  Ethernet-over-USB applications on Linux-based host systems.

\note
The following endpoint configurations are supported:
- One interrupt IN endpoint for notifications to the USB Host
- One bulk IN and one bulk OUT endpoint for data transfer

### Control Transfers {#CDC_Control_Transfers}

The documents available at
[Communications Device Class](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50)
describe the nine available request types for the Abstract Control Model (ACM). All requests specified for the Network
Control Model (NCM) by the documents in ECM120.pdf and NCM10.pdf are supported.

### USB Descriptors {#CDC_Descriptors}

The following descriptors are required in an USB CDC ACM Device:

- Standard Device Descriptor
- Standard Configuration Descriptor
- Interface Association Descriptor
- CDC Header Functional Descriptor
- CDC Union Functional Descriptor
- Call Management Functional Descriptor
- Abstract Control Management Functional Descriptor
- Standard Interface Descriptor for the CDC Class communication interface
- Standard Endpoint Descriptor for Interrupt IN endpoint
- Standard Interface Descriptor for the CDC Class data interface
- Standard Endpoint Descriptors for Bulk IN and Bulk OUT endpoints

The following descriptors are required in an USB CDC NCM Device:

- Standard Device Descriptor
- Standard Configuration Descriptor
- Interface Association Descriptor
- CDC Header Functional Descriptor
- CDC Union Functional Descriptor
- CDC Ethernet Networking Functional Descriptor
- NCM Functional Descriptor
- Standard Interface Descriptor for the CDC Class communication interface
- Standard Endpoint Descriptor for Interrupt IN endpoint
- Standard Interface Descriptor for the CDC Class data interface
- Standard Endpoint Descriptors for Bulk IN and Bulk OUT endpoints

The necessary descriptors are automatically generated by the USB Middleware Component. The page \ref USB_Descriptors
provides more information on the topic.

\note Further information on how to use CDC is given in the Reference section:
      - \ref usbd_cdcFunctions_acm "USB Device CDC (ACM) Functions"
      - \ref usbd_cdcFunctions_ncm "USB Device CDC (NCM) Functions"
      - \ref usbh_cdcacmFunctions "USB Host CDC Functions"

## HID: Human Interface Device Class {#HID}

The **Human Interface Device Class (HID)** is mainly used for devices that allow human control over a PC. Using these
devices, the host is able to react on human input (e.g. movements of a mouse or key presses). This response has to happen
quickly, so that the computer user does not notice a significant delay between his action and the expected feedback. Typical
examples for HID class devices are:

- Keyboards and pointing devices (such as mouse devices, joysticks and trackballs)
- Front-panel controls (for example buttons, knobs, sliders, and switches)
- Simulation devices (such as steering wheels, pedals, other VR input devices)
- Remote controls and telephone keypads
- Other low data-rate devices that provide for example environmental data (such as thermometers, energy meters or even
  bar-code readers)

A detailed description about HID is provided by the
[USB Implementers Forum (USB-IF)](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50).

### HID Class Features {#HID_Features}

The HID class is not necessarily a human interface. But a device using the HID class has to be able to work within the limits
of the HID class. These HID class implementation of the USB Component has the following features:

- All data is exchanged in **reports**. These are fixed-length structures that sent or requested by the USB Host in **control**
  or **interrupt transfers**. Reports have a flexible format and can contain any type of data. Every HID device needs to
  have one input report in its report descriptor. Output and feature reports are optional.
- An interrupt IN endpoint is required for sending **input reports** to the USB Host.
- The maximum number of interrupt IN and OUT endpoints is limited to 1.
- The interrupt OUT endpoint is *optional*.
- As the HID device can send data at any point in time using the interrupt IN endpoint, the USB Host driver needs to make
  sure that the data is polled periodically.
- The \ref mainpage "USB Component" supports the HID Class for \ref USB_Device and \ref USB_Host applications.

### Control Transfers {#HID_Control_Transfers}

The [HID specification](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50) defines six
class-specific requests. They enable the USB Host to inquire about the capabilities and the current state of the device.
Furthermore, the host can set the state of output and feature items. All six requests are transmitted using the control pipe.

- The `Get_Report` request enables the USB Host to receive a report via the control pipe
- The `Set_Report` request allows the USB Host to send a report to the device (for example for setting device states)
- The device's current **idle rate** can be read using the `Get_Idle` request. The idle rate determines how often a device
  resends data that has not changed since the last report.
- The **idle rate** can be set using the `Set_Idle` request. This is used to limit the reporting frequency of an interrupt
  IN endpoint. Default idle rates are 500 milliseconds for keyboards (delay before first repeat rate) and infinity for
  joysticks and mouse devices.
- During booting of the USB Host, a simplified protocol can be used for communication. The `Get_Protocol` request reads
  which protocol is currently active. The `Set_Protocol` request switches between the boot protocol and the report protocol.

### Interrupt Transfers {#HID_Interrupt_Transfers}

To ensure minimum data delay, interrupt endpoints are used. The bandwidth for interrupt transfers is guaranteed for every
device after successful enumeration. Control endpoints may face data delivery delay in case the bus is busy.

### USB Descriptors {#HID_Descriptors}

The following descriptors are required in an USB HID Device:

- Standard Device Descriptor
- Standard Configuration Descriptor
- Standard Interface Descriptor for the HID Class
- Class-Specific HID Descriptor
- Standard Endpoint Descriptor for Interrupt IN endpoint
- Class-Specific Report Descriptor

The necessary descriptors are automatically generated by the USB Middleware Component. The report descriptor is built based
on the settings in the ` USBD_Config_HID_x.h` file. The number of reports and their maximum size are specified in this file.
The page \ref USB_Descriptors provides more information on the topic.

\note Further information on how to use HID is given in the Reference section:
      - \ref usbd_hidFunctions "USB Device HID Functions"
      - \ref usbh_hidFunctions "USB Host HID Functions"

## MSC: Mass Storage Class {#MSC}

The **Mass Storage Class (MSC)** is mainly used for devices that allow access to their internal data storage. Typical
examples for MSC class devices are:

- External hard drives (HDD)
- External optical drives (such as CD or DVD drives)
- Portable Flash memory devices
- Solid-state drives (SSD)
- Digital cameras
- Card readers
- Mobile phones

A detailed description about MSC is provided by the
[USB Implementers Forum (USB-IF)](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50).

### MSC Class Features {#MSC_Features}

Not only hard disks use the MSC class. Any device that allows access to its internal data storage using MSC can be connected
to the USB Bus as a mass storage device. The MSC class implementation in the USB Component has the following features:

- It supports the **bulk-only transport (BOT)** protocol. The BOT protocol specifies that not only data is transferred
  using bulk transfer, but also command and status data.
- The maximum number of bulk IN and OUT endpoints is fixed to 1.
- \ref USB_Device supports multiple logical units (LUNs) which can operate separately, for example one unit could have an SD
  card as media and another one could have a RAM disk as media.
- The \ref mainpage "USB Component" supports the MSC Class for \ref USB_Device and \ref USB_Host applications.

### Control Transfers {#MSC_Control_Transfers}

The [MSC specification](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50)
defines five class-specific requests to be transmitted over the default (control) pipe. Only two of them are supported by
the bulk-only protocol:

- The **Bulk Only Mass Storage Reset** request enables the USB Host to reset the MSC device and its associated interface.
- The **Get Max LUN** request allows the USB Host determine the number of **Logical Units (LUNs)** that are supported
  by the device. LUNs are numbered starting from LUN 0 to LUN 15 (maximum).

### Bulk Transfers {#MSC_Bulk_Transfers}

The bulk-only protocol divides a successful data transfer into three stages:

-# Command transport: the USB Host sends a command in the **Command Block Wrapper (CBW)** structure
-# Data transport: the data is sent to or from the USB Host
-# Status transport: the USB Device sends status information in the **Command Status Wrapper (CSW)** structure

### USB Descriptors {#MSC_Descriptors}

The following descriptors are required in an USB MSC Device:

- Standard Device Descriptor
- Standard Configuration Descriptor
- Standard Interface Descriptor for the MSC Class
- Standard Endpoint Descriptors for Bulk IN and Bulk OUT endpoints
- Standard String Descriptor

The necessary descriptors are automatically generated by the USB Middleware Component. The page \ref USB_Descriptors provides
more information on the topic.

\note Further information on how to use MSC is given in the Reference section:
      - \ref usbd_mscFunctions "USB Device MSC Functions"
      - \ref usbh_mscFunctions "USB Host MSC Functions"

## Custom Class {#CustomClass}

### USB Host supporting any Device Class {#CustomClass_Host}

The Custom Class for USB Host has two use cases:

-# Implementing/supporting a
   [standard](https://www.usb.org/defined-class-codes) Device class
   other than \ref CDC "CDC", \ref HID "HID" and \ref MSC "MSC" (also CDC, HID and MSC class handling can be overridden).
-# Implementing/supporting a
   [vendor specific](https://www.usb.org/defined-class-codes) Device
   class.

Supporting Custom or Standard Class Devices requires the USB Host to be aware of the specific Class. On a PC, it is easy to
add Class support by simply installing the required driver. However, in an embedded system this is not possible.

The \ref usbh_customFunctions are a starting point for adding support for any USB Device Class. Furthermore, all **Class
Specific Requests** need to be handled by the application running on the USB Host system. For more information on these
requests, please consult the
[USB-IF Device Class Documents](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50).
The **user code template** file \ref USBH_Cust_UCT "USBH_User_CustomClass.c" contains all the functions that require
adaptation.

The number of concurrent Custom Class Devices in a system can be configured using the `USBH_Config_CustomClass.h`
configuration file.

### USB Device implementing any Device Class {#CustomClass_Device}

The Custom Class for USB Device has two use cases:
-# Implementing/supporting a
   [standard](https://www.usb.org/defined-class-codes) Device class
   other than \ref ADC "ADC", \ref CDC "CDC", \ref HID "HID", and \ref MSC "MSC".
-# Implementing/supporting a
   [vendor specific](https://www.usb.org/defined-class-codes) Device
   class.

The Custom Class gives full control of the USB handling. It is designed to react on any kind of event on any of the assigned
endpoints and to handle any control message that may reach the system. The application has the full capability of dealing
with control messages for whatever USB Device class. Message handling control can be taken over from the USB Device Core. The
**user code template** file \ref USBD_User_Device_UCT "USBD_User_Device.c" contains all functions used for Device level
Control Endpoint 0 (STANDARD, CLASS and VENDOR requests) message interception and handling, whereas file
\ref USBD_User_Cust_UCT "USBD_User_CustomClass.c" contains all the functions used for Custom Class Control Endpoint 0 CLASS
requests and Custom Class endpoint events handling.

To create the Custom Class the configuration file providing up to four interfaces with up to 8 endpoints has been provided.

### USB Descriptors {#CustomClass_Descriptors}

Every USB Device requires certain descriptors. The following list is not complete but gives an impression about the various
descriptors that may be required:

- Standard Device Descriptor
- Standard Configuration Descriptor
- Standard Interface Descriptor for the Class that is implemented
- Standard Endpoint Descriptors for IN and OUT endpoints
- Standard String Descriptor

The necessary descriptors are automatically generated by the USB Middleware Component from the Class, Subclass, and
Protocol Codes and the Interface String specified in the `USBD_Config_CustomClass_n.h` file. The page \ref USB_Descriptors provides more information on the topic.

\note Further information on how to use the Custom Class in a USB Device is given in the Reference section:
      - \ref usbd_classFunctions "USB Device Custom Class Functions"
      - \ref usbh_customFunctions "USB Host Custom Class Functions"
