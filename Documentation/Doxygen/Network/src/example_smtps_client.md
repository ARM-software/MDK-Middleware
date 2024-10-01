# SMTPS Client {#SMTPS_Client_Example}

SMTPS is used for sending e-mail notifications from an embedded system to various recipients. This example shows how to setup a SMTPS client for this purpose. The following picture shows an exemplary connection of the development board and a Computer.

![SMTPS client hardware setup](smtp_setup.png)

## Project Organization

The SMTPS Client project is available as part of the \ref nw_ref_example "Network Reference Example".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `SMTPS_Client.c` contains the main C function that initializes the board hardware, the Network Component and provides the IP address of SMTPS server.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are located in the `/RTE/` directory and can be modified by users to adjust the operation of related components. Section TBD gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the Network component, in `/RTE/Network/` folder:
   - `Net_Config_SMTP_Client.h`: \ref using_smtpc_conf "SMTP Client configuration"
   - `Net_Config_DNS_Client.h`: \ref using_dns_client_conf "DNS Client configuration"
   - `Net_Config_TCP.h`: \ref using_network_sockets_tcp_conf "TCP Socket configuration"
   - `Net_Config_UDP.h`: \ref using_network_sockets_udp_conf "UDP Socket configuration"
   - `Net_Config_ETH_0.h`: \ref using_ethernet_interfaces_conf "Ethernet Interface configuration"
   - `Net_Config.h`: \ref nw_Network_Core "Network Core configuration"
   - `Net_Security.c`: file with keys and certificates. Default values are used by default, for changing them refer to \ref cert_adding
   - `Net_Debug.h`: \ref netDebugConfig "Network Debug configuration"
 - For mbedTLS component, in `/RTE/Security/` folder:
   - `mbedTLS_config.h`, `mbedTLS_crypto_config.h`: [mbedTLS configuration files](https://github.com/ARM-software/CMSIS-mbedTLS)
 - For the CMSIS components, in the `/RTE/CMSIS/` folder:
   - `RTX_Config.c` and `RTX_Config.h`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `/Board/` directory.

<h2>Board Layer</h2>

In order to build the SMTPS Client project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_ETH`: CMSIS-Driver for Ethernet interface
 - `CMSIS_VIO`: CMSIS-Driver for virtual I/O interface
 - `STDOUT`: Standard Output redirection

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **Network** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **SMTPS_Client** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific setup such as jumpers, Ethernet ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the firmware image to the target development board.
 -# Use an Ethernet cable to connect your development board to the local area network. The PC is assumed to be already in this LAN.

**Example execution**

If you have entered your personal e-mail address as the recipient's address, you only need to open your mail program and check your inbox.
