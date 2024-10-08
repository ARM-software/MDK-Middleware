# Migration Guide {#usb_migration}

**MDK-Middleware v8** introduced the following changes:

- **General**:
  - **MDK bundle** with source code replaces MDK-Pro and MDK-Plus bundles with pre-built libraries.
  - **removed CORE component variants**.
  - added requirement for C compiler version 11 (C11).
  - removed support for legacy Arm Compiler 5.
  - removed support for CMSIS-RTOS version 1.

- **USB debug**:
  - **changed source configuration file to header** (`USB_Debug.c` -> `USB_Debug.h`).

- **USB Device specific**:
  - **changed source configuration files to headers** (`USBD_Config_0..3.c` -> `USBD_Config_0..3.h`).

- **USB Host specific**:
  - **changed source configuration files to headers** (`USBH_Config_0..3.c` -> `USBH_Config_0..3.h`).
  - **removed OHCI and EHCI drivers** (moved to [CMSIS-Driver](https://github.com/ARM-software/CMSIS-Driver) pack).

The following sections show how to migrate projects from previous version of the MDK-Middleware (v7):

- \ref usb_migration_component describes how to migrate the USB Component selection.
- \ref usb_migration_config describes how to migrate the USB Component configuration.

## Component Selection {#usb_migration_component}

To migrate an USB component, make the following changes to the component selection:

1. Bundle **MDK** has replaced bundles **MDK-Pro** and **MDK-Plus**

   Where applicable, replace the components:
   ```
     USB&MDK-Pro
   ```

     or
   ```
     USB&MDK-Plus
   ```

   with the component that refers to the **MDK** bundle:
   ```
     USB&MDK
   ```

2. Variants of the **CORE** component were removed

   Where applicable, replace the components:
   ```
     CORE&Debug
   ```

     or
   ```
     CORE&Release
   ```

   with the component without variant specified:
   ```
     CORE
   ```

3. **Event Recorder** component is now a part of the **CMSIS-View** class

   If your project is configured for debugging, replace the component:
   ```
     Compiler:Event Recorder
   ```

   with the same component of the CMSIS-View class:
   ```
     CMSIS-View:Event Recorder
   ```

## Configuration {#usb_migration_config}

The following screenshots show the previous settings on the left-hand side, while the new settings
can be seen on the right-hand side.

### Debug

- if the **USB_Debug.c** file exists, then compare it with the new **USB_Debug.h** file and copy
  old values into the new file:
  - if the old file does not exist then debugging was not configured previously
  - note that **USB Device Debug** and **USB Host Debug** order has swapped in the new configuration file.
  ![Old USB_Debug.c to new USB_Debug.h](usb_debug_migrate.png)

### USB Device

- compare the existing **USBD_Config_0..3.c** files with the new **USBD_Config_0..3.h** files and copy
  old values into the new files:
  ![Old USBD_Config_0..3.c to new USBD_Config_0..3.h](usbd_config_migrate.png)

### USB Host

- compare the existing **USBH_Config_0..3.c** files with the new **USBH_Config_0..3.h** files and copy
  old values into the new files:
  - the **USBH_Config_0..3.h** does not contain separate settings for **Custom**, **OHCI** or **EHCI**
    controllers anymore, so to migrate to the new configuration file note which **Controller Interface**
    was selected in the old **USBH_Config_0..3.c** file and copy appropriate controller's previous settings
    to the new configuration file.
  - for **Memory Pool Size** update the new value by **adding 128** since old setting did not include
    additional space necessary for memory pool internal operation.
  ![Old USBD_Config_0..3.c to new USBD_Config_0..3.h](usbh_config_migrate.png)

\note **OHCI** and **EHCI** drivers are not a part of the **MDK-Middleware** anymore but are distributed separately
      via the [CMSIS-Driver](https://github.com/ARM-software/CMSIS-Driver) pack, so please consult CMSIS-Driver documentation
      on how to configure and use those drivers.
