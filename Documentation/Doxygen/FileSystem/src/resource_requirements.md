# Resource Requirements {#resource_requirements}

This section describes the resource requirements of the **File System** component.
The actual requirements depend on the features used in the application. Therefore, the numbers provided here are guidelines
only and the actual numbers may differ.

## Stack Requirements {#fs_stack_req}

The File System component receives events sent from the interrupt service routine (ISR) of the drivers of the different
drives. The stack requirements for the ISR are typically less than 512 Bytes.

| Option (under section Stack Configuration)                        | Increase Value by
| :---------------------------------------------------------------- | :----------------------
| Stack Size (in Bytes)                                             | + 512 for Driver of a Drive

> **Note**
>
> - Stack size for the ISRs is typically configured in the application `linker script` or device `startup_device.s`.
> - The stack size for the ISRs configures only the stack space that is used by exception and interrupt service routines
>   of the drivers. The stack size requirements depend on the maximum nesting of exception and ISR execution and therefore
>   on the priority settings of the various interrupt and exception sources. The stack size of the thread that calls
>   FileSystem API function is configured in configuration file specific to the CMSIS-RTOS2 in use.
> - If you have multiple drives configured in your system with the same interrupt priority, you only need to add the stack
>   of the drive with the largest requirement. If you have drives with nested interrupts (different priorities), you need to
>   add the sum of all stack requirements.

## Heap Requirements{#fs_heap_req}

Heap is used by the standard C library memory allocation functions. The amount of heap required therefore depends on the
toolchain used to build the application image. The table below shows the heap requirements per simultaneously opened file.
If you wish to have three files open simultaneously, the value from the table must be multiplied by three.

| Toolchain       | Increase Value by
| :-------------- | :--------------------------------
| Arm Compiler 6  | + (512 + 96) for each open file
| GCC             | + (1024 + 64) for each open file

> **Note**
>
> Heap is used to hold the file descriptor and file data buffer. The size of data buffer can be changed with function
> `setvbuf` which is part of the standard C library.

## CMSIS-RTOS Requirements {#fs_rtos_req}

### Thread Stack Requirements

Although the File System component requires a CMSIS-RTOS to be present, it is not creating any additional threads.
Stack consumption depends greatly on the compiler optimization level. The table below shows approximate stack size
required by the FileSystem functions when compiled for production ready application with high optimization level.
For debugging purposes, with low or disabled optimization, the stack value should be increased.

<table class="doxtable" summary="Memory Requirements">
    <tr>
      <th>Drive Type</th>
      <th  align="left">Value</th>
    </tr>
    <tr>
      <td align="center">RAM</td>
      <td>min. 0.6 kBytes</td>
    </tr>
    <tr>
      <td align="center">MCI</td>
      <td>min. 0.7 kBytes + MCI Driver</td>
    </tr>
    <tr>
      <td align="center">NAND</td>
      <td>min. 1.3 kBytes + NAND Driver</td>
    </tr>
    <tr>
      <td align="center">NOR</td>
      <td>min. 0.4 kBytes + Flash Driver</td>
    </tr>
    <tr>
      <td align="center">USB</td>
      <td>min. 1.1 kBytes + USB Driver</td>
    </tr>
</table>

> **Note**
>
> - The stack in bytes stated is required to work with the files from top level functions like fopen, fclose, …, fdelete,
>   frename and so on.
> - The drivers for the different types of drives require additional stack. This differs from device to device, but in general
>   the stack usage for MCI, NAND, and Flash should not exceed additional 256 bytes. For USB drive, refer to \urlout{USB-Resource-Req}
>   for details.
> - If your application supports a combination of multiple drives, you need to configure your system to the the combined stack
>   size for the sum of all drives.

### Mutex Requirements

Additionally, you may also need to adjust the memory used to allocate mutex objects required by the C stdio library.
Depending on the toolchain used, the total number of mutex objects required for proper operation may differ, but in
general every active file stream requires a mutex object.

> **Note**
>
> - Arm Standard C Library and GCC Newlib require the following mutexes: 2 (internal stdio operations) + 3 (stdin, stdout and stderr file streams) + 1 for each active file stream
> - [CMSIS-Compiler](https://github.com/ARM-software/CMSIS-Compiler) provides software component `OS Interface:CMSIS-RTOS2` that implements multithreading support
>   across different toolchains when using CMSIS-RTOS2. It shall be used when file operation functions get called from multiple threads.

## Memory Requirements {#fs_spec_mem_req}

The following table shows the memory requirements for read-only (Flash) and read-write (RAM) memory.
These numbers may be used to estimate the total size requirements of the application, but the actual numbers depend
on compiler optimizations and target processor and may be therefore slightly different.

| Component                                            | RO Memory [Bytes] | RW Memory (Data) [Bytes]
| :--------------------------------------------------- | :---------------: | :------------------------
| **C stdio Library**                                  |    < 5.0 k        | < 0.3 k
| **File System:Core** Common                          |    < 3.5 k        | 0
| **File System:Core** EFS (Embedded File System)      |    < 6.0 k        | < 0.4 k
| **File System:Core** FAT with SFN (Short File Name)  |   < 13.0 k        | 1.2 k
| **File System:Core** FAT with LFN (Long File Name)   |   < 14.4 k        | 1.2 k
| **File System:Core** FAT Name caching                |      1.6 k        | 48 x *FAT Name Cache Size* (configured in `FS_Config_Drive_n.h`)
| **File System:Core** FAT Journaling                  |      0.7 k        | 0.5 k (configured in `FS_Config_Drive_n.h`)
| **File System:Drive:Memory Card** (FAT)              |      2.7 k        | < 0.2 k + *Drive Cache Size* (configured in `FS_Config_MC_n.h`)
| **File System:Drive:NAND** (FAT)                     |   < 10.6 k        | < 0.7 k + *Drive Cache Size* + *Page Caching* + *Block Indexing* (configured in `FS_Config_NAND_n.h`)
| **File System:Drive:NOR** (EFS)                      |    < 0.1 k        | < 0.1 k
| **File System:Drive:RAM** (FAT)                      |    < 0.2 k        | < 0.2 k
| **File System:Drive:USB** (FAT)                      |    < 0.6 k        | < 0.2 k + *Drive Cache Size* (configured in `FS_Config_USB_n.h`)
| **MCI Driver**                                       |   1 .. 4 k        | 0.1 .. 2 k
| **NAND Driver**                                      | 0.5 .. 4 k        | 0.1 .. 4 k
| **Flash Driver**                                     |      < 2 k        | < 0.1 k
