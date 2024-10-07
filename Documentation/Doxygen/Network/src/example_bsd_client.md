# BSD Client {#BSD_Client_Example}

The BSD client example implements BSD compliant communication. It shows you how to control the LEDs on the evaluation board of the server via the BSD socket.

BSD sockets are often used for network communication as they provide a well-defined API for exchanging data over the network by utilizing TCP and UDP sockets.

An exemplary setup could look like this:

![BSD example hardware setup](bsd_example_setup.png)

## Project Organization

The BSD Client project is available as part of the \ref examples "Network Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `BSD_Client.c` contains the application main thread which initializes the Network Component for the BSD client and provides the IP address of the BSD server.
   The client establishes a connection to a BSD server with the specified IP address.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are located in the `./RTE/` directory and can be modified by users to adjust the operation of related components.

Following configuration files are provided with this example:

 - For the Network component, in `./RTE/Network/` folder:
   - `Net_Config_BSD.h`: \ref using_network_sockets_bsd_conf "BSD Socket configuration".
   - `Net_Config_TCP.h`: \ref using_network_sockets_tcp_conf "TCP Socket configuration".
   - `Net_Config_UDP.h`: \ref using_network_sockets_udp_conf "UDP Socket configuration".
   - `Net_Config_ETH_0.h`: \ref using_ethernet_interfaces_conf "Ethernet Interface configuration".
   - `Net_Config.h`: \ref nw_Network_Core "Network Core configuration".
   - `Net_Debug.h`: \ref netDebugConfig "Network Debug configuration".
 - For the CMSIS components, in the `./RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.


<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_ETH`: CMSIS-Driver for Ethernet interface
 - `CMSIS_VIO`: CMSIS-Driver for Virtual I/O interface
 - `STDOUT`: Standard Output redirection

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **Network** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **BSD_Client** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, Ethernet ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to your development board.
 -# Use an Ethernet cable to connect your development board to the local area network. The PC is assumed to be already a member of this LAN.

**Example execution**

To test this example, you also need a \ref BSD_Server_Example device that you can connect to.

Load a BSD server and start it on another evaluation board connected to the same LAN. When BSD client is running, the LED diodes on both evaluation boards should light up synchronously.
