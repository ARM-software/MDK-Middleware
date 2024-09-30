USB Device Virtual COM Port Example
===========

This MDK-Middleware example project implements a USB Device that exchanges data with a USB Host using Communication Device Class as a virtual communication port.

For detailed description see [USB Device Virtual COM Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/USB/dev_cdc_tutorial.html).


Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

