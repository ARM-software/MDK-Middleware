# File Demo Example {#File_Demo_Example}

This example shows how to manipulate files on a memory drive using the File System Component.

You can mount and format the drive and then perform operations like write, read, rename and delete to manage files and directories on any enabled drive. Example uses serial terminal connection to interact with the user.

Typically one connects the development board to a computer via debug connection that also provides virtual COM port, downloads example application to a board and starts terminal application on computer.

The following picture shows an exemplary connection of the development board and a computer:

![Hardware setup of the File Demo example](fs_setup.png)

## Project Organization

The File Demo project is available as part of the \ref examples "File System Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

- `File_Demo.c` contains the main application thread and implementation of commands for user interaction.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components.

Following configuration files are provided with this example:

- For the File System component, in the `./RTE/File_System/` folder:
  - `FS_Config.h`: \ref fs_configuration "File System configuration".
  - `FS_Config_MC_0.h`: \ref mc_usage "File System Memory Card configuration".
  - `FS_Debug.h`: \ref fsEvrConfig "File System Debug configuration".
- For the CMSIS components, in the `./RTE/CMSIS/` folder:
  - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.

<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 
 - `CMSIS_MCI`: CMSIS-Driver for Memory Card Interface
 - `CMSIS_VIO`: CMSIS-Driver for Virtual I/O interface
 - `STDIN`: Standard Input redirection
 - `STDOUT`: Standard Output redirection

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **FileSystem** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **File_Demo** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, SD card ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Insert an SD Card to the corresponding slot on the development board.
 -# Connect your serial (virtual COM) port connected to the computer.

**Example execution**

Open any software terminal emulator application to start interacting with the example running on the Evaluation Board. After resetting the board, you should see something similar like this:

![Terminal output](fs_example_intro.png)

Now you can type **help** to the command prompt and press enter (make sure that the new line character is also sent after
pressing enter). Now something similar to the below image should be printed:

![Output of the help command](fs_example_help.png)

You can now type the listed commands and send arguments directly to the FileSystem API functions.
