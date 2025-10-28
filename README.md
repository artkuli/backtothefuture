# Back to the Future

A solution to the **“Back to the Future”** task.

## Features

- Pack a directory (recursively) into a single archive file.
- Unpack an archive into any target directory.
- Optional test suite (GoogleTest) via CMake flag.

## Requirements

- C++17 (or newer) compiler  
- CMake ≥ 3.20 (tested with 3.28.3)
- Linux/macOS; Windows should work with a recent MSVC/MinGW setup 

## Build

Prefer an out-of-source build:

```bash
mkdir -p build && cd build
cmake ..            # add -DBUILD_TESTS=1 to enable tests
cmake --build .
```

To enable tests:

```bash
cmake .. -DBUILD_TESTS=1
cmake --build .
```

## Usage

The application provides two commands: `pack` and `unpack`.

### Pack a directory

```bash
./backtothefuture pack </path/to/input_directory> </path/to/output_archive.bin>
```

### Unpack an archive

```bash
./backtothefuture unpack </path/to/input_archive.bin> </path/to/output_directory>
```

Notes:
- Paths are treated as **absolute or relative** to the current working directory.
- The output directory for `unpack` will be created if it does not exist.

## Running Tests

If you configured with `-DBUILD_TESTS=1`:

```bash
# From the build directory
ctest --output-on-failure
```

## Verified Environment

```
OS   : Ubuntu 24.04.2 LTS
CMake: 3.28.3
G++  : 13.3.0
```