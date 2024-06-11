# MDK-Middleware Examples

[Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md) that use defined interfaces (APIs) and are therefore hardware agnostic. These project examples show usage of middleware components and require a board layer with drivers for the specific target hardware.

## Configure Example

 - Open `<solution_name>.csolution.yml` in Editor.
 - Specify a Board under `target-types`.
   - Example for ARM AVH Corstone-300
     ```yaml
         target-types:
           - type: AVH_MPS3_Corstone-300
             board: ARM::V2M-MPS3-SSE-300-FVP
     ```
   - Example for STMicroelectronics B-U585I-IOT02A Board
     ```yaml
         target-types:
           - type: B-U585I-IOT02A
             board: STMicroelectronics::B-U585I-IOT02A
     ```
   - Example for NXP EVKB-IMXRT1050 Board
     ```yaml
         target-types:
           - type: EVKB-IMXRT1050
             board: NXP::EVKB-IMXRT1050_MDK
     ```
 - Specify a compatible board layer under `variables:` `Board-Layer:`.
   ```yaml
       target-types:
         - type: <target_name>
           board: <board_name>
           variables:
             - Board-Layer: <board_layer>.clayer.yml
   ```
   >Board layer path can be absolute or relative to the `<project_name>.cproject.yml`.  
   >Solution directory must be on the same drive as the selected `Board-Layer`.
 - Detect compatible board layer from packs when not specified directly.
   - Execute the following command which also detect compatible layers:  
     `cbuild setup <solution_name>.csolution.yml`
   - Open `<solution_name>.cbuild-idx.yml` and examine detected `target-configurations` and their `Board-layer` variables.
   - Copy desired `Board-Layer` variable to `<solution_name>.csolution.yml` under `target-types`.
     >Solution directory must be on the same drive as the selected `Board-Layer`.

### Compatible board layers from https://github.com/Open-CMSIS-Pack/RefApp-Framework
 - ARM AVH Corstone-300:  
   `Board-Layer: <framework_path>/layer/Board/AVH_MPS3_Corstone-300/IoT/Board.clayer.yml`  
   Examples
    - Network (excluding: FTP_Server, HTTP_Upload)
 - STMicroelectronics B-U585I-IOT02A Board:  
   `Board-Layer: <framework_path>/layer/Board/B-U585I-IOT02A/IoT/Board.clayer.yml`  
   Examples
    - USB Device
 - NXP EVKB-IMXRT1050 Board:  
   `Board-Layer: <framework_path>/layer/Board/IMXRT1050-EVKB/IoT/Board.clayer.yml`  
   Examples
    - File System
    - Network
    - USB Device and Host
