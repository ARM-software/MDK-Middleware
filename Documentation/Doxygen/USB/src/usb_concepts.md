# USB Concepts {#USB_Concepts}

The **Universal Serial Bus** (USB) is a serial interface designed to be plug-and-play making it easy to connect
peripherals to a host. The following pages are intended to help the designer to get a better understanding about the USB
protocol in general. They are divided into different pages as follows:

- \ref USB_Network shows the topology of the USB network.
- \ref USB_Transfer_Rates gives a quick glance at the theoretical maximum data rates that are supported by USB.
- \ref USB_Endpoints shows how data is exchanged on the USB bus.
- \ref USB_Protocol page gives an overview over the protocol and the basic communication model.
- \ref USB_Descriptors explains the data structure that is used in USB devices to report their attributes to a host. Most
  of these descriptors can be matched to configuration settings in MDK.

## USB Transfer Rates {#USB_Transfer_Rates}

USB uses two wires to supply power and two wires to transfer signals. The following data transfer rates are supported:

|Performance                |Attributes                                                |Applications                       |
|---------------------------|----------------------------------------------------------|-----------------------------------|
|Low-Speed: 1.5 Mbits/s     | Lower cost <br> Hot-pluggable <br> Multiple peripherals  | interactive devices: <br>    mouse, keyboards, game peripherals |
|Full-Speed: 12 Mbits/s     | Low cost <br>  Hot-pluggable <br>  Multiple peripherals <br> Guaranteed latency <br> Guaranteed bandwidth                                     | phone, audio, compressed video, printers, scanners |
|High-Speed: 480 Mbits/s    | Guaranteed latency <br>      High bandwidth              | video, mass storage |

\note SuperSpeed is not covered in this document.

## USB Network {#USB_Network}

The **physical USB network** is implemented as a tiered star network with one host (master) and several devices (slaves).

The USB Host provides one attachment port. If more peripherals are required, connect a hub to the root port to provide
additional connection ports. The USB network can support up to 127 external nodes. Due to timing constraints for signal
propagation, the maximum number of tiers allowed is seven:

- One tier for the host (bus master).
- Six tiers for hubs and devices.

![Tier Network](tiernetwork.png)

USB devices are divided into device classes:

- **Hubs** provide additional attachment points and simplify USB connectivity from the user perspective. Each hub converts
   a single attachment point into multiple attachment points referred to as ports.
- **Functions** provide capabilities to the system for transmitting or receiving data and control information. Each function
   contains configuration information describing the device capabilities and resource requirements.
- **Compound Devices** are physical packages that implement multiple functions and include an embedded hub. A compound
  device appears to the host as a hub with one or more non-removable USB devices.
- **Composite Devices** support more than one class and thus, provide more than one function to the host.

Examples of functions:

- A human interface device such as a mouse, keyboard, tablet, or game controller.
- An imaging device such as a scanner, printer, or camera.
- A mass storage device such as a CD-ROM drive, floppy drive, or  DVD drive.

The **logical USB network** appears as a star network to the developer with the host at the center. Hubs do not introduce
any programming complexity and are transparent as far as the programmer is concerned. An USB device will work the same way
whether connected directly to a root-hub or whether connected via intermediate hubs. All USB devices are available as
addressable nodes in this master/slave network. Only the host can initiate a data transfer in the network.

![Star Network](starnetwork.png)

\note
- Only one host exists in any USB system.
- Only functions can be enabled in tier seven.
- Compound devices occupy two tiers.

## USB Communication {#USB_Communication}

USB is a polled bus, where the USB Host initiates all data exchanges. The data travels to or from \ref USB_Endpoints in an USB Device.
The **client** in the USB Host stores data in buffers, but does not have endpoints.
The USB Host and the peripheral USB Device have distinct layers, as shown in the picture below. The connections between the
layers are **logical** Host-Device interfaces between each horizontal layer. Between the logical connections data is transferred
using \ref USB_Pipes.

![Logical Connections Between USB Host Clients and USB Device Endpoints](usb_endpoints.png)

### Pipes {#USB_Pipes}

Basically, two types of **pipes** exist:
- **Message pipes** have a defined USB format and are host controlled. Message pipes allow data to flow in both directions and
  support \ref USB_Control_Transfers only.
- **Stream pipes** have no defined USB format and can either be controlled by the host or by the device. The data stream has
  a predefined direction, either **IN** or **OUT**. Stream pipes support \ref USB_Interrupt_Transfers, \ref USB_Isochronous_Transfers,
  and \ref USB_Bulk_Transfers.

Most pipes come into existence when an USB Device has been connected to the USB Bus and configured by the USB Host.
A pipe originates from a **data buffer** within the host **client** and terminates inside the USB Device at an \ref USB_Endpoints
"Endpoint".

### Transfers {#USB_Transfers}

**Transfers** (data flow types) can consist of one or more transactions. A pipe supports only one of the following transfer types:
- \ref USB_Control_Transfers are typically used to setup an USB device. They always use IN/OUT Endpoint 0.
- \ref USB_Interrupt_Transfers can be used where data is sent regularly, for example for status updates.
- \ref USB_Isochronous_Transfers transmit real-time data such as audio and video. They have a guaranteed, fixed bandwidth, but no error detection.
- \ref USB_Bulk_Transfers can be used to send data where timing is not important, for example to a printer.

#### Control Transfers {#USB_Control_Transfers}

**Control Transfers** are bi-directional transfers reserved for the host to send and request configuration information to
and from the device using the IN and OUT Endpoint 0.

Each Control Transfer consists of 2 to several transactions. The maximum packet size for the control endpoint data is:

- 8 bytes for low-speed
- 8, 16, 32, or 64 bytes for full-speed
- 64 bytes for high-speed

In general, the application software does not use this type of transfer.

Control Transfers have three stages:

-# The **SETUP** stage carries 8 bytes called the Setup packet, defining the request, and specifying how many data should be
  transferred in the DATA stage.
![Control SETUP Transaction Format](ctrlTransferSetup.png)
\n
-# The **DATA** stage is optional. If present, it always starts with a transaction containing a DATA1 packet.
  Then, the transaction type alternates between DATA0 and DATA1 until all required data have been transferred.
-# The **STATUS** stage is a transaction containing a zero-length DATA1 packet. If the DATA stage was IN, then the STATUS
  stage is OUT, and vice-versa.

![Control Read and Write Sequences](ctrlTransferRedWriteSeq.png)

#### Interrupt Transfers {#USB_Interrupt_Transfers}

**Interrupt Transfers** have a limited latency to or from a device. In USB, an Interrupt Transfer, or Interrupt Pipe, has a
defined polling rate between:

- 1ms and 255ms for full and low-speed
- 125&mu;s to 4096ms for high-speed endpoints.

The maximum packet size for the interrupt endpoint data is:

- 64 or less bytes for full-speed
- 1024 or less bytes for high-speed

The developer can define how often the host can request a data transfer from the device.

For example, for a mouse, a data transfer rate at every 10 ms can be guaranteed. However, defining the polling rate does not guarantee
that data will be transferred every 10 ms, but rather that the transaction will occur somewhere within the tenth frame. For this
reason, a certain amount of timing jitter is inherent in an USB transaction.

Typically, Interrupt Transfer data consists of event notifications, characters, or coordinates from a pointing device.

![Interrupt Transfer Format](interruptTransfer.png)

#### Isochronous Transfers {#USB_Isochronous_Transfers}

**Isochronous Transfers** are used for transmitting real-time information such as audio and video data, and must be sent at
a constant rate. USB isochronous data streams are allocated a dedicated portion of USB bandwidth to ensure that data can be
delivered at the desired rate. An Isochronous pipe sends a new data packet in every frame, regardless of the success or failure
of the last packet.

The maximum packet size for the isochronous endpoint data is:

- 1023 or less bytes for full-speed
- 1024 or less bytes for high-speed

Isochronous Transfers have no error detection. Any error in electrical transmission is not corrected.

Isochronous Transfers are also subject to timing jitters as described for \ref USB_Interrupt_Transfers.

![Isochronous Transfer Format](isochronousTransfer.png)

#### Bulk Transfers {#USB_Bulk_Transfers}

**Bulk Transfers** are used for data which are not of the type Control, Interrupt, or Isochronous. Reliable exchange of data
is ensured at the hardware level using error detection.

Data are transferred in the same manner as in \ref USB_Interrupt_Transfers, but have no defined
polling rate. Bulk Transfers take up all the bandwidth that is available after the other transfers have finished. If the bus is
very busy, then a Bulk Transfer may be delayed.

The maximum packet size for the bulk endpoint data is:

- 8, 16, 32 or 64 bytes for full-speed
- 512 bytes for high-speed

For low-speed and full-speed endpoints the following is valid: If the bus is idle, multiple Bulk Transfers can take place
in a single 1ms frame (Interrupt and Isochronous Transfers are limited to a maximum of one packet per frame).

For example, Bulk Transfers send data to a printer. As long as the data is printed in a reasonable time frame, the exact transfer
rate is not important.

![Bulk Transfer Read and Writes](bulkTransfer.png)

### Endpoints {#USB_Endpoints}

**Endpoints** can be described as data sources or sinks and exists in USB Devices only. The data stored at an endpoint may either be
received from or waiting for being sent to the USB Host. An endpoint can be configured to support four \ref USB_Transfers "transfer types"
defined in the USB specification (\ref USB_Control_Transfers, \ref USB_Interrupt_Transfers, \ref USB_Isochronous_Transfers, and
\ref USB_Bulk_Transfers). Within the limits of the hardware, endpoints can be configured using the USB Middleware
(e.g. limit an endpoint to a certain transfer type).

An endpoint acts as a kind of buffer. A USB Host's client may send data to Endpoint 1 for example. Coming from the USB Host,
the data will be sent to the **OUT Endpoint 1**. The program on the microcontroller will then read the data as soon as it is
ready to do so. Returning data has to be written to the **IN Endpoint 1**, as the program cannot access the USB bus freely
(the USB bus being controlled by the USB Host). The data in IN Endpoint 1 stays there until the host sends an IN packet to
Endpoint 1 requesting the data.

These rules apply to all microcontroller devices:

- A device can have up to **16 OUT** and **16 IN** endpoints.
- Each endpoint can have only **one transfer direction**.
- **Endpoint 0** is used for control transfers only and can not be assigned to any other function.
- **OUT** always refers to the direction pointing from the host to the device.
- **IN** always refers to the direction pointing towards the host.

\note The total number of endpoints and the capability of each endpoint is defined by the underlying hardware. Please consult
the hardware reference manual of your microcontroller.

## USB Protocol {#USB_Protocol}

USB is a polled bus, where the host initiates all data exchanges.

### Transactions {#USB_Transactions}

Data is transferred in so called **transactions**. Normally, they consist of three packets:
-# The **token packet** is the header defining the transaction type and direction, the device address, and the endpoint.
-# Data is transferred in a **data packet**.
-# The final status of the transaction is acknowledges in the **handshake packet**.

![Pipe Model](pipemodel.png)

In a transaction, data is transferred either from the USB Host to an USB Device or vice-versa. The transfer direction
is specified in the token packet that is sent from the USB Host. Then, the source sends a data packet or indicates it has no
data to transfer. In general, the destination responds with a handshake packet indicating whether the transfer was successful.

![Packet Model](packetmodel.png)

### Packets {#USB_Packets}

**Packets** could be thought of as the smallest element of data transmission. Each packet transmits an integral number
of bytes at the current transmission rate. Packets start with a synchronization pattern, followed by the data bytes of the packet,
and concluded with an End-of-Packet (EOP) signal. All USB packet patterns are transmitted *least significant bit first*.
Before and after the packet, the bus is in *idle* state.

![Start-of-Frame (SOF) Packet](startofframe.png)

A special packet is the **Start-of-Frame** packet (SOF) that splits the USB bus into time segments. Each pipe is allocated
a slot in each frame. The Start-of-Frame packet is sent every 1ms on full speed links. At high speed, the 1ms frame is divided
into 8 microframes of 125&mu;s each. A Start-of-Frame packet is sent at the beginning of each microframe using the same
frame number. The frame number increments every 1ms.

## USB Descriptors {#USB_Descriptors}

USB devices report their attributes using **descriptors**, which are data structures with a defined format. Each
descriptor begins with a byte-wide field containing the total number of bytes in the descriptor followed by a byte-wide field
identifying the descriptor type.

When an USB device is attached to the USB bus, the host uses a process known as **bus enumeration** to identify and
configure the device. The USB Host sends setup requests as soon as the device has joined the USB network. The device will be
instructed to select a configuration and an interface to match the needs of the application running on the USB Host. Once a
configuration and an interface have been selected, the device must service the active endpoints to exchange data with the USB
Host.

This is not a complete list of all the possible descriptors an USB host can request. The usual number of descriptors are:

- One \ref USB_Device_Descriptor
- One \ref USB_Configuration_Descriptor
- One \ref USB_Interface_Descriptor
- One or more \ref USB_Endpoint_Descriptor "Endpoint_Descriptors"

\ref USB_String_Descriptor describe the above mentioned descriptors in human readable format.

![Device Configuration](deviceconfig.png)

Alternative information that is needed when the device can operate in different speed modes can be defined in a
\ref USB_Device_Qualifier_Descriptor.

Complex devices have multiple interfaces. Each interface can have
a number of endpoints representing a functional unit. For example, a
voice-over-IP phone might have:

- One audio class interface with 2 isochronous endpoints for transferring audio data in each direction.
- One HID interface with a single IN interrupt endpoint for a built-in keypad.

Provisions have been made in the USB component to give the user the option to
\ref Overriding_Descriptors "override the USB descriptors" if necessary. This can be the case when the device class needs
to be changed at run-time or other reports need to be created.

### Device Descriptor {#USB_Device_Descriptor}

The **Device Descriptor** (USB_DEVICE_DESCRIPTOR) is the root of the descriptor tree and contains basic device information. The unique numbers,
***idVendor*** and ***idProduct***, identify the connected device. The Windows operating system uses these numbers
to determine which device driver must be loaded.

***idVendor*** is the number assigned to each company producing USB-based devices. The [USB Implementers Forum](https://www.usb.org/)
is responsible for administering the assignment of Vendor IDs.

The ***idProduct*** is another 16-bit field containing a number assigned by the manufacturer to identify a specific product.

<table class="cmtable" summary="Device Descriptor">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>Device Descriptor Type = 1.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>bcdUSB</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>BCD</td>
      <td>USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210h).

      This field identifies the release of the USB Specification with which the device and its descriptors are compliant.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>bDeviceClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Class</td>
      <td>Class code (assigned by the USB-IF).
     
      If this field is
      - reset to zero, each interface within a
      configuration specifies its own class information and the
      various interfaces operate independently.
      - set to a value between 1 and FEh, the device
      supports different class specifications on different interfaces
      and the interfaces may not operate independently. This value
      identifies the class definition used for the aggregate
      interfaces.
      - set to FFh, the device class is vendor specific.</td>
    </tr>
    <tr>
      <td>5</td>
      <td>bDeviceSubClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>SubClass</td>
      <td>Subclass code (assigned by the USB-IF).
     
      These codes are qualified by the value of the *bDeviceClass* field.
     
      If *bDeviceClass* is
      - reset to zero, this field must also be reset to zero.
      - not set to FFh, all values are reserved for assignment by the USB-IF.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>bDeviceProtocol</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Protocol</td>
      <td>Protocol code (assigned by the USB-IF).
     
      These codes are qualified by the value of the *bDeviceClass* and *bDeviceSubClass* fields. If a device supports class-specific
      protocols on a device basis as opposed to an interface basis, this code identifies the protocols that the device uses as
      defined by the specification of the device class.
     
      If this field is
      - reset to zero, the device does not use class specific protocols on a device basis. However, it may use class
      specific protocols on an interface basis.
      - set to FFh, the device uses a vendor specific protocol on a device basis.</td>
    </tr>
    <tr>
      <td>7</td>
      <td>bMaxPacketSize0</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Maximum packet size for Endpoint zero (only 8, 16, 32, or
      64 are valid).</td>
    </tr>
    <tr>
      <td>8</td>
      <td>idVendor</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>ID</td>
      <td>Vendor ID (assigned by the USB-IF).</td>
    </tr>
    <tr>
      <td>10</td>
      <td>idProduct</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>ID</td>
      <td>Product ID (assigned by the manufacturer).</td>
    </tr>
    <tr>
      <td>12</td>
      <td>bcdDevice</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>BCD</td>
      <td>Device release number in binary-coded decimal.</td>
    </tr>
    <tr>
      <td>14</td>
      <td>iManufacturer</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Index</td>
      <td>Index of string descriptor describing manufacturer.</td>
    </tr>
    <tr>
      <td>15</td>
      <td>iProduct</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Index</td>
      <td>Index of string descriptor describing product.</td>
    </tr>
    <tr>
      <td>16</td>
      <td>iSerialNumber</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Index</td>
      <td>Index of string descriptor describing the device's serial
      number.</td>
    </tr>
    <tr>
      <td>17</td>
      <td>bNumConfigurations</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Number of possible configurations.</td>
    </tr>
</table>

### Configuration Descriptor {#USB_Configuration_Descriptor}

The **Configuration Descriptor** (USB_CONFIGURATION_DESCRIPTOR) contains information about the device power requirements
and the number of interfaces it can support. A device can have multiple configurations. The host can select the
configuration that best matches the requirements of the application software.

<table class="cmtable" summary="Configuration Descriptor">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>Configuration Descriptor Type = 2.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>wTotalLength</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>Number</td>
      <td>Total length of data returned for this configuration.
      Includes the combined length of all descriptors (configuration,
      interface, endpoint, and class or vendor specific) returned
      for this configuration.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>bNumInterfaces</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Number of interfaces supported by this configuration.</td>
    </tr>
    <tr>
      <td>5</td>
      <td>bConfigurationValue</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Value to select this configuration with *SetConfiguration()*.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>iConfiguration</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Index</td>
      <td>Index of string descriptor describing this configuration.</td>
    </tr>
    <tr>
      <td>7</td>
      <td>bmAttributes</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Bitmap</td>
      <td>Configuration characteristics
      - D7: Reserved (must be set to **one** for historical reasons)
      - D6: Self-powered
      - D5: Remote Wakeup
      - D4...0: Reserved (reset to zero)
     
      A device configuration that uses power from the bus and a local source reports a non-zero value in *bMaxPower* to indicate the
      amount of bus power required and sets D6. The actual power source at run-time can be determined using the GetStatus(DEVICE)
      request. If a device configuration supports remote wakeup, D5 is set to 1.</td>
    </tr>
    <tr>
      <td>8</td>
      <td>bMaxPower</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>mA</td>
      <td>Maximum power consumption of the USB device from the bus in this specific configuration when the device is fully
      operational. Expressed in 2mA units (i.e., 50 = 100mA).</td>
    </tr>
</table>

### Interface Descriptor {#USB_Interface_Descriptor}

The **Interface Descriptor** (USB_INTERFACE_DESCRIPTOR) defines the collection of endpoints. This interface supports a group of pipes that are suitable
for a particular task. Each configuration can have multiple interfaces. The interface can be selected dynamically by the USB
Host. The **Interface Descriptor** can associate its collection of pipes with a device class, which in turn has an associated class
device driver within the host operating system. Typically, the device class is a functional type such as a printer class or mass storage
class.

An interface descriptor never includes Endpoint 0 in the numbering of endpoints. If an interface uses only Endpoint 0, then the field
*bNumEndpoints* must be set to zero.

If no class type has been selected for the device, then none of the standard USB drivers is loaded, and the developer has to provide
its own device driver.

<table class="cmtable" summary="Interface Descriptor">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>Interface Descriptor Type = 4.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>bInterfaceNumber</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>The number of this interface.
     
      Zero-based value identifying the index in the array of concurrent interfaces supported by
      this configuration.</td>
    </tr>
    <tr>
      <td>3</td>
      <td>bAlternateSetting</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Value used to select an alternate setting for the
      interface identified in the prior field. Allows an interface to
      change the settings on the fly.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>bNumEndpoints</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Number of endpoints used by this interface (excluding endpoint zero).
     
      - If this value is zero, this interface uses the Default Control Pipe only.</td>
    </tr>
    <tr>
      <td>5</td>
      <td>bInterfaceClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Class</td>
      <td>Class code (assigned by the USB-IF).
     
      - A value of zero is reserved for future standardization.
      - If this field is set to FFh, the interface class is vendor specific.
      - All other values are reserved for assignment by the USB-IF.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>bInterfaceSubClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>SubClass</td>
      <td>Subclass code (assigned by the USB-IF).

      If *bInterfaceClass*
      - is reset to zero, this field must also be reset to zero.
      - is not set to FFh, all values are reserved for assignment by the USB-IF.</td>
    </tr>
    <tr>
      <td>7</td>
      <td>bInterfaceProtocol</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Protocol</td>
      <td>Protocol code (assigned by the USB).

      If an interface supports class-specific requests, this code identifies the protocols that the device uses as defined in the
      device class.<br>
      If this field
      - is reset to zero, the device does not use a class-specific protocol on this interface.
      - is set to FFh, the device uses a vendor specific  protocol for this interface.</td>
    </tr>
    <tr>
      <td>8</td>
      <td>iInterface</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Index</td>
      <td>Index of string descriptor describing this interface.</td>
    </tr>
</table>

For example, two devices with different interfaces are needed.

The first interface, *Interface0*, has the field *bInterfaceNumber* set to 0.
The next interface, *Interface1*, has the field *bInterfaceNumber* set to 1 and the field *bAlternativeSetting* also set to 0 (default).
It is possible to define an alternative setting for this device, by leaving the field *bInterfaceNumber* set to 1 and with the field
*bAlternativeSetting* set to 1 instead of 0.

The first two interface descriptors with *bAlternativeSettings* equal to 0 are used. However, the host can send a *SetInterface()*
request to enable the alternative setting.

![Alternative Interface](alt_interface.png)

### Endpoint Descriptor {#USB_Endpoint_Descriptor}

The **Endpoint Descriptor** (USB_ENDPOINT_DESCRIPTOR) specifies the transfer
type, direction, polling interval, and maximum packet size for each
endpoint. Endpoint 0 (zero), the default endpoint, is always assumed to
be a control endpoint and never has a descriptor.

<table class="cmtable" summary="Endpoint Descriptor">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>Endpoint Descriptor Type = 5.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>bEndpointAddress</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Endpoint</td>
      <td>The address of the endpoint on the USB device described by this descriptor. The address is encoded as follows:
      - Bit 3...0: The endpoint number
      - Bit 6...4: Reserved, reset to zero
      - Bit 7: Direction, ignored for control endpoints.
          - 0 = OUT endpoint
          - 1 = IN endpoint</td>
    </tr>
    <tr>
      <td>3</td>
      <td>bmAttributes</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Bitmap</td>
      <td>The endpoint attribute when configured through *bConfigurationValue*.
      - Bits 1..0: Transfer Type
          - 00 = Control
          - 01 = Isochronous
          - 10 = Bulk
          - 11 = Interrupt

      For non-isochronous endpoints, bits 5..2 must be set to zero. For isochronous endpoints, they are defined as:
      - Bits 3..2: Synchronization Type
          - 00 = No Synchronization
          - 01 = Asynchronous
          - 10 = Adaptive
          - 11 = Synchronous
      - Bits 5..4: Usage Type
          - 00 = Data
          - 01 = Feedback
          - 10 = Implicit feedback
          - 11 = Reserved
     
      All other bits are reserved and must be reset to zero.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>wMaxPacketSize</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>Number</td>
      <td>Is the maximum packet size of this endpoint.
      For isochronous endpoints, this value is used to reserve the
      time on the bus, required for the per-(micro)frame data
      payloads.
     
      - Bits 10..0 = max. packet size (in bytes).
     
      For high-speed isochronous and interrupt endpoints:
      - Bits 12..11 = number of additional transaction opportunities per micro-frame:
          - 00 = None (1 transaction per micro-frame)
          - 01 = 1 additional (2 per micro-frame)
          - 10 = 2 additional (3 per micro-frame)
          - 11 = Reserved
      - Bits 15..13 are reserved and must be set to zero.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>bInterval</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Interval for polling endpoint for data transfers. Expressed
      in frames or micro-frames depending on the operating speed (1ms, or 125&mu;s units).

      - For full-/high-speed isochronous endpoints, this value must be in the range from 1 to 16. The bInterval value
        is used as the exponent for a 2<sup>bInterval-1</sup> value; For example, a *bInterval* of 4 means
        a period of 8 (2<sup>4-1</sup>).
      - For full-/low-speed interrupt endpoints, the value of this field may be from 1 to 255.
      - For high-speed interrupt endpoints, the *bInterval* value is used as the exponent for a 2<sup>bInterval-1</sup> value;
        For Example, a *bInterval* of 4 means a period of 8 (2<sup>4-1</sup>). This value must be from 1 to 16.
      - For high-speed bulk/control OUT endpoints, the *bInterval* must specify the maximum NAK rate of the endpoint.
        A value of 0 indicates the endpoint never NAKs. Other values indicate at most 1 NAK each *bInterval* number of microframes.
        This value must be in the range from 0 to 255.
      </td>
    </tr>
</table>

### Device Qualifier Descriptor {#USB_Device_Qualifier_Descriptor}

A high-speed capable device that has different device information for full-speed and high-speed must have a **Device Qualifier
Descriptor** (USB_DEVICE_QUALIFIER_DESCRIPTOR). For example, if the device is currently operating at full-speed, the **Device Qualifier** returns information
about how it would operate at high-speed and vice-versa.

The fields for the vendor, product, device, manufacturer, and
serial number are not included. This information is constant for a
device regardless of the supported speeds.

If a full-speed only device receives a *GetDescriptor()* request for a *device_qualifier*, it must respond with a request
error. Then, the host must not make a request for an *other_speed_configuration descriptor*.

<table class="cmtable" summary="Device Qualifier Descriptor">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>Device Qualifier Descriptor Type = 6.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>bcdUSB</td>
      <td>uint16_t</td>
      <td>2</td>
      <td>BCD</td>
      <td>USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210h). This field identifies the release of the
      USB Specification with which the device and its descriptors are compliant. At least V2.00 is required to use this descriptor.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>bDeviceClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Class</td>
      <td>Class code (assigned by the USB-IF).
     
      If this field is
      - reset to zero, each interface within a configuration specifies its own class information and the various interfaces
      operate independently.
      - set to a value between 1 and FEh, the device supports different class specifications on different interfaces
      and the interfaces may not operate independently. This value identifies the class definition used for the aggregate interfaces.
     
      If this field is set to FFh, the device class is vendor specific.</td>
    </tr>
    <tr>
      <td>5</td>
      <td>bDeviceSubClass</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>SubClass</td>
      <td>Subclass code (assigned by the USB-IF).

      These codes are qualified by the value of the *bDeviceClass* field. If *bDeviceClass* is
      - reset to zero, this field must also be reset to zero.
      - not set to FFh, all values are reserved for assignment by the USB-IF.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>bDeviceProtocol</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Protocol</td>
      <td>Protocol code (assigned by the USB-IF). These codes are qualified by the values of the *bDeviceClass* and *bDeviceSubClass* fields.
      If a device supports class-specific protocols on a device basis as opposed to an interface basis, this code identifies the protocols
      that the device uses as defined by the specification of the device class.
     
      If this field is
      - reset to zero, the device does not use class-specific protocols on a device basis. However, it may use class-specific protocols
      on an interface basis.
      - set to FFh, the device uses a vendor specific protocol on a device basis.</td>
    </tr>
    <tr>
      <td>7</td>
      <td>bMaxPacketSize0</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Maximum packet size for other speed.</td>
    </tr>
    <tr>
      <td>8</td>
      <td>bNumConfigurations</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Number of other-speed configurations.</td>
    </tr>
    <tr>
      <td>9</td>
      <td>bReserved</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Zero</td>
      <td>Reserved for future use, must be zero.</td>
    </tr>
</table>

### String Descriptors {#USB_String_Descriptor}

**String descriptors** (USB_STRING_DESCRIPTOR) are optional and add human readable information to the other descriptors. If
a device does not support string descriptors, all references to string descriptors within device, configuration, and
interface descriptors must be set to zero.

String descriptors are encoded in [Unicode](https://en.wikipedia.org/wiki/Unicode) so that
multiple languages can be supported with a single product. When requesting a string descriptor, the requester specifies the
desired language using a 16-bit language ID (LANGID) defined by the USB-IF. String
index zero is used for all languages and returns a string descriptor that contains an array of two-byte LANGID codes
supported by the device.

The array of LANGID codes is not NULL-terminated. The size of the array (in byte) is computed by subtracting two from the
value of the first byte to the descriptor.

<table class="cmtable" summary="String Descriptor Zero">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>N + 2</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>String Descriptor Type</td>
    </tr>
    <tr>
      <td>2</td>
      <td>wLANGID[0]</td>
      <td>uint8_t</td>
      <td>2</td>
      <td>Number</td>
      <td>LANGID code zero (for example 0x0407 German (Standard)).</td>
    </tr>
    <tr>
      <td>...</td>
      <td>...</td>
      <td>...</td>
      <td>...</td>
      <td>...</td>
      <td>...</td>
    </tr>
    <tr>
      <td>N</td>
      <td>wLANGID[x]</td>
      <td>uint8_t</td>
      <td>2</td>
      <td>Number</td>
      <td>LANGID code zero x (for example 0x0409 English (United States)).</td>
    </tr>
</table>

The UNICODE string descriptor is not NULL-terminated. The string length is computed by subtracting two from the value of the
first byte of the descriptor.

<table class="cmtable" summary="String Descriptor Zero">
    <tr>
      <th>Offset</th>
      <th>Field</th>
      <th>Type</th>
      <th>Size</th>
      <th>Value</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>0</td>
      <td>bLength</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Number</td>
      <td>Size of this descriptor in bytes.</td>
    </tr>
    <tr>
      <td>1</td>
      <td>bDescriptorType</td>
      <td>uint8_t</td>
      <td>1</td>
      <td>Constant</td>
      <td>String Descriptor Type</td>
    </tr>
    <tr>
      <td>2</td>
      <td>bString</td>
      <td>uint8_t</td>
      <td>N</td>
      <td>Number</td>
      <td>UNICODE encoded string.</td>
    </tr>
</table>
