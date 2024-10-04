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
    [ "Revision History", "netw_revhist.html", null ],
    [ "Creating a Network Application", "network_create_app.html", [
      [ "RTE Component Selection", "network_create_app.html#nw_RTE_Software_Component_Selection", null ],
      [ "Network Driver and Controller Configuration", "network_create_app.html#nw_Driver_Configuration", null ],
      [ "Ethernet PHY Configuration", "network_create_app.html#nw_PHY_Configuration", null ],
      [ "Network Configuration", "network_create_app.html#nw_Device_Configuration", [
        [ "Network Core", "network_create_app.html#nw_Network_Core", null ],
        [ "Network Interface", "network_create_app.html#nw_Network_Interface", null ],
        [ "Network Socket", "network_create_app.html#nw_Network_Socket", null ],
        [ "Network Service", "network_create_app.html#nw_Network_Service", null ]
      ] ],
      [ "Security Configuration", "network_create_app.html#nw_security_config", null ],
      [ "System Resource Configuration", "network_create_app.html#nw_system_resources", null ],
      [ "User Code Implementation", "network_create_app.html#nw_User_Code_Implementation", [
        [ "User Code Templates", "network_create_app.html#autotoc_md0", null ]
      ] ],
      [ "Debugging", "network_create_app.html#nw_debugging", [
        [ "Debug Configuration", "network_create_app.html#netDebugConfig", [
          [ "Debug Levels", "network_create_app.html#netDebugLevels", null ],
          [ "Event filtering", "network_create_app.html#autotoc_md1", null ]
        ] ],
        [ "Event Recorder", "network_create_app.html#netEvrSupport", [
          [ "Event IDs", "network_create_app.html#autotoc_md2", null ]
        ] ],
        [ "STDIO Debug", "network_create_app.html#netDebugStdio", [
          [ "Module IDs", "network_create_app.html#autotoc_md3", null ]
        ] ]
      ] ],
      [ "Runtime Configuration", "network_create_app.html#nw_runtime_config", null ]
    ] ],
    [ "Troubleshooting a Network Application", "network_troubleshoot.html", [
      [ "Network library hierarchy", "network_troubleshoot.html#trbl_nw_hierachry", null ],
      [ "Best practices", "network_troubleshoot.html#trbl_bestpractice", [
        [ "Check the Ethernet interface", "network_troubleshoot.html#trbl_verify_eth", null ],
        [ "Verify Socket Communication", "network_troubleshoot.html#trbl_verify_sock", null ],
        [ "Verify Secure Communication", "network_troubleshoot.html#trbl_verify_secure", null ]
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
          [ "Converting the HTTP server to HTTPS", "secure_communication.html#add_ssl_to_http", null ],
          [ "Creating your own certificates and keys", "secure_communication.html#cert_creation", null ]
        ] ],
        [ "SMTPS Client", "secure_communication.html#smtps_client", [
          [ "Converting the SMTP client to SMTPS", "secure_communication.html#add_ssl_to_smtp", null ],
          [ "Adding server root CA certificate", "secure_communication.html#cert_adding", null ]
        ] ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ],
    [ "Cyber Security", "certification.html", [
      [ "Achilles Test Platform", "certification.html#autotoc_md7", [
        [ "Setting up the Achilles Test Software", "certification.html#autotoc_md8", null ],
        [ "Storm Rate Limit determination", "certification.html#autotoc_md9", null ]
      ] ]
    ] ],
    [ "Resource Requirements", "nw_resource_requirements.html", [
      [ "Stack Requirements", "nw_resource_requirements.html#nw_mem_req", null ],
      [ "Heap Requirements", "nw_resource_requirements.html#nw_heap_req", null ],
      [ "CMSIS-RTOS Requirements", "nw_resource_requirements.html#nw_rtos_req", null ],
      [ "Memory Requirements", "nw_resource_requirements.html#nw_spec_mem_req", [
        [ "Network (IPv4/IPv6)", "nw_resource_requirements.html#autotoc_md42", null ],
        [ "Network (IPv4)", "nw_resource_requirements.html#autotoc_md43", null ]
      ] ]
    ] ],
    [ "Function Overview", "net_func_overview.html", [
      [ "HTTP Server", "net_func_overview.html#net_fo_http", null ],
      [ "FTP Server", "net_func_overview.html#net_fo_ftps", null ],
      [ "FTP Client", "net_func_overview.html#net_fo_ftpc", null ],
      [ "TFTP Server", "net_func_overview.html#net_fo_tftps", null ],
      [ "TFTP Client", "net_func_overview.html#net_fo_tftpc", null ],
      [ "Telnet Server", "net_func_overview.html#net_fo_telnet", null ],
      [ "SNMP Agent", "net_func_overview.html#net_fo_snmp", null ],
      [ "DNS Client", "net_func_overview.html#net_fo_dnsc", null ],
      [ "SNTP Client", "net_func_overview.html#net_fo_sntpc", null ],
      [ "SMTP Client", "net_func_overview.html#net_fo_smtpc", null ],
      [ "Ping Client", "net_func_overview.html#net_fo_pingc", null ],
      [ "Sockets", "net_func_overview.html#net_fo_socket", null ],
      [ "Interfaces", "net_func_overview.html#net_interfaceover", null ],
      [ "IP Address", "net_func_overview.html#net_fo_ip_addr", null ],
      [ "System Functions (User)", "net_func_overview.html#net_fo_sys_func", null ],
      [ "Modem Driver Functions", "net_func_overview.html#net_fo_modem", null ]
    ] ],
    [ "Migration Guide", "network_diffs.html", [
      [ "Component Selection", "network_diffs.html#nw_diffs_component", null ],
      [ "Network Configuration", "network_diffs.html#mig_nw_configuration", [
        [ "Net_Config.h", "network_diffs.html#mig_nw_configuration_net_config_c", null ],
        [ "Net_Debug.h", "network_diffs.html#mig_nw_configuration_net_debug", null ]
      ] ]
    ] ],
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"BSD_Client_Example.html",
"group__netDNS__Enums.html",
"group__net__evr__bsd__func.html#gaa2f92301063dd42fdd975d510997ea3e",
"group__net__evr__ftpc__func.html#ga945304b93631d47f9e32e40926dee633",
"group__net__evr__ip6__func.html#gac25e2fa048f98b63c8802bdf167f4f23",
"group__net__evr__slip__func.html#gadfe73168ca8814651756c54b148ef83c",
"group__net__evr__teln__func.html#ga392d561ee8dc6ac01ea2f085b3e4284a",
"group__smtpc__user__api.html#ga33454db9427528430742f6863c22f871",
"network_create_app.html#nw_debugging"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';