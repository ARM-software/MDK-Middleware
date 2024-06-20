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
    [ "Revision History", "fs_revhist.html", null ],
    [ "Create a File System Application", "fs_create_app.html", [
      [ "RTE Component Selection", "fs_create_app.html#fs_RTE_Software_Component_Selection", null ],
      [ "Using Drives", "fs_create_app.html#fs_drives", [
        [ "NOR Flash Drive", "fs_create_app.html#nor_usage", null ],
        [ "RAM Disk Drive", "fs_create_app.html#ram_usage", null ],
        [ "Memory Card Drive", "fs_create_app.html#mc_usage", null ],
        [ "USB Flash Drive", "fs_create_app.html#usb_usage", null ],
        [ "NAND Flash Drive", "fs_create_app.html#nand_usage", null ]
      ] ],
      [ "File System Configuration", "fs_create_app.html#fs_Device_Configuration", null ],
      [ "Hardware Configuration", "fs_create_app.html#fs_Driver_Configuration", null ],
      [ "System Resource Configuration", "fs_create_app.html#fs_sys_req", null ],
      [ "User Code Implementation", "fs_create_app.html#fs_User_Code_Implementation", null ],
      [ "Debugging", "fs_create_app.html#fs_debugging", [
        [ "Event Recorder Support", "fs_create_app.html#fsEvrSupport", [
          [ "Event Recorder Configuration", "fs_create_app.html#fsEvrConfig", null ],
          [ "Event IDs", "fs_create_app.html#autotoc_md0", null ]
        ] ]
      ] ]
    ] ],
    [ "File System Examples", "fs_examples.html", [
      [ "File System Example", "fs_examples.html#fs_standalone_example", [
        [ "Build the \"File System\" Project", "fs_examples.html#fs_example_build", [
          [ "Source Files", "fs_examples.html#fs_example_sources", null ]
        ] ],
        [ "Using the \"File System\" Project", "fs_examples.html#fs_example_using", [
          [ "Hardware Setup", "fs_examples.html#fs_example_using_hw", null ],
          [ "PC Software", "fs_examples.html#fs_example_using_pc", null ]
        ] ]
      ] ]
    ] ],
    [ "Theory of Operation", "fs_operation.html", [
      [ "Using the File System", "fs_operation.html#fs_usage", null ],
      [ "Drives, Memory Devices and Drivers", "fs_operation.html#drive", [
        [ "Current Drive", "fs_operation.html#cur_sys_drive", null ]
      ] ],
      [ "FAT File System", "fs_operation.html#fat_fs", [
        [ "Layout", "fs_operation.html#fat_layout", null ],
        [ "Volume name", "fs_operation.html#fat_volume_label", null ],
        [ "Path name handling", "fs_operation.html#fat_path_handling", [
          [ "Absolute and relative path", "fs_operation.html#fat_abs_rel_path", null ]
        ] ],
        [ "Long and Short File Name handling", "fs_operation.html#lfn_sfn_handling", null ],
        [ "Time Stamping Support", "fs_operation.html#fat_time_stamping", null ],
        [ "Journaling for the FAT File System", "fs_operation.html#journaling_fat", [
          [ "System Initialization and Drive Formatting", "fs_operation.html#sys_init_drv_format", null ],
          [ "System failure behavior", "fs_operation.html#sys_fail_beh", null ]
        ] ],
        [ "FAT System Design Limitations", "fs_operation.html#fat_sys_design_limitations", null ],
        [ "FAT Implementation Limitations", "fs_operation.html#fat_imp_limitations", null ]
      ] ],
      [ "Memory Card Control Layer", "fs_operation.html#mc_control_layer", [
        [ "List of Supported Devices", "fs_operation.html#mci_list_of_devices", null ],
        [ "List of Supported MCI Driver Capabilities", "fs_operation.html#mci_list_of_mci_capabilities", null ],
        [ "Device Specific Features", "fs_operation.html#mci_device_specifics", [
          [ "Power Supply Voltage Control", "fs_operation.html#autotoc_md1", null ],
          [ "Data Bus Width", "fs_operation.html#autotoc_md2", null ],
          [ "High Speed Mode", "fs_operation.html#autotoc_md3", null ],
          [ "Password protection", "fs_operation.html#autotoc_md4", null ],
          [ "Health Status (S.M.A.R.T)", "fs_operation.html#autotoc_md5", null ]
        ] ],
        [ "Power Saving Features", "fs_operation.html#mci_power_save", null ],
        [ "SPI Mode Support", "fs_operation.html#mc_spi_mode_support", [
          [ "SPI bus frequency setting", "fs_operation.html#mc_spi_bus_freq_setting", null ]
        ] ]
      ] ],
      [ "NAND Flash Translation Layer", "fs_operation.html#nand_flash_TL", [
        [ "Wear Leveling", "fs_operation.html#wear_leveling", null ],
        [ "Bad Block Management", "fs_operation.html#bad_block_management", null ],
        [ "Error Correction Codes (ECC)", "fs_operation.html#slc_ecc", null ]
      ] ],
      [ "Embedded File System (EFS)", "fs_operation.html#emb_fs", [
        [ "Memory Organization", "fs_operation.html#efs_mem_org", null ],
        [ "Allocation Information", "fs_operation.html#efs_alloc_info", [
          [ "Block Signature", "fs_operation.html#autotoc_md6", null ],
          [ "Allocation Information Record", "fs_operation.html#autotoc_md7", null ]
        ] ],
        [ "File Names & Content", "fs_operation.html#efs_file_content", [
          [ "File Names", "fs_operation.html#autotoc_md8", null ],
          [ "File Content", "fs_operation.html#autotoc_md9", null ]
        ] ],
        [ "Limitations", "fs_operation.html#efs_limits", null ]
      ] ]
    ] ],
    [ "Function Overview", "fs_function_reference.html", null ],
    [ "Resource Requirements", "fs_resource_requirements.html", [
      [ "Stack Requirements", "fs_resource_requirements.html#fs_stack_req", null ],
      [ "Heap Requirements", "fs_resource_requirements.html#fs_heap_req", null ],
      [ "CMSIS-RTOS Requirements", "fs_resource_requirements.html#fs_rtos_req", null ],
      [ "Memory Requirements", "fs_resource_requirements.html#fs_spec_mem_req", null ]
    ] ],
    [ "Migration Guide", "fs_migration.html", [
      [ "Component Selection", "fs_migration.html#fs_migrate_components", null ],
      [ "Configuration", "fs_migration.html#fs_migrate_config", null ]
    ] ],
    [ "Reference_Files", "md_src_Reference_Files.html", null ],
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"fs_create_app.html",
"group__evr__fsfat.html#gaea6823784e6d24879888ca9657bd77fb",
"group__stdio__routines.html#gab5de865c3093158c70bf4bd88a9deea5"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';