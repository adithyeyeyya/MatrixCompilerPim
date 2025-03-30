# PIM Compiler

## Overview
The PIM (Processing-in-Memory) Compiler is a specialized compiler for translating C++ matrix multiplication code into optimized instructions for Processor-in-Memory architectures. This project enables efficient hardware-level matrix computations with advanced code optimization capabilities, bridging the gap between high-level code and specialized PIM hardware.

## Key Features
- **C++ to PIM Instruction Translation:** Converts standard C++ matrix multiplication code to specialized PIM architecture instructions
- **AI-Powered Code Refactoring:** Implements a rule-based optimization system for matrix operations
- **Performance Optimization:** Applies loop reordering, blocking/tiling, and matrix transposition techniques
- **Memory Access Optimization:** Maps memory operations to align with PIM architecture for improved efficiency
- **Custom Instruction Set:** Implements a 32-bit instruction format designed specifically for PIM operations

## Architecture
The compiler follows a multi-stage pipeline:
1. **Parser:** Processes C++ source code into an abstract syntax tree
2. **IR Generator:** Converts the AST to LLVM intermediate representation
3. **Memory Mapper:** Transforms memory accesses according to PIM architecture requirements
4. **PIM Backend:** Generates custom PIM instructions from the IR
5. **Refactoring Assistant:** Analyzes code for optimization opportunities

## Requirements
- CMake (version 3.10 or higher)
- LLVM libraries (with development headers)
- Clang libraries (optional, but recommended for full functionality)
- C++17 compatible compiler

## Building from Source
```bash
mkdir -p build
cd build
cmake ..
make
```

If Clang is not available, you can disable it:
```bash
cmake -DHAVE_CLANG=OFF ..
make
```

## Usage
Basic compilation:
```bash
./pim_compiler input_file.cpp -o output.txt
```

Compilation with verbose output:
```bash
./pim_compiler -v input_file.cpp -o output.txt
```

Generate refactoring suggestions:
```bash
./pim_compiler --refactor input_file.cpp
```

Only generate refactoring suggestions without compiling:
```bash
./pim_compiler --refactor-only input_file.cpp
```

Generate detailed refactoring suggestions:
```bash
./pim_compiler --refactor-detailed input_file.cpp
```

## Instruction Set
The PIM instruction set includes:
- Memory operations: LOAD, STORE, MOVE
- Arithmetic operations: ADD, SUB, MUL, DIV
- Logical operations: AND, OR, XOR, NOT, SHL, SHR
- Control flow: JUMP, JUMPZ, JUMPNZ
- Configuration: CONFIG

## Optimization Techniques
1. **Loop Reordering:** Transforms i-j-k loop ordering to i-k-j for better cache locality
2. **Blocking/Tiling:** Divides matrices into smaller blocks that fit optimally in memory
3. **Matrix Transposition:** Implements transposed layouts to optimize memory access patterns
4. **Register Blocking:** Maximizes register usage and reduces redundant operations

## Example Output
The compiler generates PIM instructions in both human-readable and binary format:
```
CONFIG 0, 4 ; 0x44001000
LOAD 0, 0 ; 0x04000000
MUL 2, 0, 1 ; 0x18080004
ADD 3, 3, 2 ; 0x100c0c08
STORE 0, 8 ; 0x08002000
```

## Applications
- Machine learning acceleration
- High-performance computing
- Scientific simulations
- Data analytics
- Education and research on PIM architectures

## Project Structure
```
PIM_Compiler/
├── src/               # Source code
│   ├── compiler/      # Core compiler components
│   ├── optimizer/     # Optimization framework
│   └── utils/         # Utility functions
├── include/           # Header files
├── examples/          # Example matrix multiplication code
├── test/              # Test cases
└── CMakeLists.txt     # Build configuration
```

