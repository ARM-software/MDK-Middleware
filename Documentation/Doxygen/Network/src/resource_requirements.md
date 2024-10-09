# Resource Requirements {#resource_requirements}

This section describes the resource requirements of the **Network** component. 
The actual requirements depend on the features used in the application. Therefore, the numbers provided here are guidelines
only and the actual numbers may differ.

\section nw_mem_req Stack Requirements

The Network Core receives events sent from the interrupt service routine (ISR) of the Ethernet (or Serial) **Driver**.
The stack requirements for the ISR are typically less than 512 Bytes. The total stack space required for ISR depends
on the interrupt nesting and therefore on the priority settings of these ISR.
The stack requirements for ISR is configured in the `startup_device.s` file located under the **Device** component class.
If the stack settings are not available there, the stack settings must be configured in the **linker script** file.

| Option (under section Stack Configuration)  | Increase Value by
| :-------------------------------------------| :----------------------
| Stack Size (in Bytes)                       | + 512 for Ethernet (or Serial) Driver

\note
When using a CMSIS-RTOS, the Stack Size in the startup_*device*.s file configures only the 
stack space that is used by exception and interrupt service routines of the drivers. The stack size requirements
depend on the maximum nesting of exception and ISR execution and therefore on the priority settings 
of the various interrupt and exception sources.

User code that calls API functions of the Network Component should have a minimum of 512 Bytes of stack space available.
Since API functions are frequently called from threads, the thread stack size should be at least 512 Bytes (see below).

## Heap Requirements {#nw_heap_req}

The mbed TLS component requires 90 kB of heap memory. The heap requirements are configured in the `startup_device.s` file,
which is located under the **Device** component class. If the heap settings are not available in the startup file, the heap
settings must be configured in the **linker script** file.

| Option (under section Stack Configuration)  | Increase Value to at least
| :-------------------------------------------| :----------------------
| Heap Size (in Bytes)                        | 0x16000

## CMSIS-RTOS Requirements {#nw_rtos_req}

Currently, the Network component works in two different RTOS configurations:

-# **CMSIS-RTOS2 RTX5**: This mode uses Keil RTX5 natively. In the Manage Run-Time Environment window, the component
   **CMSIS:RTOS2 (API):Keil RTX5** must be selected. The RTOS resources are static, configured by the system. There is
   **no need for additional user configuration**. This is the suggested option for new development.
-# **CMSIS-RTOS2 with a generic RTOS**: this mode uses a generic RTOS (such as FreeRTOS). The RTOS
   resources (thread, mutex and semaphore control blocks and thread stacks) are dynamically allocated. The user needs to
   provide memory pools big enough for the allocations in RTOS configuration.

The Network component uses threads. The Network Core thread is always started. Each Network interface starts
its own thread when the interface is enabled. Thread execution starts with \ref netInitialize.

The following table lists the components, the default thread name and priority, the default stack size
and the configuration file with **OS Resource Settings** for stack size adjustments.

| Component                   | Thread Name     | Default Priority        | Default Stack Size | Configuration File
| :---------------------------| :---------------| :---------------------: | :----------------- | :-------------------
| **Network:Core**            | netCore_Thread  | `osPriorityNormal`      | 2048 Bytes         | `Net_Config.h`
| **Network:Interface:ETH**   | netETH0_Thread  | `osPriorityAboveNormal` | 512 Bytes          | `Net_Config_ETH_0.h`
| **Network:Interface:WiFi**  | netWiFi0_Thread | `osPriorityAboveNormal` | 512 Bytes          | `Net_Config_WiFi_0.h`
| **Network:Interface:PPP**   | netPPP_Thread   | `osPriorityAboveNormal` | 512 Bytes          | `Net_Config_PPP.h`
| **Network:Interface:SLIP**  | netSLIP_Thread  | `osPriorityAboveNormal` | 512 Bytes          | `Net_Config_SLIP.h`
| **Security:mbed TLS**       | netTLS_Thread   | `osPriorityNormal`      | 4096 Bytes         | `TLS_mbed.c`

For the Network Component, the number of threads and stack size requirements are calculated as follows:

- **Number of Threads**: sum up the number of used *Components*.
- **Total stack size**: sum up the *Default Stack Sizes* of the related threads.

#### Timers

The Network component requires timers which need to be enabled in RTOS.

#### Signals

When using BSD sockets in blocking mode, the signal (1<<15) in that thread is reserved for the Network library. A user
thread, when blocked on BSD sockets, is waiting for the signal 0x8000 to resume. The event, which has caused the thread to
resume, is provided internally in the library. Therefore, signal 0x8000 must not be used by the user application in that
thread.

## Memory Requirements {#nw_spec_mem_req}

The following table shows the memory requirements for read-only (Flash) and read-write (RAM) memory.
These numbers may be used to estimate the total size requirements of the application, but the actual numbers depend
on compiler optimizations and target processor and may be therefore slightly different.

### Network (IPv4/IPv6)

| Component                                                   | RO Memory [Bytes] | RW Memory (Data) [Bytes] 
| :-----------------------------------------------------------| :---------------: | :------------------------:
| **Network:Core** (with IPv4/IPv6)                           |      7.5 k        | 0.4 k + *Memory Pool size* (configured in Net_Config.h)
| **Network:Interface:ETH** (with ARP, NDP, IGMP, DHCP, NBNS) |     15.7 k        | 0.9 k
| **Network:Interface:WiFi** (with ARP, NDP, IGMP, DHCP, NBNS)|     15.6 k        | 0.9 k
| **Network:Interface:PPP** (with CHAP/PAP and Modem)         |      8.3 k        | 0.6 k
| **Network:Interface:SLIP** (with Modem)                     |      2.8 k        | 0.5 k
| **Network:Socket:UDP**                                      |      1.5 k        | 0.1 k
| **Network:Socket:TCP**                                      |      6.1 k        | 0.6 k
| **Network:Socket:BSD** (without UDP/TCP)                    |      6.5 k        | 0.3 k
| **Network:Service:DNS Client**                              |      1.8 k        | 0.3 k
| **Network:Service:FTP Client**                              |      2.5 k        | < 0.1 k
| **Network:Service:FTP Server**                              |      5.4 k        | 0.2 k
| **Network:Service:SMTP Client**                             |      5.6 k        | 0.2 k
| **Network:Service:SNMP Agent**                              |      2.7 k        | < 0.1 k
| **Network:Service:SNTP Client**                             |      0.8 k        | < 0.1 k
| **Network:Service:TFTP Client**                             |      1.4 k        | < 0.1 k
| **Network:Service:TFTP Server**                             |      2.3 k        | 0.1 k
| **Network:Service:Telnet Server**                           |      3.2 k        | 0.4 k
| **Network:Service:Web Server** (HTTP Server)                |      7.6 k        | 0.5 k
| **Ethernet MAC Driver**                                     |     1 .. 4 k      | 0 .. 10 k
| **Ethernet PHY Driver**                                     |      < 1 k        | < 0.1 k

### Network (IPv4)

| Component                                               | RO Memory [Bytes] | RW Memory (Data) [Bytes] 
| :-------------------------------------------------------| :---------------: | :------------------------:
| **Network:Core** (with IPv4)                            |      4.6 k        | 0.4 k + *Memory Pool size* (configured in Net_Config.h)
| **Network:Interface:ETH** (with ARP,IGMP, DHCP, NBNS)   |      9.3 k        | 0.3 k
| **Network:Interface:WiFi** (with ARP, IGMP, DHCP, NBNS) |      9.3 k        | 0.3 k
| **Network:Interface:PPP** (with CHAP/PAP and Modem)     |      8.3 k        | 0.6 k
| **Network:Interface:SLIP** (with Modem)                 |      2.8 k        | 0.5 k
| **Network:Socket:UDP**                                  |      1.4 k        | < 0.1 k
| **Network:Socket:TCP**                                  |      5.7 k        | 0.5 k
| **Network:Socket:BSD** (without UDP/TCP)                |      5.9 k        | 0.2 k
| **Network:Service:DNS Client**                          |      1.6 k        | 0.2 k
| **Network:Service:FTP Client**                          |      2.5 k        | < 0.1 k
| **Network:Service:FTP Server**                          |      5.4 k        | 0.2 k
| **Network:Service:SMTP Client**                         |      5.6 k        | 0.2 k
| **Network:Service:SNMP Agent**                          |      2.7 k        | < 0.1 k
| **Network:Service:SNTP Client**                         |      0.8 k        | < 0.1 k
| **Network:Service:TFTP Client**                         |      1.4 k        | < 0.1 k
| **Network:Service:TFTP Server**                         |      2.3 k        | 0.1 k
| **Network:Service:Telnet Server**                       |      3.2 k        | 0.4 k
| **Network:Service:Web Server** (HTTP Server)            |      7.6 k        | 0.5 k
| **Ethernet MAC Driver**                                 |     1 .. 4 k      | 0 .. 10 k
| **Ethernet PHY Driver**                                 |      < 1 k        | < 0.1 k
