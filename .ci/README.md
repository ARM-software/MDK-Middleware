# MDK-Middleware Build Test

This directory contains CMSIS Solution projects for build tests that generate libraries and examples (for a dummy device) using various compilers and the [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/README.md) as build system.

## CI Test with GitHub Actions

These tests are executed with [GitHub Actions](https://github.com/features/actions) using the following *build test scripts*.

Build test script | Description
:-----------------|:--------------------
[`./github/workflows/build_fs.yml`](../.github/workflows/build_fs.yml) | Executes build test for File System library and related examples.
[`./github/workflows/build_net.yml`](../.github/workflows/build_net.yml) | Executes build test for Network library and related examples.
[`./github/workflows/build_usb.yml`](../.github/workflows/build_uss.yml) | Executes build test for USB Device and Host library and related examples.


## Analyse Problems in Desktop Environment

> **Note:** 
>
> - For testing examples, it is required to define a `target-type` along with a variable to a board layer. Refer to the *build test scripts* listed above for details.

- Use VS Code with [Arm Keil Studio Pack extensions](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack).

- Open the directory `.ci` and install the tools using the `vcpkg-configuration.json` file.

- Enter the related `cbuild` command in the Terminal Window, for example:
  ```
  > cbuild 
  ```
