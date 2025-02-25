# USB Device Reference Applications

**USB Device Reference Examples** contain the following projects (in the corresponding subfolders) that implement specific USB Device functionality using the MDK-Middleware USB component:

| Example Project                           | Description                                                                   |
|-------------------------------------------|-------------------------------------------------------------------------------|
| [USB Device HID](./HID)                   | Uses USB HID class to control LEDs and push buttons on the development board. |
| [USB Device Mass Storage](./MassStorage)  | Implements a mass storage USB device using internal RAM as storage media.     |
| [USB Device Virtual COM](./VirtualCOM/)   | Provides a virtual communication port to UART bridge to the USB Host.         |

For more details refer to [USB Device Examples section in the MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbd_examples.html).

Also see [CMSIS-Toolbox - Reference Applications](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/) to learn more about the concept of reference application in Open CMSIS Pack used by these examples.
