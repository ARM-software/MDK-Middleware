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
  [ "File System Component", "index.html", [
    [ "Overview", "index.html", "index" ],
    [ "Revision History", "rev_hist.html", null ],
    [ "Create a File System Application", "create_app.html", [
      [ "RTE Component Selection", "create_app.html#RTE_Component_Selection", null ],
      [ "Using Drives", "create_app.html#fs_drives", [
        [ "NOR Flash Drive", "create_app.html#nor_usage", null ],
        [ "RAM Disk Drive", "create_app.html#ram_usage", null ],
        [ "Memory Card Drive", "create_app.html#mc_usage", null ],
        [ "USB Flash Drive", "create_app.html#usb_usage", null ],
        [ "NAND Flash Drive", "create_app.html#nand_usage", null ]
      ] ],
      [ "File System Configuration", "create_app.html#fs_configuration", null ],
      [ "Hardware Configuration", "create_app.html#hw_configuration", null ],
      [ "System Resource Configuration", "create_app.html#system_configuration", null ],
      [ "User Code Implementation", "create_app.html#User_Code_Implementation", null ],
      [ "Debugging", "create_app.html#fs_debugging", [
        [ "Event Recorder", "create_app.html#fsEvr", [
          [ "Configuration", "create_app.html#fsEvrConfig", null ],
          [ "Event IDs", "create_app.html#autotoc_md0", null ]
        ] ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ],
    [ "Theory of Operation", "operation.html", [
      [ "Using the File System", "operation.html#fs_usage", null ],
      [ "Drives, Memory Devices and Drivers", "operation.html#drive", [
        [ "Current Drive", "operation.html#cur_sys_drive", null ]
      ] ],
      [ "FAT File System", "operation.html#fat_fs", [
        [ "Layout", "operation.html#fat_layout", null ],
        [ "Volume name", "operation.html#fat_volume_label", null ],
        [ "Path name handling", "operation.html#fat_path_handling", [
          [ "Absolute and relative path", "operation.html#fat_abs_rel_path", null ]
        ] ],
        [ "Long and Short File Name handling", "operation.html#lfn_sfn_handling", null ],
        [ "Time Stamping Support", "operation.html#fat_time_stamping", null ],
        [ "Journaling for the FAT File System", "operation.html#journaling_fat", [
          [ "System Initialization and Drive Formatting", "operation.html#sys_init_drv_format", null ],
          [ "System failure behavior", "operation.html#sys_fail_beh", null ]
        ] ],
        [ "FAT System Design Limitations", "operation.html#fat_sys_design_limitations", null ],
        [ "FAT Implementation Limitations", "operation.html#fat_imp_limitations", null ]
      ] ],
      [ "Memory Card Control Layer", "operation.html#mc_control_layer", [
        [ "List of Supported Devices", "operation.html#mci_list_of_devices", null ],
        [ "List of Supported MCI Driver Capabilities", "operation.html#mci_list_of_mci_capabilities", null ],
        [ "Device Specific Features", "operation.html#mci_device_specifics", [
          [ "Power Supply Voltage Control", "operation.html#autotoc_md4", null ],
          [ "Data Bus Width", "operation.html#autotoc_md5", null ],
          [ "High Speed Mode", "operation.html#autotoc_md6", null ],
          [ "Password protection", "operation.html#autotoc_md7", null ],
          [ "Health Status (S.M.A.R.T)", "operation.html#autotoc_md8", null ]
        ] ],
        [ "Power Saving Features", "operation.html#mci_power_save", null ],
        [ "SPI Mode Support", "operation.html#mc_spi_mode_support", [
          [ "SPI bus frequency setting", "operation.html#mc_spi_bus_freq_setting", null ]
        ] ]
      ] ],
      [ "NAND Flash Translation Layer", "operation.html#nand_flash_TL", [
        [ "Wear Leveling", "operation.html#wear_leveling", null ],
        [ "Bad Block Management", "operation.html#bad_block_management", null ],
        [ "Error Correction Codes (ECC)", "operation.html#slc_ecc", null ]
      ] ],
      [ "Embedded File System (EFS)", "operation.html#emb_fs", [
        [ "Memory Organization", "operation.html#efs_mem_org", null ],
        [ "Allocation Information", "operation.html#efs_alloc_info", [
          [ "Block Signature", "operation.html#autotoc_md9", null ],
          [ "Allocation Information Record", "operation.html#autotoc_md10", null ]
        ] ],
        [ "File Names & Content", "operation.html#efs_file_content", [
          [ "File Names", "operation.html#autotoc_md11", null ],
          [ "File Content", "operation.html#autotoc_md12", null ]
        ] ],
        [ "Limitations", "operation.html#efs_limits", null ]
      ] ]
    ] ],
    [ "Function Overview", "function_overview.html", null ],
    [ "Resource Requirements", "resource_requirements.html", [
      [ "Stack Requirements", "resource_requirements.html#fs_stack_req", null ],
      [ "Heap Requirements", "resource_requirements.html#fs_heap_req", null ],
      [ "CMSIS-RTOS Requirements", "resource_requirements.html#fs_rtos_req", [
        [ "Thread Stack Requirements", "resource_requirements.html#autotoc_md13", null ],
        [ "Mutex Requirements", "resource_requirements.html#autotoc_md14", null ]
      ] ],
      [ "Memory Requirements", "resource_requirements.html#fs_spec_mem_req", null ]
    ] ],
    [ "Migration Guide", "migration_guide.html", [
      [ "Component Selection", "migration_guide.html#migrate_components", null ],
      [ "Configuration", "migration_guide.html#migrate_config", null ]
    ] ],
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"File_Demo_Example.html",
"group__evr__fsmcmci.html#gad325a4955199a83e5bc9036f8b80d425",
"operation.html#fat_layout"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';