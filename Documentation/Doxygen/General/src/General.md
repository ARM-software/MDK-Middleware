# Overview {#mainpage}

The free-to-use **MDK-Middleware** contains software components for IPv4 and IPv6 networking, USB Host and Device
communication, and file system for data storage. The components can for example be used by the user's application, IoT
connectors for cloud connectivity, or the \urlout{SDS}.

![MDK Software Packs](./images/MDK-Middleware.png)

This documentation describes:

- \url{MW-FileSystem}: create, save, read, and modify files in storage devices such as RAM, Flash, SD/SDHC/MMC memory cards,
  or USB memory devices.
- \url{MW-Network}: services, protocol sockets, and physical communication interfaces for creating networking applications.
- \url{MW-USB}: create USB Device and USB Host applications with standard USB device classes.

Software components or APIs outside the scope of this documentation are:

- \urlout{CMSIS-RTOS2}: the real-time operating system API used by all middleware components (required).
- \urlout{mbedTLS}: provides encryption and secure communication for network components (optional).
- \urlout{CMSIS-Compiler}: simplifies retargeting of the standard C run-time library (optional).
- \urlout{CMSIS-View}: equips software developers with software components, utilities and methodologies that provide
  visibility into internal operation of embedded applications and software components (optional).
- \urlout{CMSIS-Driver}: required \ref DriverInterface "driver interface" for the MDK-Middleware software components  (required).

> **Notes**
> - Before using MDK-Middleware, please make sure that your selected microcontroller device offers \urlout{CMSIS-Driver} support.
> - The middleware components require a \urlout{CMSIS-RTOS2} compliant real-time operating system for task scheduling.
> - MDK-Middleware uses software components from the new \urlout{CMSIS-View} and \urlout{CMSIS-Compiler} CMSIS-Packs.

## Supported toolchains {#gen_supported_toolchains}

MDK-Middleware components are available as source code and will work with all major toolchains including Arm Compiler 6, GCC and IAR.

## Documentation {#mw_using_mw}

This user's guide contains the following chapters:

- \subpage mw_using_generic_steps explains the necessary steps to create from scratch a project containing a Middleware component.
- \subpage mw_using_stack_and_heap describes how to configure a project to meet the resource requirements of the Middleware.

Every middleware component has a section on the following topics to explain how to create an application:

- **Create an Application** contains step-by-step instructions for using the components in an embedded application.
- **Example projects** outline key product features of the software components. They are tested, implemented, and proven on several evaluation boards and can be used as a starting point for application development.
- **Resource Requirements** state the stack, thread and memory resource requirements for every component.
- **Reference** documents the files of a component and each API function.

The following table gives you links to the respective sections of each middleware component:

\anchor doc_mw
<table class="doxtable">
  <tr>
    <th>Topic</th>
    <th colspan="5" style="text-align:center;">Links to Documentation</th>
  </tr>
  <tr>
    <td><b>Create an Application</b></td>
    <td><a href="../FileSystem/fs_create_app.html"><b>File System</b></a></td>
    <td><a href="../Network/network_create_app.html"><b>Network</b></a></td>
    <td><a href="../USB/USB_Device.html#Creation_Steps"><b>USB Device</b></a></td>
    <td><a href="../USB/USB_Host.html#Create_a_USB_Host_Application"><b>USB Host</b></a></td>
  </tr>
  <tr>
    <td><b>Example projects</b></td>
    <td><a href="../FileSystem/fs_examples.html"><b>File System</b></a></td>
    <td><a href="../Network/nw_examples.html"><b>Network</b></a></td>
    <td><a href="../USB/USB_Device_tutorial.html"><b>USB Device</b></a></td>
    <td><a href="../USB/USB_Host_tutorial.html"><b>USB Host</b></a></td>
  </tr>
  <tr>
    <td><b>Resource Requirements</b></td>
    <td><a href="../FileSystem/fs_resource_requirements.html"><b>File System</b></a></td>
    <td><a href="../Network/nw_resource_requirements.html"><b>Network</b></a></td>
    <td><a href="../USB/usb_resource_requirements.html#usbd_res_req"><b>USB Device</b></a></td>
    <td><a href="../USB/usb_resource_requirements.html#usbh_res_req"><b>USB Host</b></a></td>
  </tr>
  <tr>
    <td><b>Reference</b></td>
    <td><a href="../FileSystem/modules.html"><b>File System</b></a></td>
    <td><a href="../Network/modules.html"><b>Network</b></a></td>
    <td><a href="../USB/group__usbd.html"><b>USB Device</b></a></td>
    <td><a href="../USB/group__usbh.html"><b>USB Host</b></a></td>
  </tr>
</table>
