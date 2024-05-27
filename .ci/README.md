# MDK-Middleware Build Test

This directory contains CMSIS Solution projects for build tests.

## Prerequisites

- VS Code with [Arm Keil Studio Pack extensions](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack)

## Build

- Open MDK-Middleware repository in VS Code

- Use Explorer and find required solution file:
  - .ci/FileSystem/FileSystem.csolution.yml
  - .ci/Network/Network.csolution.yml
  - .ci/USB/Device/USB_Device.csolution.yml
  - .ci/USB/Host/USB_Host.csolution.yml

- Next to the solution file one can find vcpkg-configuration.json
  - Right click on it and select "Activate Environment"

- Right click on solution file and select "Open Solution"

- CMSIS solution window opens
  - Select "Build" to build complete solution
  - Select "Active Context" to build required context

> Alternatively, use cbuild to build the examples from the bash terminal
