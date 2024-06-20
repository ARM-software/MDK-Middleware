# Overview {#mainpage}

The **File System Component** allows your embedded applications to create, save, read, and modify files in storage devices
such as **RAM**, **Flash**, **memory cards**, or **USB memory devices**.

![File System Component Structure](FileSystem.svg)

The File System Component is structured as follows:

- Storage devices are referenced as \ref drive "drives" which can be accessed by the user
.
- **Multiple instances** of the same storage device can be implemented (for example you might want to have two SD cards
  attached to your system).
- The File System **CORE** supports thread-safe operation and uses an \ref emb_fs (for NOR and SPI Flashes) or a \ref fat_fs
  (for RAM, NAND, SD/eMMC and USB devices).
- The Core allows **simultaneous access** to multiple storage devices (for example backing up data from internal flash to
  an external USB device).
- For accessing the drives appropriate **drivers** are in place to support
  - Flash chips (\ref nand_usage "NAND", \ref nor_usage "NOR, and SPI")
  - Memory card interfaces (\ref mc_usage "SD/SDxC/MMC/eMMC")
  - \ref usb_usage "USB devices"
  - On-chip \ref ram_usage "RAM", Flash and external memory interfaces. 

## Documentation Structure {#fs_docs_structure}

This user's guide contains the following chapters:

- \ref fs_create_app explains the necessary steps to develop a project containing a file system from scratch.
- \ref fs_examples are a good starting point for implementing your own storage device.
- \ref fs_operation gives more detail on the basics of the File System Component.
- \ref fs_function_reference lists the complete API of the File System Component.
