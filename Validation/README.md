# MDK-Middleware Component Validation (MW CV)

The **MW CV Test Suite** is used for **validation** of the **MDK-Middleware Components** compliance
to the specified API and functionality.

## Available Validations

Validation projects are available for the following Components:

- **File System**
  - **Drive F** - NOR flash (skeleton project; no tests implemented yet)
  - **Drive M** - Memory card (skeleton project; no tests implemented yet)
  - **Drive N** - NAND flash (skeleton project; no tests implemented yet)
  - **Drive R** - RAM (skeleton project; no tests implemented yet)
- **Network**
  - **BSD Sockets**
- **USB Device**
  - **Core**
  - **CDC-ACM**
  - **HID**
  - **MSC**
- **USB Host**
  - **Core**
  - **CDC-ACM**
  - **HID**
  - **MSC**

## Organization

The files for the MW CV are organized in the following folders:

  | Folder      | Description
  |:------------|:----------------------
  | .           | This `README.md` file
  | Config      | MW CV configuration file
  | Include     | MW CV public include file
  | Projects    | Validation projects (csolution)
  | Results     | Results of validation
  | Source      | MW CV source files

> Note: **SockServer** required by the Network validation is not a part of this GitHub repository and can be found on
  the [CMSIS-Driver_Validation](https://github.com/ARM-software/CMSIS-Driver_Validation/tree/main/Tools/SockServer/PC/Win) GitHub repository.

## Configuration

The selection and configuration of Components for testing is done via [MW_CV_Config.h](./Config/MW_CV_Config.h) file.

### Board layer

In order to build the validation project it shall be extended with a compatible board layer that provides the interfaces required by the selected project as
[connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections):

- `CMSIS_ETH`:        CMSIS-Driver for Ethernet interface.
- `CMSIS_FLASH`:      CMSIS-Driver for NOR Flash interface.
- `CMSIS_MCI`:        CMSIS-Driver for MCI interface.
- `CMSIS_NAND`:       CMSIS-Driver for NAND Flash interface.
- `CMSIS_USB_Device`: CMSIS-Driver for USB Device interface.
- `CMSIS_USB_Host`:   CMSIS-Driver for USB Host interface.
- `STDOUT`:           Standard Output channel

> Note: Two USB ports are necessary (one configured as Device and the other as Host).  
> The SD Card is also necessary for most USB tests.

## Usage

### Build the Validation Project

- Open the **<FileSystem|Network|USB_Device|USB_Host>.csolution.yml**.
- Copy the compatible board layer with accompanying files from the suitable **BSP** to the  
  `<FileSystem|Network|USB_Device|USB_Host.csolution.yml root>/Board/<board name>/` folder.
  The location `<FileSystem|Network|USB_Device|USB_Host.csolution.yml root>/Board/<board name>/`
  should contain **Board.clayer.yml** with other accompanying board layer files.
- Edit the **<FileSystem|Network|USB_Device|USB_Host>.csolution.yml** file and add board description under **target-types**,
  for example for board **STM32H743I-EVAL** target type would look like below:

  ```yml
  target-types:
    - type: STM32H743I-EVAL
      board: STMicroelectronics::STM32H743I-EVAL:Rev.B
      variables:
        - Board-Layer: $SolutionDir()$/Board/STM32H743I-EVAL/Board.clayer.yml
  ```

- Select the desired **Component** as an active project for the build process.
- Build the project and observe that no errors are reported.

### Execute the Validation

#### Setup

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

- For Network validation, **connect the Ethernet cable from your local network to the board**.
- For USB validation, **connect the USB cable between two USB ports on the board**.
- For File System drive `M:` **insert the SD Card into an SD Card slot on the board**.
- For File System drives `F:` and `N:`, ensure the corresponding flash device is available on the board.

#### File System Validation execution

- Load the executable image to the target development board.
- Results of the validation should appear in the **STDIO** channel (usually Virtual COM port).

#### Network Validation execution

- Start the **SockServer** application from the [CMSIS-Driver_Validation](https://github.com/ARM-software/CMSIS-Driver_Validation/tree/main/Tools/SockServer/PC/Win)
  GitHub repository, and note the IP **Address**.
  > Note: PC running SockServer has to be connected to the same network as board used for testing.
- Open the `MW_CV_Config.h` file located in the ./Network/BSD_Sockets folder and change the `MW_CV_NET_SOCKSERVER_IP`
  define value to the previously noted **SockServer** IP **Address**.
- Load the executable image to the target development board.
- Results of the validation should appear in the **STDIO** channel (usually Virtual COM port).

#### USB Validation execution

- Load the executable image to the target development board.
- Results of the validation should appear in the **STDIO** channel (usually Virtual COM port).
