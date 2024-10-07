# FTP Server {#FTP_Server_Example}

This examples implements an FTP server on a device that allows you to manage files from a computer connected to the same network.

The following picture shows an exemplary connection of the development board and a Computer.

![FTP server hardware setup](ftp_setup.png)

## Project Organization

The FTP Server project is available as part of the \ref examples "Network Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `FTP_Server.c` contains the application main thread which initializes the Network Component.

You can add the following optional files, which are not included in the project by default:

 - `FTP_Server_Multiuser.c` is used for specifying multiple users and access rights for these users.
 - `FTP_Server_Access.c` enables the server to accept or block connection requests from certain remote clients.
 - `FTP_Server_Event.c` contains a routine that notifies the user application about certain events happening on the FTP server (such as file/directory manipulation events).

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components.

Following configuration files are provided with this example:

 - For the Network component, in `./RTE/Network/` folder:
   - `Net_Config_FTP_Server.h`: \ref using_ftp_server_conf "FTP Server configuration"
   - `Net_Config_TCP.h`: \ref using_network_sockets_tcp_conf "TCP Socket configuration"
   - `Net_Config_UDP.h`: \ref using_network_sockets_udp_conf "UDP Socket configuration"
   - `Net_Config_ETH_0.h`: \ref using_ethernet_interfaces_conf "Ethernet Interface configuration"
   - `Net_Config.h`: \ref nw_Network_Core "Network Core configuration"
   - `Net_Debug.h`: \ref netDebugConfig "Network Debug configuration"
 - For the File System component, in the `./RTE/File_System/` folder:
   - `FS_Config.h`, `FS_Config_MC_0.h`, `FS_Debug.h`: [File System](../FileSystem/index.html) configuration files
 - For the CMSIS components, in the `./RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.

<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following CMSIS-Driver interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_ETH`: CMSIS-Driver for Ethernet interface
 - `CMSIS_MCI`: CMSIS-Driver for Memory Card Interface
 - `CMSIS_VIO`: CMSIS-Driver for Virtual I/O interface
 - `STDOUT`: Standard Output redirection

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **Network** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **FTP_Server** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, Ethernet ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Use an Ethernet cable to connect your development board to the local area network. The PC is assumed to be already a member of this LAN.
 -# If your hardware supports it, insert a SD card into the SD card slot. This will be the data storage that you will have access to. Hardware that does not have a SD card interface will most probably use a RAM disk for FTP demonstration.

**Example execution**

You can use either a Command Line utility or a GUI FTP client program to connect to the FTP server from your host PC.

In the Command console of your PC type `ftp my_host` (or the respective hostname specified in the `Net_Config.h` file). If you have a DCHP server in your network, it will automatically connect to the FTP server.

You will be asked for a username and password combination. Use *admin* for the user name and no password (you can change the defaults in the `Net_Config_FTP_Server.h` file). After a successful log in, you should see something like this:

![FTP server command line interface](ftp_server.png)
