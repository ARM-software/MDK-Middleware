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
    [ "Overview", "index.html#net_overview", [
      [ "Documentation Structure", "index.html#net_docs_structure", null ]
    ] ],
    [ "Revision History", "netw_revhist.html", null ],
    [ "Creating a Network Application", "network_create_app.html", [
      [ "RTE Component Selection", "network_create_app.html#nw_RTE_Software_Component_Selection", null ],
      [ "Network Driver and Controller Configuration", "network_create_app.html#nw_Driver_Configuration", null ],
      [ "Ethernet PHY Configuration", "network_create_app.html#nw_PHY_Configuration", null ],
      [ "Network Configuration", "network_create_app.html#nw_Device_Configuration", [
        [ "Network Core", "network_create_app.html#autotoc_md0", null ],
        [ "Network Interface", "network_create_app.html#autotoc_md1", null ],
        [ "Network Socket", "network_create_app.html#autotoc_md2", null ],
        [ "Network Service", "network_create_app.html#autotoc_md3", null ]
      ] ],
      [ "Security Configuration", "network_create_app.html#nw_security_config", null ],
      [ "System Resource Configuration", "network_create_app.html#nw_system_resources", null ],
      [ "User Code Implementation", "network_create_app.html#nw_User_Code_Implementation", [
        [ "User Code Templates", "network_create_app.html#autotoc_md4", null ]
      ] ],
      [ "Debugging", "network_create_app.html#nw_debugging", [
        [ "Debug Configuration", "network_create_app.html#autotoc_md5", null ],
        [ "Debug Levels", "network_create_app.html#autotoc_md6", null ],
        [ "Event Recorder Support", "network_create_app.html#netEvrSupport", null ],
        [ "Event filtering", "network_create_app.html#autotoc_md7", null ],
        [ "Event IDs", "network_create_app.html#autotoc_md8", null ]
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
    [ "Secure Communication", "secure_communication.html", "secure_communication" ],
    [ "Cyber Security", "certification.html", null ],
    [ "Network Examples", "nw_examples.html", "nw_examples" ],
    [ "Migration", "network_diffs.html", [
      [ "Project Migration", "network_diffs.html#nw_diffs_project_migration", [
        [ "Select a New Core Variant", "network_diffs.html#nw_diffs_select_variant", null ],
        [ "Update Configuration Files", "network_diffs.html#nw_diffs_update_configs", null ],
        [ "Configure Keil RTX", "network_diffs.html#nw_diffs_rtx_configuration", null ],
        [ "Build and Run the Network Project", "network_diffs.html#nw_diffs_build_and_run", null ]
      ] ],
      [ "Network Configuration", "network_diffs.html#mig_nw_configuration", [
        [ "Net_Config.c", "network_diffs.html#mig_nw_configuration_net_config_c", null ],
        [ "Net_Debug.c", "network_diffs.html#mig_nw_configuration_net_debug", null ]
      ] ],
      [ "STDIO Debug", "network_diffs.html#netDebugStdio", [
        [ "Module IDs", "network_diffs.html#autotoc_md110", null ]
      ] ]
    ] ],
    [ "Resource Requirements", "nw_resource_requirements.html", [
      [ "Stack Requirements", "nw_resource_requirements.html#nw_mem_req", null ],
      [ "Heap Requirements", "nw_resource_requirements.html#nw_heap_req", null ],
      [ "CMSIS-RTOS Requirements", "nw_resource_requirements.html#nw_rtos_req", [
        [ "RTX v5 Requirements", "nw_resource_requirements.html#nw_req_rtx5", null ]
      ] ],
      [ "Memory Requirements", "nw_resource_requirements.html#nw_spec_mem_req", null ]
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
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"BSD_Examples.html",
"group__ftpc__user__api.html#ga3ef24b53e54fc1cfb9e325d861679065",
"group__net__evr__bsd__func.html#ga9f93daa0f86e43c49267a996f2ba2795",
"group__net__evr__ftpc__func.html#ga8debdbca6509175132c32f26117596bd",
"group__net__evr__ip6__func.html#ga929a14576117ae1a2566fc0c1e760e3f",
"group__net__evr__slip__func.html#gac8f0d6ae51c50855bfe86de6608491e1",
"group__net__evr__teln__func.html#ga29b9d3c52351ac314251110479367e52",
"group__smtpc__operation.html#ga58a7a56c6bda480e21591ee33c028aa5",
"network_create_app.html#nw_User_Code_Implementation"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';