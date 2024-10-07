# Migration Guide {#migration_guide}

The chapters below describe migration from an existing project using File System library before version 8.

- Refer to \ref migrate_components to learn how to migrate the File System components.
- Refer to \ref migrate_config to learn what changed in the File System configuration files.

## Component Selection {#migrate_components}

There are three major changes in File System version 8 that affect how File System components
get selected.

1. Bundle **MDK** has replaced bundles **MDK-Pro** and **MDK-Plus**

   Where applicable replace components:

   ```txt
     Keil::File System&MDK-Pro
     or
     Keil::File System&MDK-Plus
   ```

   with components that reference bundle **MDK**:

   ```txt
     Keil::File System&MDK
   ```

2. Legacy Standard C Library retarget components are no longer supported

   This means that using components from the CMSIS-Compiler is now mandatory. Nevertheless,
   migrating from Compiler to CMSIS-Compiler components is straightforward.

   To migrate your project replace component:

   ```txt
     Keil::Compiler::I/O:File&File System
   ```

   with components from CMSIS-Compiler class:

   ```txt
     ARM::CMSIS-Compiler:CORE
     and
     ARM::CMSIS-Compiler:File Interface:MDK-MW File System
   ```

   Components from **Compiler** and **CMSIS-Compiler** are incompatible and cannot coexist hence all other components from Compiler class must also be migrated to components from CMSIS-Compiler:

   Migrate components:

   ```txt
     Keil::Compiler:I/O:STDERR&{Cvariant}  ->  ARM::CMSIS-Compiler:STDERR:{Csub}
     Keil::Compiler:I/O:STDIN&{Cvariant}   ->  ARM::CMSIS-Compiler:STDIN:{Csub}
     Keil::Compiler:I/O:STDOUT&{Cvariant}  ->  ARM::CMSIS-Compiler:STDOUT:{Csub}
     Keil::Compiler:I/O:TTY&{Cvariant}     ->  ARM::CMSIS-Compiler:TTY:{Csub}
   ```

3. **Event Recorder** component is now part of **CMSIS-View** class

   If your project is configured for debugging then you need to replace component:

   ```txt
     Keil::Compiler:Event Recorder
   ```

   with component from CMSIS-View class:

   ```txt
     ARM::CMSIS-View:Event Recorder
   ```

## Configuration {#migrate_config}

In general two major changes were made to the File System version 8 configuration files and affect
how File System gets configured.

1. The core configuration has been moved from C source file to C header file

   To migrate configuration of an existing project use compare/merge tool on the following files:

   ```txt
      FS_Config.c  and  FS_Config.h
   ```

   > Note: FS_Config.h does not have configuration for Initial Current Drive anymore. To understand how current drive is set see chapter \ref cur_sys_drive.

2. Drive configuration changed the way how drive cache and drive buffer placement works

   On microcontrollers where DMA has limited access to memory areas one must place drive buffers into the appropriate memory in order for peripheral driver to work. In File System version 8 this is supported by placing drive buffer in a dedicated section which must be properly located with the toolchain linker script.

   The latest drive configuration files contain a define named **_CACHE_SECTION** where one can define the section name.

   For example:

   ```txt
      //     <s>Section Name
      //     <i>Define the name of the section for the drive cache and drive buffers.
      //     <i>Linker script shall have this section defined.
      #define MC0_CACHE_SECTION       ".driver.mci0"
   ```
  
  When using RAM drive one can move the file system data buffer (buffer that holds directories, files and files content) into a specific memory. The latest drive configuration file supports this by using define **_SECTION**.

  The define for RAM drive 0 looks like shown below:

  ```txt
     //     <s>Section Name
     //     <i>Define the name of the section for the file system buffer.
     //     <i>Linker script shall have this section defined.
     #define RAM0_SECTION           ".filesystem.ram0"
  ```

  To migrate configuration of an existing project use compare/merge tool on drive configuration files and update toolchain linker script to support defined sections.
