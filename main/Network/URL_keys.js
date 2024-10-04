// The file creates links to external webpages.
// External means - urls to ARM, Keil, and CMSIS pages
//                - other external pages have not been added so far.

// constructor for the array of objects
function urlKey(key, url, txt)  {
  try{
	this.key = key;                     // key - links the url to code
	this.url = url;                     // web link 
	this.txt = txt;                     // Text shown for the link
	this.ext  = '<a class="el" href="' + this.url + '" target="_blank">' + this.txt + '</a>';  // for \urlout{1 parameter}
	this.ext2 = '<a class="el" href="' + this.url + '" target="_blank">';                      // for urlout{2 parameter}
	this.loc  = '<a class="el" href="' + this.url + '">' + this.txt + '</a>';                  // for \url{1 parameter}
	this.loc2 = '<a class="el" href="' + this.url + '">';                                      // for \url{2 parameter}
  }
  catch(err) {
    alert(err.message);
  }
};

// array of objects
var arrurl = [];

// fill array with
//                         Key                      url; relative links are possible                                               displayed Text           
//                         for uv4 docs - page name
//                         for rest - create new key 

 arrurl.push( new urlKey( "keilweb",                        "https://www.keil.arm.com",                                                                        "Keil webpage")       );
 arrurl.push( new urlKey( "keilboards",                     "https://www.keil.arm.com/boards/",                                                                "Board List") );
 arrurl.push( new urlKey( "ARM-SW-Pack",                    "https://www.keil.arm.com/packs/",                                                                 "ARM:CMSIS") );
 arrurl.push( new urlKey( "SW-Pack",                        "https://www.keil.arm.com/packs/",                                                                 "Software Packs") );
 arrurl.push( new urlKey( "MDK",                            "https://www.keil.arm.com/keil-mdk/",                                                              "Keil MDK") );

 arrurl.push( new urlKey( "CMSIS-Driver",                   "https://arm-software.github.io/CMSIS_6/latest/Driver/index.html",                                 "CMSIS-Driver")     );
 arrurl.push( new urlKey( "CMSIS-Driver-Ref",               "https://arm-software.github.io/CMSIS_6/latest/Driver/modules.html",                               "CMSIS-Driver Reference"));
 arrurl.push( new urlKey( "CMSIS-RTOS2",                    "https://arm-software.github.io/CMSIS_6/latest/RTOS2/index.html",                                  "CMSIS-RTOS v2 API")     );
 arrurl.push( new urlKey( "CMSIS-RTOS2-Ref",                "https://arm-software.github.io/CMSIS_6/latest/RTOS2/modules.html",                                "CMSIS-RTOS v2 API Reference")     );
 arrurl.push( new urlKey( "CMSIS-RTX5",                     "https://arm-software.github.io/CMSIS-RTX/latest/index.html",                                      "CMSIS-RTOS v2")     );
 arrurl.push( new urlKey( "CMSIS-Compiler",                 "https://arm-software.github.io/CMSIS-Compiler/latest/",                                           "CMSIS-Compiler")    );
 arrurl.push( new urlKey( "CMSIS-View",                     "https://arm-software.github.io/CMSIS-View/latest/",                                               "CMSIS-View")     );
 arrurl.push( new urlKey( "RTX-Conf-CM5",                   "https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html",                                "RTX_Config.h")      );

 arrurl.push( new urlKey( "Flash-API",                      "https://arm-software.github.io/CMSIS_6/latest/Driver/group__flash__interface__gr.html",           "Flash (API)"));
 arrurl.push( new urlKey( "SPI-API",                        "https://arm-software.github.io/CMSIS_6/latest/Driver/group__spi__interface__gr.html",             "SPI (API)"));
 arrurl.push( new urlKey( "MCI-API",                        "https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html",             "MCI (API)"));
 arrurl.push( new urlKey( "USB-API",                        "https://arm-software.github.io/CMSIS_6/latest/Driver/group__usb__interface__gr.html",             "USB (API)"));
 arrurl.push( new urlKey( "NAND-API",                       "https://arm-software.github.io/CMSIS_6/latest/Driver/group__nand__interface__gr.html",            "NAND (API)"));
 arrurl.push( new urlKey( "ARM-MCI-Capabilities",           "https://arm-software.github.io/CMSIS_6/latest/Driver/group__mci__interface__gr.html#structARM__MCI__CAPABILITIES",  "MCI driver capabilities"));

 arrurl.push( new urlKey( "mbedTLS",                        "https://www.keil.com/pack/doc/mbedTLS/html/index.html",                                           "Arm Mbed TLS")           );
 arrurl.push( new urlKey( "mbedTLS-Pack",                   "https://www.keil.arm.com/packs/mbedtls-arm/versions/",                                            "ARM:mbedTLS")           );
 arrurl.push( new urlKey( "SDS",                            "https://github.com/ARM-software/SDS-Framework",                                                   "Synchronous Data Stream Framework") );

 arrurl.push( new urlKey( "uv4_ca_packinstaller",           "https://developer.arm.com/documentation/101407/latest/Creating-Applications/Software-Components/Pack-Installer", "Pack Installer")     );
 arrurl.push( new urlKey( "uv4_ut_fcarmuv",                 "https://developer.arm.com/documentation/101407/latest/Utilities/Using-FCARM-with-uVision",       "FCARM"));
 arrurl.push( new urlKey( "uv4_ca_sourcefiles",             "https://developer.arm.com/documentation/101407/latest/Creating-Applications/Creating-Projects/Add-Source-Files-to-Project", "User code template") );
 arrurl.push( new urlKey( "uv4_dg_mergetoolmnu",            "https://developer.arm.com/documentation/101407/latest/Dialogs/Tools/Configure-Merge-Tool",        "Configure Merge Tool"));

 arrurl.push( new urlKey( "uv4-Event-Recorder",             "https://developer.arm.com/documentation/101407/latest/Debugging/Debug-Windows-and-Dialogs/Event-Recorder", "Event Recorder Window"));
 
 arrurl.push( new urlKey( "Event-Recorder",                 "https://arm-software.github.io/CMSIS-View/latest/evr.html",                                       "Event Recorder")     );
 arrurl.push( new urlKey( "Event-Recorder-About",           "https://arm-software.github.io/CMSIS-View/latest/evr.html#about_evr",                             "Event Recorder")     );
 arrurl.push( new urlKey( "Event-Recorder-Enable",          "https://arm-software.github.io/CMSIS-View/latest/er_use.html#Add_Event_Recorder",                 "Add Event Recorder"));
 arrurl.push( new urlKey( "Event-Recorder-Initialize",      "https://arm-software.github.io/CMSIS-View/latest/group__EventRecorder__Setup.html",               "Add Initialization Code"));
 arrurl.push( new urlKey( "Event-Recorder-Configure",       "https://arm-software.github.io/CMSIS-View/latest/er_theory.html#er_config",                       "Configure Event Recorder"));
 arrurl.push( new urlKey( "Debug-Variant-Select",           "https://arm-software.github.io/CMSIS-View/latest/er_use.html#Debug_Variants",                     "Select Debug Variant"));
 arrurl.push( new urlKey( "RTX5-Event-Recorder-Config",     "https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html#evtrecConfig",                   "RTX5 Event Recorder Configuration"));
 arrurl.push( new urlKey( "Event-Recorder-Initialize-Func", "https://arm-software.github.io/CMSIS-View/latest/group__EventRecorder__Setup.html",               "EventRecorderInitialize"));
 
 
 arrurl.push( new urlKey( "Component-Viewer-About",         "https://arm-software.github.io/CMSIS-View/latest/cmp_viewer.html#about_cmp_viewerr",              "Component Viewer"));

 arrurl.push( new urlKey( "USB-Resource-Req",               "https://arm-software.github.io/MDK-Middleware/latest/USB/resource_requirements.html",         "USB Resource Requirements"));
 arrurl.push( new urlKey( "MW-General",                     "https://arm-software.github.io/MDK-Middleware/latest/General/index.html",                         "MDK-Middleware"));
 arrurl.push( new urlKey( "MW-FileSystem",                  "https://arm-software.github.io/MDK-Middleware/latest/FileSystem/index.html",                      "File System Component"));
 arrurl.push( new urlKey( "MW-Network",                     "https://arm-software.github.io/MDK-Middleware/latest/Network/index.html",                         "Network Component"));
 arrurl.push( new urlKey( "MW-USB",                         "https://arm-software.github.io/MDK-Middleware/latest/USB/index.html",                             "USB Component"));

 arrurl.push( new urlKey( "STM32Cube",                      "https://www.st.com/en/development-tools/stm32cubemx.html",                                        "STMicroelectronic's STM32CubeMX"));
 arrurl.push( new urlKey( "MCUXpresso",                     "https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-config-tools-pins-clocks-and-peripherals:MCUXpresso-Config-Tools",  "NXP's MCUXpresso Config Tools"));


// helper variable
var arr_length = arrurl.length;


// this function is called from doxygen with the command \url{key}
// where key - is the Key in the array arrurl
// the function opens the link in current browser window
function link_loc(key)  {
  for ( var i=0; i < arr_length; i++ ) {
    if (arrurl[i].key == key)  {              // find link in array
       document.write(arrurl[i].loc);         // show default text and open link in browser
       return;
    }
  }
  key_is_missing(key);
}; 

// called with \url{key, your text}
function link_loc2(key, txt)  {
  for ( var i=0; i < arr_length; i++ ) {
    if (arrurl[i].key == key)  {                              // find link in array
       document.write(arrurl[i].loc2 + txt + '</a>');         // show your text and open link in new browser
       return;
    }
  }
  key_is_missing(key);
}; 

// this function is called from doxygen with the command \urlout{key}
// where key - is the Key in the array arrurl
// opens the link in a new browser window
function link_ext(key)  {
  for ( var i=0; i < arr_length; i++) {
    if (arrurl[i].key == key)  {              // find link in array
       document.write(arrurl[i].ext);         // show default text and open link in new browser window
       return;
    }
  }
  key_is_missing(key);
}; 

// called with \urlout{key, your text}
function link_ext2(key, txt)  {
  for ( var i=0; i < arr_length; i++ ) {
    if (arrurl[i].key == key)  {                        // find link in array
       document.write(arrurl[i].ext2 + txt + '</a>');   // show your text and open link in another browser window
       return;
    }
  }
  key_is_missing(key);
}; 

// inform user about missing key
function key_is_missing(key) {
    alert('URL-key: "' + key + '" not found. \n - Verify your document or \n - add the key to "URL_keys.js"');
};



