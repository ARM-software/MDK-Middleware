/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Network Component", "index.html", [
    [ "Overview", "index.html", "index" ],
    [ "Revision History", "rev_hist.html", null ],
    [ "Creating a Network Application", "create_app.html", [
      [ "RTE Component Selection", "create_app.html#nw_RTE_Software_Component_Selection", null ],
      [ "Network Driver and Controller Configuration", "create_app.html#nw_Driver_Configuration", null ],
      [ "Ethernet PHY Configuration", "create_app.html#nw_PHY_Configuration", null ],
      [ "Network Configuration", "create_app.html#nw_Device_Configuration", [
        [ "Network Core", "create_app.html#nw_Network_Core", null ],
        [ "Network Interface", "create_app.html#nw_Network_Interface", null ],
        [ "Network Socket", "create_app.html#nw_Network_Socket", null ],
        [ "Network Service", "create_app.html#nw_Network_Service", null ]
      ] ],
      [ "Security Configuration", "create_app.html#nw_security_config", null ],
      [ "System Resource Configuration", "create_app.html#nw_system_resources", null ],
      [ "User Code Implementation", "create_app.html#nw_User_Code_Implementation", [
        [ "User Code Templates", "create_app.html#autotoc_md0", null ]
      ] ],
      [ "Debugging", "create_app.html#nw_debugging", [
        [ "Debug Configuration", "create_app.html#netDebugConfig", [
          [ "Debug Levels", "create_app.html#netDebugLevels", null ],
          [ "Event filtering", "create_app.html#autotoc_md1", null ]
        ] ],
        [ "Event Recorder", "create_app.html#netEvrSupport", [
          [ "Event IDs", "create_app.html#autotoc_md2", null ]
        ] ],
        [ "STDIO Debug", "create_app.html#netDebugStdio", [
          [ "Module IDs", "create_app.html#autotoc_md3", null ]
        ] ]
      ] ],
      [ "Runtime Configuration", "create_app.html#nw_runtime_config", null ]
    ] ],
    [ "Troubleshooting a Network Application", "troubleshoot.html", [
      [ "Network library hierarchy", "troubleshoot.html#trbl_nw_hierachry", null ],
      [ "Best practices", "troubleshoot.html#trbl_bestpractice", [
        [ "Check the Ethernet interface", "troubleshoot.html#trbl_verify_eth", null ],
        [ "Verify Socket Communication", "troubleshoot.html#trbl_verify_sock", null ],
        [ "Verify Secure Communication", "troubleshoot.html#trbl_verify_secure", null ]
      ] ]
    ] ],
    [ "Secure Communication", "secure_communication.html", [
      [ "Using Mbed TLS", "secure_communication.html#use_mbed_tls", [
        [ "Why Mbed TLS?", "secure_communication.html#autotoc_md4", null ],
        [ "Difference between SSL/TLS", "secure_communication.html#autotoc_md5", null ],
        [ "Parts of an SSL/TLS Library", "secure_communication.html#autotoc_md6", null ],
        [ "Add the mbed TLS library to a project", "secure_communication.html#add_mbedtls", null ]
      ] ],
      [ "Using Secure Services", "secure_communication.html#use_secure_components", [
        [ "HTTPS Server", "secure_communication.html#https_server", [
          [ "Creating your own certificates and keys", "secure_communication.html#cert_creation", null ]
        ] ],
        [ "SMTPS Client", "secure_communication.html#smtps_client", [
          [ "Adding server root CA certificate", "secure_communication.html#cert_adding", null ]
        ] ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ],
    [ "Cyber Security", "cyber_security.html", [
      [ "Achilles Test Platform", "cyber_security.html#autotoc_md7", [
        [ "Setting up the Achilles Test Software", "cyber_security.html#autotoc_md8", null ],
        [ "Storm Rate Limit determination", "cyber_security.html#autotoc_md9", null ]
      ] ]
    ] ],
    [ "Function Overview", "function_overview.html", [
      [ "HTTP Server", "function_overview.html#net_fo_http", null ],
      [ "FTP Server", "function_overview.html#net_fo_ftps", null ],
      [ "FTP Client", "function_overview.html#net_fo_ftpc", null ],
      [ "TFTP Server", "function_overview.html#net_fo_tftps", null ],
      [ "TFTP Client", "function_overview.html#net_fo_tftpc", null ],
      [ "Telnet Server", "function_overview.html#net_fo_telnet", null ],
      [ "SNMP Agent", "function_overview.html#net_fo_snmp", null ],
      [ "DNS Client", "function_overview.html#net_fo_dnsc", null ],
      [ "SNTP Client", "function_overview.html#net_fo_sntpc", null ],
      [ "SMTP Client", "function_overview.html#net_fo_smtpc", null ],
      [ "Ping Client", "function_overview.html#net_fo_pingc", null ],
      [ "Sockets", "function_overview.html#net_fo_socket", null ],
      [ "Interfaces", "function_overview.html#net_interfaceover", null ],
      [ "IP Address", "function_overview.html#net_fo_ip_addr", null ],
      [ "System Functions (User)", "function_overview.html#net_fo_sys_func", null ],
      [ "Modem Driver Functions", "function_overview.html#net_fo_modem", null ]
    ] ],
    [ "Resource Requirements", "resource_requirements.html", [
      [ "Stack Requirements", "resource_requirements.html#nw_mem_req", null ],
      [ "Heap Requirements", "resource_requirements.html#nw_heap_req", null ],
      [ "CMSIS-RTOS Requirements", "resource_requirements.html#nw_rtos_req", null ],
      [ "Memory Requirements", "resource_requirements.html#nw_spec_mem_req", [
        [ "Network (IPv4/IPv6)", "resource_requirements.html#autotoc_md42", null ],
        [ "Network (IPv4)", "resource_requirements.html#autotoc_md43", null ]
      ] ]
    ] ],
    [ "Migration Guide", "migration_guide.html", [
      [ "Component Selection", "migration_guide.html#nw_diffs_component", null ],
      [ "Network Configuration", "migration_guide.html#mig_nw_configuration", [
        [ "Net_Config.h", "migration_guide.html#mig_nw_configuration_net_config_c", null ],
        [ "Net_Debug.h", "migration_guide.html#mig_nw_configuration_net_debug", null ]
      ] ]
    ] ],
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"BSD_Client_Example.html",
"group__ftp__fs__interface.html#ga058e1483a1bf2d52e7fe296b52d88901",
"group__net__evr__bsd__func.html#ga733db0e72bd7a8bf4f1922e2e71103f6",
"group__net__evr__eth__func.html#gac51f8d76778823738f2c76b9484c8d20",
"group__net__evr__igmp__func.html#gaa3cd0b47b66a64de1137c7070f0c4ebb",
"group__net__evr__ppp__func.html#gad858ed9c7a88e3bec4ca048ac05c1aa0",
"group__net__evr__tcp__func.html#gab71d12870266f7dbebf936a9ccbe2b6c",
"group__network__structs.html#a8e0798404bf2cf5dabb84c5ba9a4f236",
"index.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';