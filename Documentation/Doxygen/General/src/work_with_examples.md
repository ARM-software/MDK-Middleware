# Working with MDK-Middleware Examples {#working_with_examples}

The MDK-Middleware examples are implemented as [CMSIS-Toolbox Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md)
that use [CMSIS-Driver](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html) interfaces. These Reference Applications are hardware agnostic and need to be extended with a compatible [board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#board-layer) to run on a specific hardware target.

Several CMSIS Board Support Packs (BSP) available in [github.com/Open-CMSIS-Pack](https://github.com/Open-CMSIS-Pack) contain  board layers that support the MDK-Middleware components. When such a board layer is not available, it is possible to [create a compatible board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#structure).

## Available examples

The examples are provided as part of the MDK-Middleware pack and maintained as part of its GitHub repository, see \ref mw_access.

For detailed list of available examples and their detailed description, see the **Examples** section in the component documentation.

 - [**File System examples**](../FileSystem/examples.html)
 - [**Network examples**](../Network/examples.html)
 - [**USB Device examples**](../USB/usbd_examples.html)
 - [**USB Host examples**](../USB/usbh_examples.html)

This chapter gives a generic overview on how to access, configure and build an MDK-Middleware example project for your target hardware.

## Using VS Code

This section explains how to use MDK-Middleware with the [Arm CMSIS Solution](https://marketplace.visualstudio.com/items?itemName=Arm.cmsis-csolution) for VS Code. This extension is for example part of [Keil Studio](https://www.keil.arm.com/).

### Explore MDK-Middleware Examples

An MDK-Middleware reference example can be selected in the [Create a new solution](https://developer.arm.com/documentation/108029/latest/Arm-CMSIS-Solution-extension/Create-a-solution) dialog for boards with layers in the BSP.

![Select Reference Application](SelectReferenceApplication.png)

Once the *csolution project* is loaded the VS Code IDE presents you with a dialog that lets you select a compatible software layer and a compiler toolchain that is available on your computer.

![Configure Solution](ConfigureSolution.png)

> **Notes:**
>
> - The **Add Software Layer** dialog only appears when the BSP contains a board layer with compatible API Interfaces (see next section).
> - ST board layers are configured for the Arm Compiler (AC6) using CubeMX.  However it is easy to reconfigure for different compilers. The steps are provided in the BSP overview.

### API Interfaces

The MDK Middleware reference applications are hardware agnostic but require API Interfaces that are expressed using the *csolution project* [connections:](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md#connections) node. The various reference applications consume the following API Interfaces. These [interfaces should be provided by the board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) that is part of the Board Support Pack (BSP).

Consumed API Interface      | Description
:---------------------------|:----------------
**File System**             | .
CMSIS_MCI                   | [CMSIS-Driver MCI](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html) Interface to memory.
STDIN, STDOUT               | Standard I/O for user input/output via UART.
**Network**                 | .
CMSIS_Ethernet              | [CMSIS-Driver Ethernet](https://arm-software.github.io/CMSIS_6/latest/Driver/group__eth__interface__gr.html) MAC/PHY Interface.
CMSIS_VIO                   | [CMSIS-Driver VIO](https://arm-software.github.io/CMSIS_6/latest/Driver/group__vio__interface__gr.html) connected to LEDs and button
STDOUT                      | Standard I/O connected for printf messages.
**USB Device**              | .
CMSIS_USB_Device            | [CMSIS-Driver USB Device](https://arm-software.github.io/CMSIS_6/latest/Driver/group__usbd__interface__gr.html) Interface.
CMSIS_VIO                   | [CMSIS-Driver VIO](https://arm-software.github.io/CMSIS_6/latest/Driver/group__vio__interface__gr.html) connected to LEDs and button
CMSIS_USART                 | [CMSIS-Driver USART](https://arm-software.github.io/CMSIS_6/latest/Driver/group__usart__interface__gr.html) for VirtualCOM project.
**USB Host**                | .
CMSIS_USB_Host              | [CMSIS-Driver USB Host](https://arm-software.github.io/CMSIS_6/latest/Driver/group__usbh__interface__gr.html) Interface.

### Compile for Board

The MDK-Middleware Reference Applications are typically bundles of several similar projects. Use the command `CMSIS:Manage Solution Settings` to choose a one project that you want to explore.

![Select Project](SelectProject.png)

### Compile for Custom Hardware

The steps to add a custom hardware configuration are:

- Open the `*.csolution.yml` file and add a new `target-type`.

    ```yml
    target-types:
    - type: MyHardware
      device: STM32U535CBTx
      variables:
        - Board-Layer: $SolutionDir()$\MyHardware\Board.clayer.yml
    ```

- Add a board layer that implements the API interfaces described above.

   > **Note:**
   >
   > - You may copy an existing board layer as starting point. But typically the support a range of reference applications and therefore several interfaces may be removed.

### Using uVision for Debug

The [uVision Debugger](https://developer.arm.com/documentation/101407/0541/Debugging) offers advanced debug features such as Event Recorder and Component Viewer to analyze the MDK-Middleware.

To call uVision with the *csolution project* that you are using in VS Code, add to the file `.vscode\tasks.json` the following task. The `command:` is the path to the uVision executable on your computer.

```json
    "tasks": [
        {
            "label": "Start uVision",
            "type": "process",
            "command": "C:\\Keil_v5\\UV4\\UV4.exe",
            "args": [
                "${command:cmsis-csolution.getSolutionPath}"
            ],
            "problemMatcher": []
        }
    ]
```

Use the VS Code menu command **Terminal - Run Task...** to Start uVision.  uVision can directly load *csolution projects*. After you have configured the debugger for your target system you may use the [Debug Windows and Dialogs](https://developer.arm.com/documentation/101407/0541/Debugging/Debug-Windows-and-Dialogs) to validate your application.

Refer to [Application Note 320: Using Event Recorder for debugging a network performance issue](https://developer.arm.com/documentation/kan320/latest/) for an tutorial on how to analyze MDK Middleware issues.

## Using uVision IDE

This section explains how to create applications using the uVision IDE.

