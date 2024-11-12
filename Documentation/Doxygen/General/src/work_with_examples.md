# Working with Examples {#working_with_examples}

The MDK-Middleware examples are implemented as [CMSIS-Toolbox Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md)
that use [CMSIS-Driver](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html) interfaces. These Reference Applications are hardware agnostic and need to be extended with a compatible [board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#board-layer) to run on a specific hardware target.

Several [Board Support Packs (BSP)](https://www.keil.arm.com/packs/) contain board layers that support the MDK-Middleware components. Refer to the *Overview* page of the pack to check the *Provided connection API Interface* of the layers. When such a board layer is not available, it is possible to [create a compatible board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#structure).

## Available examples

The examples are provided as part of the MDK-Middleware pack and maintained as part of its GitHub repository, see \ref mw_access.

For a detailed list of available examples and their detailed description, see the **Examples** section in the component documentation.

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
> - ST board layers are configured for the Arm Compiler (AC6) using STM32CubeMX.  However, it is easy to reconfigure for different compilers. The steps are provided in the BSP overview.

### API Interfaces

The MDK-Middleware Reference Applications are hardware agnostic but require API Interfaces that are expressed using the *csolution project* [connections:](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md#connections) node. The various reference applications consume the following API Interfaces. These [interfaces should be provided by the board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) that is part of the Board Support Pack (BSP).

Consumed API Interface      | Description
:---------------------------|:----------------
**File System**             | .
CMSIS_MCI                   | [CMSIS-Driver MCI](https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html) Interface to memory.
STDIN, STDOUT               | Standard I/O for user input/output via UART.
**Network**                 | .
CMSIS_ETH                   | [CMSIS-Driver Ethernet](https://arm-software.github.io/CMSIS_6/latest/Driver/group__eth__interface__gr.html) MAC/PHY Interface.
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
   > - You may copy an existing board layer as starting point. But typically these board layers support a range of reference applications and contain driver API interfaces that may be removed.

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

Refer to [Application Note 320: Using Event Recorder for debugging a network performance issue](https://developer.arm.com/documentation/kan320/latest/) for an tutorial on how to analyze MDK-Middleware issues.

## Using uVision IDE

The [uVision v5.41](https://www.keil.arm.com/mdk-community/) IDE or higher allows to directly work with *csolution projects*. Source code can be modified, build commands can be executed, and after configuration the [uVision Debugger](https://developer.arm.com/documentation/101407/0541/Debugging) can be used. Adding files or software components is possible by modifying the *csolution project yml files*. It is not directly supported with a user interface.

![Import csolution project in uVision](Use-uVision.png)

### Create a native uVision Project

As uVision IDE is easy-to-use and powerful many developers want to use this IDE for productive software development. The MDK-Middleware Reference Applications can be recreated as clean, native uVision project by following the steps listed below, once configured with a compatible board layer. Such a board layer can be copied from an existing working csolution project in VS Code by following the section [Using VS Code](https://arm-software.github.io/MDK-Middleware/latest/General/working_with_examples.html#autotoc_md1) above, or from an exisitng BSP pack that contains a board layer.

1. Create a new folder and copy the source files of the MDK-Middleware Reference Application and the software layer, e.g. the board layer. It is recommended to keep the folder structure. In this new folder create a new uVision project for the selected target device.
2. Add source files and software components listed in the `*.cproject.yml` of the MDK-Middleware Reference Application and `*.clayer.yml` 
of the software layer e.g. the board layer into the new µVision project using the uVision dialogs.
3. Copy the existing Run-Time-Environment (RTE) configuration files as well as the board layer files to the correct subdirectory of the new uVision project.
4. Configure tool settings using the uVision *Options for Target* dialogs and add linker script.



**These steps are described in more detail below.**

MDK-Middleware Reference Applications contain a collection of projects. Taking the USB Device HID project from the MDK-Middleware Reference Applications as an example. Case 1 represents the case with an existing csolution project created in VS Code by following the section [Using VS Code](https://arm-software.github.io/MDK-Middleware/latest/General/working_with_examples.html#autotoc_md1) above. Case 2 represents the case without an existing csolution project when copying from the Examples subfolder of the MDK-Middleware pack v8.x as well as from a BSP pack containing the board layer.

#### Create new Project Folder

 Create a new folder for the µVision project and copy the source files from the MDK-Middleware Reference Application.


Case 1: from csolution project | copy to new uVision project folder   | Notes
:------------------------------|:-------------------------------------|:------
`./HID`                        | `<MyFolder>/HID`                     | Only copy content from the root directory without subfolders
`./Board/<board_name>`         | `<MyFolder>/Board/<board_name>`      | Only copy source files (`*.c` and `*.h`) from the root directory without subfolders

Case 2: from packs             | copy to new uVision project folder   | Notes
:------------------------------|:-------------------------------------|:------
`./Examples/USB/Device/HID` from MDK-Middleware pack| `<MyFolder>/HID`| Only copy content from the root directory without subfolders
`./Layers/Default/` from a BSP pack| `<MyFolder>/Board/<board_name>`  | Only copy source files (`*.c` and `*.h`) from the root directory without subfolders

> **Note:**
>
> - Different BSP pack may have different folder struture other than `./Layers/Default/` that contains board layer files.

From the uVision menu use *Project - New uVision Project...* dialog to select the device that you are using and create a new µVision project.

#### Add Source Files and Components

The `*.cproject.yml` of the reference application and `*.clayer.yml` of the software layer, e.g. board layer contain a list of source files and components that should be added to the new uVision project.

- Add source files: In the uVision Project Window, click on a file group and *Add Existing Files to Group*. Feel free to add more file groups to structure your project.
  
- Add components: From the uVision menu use *Project - Manage - Run Time Environment...* and select the components.

> **Note:**
>
> - Do not start a generator such as STM32CubeMX at this step as the configuration files are copied in the next step.

#### Copy Config Files

The RTE configuration files and generator files (for STM32CubeMX or MCUXpresso Config) are fully compatible with uVision. Follow the table below to copy these configuration files to the µVision project folder

Case 1: from csolution project   | copy to new uVision project folder       | Notes
:--------------------------------|:-----------------------------------------|:------------
`./HID/RTE`                      | `<MyFolder>/RTE`                         | Only copy component folders excluding folders that start with `_` 
`./Board/<board_name>/RTE`       | `<MyFolder>/RTE`                         | Only copy component folders excluding folders that start with `_` 
`./Board/<board_name/CubeMX>`    | `<MyFolder>/STM32CubeMX/<uVision_target_name>` | Rename the `*.cgen.yml` file

Case 2: from packs  | copy to new uVision project folder       | Notes
:--------------------|:-----------------------------------------|:------------
`./Examples/USB/Device/HID/RTE` from MDK-Middleware pack| `<MyFolder>/RTE`                         | Only copy component folders excluding folders that start with `_` 
`./Layers/Default/RTE` from a BSP pack  | `<MyFolder>/RTE`                         | Only copy component folders excluding folders that start with `_` 
`./Layers/Default/CubeMX` from a BSP pack | `<MyFolder>/STM32CubeMX/<uVision_target_name>` | Rename the `*.cgen.yml` file

> **Note:**
>
> - The `*.cgen.yml` file copied from the existing csoution project or from the BSP pack must be renamed to match your µVision project name, i.e. `<MyProject>.cgen.yml`.
> - Different BSP pack may have different folder struture other than `./Layers/Default/` that contains board layer files.

#### Configure Tool Settings

In the Case 1 the compiler toolchain relevant settings of the *csolution project* are in the `*.csolution.yml` and `*.cproject.yml` files. Based on these settings configure compiler toolchain in uVision using the tabs of the *Options* dialog:

- *Target*: verify that hardware settings are correctly reflected, i.e. `Software Model: TrustZone Off`.

- *C/C++ (AC6)*: Adjust Language / Code Generation.
  - Typical settings: Optimization: `-O1` (for Debug), `-O3` (for Release), Warnings: `AC5-like`, Language C: `C11`.
  - Board layer specific definitions are defined in a header file, such as `board_name.h`, which can be found in `./Board/<board_name>` of the *csolution project* in the Case 1 or in the `./Layers/Default/` of the BSP pack in the Case 2. This `board_name.h` should be set as the value of a preprocessor symbol `CMSIS_target_header` in *Options for Target - C/C++(AC6) - Preprocessor Symbols*  

  ![Typical Compiler Options Settings](Options-C.png)

- *Linker*: Configure Scatter File and adjust warnings.
  - In the Case 1 with an existing csolution project, a read-to-use linker scatter file that has already been preprocessed by CMSIS-Toolbox in VS Code, typically located in `.\tmp\1\ac6_linker_script.sct`, should be copied to `<MyFolder>/Board/<board_name>` . In the Case 2 without an existing csolution project, since the native uVision project manager does not offer the same [linker script management](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-overview.md#linker-script-management) as a csoution project in VS Code IDE. follow these steps to configure a scatter file in µVision.
        1. Copy Linker Script Templates file e.g. `ac6_linker_script.sct.src` from the directory <cmsis-toolbox-installation-dir>/etc of the CMSIS-Toolbox or from the RTE directory of the BSP pack, such as `./Layers/Default/RTE`, to `<MyFolder>/Board/<board_name>`.
        2. Copy the memory regions header file e.g. regions_xxx.h from the same RTE directory of the BSP pack to `<MyFolder>/Board/<board_name>`.
        3. Add C preprocessor, such as `#! armclang --target=arm-arm-none-eabi -march=armv7-m -E -x c`, as the first line of `ac6_linker_script.sct.src`
        4. Add `#include "regions_xxx.h"` as the second line to `ac6_linker_script.sct.src`
  - In `cdefault.yml` included in some MDK-Middleware Reference Applications there may be some linker controls that should be reflected in this dialog, for example `--diag_suppress=L6314W`.

  ![Typical Linker Options Settings](Options-Linker.png)

#### Expand with additional components
Once the new project is created, it may be expanded with additional software components or modified to custom hardware as shown in the picture below. Note that uVision projects have no dependency on specify hardware boards.

![Create new project in uVision](Create-uVision-Project.png)
