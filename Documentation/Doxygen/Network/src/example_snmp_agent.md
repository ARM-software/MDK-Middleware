# SNMP Agent {#SNMP_Agent_Example}

This example shows how to use SNMP to control an embedded system.

The Simple Network Management Protocol (SNMP) is mainly used in network management systems to monitor network-attached devices for conditions that warrant administrative attention. It is the most popular network management protocol in the TCP/IP protocol suite.

The following picture shows an exemplary connection of the development board and a Computer.

![SNMP agent hardware setup](snmp_example_setup.png)

## Project Organization

The SNMP Agent project is available as part of the \ref examples "Network Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `SNMP_Agent.c` contains the application main thread which initializes the Network Component.
 - `SNMP_Agent_MIB.c` contains the \ref mib_data_table. Change this to the application's needs.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified to adjust the operation of related components.

Following configuration files are provided with this example:

 - For the Network component, in `./RTE/Network/` folder:
   - `Net_Config_SNMP_Agent.h`: \ref using_snmp_agent_conf "SNMP Agent configuration".
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
 -# Select **SNMP_Agent** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Hardware Setup**

Board-specific hardware setup such as jumpers, Ethernet ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Use an Ethernet cable to connect your development board to the local area network. The PC is assumed to be already in this LAN.

**Example execution**

To test this example, run the Windows application *SNMPTest.exe* that is available as part of [Keil MDK uVision](https://developer.arm.com/documentation/101407/latest/About-uVision/Installation) and is located in `<install_dir>\ARM\Utilities\SNMPTest\Release\` folder, where `<install_dir>` refers to the Keil MDK uVision installation directory. The program runs stand-alone without installation.

Type in the IP address of your Embedded SNMP Agent and click *Connect*. When connected, you can change the LCD text on the target, control the on-board LED diodes and monitor the state of on-board push-buttons.

![SNMP Test Application](SNMPTest_exe.png)
