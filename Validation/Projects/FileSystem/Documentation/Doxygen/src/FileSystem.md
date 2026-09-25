# Overview {#mainpage}

The **FileSystem Validation** project verifies the public MDK-Middleware File System API and selected standard C
library file operations on representative storage backends. It covers successful operation, documented error handling,
filesystem-specific behavior, data integrity, persistence, concurrency, stress, and throughput.

The validation is provided as a target-independent reference application. The solution contains the validation
projects and shared test sources, but no predefined target type. Before building it, identify a compatible board layer
and add the required pack and target-type description to `FileSystem.csolution.yml`.

## Validation Projects {#fs_validation_projects}

The solution contains one project for each supported File System drive type:

| Project | Drive | Storage | File system | Connections required from the board layer |
|---------|-------|---------|-------------|-------------------------------------------|
| `Drive_F` | `F:` | NOR flash | EFS | `CMSIS_FLASH` and `STDOUT` |
| `Drive_M` | `M:` | SD, SDHC, MMC, or eMMC | FAT | `CMSIS_MCI` and `STDOUT` |
| `Drive_N` | `N:` | NAND flash | FAT | `CMSIS_NAND` and `STDOUT` |
| `Drive_R` | `R:` | RAM | FAT | `STDOUT` |

All projects build the same validation sources. Each project's `MW_CV_Config.h` enables the matching drive and selects
the optional C library, I/O Control, stress, and performance test families applicable to that configuration.

## Adapting the Reference Application {#fs_validation_adaptation}

Complete the hardware adaptation before attempting to build the validation:

1. Identify a board layer that provides the storage connection required by the drive to be validated and a working
   `STDOUT` channel for the validation report.
2. Select the matching drive project. A board layer may support only one drive, and a target type must not be assumed
   to work with every project in the solution.
3. Make the board layer and its accompanying files available under `Board/<target-name>/`, or use another maintained
   location and adjust the path accordingly.
4. Add the board or device pack, together with any additional packs required by the layer, to the solution-level
   `packs` list in `FileSystem.csolution.yml`.
5. Add a matching entry under `target-types`. Its `board` identifier must resolve through the selected pack, and its
   `Board-Layer` variable must point to the chosen `Board.clayer.yml`.
6. List the available solution contexts and confirm that the board layer satisfies all connections consumed by the
   selected drive project.

A typical solution extension has this form:

```yml
solution:
  packs:
    - pack: <vendor>::<board-or-device-pack>@<version>

  target-types:
    - type: <target-name>
      board: <vendor>::<board-name>:<revision>
      variables:
        - Board-Layer: $SolutionDir()$/Board/<target-name>/Board.clayer.yml
```

Replace every placeholder with the exact pack, board identifier, revision, and layer path for the selected hardware.

## Test Suite Organization {#fs_validation_organization}

The API Reference presents every test case under its validation area:

- \ref mw_cv_fs_core_test_funcs validates lifecycle, file, utility, time, and other behavior common to applicable file
  systems.
- \ref mw_cv_fs_clib_test_funcs validates supported standard C library stream operations through the File System
  retarget interface.
- \ref mw_cv_fs_fat_test_funcs validates FAT directories, names, attributes, timestamps, and other FAT-specific
  behavior.
- \ref mw_cv_fs_efs_test_funcs validates EFS operations and documented EFS limitations.
- \ref mw_cv_fs_mc_test_funcs validates memory-card media and control callbacks.
- \ref mw_cv_fs_ioc_test_funcs validates direct media identification, locking, geometry, cache, control, and sector
  access.
- \ref mw_cv_fs_stress_test_funcs exercises persistence, copying, concurrency, directory searches, and name-cache
  pressure.
- \ref mw_cv_fs_performance_test_funcs measures C library and I/O Control read/write throughput.

Each test case documents its method and expected result in the Doxygen block immediately preceding the test function.
Compile-time switches ensure that only tests applicable to the selected filesystem and enabled feature families are
registered for execution.

## Running the Validation {#fs_validation_running}

After adapting the solution, list its contexts from `Validation/Projects/FileSystem`:

```sh
csolution list contexts FileSystem.csolution.yml
```

Build only a context supported by the selected board layer, for example:

```sh
cbuild FileSystem.csolution.yml --context Drive_M.Debug+<target-name> --toolchain AC6
```

Program the image on the target, prepare removable media where required, and review the complete report through the
board layer's `STDOUT` channel.
