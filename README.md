[![Version](https://img.shields.io/github/v/release/arm-software/MDK-Middleware)](https://github.com/ARM-software/MDK-Middleware/releases/latest)
[![FileSystem Build Test](https://img.shields.io/github/actions/workflow/status/Arm-Software/MDK-Middleware/build_fs.yml?logo=arm&logoColor=0091bd&label=FileSystem%20Build%20Test)](./.ci)
[![Network Build Test](https://img.shields.io/github/actions/workflow/status/Arm-Software/MDK-Middleware/build_net.yml?logo=arm&logoColor=0091bd&label=Network%20Build%20Test)](./.ci)
[![USB Build Test](https://img.shields.io/github/actions/workflow/status/Arm-Software/MDK-Middleware/build_usb.yml?logo=arm&logoColor=0091bd&label=USB%20Build%20Test)](./.ci)

# MDK-Middleware

This is the development repository of the [Keil MDK-Middleware](https://www.keil.arm.com/packs/mdk-middleware-keil) Version 8. MDK-Middleware uses the [CMSIS](https://arm-software.github.io/CMSIS_6/main/General/index.html) software framework and can be compiled with various toolchains such as Arm Compiler 6, GCC, IAR, and LLVM.

## Useful Links

- [Documentation](https://arm-software.github.io/MDK-Middleware/latest/General/index.html) - access to MDK-Middleware user's manual.
- [Raise Issues](https://github.com/ARM-software/MDK-Middleware/issues) - to provide feedback or report problems.

## Software Pack

The MDK-Middleware is released as [CMSIS software pack](https://www.keil.arm.com/packs/mdk-middleware-keil/versions/) and therefore accessible by CMSIS-Pack enabled software development tools. The pack is compatible with tools such as [µVision 5.40](https://www.keil.arm.com/#km6-pro-tools) and the [CMSIS-Pack Management for Eclipse](https://github.com/ARM-software/cmsis-pack-eclipse). Examples utilize new features of the [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox) and the VS Code integration.

The MDK-Middleware is annotated for the [CMSIS-View](https://arm-software.github.io/CMSIS-View/latest/index.html) Event Recorder and Component Viewer and uses [CMSIS-Compiler](https://arm-software.github.io/CMSIS-Compiler/main/index.html) for retargeting of standard C run-time library functions. It works therefore with the following other software packs:

- [ARM::CMSIS](https://www.keil.arm.com/packs/cmsis-arm) version 5.9.0 or higher.
- [ARM::CMSIS-Compiler](https://www.keil.arm.com/packs/cmsis-compiler-arm) version 2.0.0 or higher.
- [ARM::CMSIS-View](https://www.keil.arm.com/packs/cmsis-view-arm) version 1.0.0 or higher.

The MDK-Middleware examples use the *csolution project* format of the [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox) and are framed as [Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md) that require an additional software layer to run on an evaluation board. The CMSIS-Toolbox is developed under the [Open-CMSIS-Pack project](https://github.com/Open-CMSIS-Pack) and an integral part of several IDEs.

### Using the development repository

This development repository of the MDK-Middleware can be used in a local directory and [mapped as software pack](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-tools.md#install-a-repository) using for example `cpackget` with:

    cpackget add <path>/Keil.MDK-Middleware.pdsc

### Generate software pack

The software pack is generated using bash shell scripts.

- `./gen_pack.sh` (provided via [Open-CMSIS-Pack/gen-pack](
https://github.com/Open-CMSIS-Pack/gen-pack)) generates the software pack. Run this script locally with:

      MDK-Middleware $ ./gen_pack.sh

- `./Documentation/Doxygen/gen_doc.sh` generates the HTML documentation from the input in the folder `/Documentation/Doxygen/`. Run this script locally with:

      MDK-Middleware $ ./Documentation/Doxygen/gen_doc.sh

### GitHub Actions

The repository uses GitHub Actions to generate the pack, publish documentation, and verify reference applications:

- `.github/workflows/pack.yml` based on [Open-CMSIS-Pack/gen-pack-action](https://github.com/Open-CMSIS-Pack/gen-pack-action) generates pack and documentation using the [Generate software pack](#generate-software-pack) scripts.

- `.github/workflows/gh-pages.yml` publishes the documentation to [arm-software.github.io/MDK-Middleware](https://arm-software.github.io/MDK-Middleware/latest/General/index.html). The version drop-down selects previous documentation releases.

- `.github/workflows/build_fs.yml` is a build test for the [File System reference applications](https://github.com/ARM-software/MDK-Middleware/tree/main/Examples/FileSystem).

- `.github/workflows/build_net.yml` is a build test for the [Network reference applications](https://github.com/ARM-software/MDK-Middleware/tree/main/Examples/Network).

- `.github/workflows/build_usb.yml` is a build test for the [USB reference applications](https://github.com/ARM-software/MDK-Middleware/tree/main/Examples/USB).

## Repository top-level structure

Directory                   | Description
:---------------------------|:--------------
[.ci](https://github.com/ARM-software/MDK-Middleware/tree/main/.ci)                              | Build tests for components and reference applications.
[.github/workflows](https://github.com/ARM-software/MDK-Middleware/tree/main/.github/workflows)  | [GitHub Actions](#github-actions).
[Components](https://github.com/ARM-software/MDK-Middleware/tree/main/Components)                | Source code of MDK-Middleware components.
[Documentation](https://github.com/ARM-software/MDK-Middleware/tree/main/Documentation)          | Doxygen source of the [documentation](https://arm-software.github.io/MDK-Middleware/latest/General/index.html).
[Examples](https://github.com/ARM-software/MDK-Middleware/tree/main/Examples)                    | Reference applications that show the usage of MDK-Middleware.
[license_terms](https://github.com/ARM-software/MDK-Middleware/tree/main/license_terms)          | Standard Arm Tools license with MDK-Middleware usage permission.

## License

The MDK-Middleware can be used free-of-charge with Arm-based devices and deployed as part of software applications. Refer to [redistributables.txt](https://github.com/ARM-software/MDK-Middleware/blob/main/license_terms/redistributables.txt) for more information.

Users of the [MDK-Professional or MDK-Essential edition](https://www.keil.arm.com/keil-mdk/#mdk-v6-editions) are entitled to contact Arm for support and maintenance.
