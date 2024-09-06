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
  [ "USB Component", "index.html", [
    [ "Overview", "index.html", "index" ],
    [ "Revision History", "usb_revhist.html", null ],
    [ "USB Device", "USB_Device.html", [
      [ "RTE Components", "USB_Device.html#usbd_rte_components", null ],
      [ "Create an Application", "USB_Device.html#Creation_Steps", [
        [ "RTE Component Selection", "USB_Device.html#RTE_Software_Component_Selection", null ],
        [ "USB Driver and Controller", "USB_Device.html#USB_Driver_Configuration", null ],
        [ "USB Device Configuration", "USB_Device.html#USB_Device_Configuration", null ],
        [ "USB Device Class Configuration and USB Endpoint Settings", "USB_Device.html#USB_Device_Class_Configuration", null ],
        [ "System Resource Configuration", "USB_Device.html#usbd_system_resources", null ],
        [ "User Code Implementation", "USB_Device.html#User_Code_Implementation", null ],
        [ "Changing Default USB Descriptors", "USB_Device.html#Overriding_Descriptors", [
          [ "Static change", "USB_Device.html#autotoc_md0", null ],
          [ "Dynamic change", "USB_Device.html#autotoc_md1", null ]
        ] ],
        [ "Debugging", "USB_Device.html#usbd_debugging", [
          [ "Event Recorder Support", "USB_Device.html#usbDevEvrSupport", null ],
          [ "Event Recorder Configuration", "USB_Device.html#usbDevEvrConfig", null ],
          [ "Event IDs", "USB_Device.html#autotoc_md2", null ]
        ] ]
      ] ],
      [ "Examples", "USB_Device.html#USB_Device_Tutorial", [
        [ "USB Device HID", "USB_Device.html#dev_hid_tutorial", [
          [ "Build the \"USB Device HID\" Project", "USB_Device.html#autotoc_md3", [
            [ "Source Files", "USB_Device.html#autotoc_md4", null ]
          ] ],
          [ "Using the \"USB Device HID\" Project", "USB_Device.html#autotoc_md5", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md6", null ],
            [ "PC Software", "USB_Device.html#autotoc_md7", null ]
          ] ]
        ] ],
        [ "USB Device Mass Storage", "USB_Device.html#dev_msc_tutorial", [
          [ "Build the \"USB Device Mass Storage\" Project", "USB_Device.html#autotoc_md8", [
            [ "Source Files", "USB_Device.html#autotoc_md9", null ]
          ] ],
          [ "Using the \"USB Device Mass Storage\" Project", "USB_Device.html#autotoc_md10", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md11", null ],
            [ "PC Software", "USB_Device.html#autotoc_md12", null ]
          ] ]
        ] ],
        [ "USB Device Virtual COM Port", "USB_Device.html#dev_cdc_tutorial", [
          [ "Build the \"USB Device Virtual COM\" Project", "USB_Device.html#autotoc_md13", [
            [ "Source Files", "USB_Device.html#autotoc_md14", null ]
          ] ],
          [ "Using the \"USB Device Virtual COM\" Project", "USB_Device.html#autotoc_md15", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md16", null ],
            [ "PC Software", "USB_Device.html#autotoc_md17", null ]
          ] ]
        ] ],
        [ "USB Device Audio", "USB_Device.html#dev_adc_tutorial", [
          [ "Build the \"USB Device Audio\" Project", "USB_Device.html#autotoc_md18", [
            [ "Source Files", "USB_Device.html#autotoc_md19", null ]
          ] ],
          [ "Using the \"USB Device Audio\" Project", "USB_Device.html#autotoc_md20", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md21", null ],
            [ "PC Software", "USB_Device.html#autotoc_md22", null ]
          ] ]
        ] ],
        [ "Custom USB Device (WinUSB_Echo)", "USB_Device.html#dev_cc_tutorial", [
          [ "Build the \"WinUSB_Echo\" Project", "USB_Device.html#autotoc_md23", [
            [ "Source Files", "USB_Device.html#autotoc_md24", null ]
          ] ],
          [ "Using the \"WinUSB_Echo\" Project", "USB_Device.html#autotoc_md25", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md26", null ],
            [ "PC Software", "USB_Device.html#autotoc_md27", null ],
            [ "Examples", "USB_Device.html#autotoc_md28", null ]
          ] ]
        ] ],
        [ "USB Composite Device", "USB_Device.html#dev_comp_tutorial", [
          [ "Build the \"USB Composite\" Project", "USB_Device.html#autotoc_md29", [
            [ "Source Files", "USB_Device.html#autotoc_md30", null ],
            [ "USB Device Configuration", "USB_Device.html#autotoc_md31", null ]
          ] ],
          [ "Using the \"USB Composite\" Project", "USB_Device.html#autotoc_md32", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md33", null ],
            [ "PC Software", "USB_Device.html#autotoc_md34", null ]
          ] ]
        ] ],
        [ "USB Mouse", "USB_Device.html#dev_mouse_tutorial", [
          [ "Create the \"USB Mouse\" Project", "USB_Device.html#autotoc_md35", [
            [ "Source Files", "USB_Device.html#autotoc_md36", null ]
          ] ],
          [ "Using the \"USB Mouse\" Project", "USB_Device.html#autotoc_md37", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md38", null ]
          ] ]
        ] ],
        [ "Ethernet-over-USB (for Linux hosts)", "USB_Device.html#dev_cdc_ncm", [
          [ "Create the \"Ethernet-over-USB\" Project", "USB_Device.html#autotoc_md39", [
            [ "Source Files", "USB_Device.html#autotoc_md40", null ]
          ] ],
          [ "Using the \"Ethernet-over-USB\" Project", "USB_Device.html#autotoc_md41", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md42", null ],
            [ "Troubleshooting", "USB_Device.html#autotoc_md43", null ]
          ] ]
        ] ],
        [ "USB Device RNDIS to Ethernet Bridge", "USB_Device.html#dev_cdc_acm_rndis_bridge", [
          [ "Build the \"RNDIS to Ethernet Bridge\" project", "USB_Device.html#autotoc_md44", [
            [ "Source Files", "USB_Device.html#autotoc_md45", null ]
          ] ],
          [ "Using the \"RNDIS to Ethernet Bridge\" Project", "USB_Device.html#autotoc_md46", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md47", null ]
          ] ]
        ] ],
        [ "USB Web Server (for Windows hosts)", "USB_Device.html#dev_cdc_acm_rndis", [
          [ "Create the \"USB Web Server\" project", "USB_Device.html#autotoc_md48", null ],
          [ "Using the \"USB Web Server\" project", "USB_Device.html#autotoc_md49", [
            [ "Hardware Setup", "USB_Device.html#autotoc_md50", null ]
          ] ]
        ] ]
      ] ],
      [ "USB Host Computer Applications", "USB_Device.html#usb_sw_utilities", [
        [ "Software Utilities", "USB_Device.html#autotoc_md51", null ],
        [ "HID Client", "USB_Device.html#hid_client_app", [
          [ "HID Client Source Code", "USB_Device.html#client_app_cpp", [
            [ "Header Files", "USB_Device.html#autotoc_md52", null ],
            [ "Resource Files", "USB_Device.html#autotoc_md53", null ],
            [ "Source Files", "USB_Device.html#autotoc_md54", null ]
          ] ]
        ] ],
        [ "WinUSB Test", "USB_Device.html#winusb_app", [
          [ "WinUSB application source code", "USB_Device.html#winusb_app_cpp", [
            [ "Header Files", "USB_Device.html#autotoc_md55", null ],
            [ "Resource Files", "USB_Device.html#autotoc_md56", null ],
            [ "Source Files", "USB_Device.html#autotoc_md57", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "USB Host", "USB_Host.html", [
      [ "RTE Components", "USB_Host.html#usbh_rte_components", null ],
      [ "Create an Application", "USB_Host.html#Create_a_USB_Host_Application", [
        [ "RTE Component Selection", "USB_Host.html#RTE_Software_Component_Selection_USBH", null ],
        [ "USB Driver and Controller", "USB_Host.html#USB_Driver_Configuration_USBH", null ],
        [ "USB Host Configuration", "USB_Host.html#USB_Host_Configuration_USBH", null ],
        [ "System Resource Configuration", "USB_Host.html#usbh_system_resources", null ],
        [ "Configuration of Attachable USB Devices", "USB_Host.html#USB_Device_Class_Configuration_USBH", null ],
        [ "User Code Implementation", "USB_Host.html#USBH_User_Code_Implementation", null ],
        [ "Debugging", "USB_Host.html#usbh_debugging", [
          [ "Event Recorder Support", "USB_Host.html#usbHostEvrSupport", null ],
          [ "Event Recorder Configuration", "USB_Host.html#usbHostEvrConfig", null ],
          [ "Event IDs", "USB_Host.html#autotoc_md58", null ]
        ] ]
      ] ],
      [ "Examples", "USB_Host.html#USB_Host_Tutorial", [
        [ "USB Host Keyboard", "USB_Host.html#host_hid_tutorial", [
          [ "Build the \"USB Host Keyboard\" Project", "USB_Host.html#autotoc_md59", [
            [ "Source Files", "USB_Host.html#autotoc_md60", null ]
          ] ],
          [ "Using the \"USB Host Keyboard\" Project", "USB_Host.html#autotoc_md61", [
            [ "Hardware Setup", "USB_Host.html#autotoc_md62", null ]
          ] ]
        ] ],
        [ "USB Host Mass Storage", "USB_Host.html#host_msc_tutorial", [
          [ "Build the \"USB Host Mass Storage\" Project", "USB_Host.html#autotoc_md63", [
            [ "Source Files", "USB_Host.html#autotoc_md64", null ]
          ] ],
          [ "Using the \"USB Host Mass Storage\" Project", "USB_Host.html#autotoc_md65", [
            [ "Hardware Setup", "USB_Host.html#autotoc_md66", null ]
          ] ]
        ] ],
        [ "USB Host Custom Class", "USB_Host.html#host_cust_tutorial", [
          [ "Create the \"USB Host Custom Class PL2303\" Project", "USB_Host.html#autotoc_md67", [
            [ "Source Files", "USB_Host.html#autotoc_md68", null ]
          ] ],
          [ "Using the \"USB Host Custom Class PL2303\" Project", "USB_Host.html#autotoc_md69", [
            [ "Hardware Setup", "USB_Host.html#autotoc_md70", null ]
          ] ]
        ] ],
        [ "USB Host CDC ACM", "USB_Host.html#host_cdc_tutorial", [
          [ "Create the \"USB Host CDC\" Project", "USB_Host.html#autotoc_md71", [
            [ "Source Files", "USB_Host.html#autotoc_md72", null ]
          ] ],
          [ "Using the \"USB Host CDC\" Project", "USB_Host.html#autotoc_md73", [
            [ "Hardware Setup", "USB_Host.html#autotoc_md74", null ]
          ] ]
        ] ]
      ] ]
    ] ],
    [ "USB Concepts", "USB_Concepts.html", [
      [ "USB Transfer Rates", "USB_Concepts.html#USB_Transfer_Rates", null ],
      [ "USB Network", "USB_Concepts.html#USB_Network", null ],
      [ "USB Communication", "USB_Concepts.html#USB_Endpoints", [
        [ "Pipes", "USB_Concepts.html#pipe_types", null ],
        [ "Transfers", "USB_Concepts.html#transfers", [
          [ "Control Transfers", "USB_Concepts.html#USB_Control_Transfers", null ],
          [ "Interrupt Transfers", "USB_Concepts.html#USB_Interrupt_Transfers", null ],
          [ "Isochronous Transfers", "USB_Concepts.html#USB_Isochronous_Transfers", null ],
          [ "Bulk Transfers", "USB_Concepts.html#USB_Bulk_Transfers", null ]
        ] ],
        [ "Endpoints", "USB_Concepts.html#endpoints", null ]
      ] ],
      [ "USB Protocol", "USB_Concepts.html#USB_Protocol", [
        [ "Transactions", "USB_Concepts.html#autotoc_md75", null ],
        [ "Packets", "USB_Concepts.html#autotoc_md76", null ]
      ] ],
      [ "USB Descriptors", "USB_Concepts.html#USB_Descriptors", [
        [ "Device Descriptor", "USB_Concepts.html#USB_Device_Descriptor", null ],
        [ "Configuration Descriptor", "USB_Concepts.html#USB_Configuration_Descriptor", null ],
        [ "Interface Descriptor", "USB_Concepts.html#USB_Interface_Descriptor", null ],
        [ "Endpoint Descriptor", "USB_Concepts.html#USB_Endpoint_Descriptor", null ],
        [ "Device Qualifier Descriptor", "USB_Concepts.html#USB_Device_Qualifier_Descriptor", null ],
        [ "String Descriptors", "USB_Concepts.html#USB_String_Descriptor", null ]
      ] ]
    ] ],
    [ "Supported USB Classes", "USB_Classes.html", [
      [ "ADC: Audio Device Class", "USB_Classes.html#ADC", null ],
      [ "CDC: Communication Device Class", "USB_Classes.html#CDC", [
        [ "CDC Class Features", "USB_Classes.html#autotoc_md77", null ],
        [ "Control Transfers", "USB_Classes.html#autotoc_md78", null ],
        [ "USB Descriptors", "USB_Classes.html#autotoc_md79", null ]
      ] ],
      [ "HID: Human Interface Device Class", "USB_Classes.html#HID", [
        [ "HID Class Features", "USB_Classes.html#autotoc_md80", null ],
        [ "Control Transfers", "USB_Classes.html#autotoc_md81", null ],
        [ "Interrupt Transfers", "USB_Classes.html#autotoc_md82", null ],
        [ "USB Descriptors", "USB_Classes.html#autotoc_md83", null ]
      ] ],
      [ "MSC: Mass Storage Class", "USB_Classes.html#MSC", [
        [ "MSC Class Features", "USB_Classes.html#autotoc_md84", null ],
        [ "Control Transfers", "USB_Classes.html#autotoc_md85", null ],
        [ "Bulk Transfers", "USB_Classes.html#autotoc_md86", null ],
        [ "USB Descriptors", "USB_Classes.html#autotoc_md87", null ]
      ] ],
      [ "Custom Class", "USB_Classes.html#CustomClass", [
        [ "USB Host supporting any Device Class", "USB_Classes.html#autotoc_md88", null ],
        [ "USB Device implementing any Device Class", "USB_Classes.html#autotoc_md89", [
          [ "USB Descriptors", "USB_Classes.html#autotoc_md90", null ]
        ] ]
      ] ]
    ] ],
    [ "Function Overview", "usb_function_reference.html", [
      [ "USB Device", "usb_function_reference.html#usb_fo_usbd", null ],
      [ "USB Host", "usb_function_reference.html#usb_fo_usbh", null ]
    ] ],
    [ "Resource Requirements", "usb_resource_requirements.html", [
      [ "USB Device Resource Requirements", "usb_resource_requirements.html#usbd_res_req", [
        [ "Stack Requirements", "usb_resource_requirements.html#usbd_stack_req", null ],
        [ "CMSIS-RTOS2 Requirements", "usb_resource_requirements.html#usbd_rtos_req", null ],
        [ "RTX v5 Requirements", "usb_resource_requirements.html#usbd_req_rtx5", null ],
        [ "Memory Requirements", "usb_resource_requirements.html#usbd_mem_req", null ]
      ] ],
      [ "USB Host Resource Requirements", "usb_resource_requirements.html#usbh_res_req", [
        [ "Stack Requirements", "usb_resource_requirements.html#usbh_stack_req", null ],
        [ "CMSIS-RTOS2 Requirements", "usb_resource_requirements.html#usbh_rtos_req", null ],
        [ "RTX v5 Requirements", "usb_resource_requirements.html#usbh_req_rtx5", null ],
        [ "Memory Requirements", "usb_resource_requirements.html#usbh_mem_req", null ]
      ] ]
    ] ],
    [ "Migration Guide", "usb_migration.html", [
      [ "Component migration", "usb_migration.html#usb_migration_component", null ],
      [ "Configuration migration", "usb_migration.html#usb_migration_config", [
        [ "Debug", "usb_migration.html#autotoc_md91", null ],
        [ "USB Device", "usb_migration.html#autotoc_md92", null ],
        [ "USB Host", "usb_migration.html#autotoc_md93", null ]
      ] ]
    ] ],
    [ "API Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"USB_Classes.html",
"group__usbd__classFunctions__api.html",
"group__usbd__evr__core__func.html#ga70623010b35a62309ea2dc5e8ca67614",
"group__usbh__data__types.html#af77b1a54442f2443cf64a4766f916bcd",
"group__usbh__hidFunctions__api.html#ga9b98d20c76bcfd8dd4cd1bd4bb00bcd0"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';