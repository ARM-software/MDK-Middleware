USB Device Mass Storage Example
===========

This MDK-Middleware example project implements a mass storage USB Device using Mass Storage Device Class.

For detailed description see [USB Device MSC Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/USB/dev_msc_tutorial.html).


Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

The implementation uses on-chip RAM as a storage device. The USB Host can access this memory drive using standard file access methods.