# Working with Examples {#working_with_examples}

The MDK-Middleware examples are implemented as [CMSIS-Toolbox Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md)
that use [CMSIS-Driver](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html) interfaces. These Reference Applications are hardware agnostic and need to be extended with a compatible [board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#board-layer) to run on a specific hardware target.

Several [Board Support Packs (BSP)](https://www.keil.arm.com/packs/) contain board layers that support the MDK-Middleware components. Refer to the *Overview* page of the pack to check the *Provided connection API Interface* of the layers. When such a board layer is not available, it is possible to [create a compatible board layer](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#structure).

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

Refer to [Application Note 320: Using Event Recorder for debugging a network performance issue](https://developer.arm.com/documentation/kan320/latest/) for an tutorial on how to analyze MDK Middleware issues.

## Using uVision IDE

The [uVision v5.41](https://www.keil.arm.com/mdk-community/) IDE or higher allows to directly work with *csolution projects*. Source code can be modified, build commands can be executed, and after configuration the [uVision Debugger](https://developer.arm.com/documentation/101407/0541/Debugging) can be used. Adding files or software components is possible by modifying the *csolution project yml files*. It is not directly supported with a user interface.

![Import csolution project in uVision](Use-uVision.png)

### Create a native uVision Project

As uVision IDE is easy-to-use and powerful many developers want to use this IDE for productive software development. Once configured with a compatible board layer, the Reference Applications can be recreated as clean, native uVision project using these steps.

1. Create a new folder and copy the source files of the reference application and the software layer. It it recommended to keep the folder structure. In this new folder create a new uVision project and select the target device.
2. Add source files and software components listed in the `cproject.yml` and `clayer.yml` using the uVision dialogs.
3. Copy the existing configuration files to the RTE directory of the new uVision project.
4. Configure tool settings using the uVision options dialogs and add linker script.

These steps are described in more detail below.

Once the new project is created, it may be expanded with additional software components or modified to custom hardware as shown in the picture below. Note that uVision projects have no dependency on specify hardware boards.

![Create new project in uVision](Create-uVision-Project.png)

#### Create new Project Folder

Reference Applications contain typically a collection of projects. In a bespoke uVision project, most likely only a subset is required. Choose the example that to want to start from, then create a new folder and copy the source files from the Reference Application. Below this is exemplified on USB Device HID.

*csolution project*        | copy to new uVision project folder      | Notes
:--------------------------|:----------------------------------------|:-------------
`./HID`                    | `<MyFolder>/HID`                        | Only copy content from root.
`./Board/<board>`          | `<MyFolder>/Board/<board>`              | Only copy source files (`*.c` and `*.h`).

From the uVision menu use *Project - New uVision Project...* dialog to select the device that you are using.

#### Add Source Files and Components

The `cproject.yml` and `clayer.yml` contains a list of source files and components that should be added to the new uVision project.

- Add source files: In the uVision Project Window, click on a file group and *Add Existing Files to Group*. Feel free to add more file groups to structure your project.
  
- Add components: From the uVision menu use *Project - Manage - Run Time Environment...* and select the components.

> **Note:**
>
> - Do not start a generator such as CubeMX as the configuration is copied in the next step.

#### Copy Config Files

The RTE configuration files and generator files (for CubeMX or MCUXpresso Config) are fully compatible with uVision. However the folder structure is different.

*csolution project*      | copy to new uVision project folder       | Notes
:------------------------|:-----------------------------------------|:------------
`./HID/RTE`              | `<MyFolder>/RTE`                         | Only copy component folders; exclude folders that start with `_` .
`./Board/RTE`            | `<MyFolder>/RTE`                         | Only copy component folders; exclude folders that start with `_` .
`./Board/<board>`        | `<MyFolder>/STM32CubeMX/<target>`        | Rename the `*.cgen.yml` file.

> **Note:**
>
> - The `*.cgen.yml` file has a different name that is derived from your project name, i.e. `<MyProject>.cgen.yml`.

#### Configure Tool Settings

The settings of the *csolution project* are in the `*.yml` files. Adjust these settings in uVision using the tabs of the *Options* dialog:

- *Target*: verify that hardware settings are correctly reflected, i.e. `Software Model: TrustZone Off`.

- *C/C++ (AC6)*: Adjust Language / Code Generation as the default settings of the CMSIS-Toolbox differ.
  - Typical settings: Optimization: `-O1` (for Debug), `-O3` (for Release), Warnings: `AC5-like`, Language C: `C11`.
  - Defines that are in the *csolution project* should be reflected. Typically there is a define of `CMSIS_target_header` in the `Board.clayer.yml` that also requires an include path.

  ![Typical Compiler Options Settings](Options-C.png)

- *Linker*: Configure Scatter File and adjust warnings.
  - The native uVision project manager does not offer the same [linker script management](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-overview.md#linker-script-management). Copy therefore the preprocessor output of the CMSIS-Toolbox, typically the file `.\tmp\1\ac6_linker_script.sct` to `<MyFolder>/Board/<board>`.
  - In `cdefault.yml` there may be some linker controls that should be reflected in this dialog, for example `--diag_suppress=L6314W`.

  ![Typical Linker Options Settings](Options-Linker.png)