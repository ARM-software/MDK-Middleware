# Migration Guide {#migration_guide}

The following sections show how you can migrate MDK Middleware projects from version 7 to version 8:

- See \ref nw_diffs_component to learn how to migrate the Network component.
- Under \ref mig_nw_configuration you can find out what has changed in the configuration files of the Network component. 

## Component Selection {#nw_diffs_component}

To change a Network component, make the following changes to the component selection:

1. Bundle **MDK** has replaced bundles **MDK-Pro** and **MDK-Plus**

   Where applicable, replace the components:
   ```
     Keil::Network&MDK-Pro
     or
     Keil::Network&MDK-Plus
   ```

   with component that refer to the **MDK** bundle:
   ```
     Keil::Network&MDK
   ```

2. **Event Recorder** component is now part of the **CMSIS-View** class

   If your project is configured for debugging, you must replace the component:
   ```
     Keil::Compiler:Event Recorder
   ```

   with a component of the CMSIS-View class:
   ```
     ARM::CMSIS-View:Event Recorder
   ```

## Network Configuration {#mig_nw_configuration}

The following screenshots show the previous settings on the left-hand side, while the new settings
can be seen on the right-hand side.

### Net_Config.h {#mig_nw_configuration_net_config_c}

The network core configuration file has been changed from `Net_Config.c` to `Net_Config.h`.

![Old Net_Config.c vs. new Net_Config.h](diff_net_config_c.png)

The following configuration changes have been made: 
- **Variant** selection added:
  - IPv4 only
  - IPv4/IPv6 dual stack
- **net_sys_error()** handler function removed

### Net_Debug.h {#mig_nw_configuration_net_debug}

The network debug configuration file has been changed from `Net_Debug.c` to `Net_Debug.h`.

![Old Net_Debug.c vs. new Net_Debug.h](diff_net_debug_c.png)

The following configuration changes have been made:
- **Network Debug** enable switch added
- **Output Channel** selection added:
  - Event Recorder
  - STDIO
  - STDIO + Timestamp
- **Print Time Stamps** option integrated into the **Output Channel** option
- **Multicast Listener Discovery (MLD)** option added
- **net_debug_init()** function removed
- `DBG_XXX` prefix changed to `NET_DEBUG_XXX`
  

