# USB Host Computer Applications {#usb_sw_utilities}

Most of the available USB Device Classes are directly supported in operating systems. There is no need to install a particular driver on the USB Host Computer to connect to the USB Device peripheral.

Some software needs to be written for the USB Host Computer when USB Driver classes are used to provide application specific functionality. For example, the USB HID Device Class may be used for generic data exchange. Therefore the data exchange interface needs to be developed at the USB Host Computer.

Refer to the following web pages for more information on USB Host software development:

 - [USB Serial Bus (USB)](https://learn.microsoft.com/en-us/windows-hardware/drivers/) for Microsoft Windows.
 - [Linux USB Project](http://www.linux-usb.org/) for Linux based operating systems.
 - [DriverKit](https://developer.apple.com/documentation/driverkit) for Apple OS.

## Software Utilities

The USB Component comes with a set of software utilities that you can use as templates to create your own programs for the USB Host computer. The utilities are specific for the different USB Device Classes. These are the available utilities:

 - The \ref hid_client_app  can be used to interact with an USB HID Device. You can toggle LEDs on the development board and check the state of the on-board push-buttons.
 - The \ref winusb_app can be used to test the connection between the USB Host PC and the client device. This application uses WinUSB driver on the USB Host PC and demonstrates the basics of using the Microsoft Foundation Classes and can be used as a starting point for writing your application.

## WinUSB Test {#winusb_app}

If you are following the \ref dev_cc_tutorial tutorial, you can test the connection between your custom class USB device and a Microsoft Windows PC with this application. The binary `WinUSB_Test.exe` (which does not require installation) is available in the `install_dir\ARM\PACK\Keil\MDK-Middleware\x.y.z\Utilities\WinUSB_Test\Release` folder (where `install_dir` refers to the installation directory of Arm Keil MDK uVision).

To check the WinUSB utility with your board, do the following:
 -# Download the USB Device `WinUSB_Echo` application to your board.
 -# Verify all jumper settings on the board.
 -# Connect the board to a Windows PC. The PC should recognize the WinUSB device and install the correct driver automatically.
 -# Run `WinUSB_Test.exe`.
 -# Select the **Device** to establish the communication channel. 
 -# For further settings and tests refer to \ref dev_cc_tutorial

![WinUSB test app](WinUSB_Test_application.png)

### WinUSB application source code {#winusb_app_cpp}

The WinUSB source code can be found in `install_dir\ARM\PACK\Keil\MDK-Middleware\x.y.z\Utilities\WinUSB_Test`.
A Visual Studio 2010 (or later) based solution named WinUSB_Test.sln is available. The following is a summary of what you
will find in each of the files that make up your WinUSB_Test application.

![WinUSB app solutions view](WinUSB_test_sln.png)

#### Header Files

- `.\USB\WinUsbIF.h` includes the function declarations of the functions that are defined in `.\USB\WinUsbIF.cpp`.
- `.\WinUSB_Test\WinUSB_Test.h` is the main header file for the application. It includes other project specific headers
  (including resource.h) and declares the `CWinUSB_TestApp` application class.
- `.\WinUSB_Test\WinUSB_TestDlg.h` defines the behavior of the application's main dialog.

### Resource Files

- `.\WinUSB_Test\res\WinUSB_Test.ico` is an icon file, which is used as the application's icon. This icon is included
  by the main resource file `.\WinUSB_Test\WinUSB_Test.rc`.
- `.\WinUSB_Test\res\Refresh_grey.ico` is an icon file, which is used for the refresh button in the application.
- `.\WinUSB_Test\res\WinUSB_Test.rc2` contains resources that are not edited by Microsoft Visual C++. You should
  place all resources not editable by the resource editor in this file.

#### Source Files

- `.\USB\WinUsbIF.cpp` contains the necessary functions that are used in this example application to communicate with
  a USB device via the WinUSB interface. Refer to the
  <a href="https://learn.microsoft.com/en-us/windows/win32/api/winusb/" target=_blank>WinUSB Functions</a>
  documentation for an overview of the available functions.	The function:
  - `WinUsbIF_Initialize` initializes the WinUSB interface to be used with the USB Host.
  - `WinUsbIF_Uninitialize` de-initializes the WinUSB interface from the USB Host.
  - `WinUsbIF_SetGUID` sets the USB interface Globally Unique Identifier (GUID).
  - `WinUsbIF_GetGUID` retrieves the USB interface Globally Unique Identifier (GUID).
  - `WinUsbIF_FindDevices` scans the USB Bus and lists all available devices, containing interface with set GUID,
    for connection to the application.
  - `WinUsbIF_GetNumOfInterfaces` retrieves the number of interfaces available for a device.
  - `WinUsbIF_GetEndpointsMask` retrieves the endpoint mask of the selected device.
  - `WinUsbIF_GetName` evaluates the VID + PID + Serial Number or Unique ID of the device to be shown in the drop down box
    of the application.
  - `WinUsbIF_GetManufacturerString` retrieves the device's manufacturer string.
  - `WinUsbIF_GetProductString` retrieves the device's product string.
  - `WinUsbIF_GetSerialNumberString` retrieves the device's serial number string.
  - `WinUsbIF_GetInterfaceString` retrieves the device's interface string.
  - `WinUsbIF_GetVID` retrieves the device's vendor ID.
  - `WinUsbIF_GetPID` retrieves the device's product ID.
  - `WinUsbIF_OpenDevice` opens the device for USB communication.
  - `WinUsbIF_CloseDevice` closes the device from USB communication.
  - `WinUsbIF_ControlTransfer` executes a control transfer on the USB Bus.
  - `WinUsbIF_WritePipe` executes an USB OUT transfer on a selected pipe to a bulk or interrupt endpoint of the device.
  - `WinUsbIF_ReadPipe` executes an USB IN transfer on a selected pipe to a bulk or interrupt endpoint of the device.
  - `WinUsbIF_AbortPipe` aborts pending pipe transfers.
  - `WinUsbIF_FlushPipe` flushes the cached data of a pipe.
  - `WinUsbIF_ResetPipe` resets a pipe.
  - `WinUsbIF_GetOverlappedResult` retrieves the overlapped result.
- `.\WinUSB_Test\WinUSB_Test.cpp` is the main application source file that contains the application class
  `CWinUSB_TestApp`.
- `.\WinUSB_Test\WinUSB_Test.rc` is a listing of all of the Microsoft Windows resources that the program uses (located
  in the `.\WinUSB_Test\res` subdirectory).
- `.\WinUSB_Test\WinUSB_TestDlg.cpp` implements the code of the client's dialog and calls the functions specified in
  `.\USB\WinUsbIF.cpp`. This is the actual place where the interaction between the USB Host and the USB Device is
  defined.