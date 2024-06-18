# File System Examples {#fs_examples}

The File System Component is used in many different applications and examples. One stand-alone example is available to demonstrate
the usage of the File System. Other examples use the File System Component in conjunction with other Components (such as USB or
Network for example).
- The \ref fs_standalone_example shows the basic functionality of the File System.
- The \url{USBD-MSC-Ex} shows how to
  create an USB MSC Device that is recognized by an USB Host controller.
- The \url{USBH-MSC-Ex} explains how to access file system data from an attached USB memory device.
- The \url{FTP-Server-Ex} accesses the device's file system via a network connection.

These examples are available through the \urlout{uv4_ca_packinstaller}; select the related board and copy the example.

## File System Example {#fs_standalone_example}

This example shows how to manipulate files on a given drive using the File System Component. You can mount and format the drive
and then perform operations like write, read, rename and delete to manage files and directories on any enabled drive. Example
uses serial terminal connection to interact with the user.

Typically one connects the development board to a computer via debug connection that also provides virtual COM port, downloads
example application to a board and starts terminal application on computer. The following picture shows an exemplary connection
of the development board and a computer:

\image html "fs_setup.png"

### Build the "File System" Project {#fs_example_build}

Open the example project in MDK (the \urlout{uv4_ca_packinstaller} web page explains how to do this). The project tree should have
a similar structure:

\image html "fs_proj_structure.png" "File System Project Structure"

#### Source Files {#fs_example_sources}

- `File_Demo.c` contains the main application thread and implementation of commands for user interaction.

You may now build and download the example project to the evaluation board.

After these steps, the project should start executing on your evaluation kit. In case of errors, refer to the Evaluation Board
User's Guide for configuration information.

### Using the "File System" Project {#fs_example_using}

#### Hardware Setup {#fs_example_using_hw}

One you have example application executing on the Evaluation Board hardware you need to have serial (virtual COM) port
connected to the computer.

#### PC Software {#fs_example_using_pc}

Open any software terminal emulator application to start interacting with the example running on the Evaluation Board.
After resetting the board, you should see something similar like this:

\image html "fs_example_intro.png"

Now you can type **help** to the command prompt and press enter (make sure that the new line character is also sent after
pressing enter). Now something similar to the below image should be printed:

\image html "fs_example_help.png"

You can now type the listed commands and send arguments directly to the FileSystem API functions.
