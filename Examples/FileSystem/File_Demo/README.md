# File Demo Example

This MDK-Middleware example project implements a **File Demo**. It shows how to manipulate files on a memory drive using the File System Component

For detailed description see [File Demo Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/FileSystem/File_Demo_Example.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

You can mount and format drives and manage files and directories using commands such as `write`, `read`, `rename`, `delete`, `mkdir` and `rmdir`.
