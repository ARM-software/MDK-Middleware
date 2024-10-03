# Function Overview {#function_overview}

The following list provides a brief overview of all File System Component functions.

- \ref stdio_routines
  - \ref fclose : Causes stream flush.
  - \ref fflush : Enforces a write operation to a data stream.
  - \ref fopen : Opens a file for reading or writing.
  - \ref freopen : Reopens a file for reading or writing.
  - \ref setbuf : Sets stream buffer.
  - \ref setvbuf : Sets stream buffer.
  - \ref fprintf : Writes a string to a data stream.
  - \ref fscanf : Reads values from a data stream.
  - \ref vfscanf : Reads data from the stream.
  - \ref vfprintf : Writes a string to a data stream.
  - \ref fgetc : Reads a single character from a data steam.
  - \ref fgets : Reads a string from a data steam.
  - \ref fputc : Writes a character to a data stream.
  - \ref fputs : Writes a string to a data stream.
  - \ref getc : Reads a single character from a data steam.
  - \ref putc : Writes a character to a data stream.
  - \ref ungetc : Stores a character back into the data stream.
  - \ref fread : Reads a number of items with a given size from a data stream.
  - \ref fwrite : Writes items from a buffer to a data stream.
  - \ref fgetpos : Stores the current value of the file position indicator.
  - \ref fseek : Positions the file cursor to a new location.
  - \ref fsetpos : Sets the file position indicator.
  - \ref ftell : Reads the file cursor position.
  - \ref rewind : Rewinds positions of the file cursor to the beginning of a data stream.
  - \ref clearerr : Clears the end-of-file and error indicators.
  - \ref feof : Determines whether an end-of-file condition has occurred.
  - \ref ferror : Tests a data stream for read or write errors.

- \ref system_routines
  - \ref finit : Initializes the required software library resources.
  - \ref funinit : Uninitializes the File System.
  - \ref fmount : Mounts the File System of a specified drive.
  - \ref funmount : Unmounts a File System volume.

- \ref file_maintenance_routines
  - \ref fdelete : Deletes one or more files.
  - \ref ffind : Searches for files or directories that match a specific pattern.
  - \ref frename : Replaces the name of a file or directory.
  - \ref fattrib : Changes file or directory attributes.

- \ref directory_maintenance_routines
  - \ref fpwd : Prints the current working directory.
  - \ref fchdir : Changes the current working directory.
  - \ref fmkdir : Creates a new directory or any intermediate directories in the path.
  - \ref frmdir : Removes a directory if it is empty.

- \ref utility_routines
  - \ref fchdrive : Changes the "Current Drive".
  - \ref ffree : Calculates the free space on a device.
  - \ref fformat : Formats an EFS or FAT storage media for using a file system.
  - \ref fanalyse : Examines the Embedded File System and checks for file fragmentation.
  - \ref fcheck : Analyses the consistency of the Embedded File System and determines if it has been initialized.
  - \ref fdefrag : De-fragments the Embedded File System.
  - \ref fmedia : Detects the presence of a removable drive in the system.
  - \ref finfo : Reads general drive information.
  - \ref fvol : Reads the volume label.

- \ref file_time_routines
  - \ref fs_get_time : Returns a time value.

- \ref media_support_routines
  - \ref fs_mc_read_cd : Returns memory card detection state.
  - \ref fs_mc_read_wp : Returns memory card write protection state.
  - \ref fs_mc_spi_control_ss : Controls the Slave Select (SS) signal state.
  - \ref fs_mc_lock_unlock : Returns the media lock/unlock information structure.

- \ref ioc_interface_routines
  - \ref fs_ioc_get_id : Returns the drive identifier associated with the specified drive letter.
  - \ref fs_ioc_lock : Locks a drive.
  - \ref fs_ioc_unlock : Unlocks a drive and releases media access to the file system layer.
  - \ref fs_ioc_get_cache : Gains information about the file system cache buffer location and its size.
  - \ref fs_ioc_read_sector : Reads data from one or more sectors of FAT formatted media.
  - \ref fs_ioc_write_sector : Writes data to one or more sectors of FAT formatted media.
  - \ref fs_ioc_read_info : Reads the media configuration.
  - \ref fs_ioc_device_ctrl : Sends control codes directly to the device driver.

- \ref retarget_interface_routines
  - \ref fs_fopen : Opens a file.
  - \ref fs_fclose : Closes a file.
  - \ref fs_fwrite : Writes to a file.
  - \ref fs_fread : Reads from a file.
  - \ref fs_fflush : Flushes the file buffers.
  - \ref fs_fseek : Moves the file position pointer.
  - \ref fs_fsize : Returns the file size.
