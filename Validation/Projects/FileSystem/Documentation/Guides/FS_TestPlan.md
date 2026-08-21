# File System Validation Test Plan {#fs_validation_test_plan}

## Purpose

This plan defines validation coverage for the public MDK-Middleware File System API: system, file, directory,
utility, time, media-support callback, and I/O Control functions. It also covers these selected ISO C stream
functions provided through the MDK-Middleware File System interface:

- `fopen`
- `fclose`
- `fwrite`
- `fread`
- `fgetc`
- `fseek`

The suite validates behavior through the public File System API and the selected C library API. Direct testing of
the low-level `fs_fopen`, `fs_fclose`, `fs_fwrite`, `fs_fread`, `fs_fflush`, `fs_fseek`, and `fs_fsize` retarget
functions is out of scope.

Individual test methods and expected results are documented exclusively in the Doxygen blocks of their test
functions. This plan intentionally does not duplicate test-case descriptions.

## Test-suite organization

| Module | Responsibility |
|--------|----------------|
| `MW_CV_FS.c/.h` | Shared initialization, cleanup, paths, data patterns, and type detection |
| `MW_CV_FS_Core.c/.h` | Public APIs and behavior common to FAT and EFS |
| `MW_CV_FS_FAT.c/.h` | FAT-only APIs, capabilities, and documented limitations |
| `MW_CV_FS_EFS.c/.h` | EFS-only APIs, capabilities, and documented limitations |
| `MW_CV_FS_Clib.c/.h` | C library behavior common to FAT and EFS plus backend-specific behavior |
| `MW_CV_FS_IOC.c/.h` | FAT I/O Control and raw-sector tests isolated from normal functional tests |
| `MW_CV_FS_MC.c/.h` | Memory-card-specific integration and removable-media behavior |
| `MW_CV_FS_Performance.c/.h` | C library and IOC read/write throughput measurements |
| `MW_CV_FS_Stress.c/.h` | Persistence, resource-pressure, concurrency, search, and cache-coherency workflows |

`MW_CV_FS.c` provides shared infrastructure and must not become a container for API-specific test cases. Direct
retarget-interface test modules are not part of this plan.

## Applicability

Run every applicable test on each enabled File System validation project:

| Project | Drive | Storage | Filesystem |
|---------|-------|---------|------------|
| `Drive_F` | `F:` | NOR flash | EFS |
| `Drive_M` | `M:` | SD/MMC memory card | FAT |
| `Drive_N` | `N:` | NAND flash | FAT |
| `Drive_R` | `R:` | RAM | FAT |

After mounting or formatting a drive, setup must call `finfo` and verify the reported `fs_type`. A mismatch between
the selected project and actual filesystem is a setup failure.

Tests common to FAT and EFS run on every applicable drive. FAT- and EFS-specific cases remain separate and have
unambiguous expected results; a test must not silently change its pass criteria according to the backend.

## Public API allocation

| API family | Common (`Core`) | FAT-specific | EFS-specific |
|------------|-----------------|--------------|--------------|
| System | `finit`, `funinit`, `fmount`, `funmount`, `fversion` | — | — |
| File maintenance | `fdelete`, `ffind`, `frename` | `fattrib` | Unsupported FAT-only behavior |
| Directory maintenance | — | `fpwd`, `fchdir`, `fmkdir`, `frmdir` | Directory operations rejected |
| Utility | `fchdrive`, `ffree`, `fformat`, `finfo` | `fvol`; `fmedia` on removable drives | `fanalyse`, `fcheck`, `fdefrag` |
| File time | `fs_get_time` directly and indirectly | `ftime_set`, `ftime_get` | Timestamp operations rejected |
| Media support | — | Memory-card callbacks in `MW_CV_FS_MC` | — |
| I/O Control | — | All `fs_ioc_*` functions in `MW_CV_FS_IOC` | IOC access rejected |
| C library | Six selected stream functions | FAT update, seek, path, and name behavior | EFS documented limitations |

The callback functions `fs_get_time`, `fs_mc_read_cd`, `fs_mc_read_wp`, `fs_mc_spi_control_ss`, and
`fs_mc_lock_unlock` are application integration points and part of the public API. Validation supplies deterministic
implementations, tests their contracts directly, and verifies their effect through the component where applicable.

## Test selection and registration

- `MW_CV_FS` enables File System validation and the active `MW_CV_FS_DRIVE_*` selector chooses the drive project.
- `MW_CV_FS_CLIB` controls registration of the C library test unit.
- `MW_CV_FS_IOC` enables FAT I/O Control cases, including destructive raw-sector writes.
- `MW_CV_FS_PERFORMANCE` enables File System throughput measurements.
- `MW_CV_FS_STRESS` enables persistence, concurrency, resource-pressure, search, and name-cache workflows.
- Common native API tests are registered for every File System project.
- FAT- or EFS-specific tests are registered according to the configured drive type and confirm that type at runtime.
- Memory-card callback tests are registered only for `Drive_M`.

Raw-sector writes, media removal, fault injection, and long-running stress cases are destructive or
hardware-dependent. They must be separately selectable and run only on dedicated validation media.

## Preconditions and safety

- Exactly one `MW_CV_FS_DRIVE_*` selector is enabled by the active project.
- The selected drive is initialized, mounted, formatted, and made current before access, except when lifecycle state
  is itself under test.
- Tests run only on dedicated validation media; initialization and formatting may destroy existing content.
- Each test owns its names, removes stale state before setup, and performs best-effort cleanup afterward.
- Every successfully opened stream is closed, including failure paths.
- Test buffers are statically allocated where practical so results do not depend on thread stack size.
- Destructive IOC tests remain after functional and stress tests so raw-sector changes cannot affect them.
- Performance tests format the drive before each run and execute after stress tests.

## Common test data and helpers

Use deterministic binary data so every byte value, including `0x00`, values above `0x7F`, and `0xFF`, is handled
without text assumptions:

```text
data[i] = (i * 37 + 11) & 0xFF
```

Use lengths around common boundaries where practical: `0`, `1`, `7`, `511`, `512`, and `513` bytes. A 513-byte
transfer crosses a typical 512-byte media sector boundary.

Private helpers may prepare the configured drive, construct paths, remove test-owned objects, create deterministic
files, compare content, and close valid streams. A failed prerequisite must be reported with `ASSERT_TRUE` and must
prevent dependent operations from using invalid handles or pointers.

## Validation dimensions

- Verify exact documented `fsStatus` and `errno` values, not only success or failure.
- Exercise valid, current, empty, malformed, nonexistent, uninitialized, and unmounted drive specifications.
- Verify short-name case handling, FAT long-file-name behavior, similar-name collisions, and filename boundaries.
- Verify `ffind` initialization, continuation, reset, exact lookup, wildcard matching, search scope, metadata, and
  end-of-search behavior.
- Exercise configured simultaneous-open limits and verify recovery after a handle is released.
- Repeat create, open, and close cycles to expose leaked handles or stale name-cache entries.
- Verify persistence across unmount, uninitialize, reinitialize, and remount.
- On FAT, exercise working directories, file-versus-directory errors, timestamp granularity, name-cache coherency,
  directory pressure, and mixed short and long names.
- On EFS, exercise its flat namespace, name-length limit, fragmentation, analysis, checking, and defragmentation.
- Use explicit RTOS synchronization for repeatable concurrency workflows; do not rely on timing delays or priority.
- Verify data integrity for performance transfers while excluding data preparation from measured intervals.

Configuration-dependent cases must query or derive configured limits instead of embedding legacy values. Stress
sizes must be bounded for the target medium.

## Test case granularity and Doxygen requirements

Each API function should have independently registered cases for its principal valid, boundary, state-transition,
and invalid behaviors. A test function validates one primary behavior and must not depend on another test having run.

For every test function:

- Provide a Doxygen block with `\brief`, `\par Method`, and `\par Expected`.
- Describe the method and expected behavior in natural language.
- Use C expressions only when they materially clarify an exact return value, boundary, flag, or data condition.
- Keep the expected behavior beside the implementation; do not duplicate it in this plan.
- Use suffix `_0` for the simplest valid operation and higher suffixes for increasing complexity.

## Execution and isolation rules

- A test must not depend on execution order or content left by another test.
- If setup fails, report the failure and skip dependent calls.
- Verify every return value from the API under test.
- Use `feof` and `ferror` to distinguish normal end-of-file from an I/O error.
- Close every valid stream and remove test-owned files and directories where possible.
- At suite completion, unmount and uninitialize the selected drive.

## Traceability summary

### Native API, workflow, and performance modules

| Module | Primary public APIs | Test functions |
|--------|--------------------:|---------------:|
| Core | 13 | 53 |
| FAT | 8 | 37 |
| EFS | 3 | 21 |
| MC | 5 | 11 |
| IOC | 8 | 22 |
| Stress | Cross-API | 14 |
| Performance | C library and IOC | 2 |
| **Total** | **37** | **160** |

### C library module

| Function | Common | FAT-specific | EFS-specific |
|----------|-------:|-------------:|-------------:|
| `fopen` | 10 | 8 | 5 |
| `fclose` | 3 | 0 | 0 |
| `fwrite` | 5 | 0 | 0 |
| `fread` | 5 | 0 | 0 |
| `fgetc` | 3 | 0 | 0 |
| `fseek` | 6 | 2 | 1 |
| **Total** | **32** | **10** | **6** |

The complete source suite contains 208 test functions. A FAT project can execute 42 C library cases; the EFS
project can execute 38. Feature selection, backend type, hardware applicability, and configured limits determine the
cases registered for a particular project.

## Deferred coverage

- Files larger than 2 GiB and large-offset boundary behavior.
- Media-full, write-protection, media-removal, and injected driver failures.
- Endurance measurements.
