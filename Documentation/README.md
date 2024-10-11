# MDK-Middleware

The MDK-Middleware software pack contains components for IPv4 and IPv6 networking, USB Host and Device communication, as well as file system for data storage. It is restricted for commercial free-of-charge usage with Arm-based devices.

![MDK-Middleware](./MDK-Middleware.svg)

The MDK-Middleware can be used by any user application. It is used by the [Synchronous Data Streaming Framework](https://github.com/ARM-software/SDS-Framework) to save data recorded from sensors. IoT Connectors can use the *Network* component to connect to the Internet.

MDK-Middleware uses a CMSIS-RTOS2-based real-time operating system for task scheduling, for example [Keil RTX5](https://github.com/ARM-software/CMSIS-RTX) or [CMSIS-FreeRTOS](https://github.com/arm-software/CMSIS-freertos).

The *Network* component uses the [Arm Mbed TLS](https://www.keil.com/pack/ARM.mbedTLS.3.1.0.pack) stack to secure the TCP/IP communication. [CMSIS-Drivers](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html) are a required for the components to work with the underlying hardware.

The [CMSIS-Compiler](https://github.com/arm-software/cmsis-compiler) and [CMSIS-View](https://github.com/arm-software/cmsis-view) components can display events and static information from all MDK-Middleware components.

The software components that are part of this [CMSIS-Pack](https://open-cmsis-pack.github.io/Open-CMSIS-Pack-Spec/main/html/index.html) are:

- **File System Component**: create, save, read, and modify files in storage devices such as RAM, Flash, SD/SDHC/MMC memory cards, or USB memory devices.
- **Network Component**: services, protocol sockets, and physical communication interfaces for creating networking applications. It supports both, IPv4 and IPv6.
- **USB Component**: create USB Device and USB Host applications with standard USB device classes.

> *Note:*  
> Each component is configurable for a wide range of applications and requires the driver interface as described by the [CMSIS-Driver standard](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html). Check with your silicon vendor about the availability of CMSIS-Drivers for your selected microcontroller device.

## Availability

The MDK-Middleware is available free-of-charge to all users of Arm Cortex-M-based processors. It does not require a specific toolchain or IDE license and can be built with major toolchains, such as Arm Compiler 6, GCC, and LLVM.
