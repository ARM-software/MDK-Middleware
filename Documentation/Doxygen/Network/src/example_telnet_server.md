# Telnet Server {#Telnet_Server_Example}

Often, it is necessary to have a **Command Line Interface (CLI)** for interaction with the device. This example shows how to use the Telnet server for this purpose. The following picture shows an exemplary connection of the development board and a Computer.

![Telnet example hardware setup](telnet_setup.png)

## Project Organization

The Telnet Server project is available as part of the \ref nw_ref_example "Network Reference Example".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `Telnet_Server.c` contains the main C function that initializes the board hardware and the Network Component.
 - `Telnet_Server_UIF.c` defines the functions for the user interface and all the available CLI commands.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are located in the `/RTE/` directory and can be modified by users to adjust the operation of related components. Section TBD gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the Network component, in `/RTE/Network/` folder:
   - `Net_Config_Telnet_Server.h`: \ref telnet_server_conf "Telnet Server configuration"
   - `Net_Config_TCP.h`: \ref using_network_sockets_tcp_conf "TCP Socket configuration"
   - `Net_Config_UDP.h`: \ref using_network_sockets_udp_conf "UDP Socket configuration"
   - `Net_Config_ETH_0.h`: \ref using_ethernet_interfaces_conf "Ethernet Interface configuration"
   - `Net_Config.h`: \ref nw_Network_Core "Network Core configuration"
   - `Net_Debug.h`: \ref netDebugConfig "Network Debug configuration"
 - For the CMSIS components, in the `/RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.h`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `/Board/` directory.

<h2>Board Layer</h2>

In order to build the Telnet Server project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_ETH`: CMSIS-Driver for Ethernet interface
 - `CMSIS_VIO`: CMSIS-Driver for virtual I/O interface
 - `STDIN`: Standard Input redirection
 - `STDOUT`: Standard Output redirection

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **Network** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **Telnet_Server** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific setup such as jumpers, Ethernet ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the firmware image to the target development board.
 -# Use an Ethernet cable to connect your development board to the local area network. The PC is assumed to be already in this LAN.

**Example execution**

For opening a Telnet session to your development , you need a Telnet client on the host PC, for example PuTTY.

In the Telnet client use `my_host` (or the respective hostname that you have chosen in the `Net_Config.h` file) as the host name  and use the standard Telnet port 23.

You will be asked for a username and password combination. use *admin* for the user name and no password (you can change the defaults in the `Net_Config_Telnet_Server.h` file). After a successful log in, eneter `help` command and you should see the list of available commands like this:

![Telnet Command Line Interface](telnet_cli.png)
