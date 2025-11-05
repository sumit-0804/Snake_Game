# Multi-OS Build Guide

This project supports building on Windows, Linux, and macOS.

## Prerequisites

- CMake 3.12 or higher
- C++17 compatible compiler:
  - Windows: MSVC or MinGW-w64
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode Command Line Tools (Clang)

## Build Commands

### Windows (PowerShell)

```powershell
# Release build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run
.\build\snake.exe
```

### Windows (MinGW)

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Linux

```bash
# Release build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# Run
./build/snake
```

### macOS

```bash
# Release build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# Run
./build/snake
```

## Build Types

- **Release**: Optimized build (`-O3` or `/O2`)
- **Debug**: Debug symbols, no optimization (`-g -O0` or `/Od /Zi`)

## Clean Build

To start fresh:

```bash
# Windows PowerShell
Remove-Item -Recurse -Force build

# Linux/macOS
rm -rf build
```

## Cross-Compilation

For cross-compilation, specify the toolchain file:

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
cmake --build build
```

