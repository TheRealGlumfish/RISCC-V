# RIS`CC`-V
C90 compiler targeting RISC-V (RV32IMDF).

## Style Guide

- Camel case `int createVector()` - Variables, structure fields, function parameters and functions
- Capital camel case `typedef struct { int x, y; } Vec2;` - Custom/user-defined types
- Capitalized `#define PPM_DEPTH 255` - Macros and enum members

## Licensing
This project has been licensed under a BSD 3-Clause "New" or "Revised" License.
The license includes all files in the project with notable exceptions.
This includes files in the `compiler_tests`, `scripts`, `.devcontainer` directories as well as `Dockerfile`.
These files were provided by Dr. John Wickerson and the Instruction Architectures and Compilers team and were used with permission.
Thus, copyright is attributed to them and the license does not apply to them.
<!---
TODO: Specify which tests we wrote
-->

