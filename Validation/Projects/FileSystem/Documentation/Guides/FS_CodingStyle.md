# FileSystem Validation Coding Style {#fs_validation_coding_style}

This guide defines the style for FileSystem validation code. It is based:

- primarily on the production sources in `Components/FileSystem/Source`

Use the current suite's `MW_CV_FS_*` names, framework macros, and Doxygen `Method`/`Expected` terminology.

## File organization

Use the same broad organization as a FileSystem component source:

1. Standard Arm copyright, component, file-name, and purpose banner.
2. C library, CMSIS, middleware, and local includes, grouped by origin.
3. File-local constants, macros, types, buffers, and state.
4. Application callbacks required by the FileSystem component.
5. Reusable `static` helpers and RTOS thread functions.
6. Public test-case implementations, grouped by API and backend.

Every implementation file starts with this banner form:

```c
/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - <area>
 * Copyright (c) <year> Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    <file-name>.c
 * Purpose: MDK Middleware - Component Validation - File System - <area> -
 *          tests module
 *----------------------------------------------------------------------------*/
```

Use the applicable copyright year or year range. Keep `Name` identical to the implementation file name. Describe a
common support file as a `common module`; describe test implementations as a `tests module`. Wrap a long `Purpose`
value on aligned continuation lines.

Keep implementation details file-local with `static`. Expose declarations through the matching header only when
another translation unit needs them. Use a file-local buffer only when its lifetime or size makes it preferable to a
stack allocation.

## Naming and declarations

- Keep public test names systematic: component, API, and numeric complexity suffix. In the current suite this is
  `MW_CV_FS_<group>_<api>_<n>`.
- Use `_0` for the simplest valid operation and increase the suffix as state, boundary, error, or concurrency
  complexity increases.
- Use lowercase, underscore-separated names for private helpers, matching names such as `file_name_validate` and
  `read_sector` in the component. Use the established public name when implementing a required callback.
- Use descriptive local names such as `file`, `path`, `info`, `status`, `sector`, and `count`. Short names such as
  `i` are suitable for tightly scoped loop indices.
- Declare variables at the start of a function or block, before executable statements.
- Place the pointer asterisk with the variable name, for example `FILE *file` and `const char *name`.
- Use fixed-width integer types for stored values and API data. Use `bool` for logical state.
- Append `U` to unsigned constants and use explicit casts where signedness, width, or pointer representation changes.
- Name macros and compile-time constants with uppercase underscore-separated identifiers.

## Formatting

- Indent with two spaces; do not use tabs.
- Put an opening brace on the same line as a function, `if`, `for`, `while`, or `switch` statement.
- Put spaces around binary operators and after commas.
- Align related declarations, assignments, structure initializers, and macro definitions when this makes their
  relationship easier to scan.
- Insert an empty line before an `if`, `for`, `while`, or `switch` when it starts a new logical step after local
  declarations or completed statements. The empty line separates phases; it is not required merely because a
  control keyword is present.
- Put one empty line between a block's local declarations and its first executable statement.
- Use one empty line to separate independent logical phases. When a phase starts with a comment, put the empty line
  before the comment and keep the comment attached to the code it describes.
- After a control block's closing brace, use one empty line when the following code starts an independent peer
  control, a new phase introduced by a comment, or a final return that concludes a substantial multi-phase body.
- Treat an early prerequisite or availability guard as a completed phase. Put one empty line after its closing brace
  before the operation that follows.
- Keep an API call adjacent to its immediate result check or guard. Also keep related conditions, nested controls,
  loop bookkeeping, cleanup operations, and short final returns compact.
- After a control block's closing brace, do not use an empty line before an immediate outcome comment, loop
  bookkeeping, or another direct continuation of that block.
- Do not put an empty line between `if` and `else`, before the closing `while` of a `do` statement, between nested
  closing braces, or immediately after an opening brace.
- Use two empty lines between top-level function definitions and their documentation blocks. Do not force this
  spacing between related preprocessor directives or across an `#else` or `#endif` continuation.
- Use parentheses to make mixed logical, arithmetic, and bitwise expressions unambiguous.
- Wrap multi-statement macros in `do { ... } while (0)`.
- Keep preprocessor directives visually separate from normal control flow.
- Keep lines within 120 columns and wrap long conditions or comments at a logical boundary.

## Test documentation and comments

Use `/** ... */` Doxygen blocks for every test. Document validation helpers and callbacks with `/* ... */` blocks so
Doxygen does not add them to the generated test documentation. Use a Doxygen-like format inside a helper block:

```c
/*
\brief Concise description of the helper.
\details Optional explanation for non-obvious behavior or side effects.
\param[in] argument Description of an input argument.
\param[out] result Description of an output argument.
\return Description of every possible return value.
*/
```

Document every parameter with `\param[in]`, `\param[out]`, or `\param[in,out]`. Document the return value of every
non-`void` helper; omit `\return` for a `void` helper. Add `\details` only when `\brief`, parameters, and return-value
documentation do not describe the contract adequately.

Every test block contains:

- the exact test name;
- applicable configuration or backend restrictions;
- an ordered method describing setup and API calls;
- explicit expected results, including return values, status codes, `errno`, and persistent state.

Inside the function, add concise comments for each meaningful test phase, including drive preparation, stale-state
cleanup, test-object creation, the API operation under test, result or persistent-state verification, and final
cleanup. Also comment non-obvious intent, branch or loop state transitions, protocol, media, or hardware constraints,
error recovery and fallback decisions, and the meaning of masks, units, or sentinel values. A short action-oriented
comment may describe a straightforward call when it separates and identifies a documented test step; do not comment
declarations or control syntax by themselves.

Write implementation comments as short, single-line `/* Comment */` phrases. Start with a capital letter and omit
terminal punctuation. Indent a comment with the code it describes and place it immediately before that code. Prefer
rewriting an overlong comment as a concise phrase instead of wrapping it. Use an end-of-line `/* Comment */` only for
a compact declaration or macro annotation, and align related trailing comments. Do not use `//` comments.

Use short section comments for groups of local variables, functions, or test cases. Keep terminology consistent with
the FileSystem API, for example drive, volume, media, sector, cluster, handle, and cache.

## Test implementation pattern

Use this sequence where applicable:

1. Declare local state and establish deterministic test data.
2. Prepare a unique file or directory and remove stale state when required.
3. Call the API under test.
4. Assert the exact result immediately.
5. Guard dependent operations with `if (file != NULL)` or `if (stat == fsOK)`.
6. Verify observable state or file content, not only the return value.
7. Close every successfully opened stream and restore filesystem state.

Use `ASSERT_TRUE`/`CHECK` for all relevant API results. Preserve the first failure by avoiding dependent calls after a
failed prerequisite. Multi-step state tests commonly carry an `fsStatus` forward and continue only while it is
`fsOK`.

Validate parameters and state before dereferencing pointers or accessing a drive. Keep status changes explicit and
use the FileSystem `fsStatus` value that precisely describes the outcome. Prefer a single, clearly initialized result
variable when several branches converge on one return path.

## Data, boundaries, and resources

- Generate deterministic byte patterns and verify them byte-for-byte after reopening a file.
- Exercise boundary values with loops when the operation is identical across the range.
- Derive array iteration counts with an array-size expression rather than duplicating a literal count.
- Check both success and the precise documented failure code.
- Close resources explicitly and check `fclose`; cleanup must remain safe after partial setup failure.
- Use masks and named constants for flags, attributes, modes, sizes, and sentinel values.
- Keep media operations bounded and use the configured drive geometry or resource limit instead of an assumed value.
- For concurrency tests, use explicit thread attributes and synchronization flags so scheduling remains controlled.

## Review checklist

- The file follows the component banner, organization, naming, indentation, and documentation conventions.
- Helpers and data have the narrowest practical scope.
- Every prerequisite is checked before dependent work.
- Every expected status, byte count, position, attribute, or data pattern is verified explicitly.
- Buffers and formatted paths are bounded.
- Every acquired resource is released and the configured drive is returned to a known state.
