# internal-order-book

A C++17 backend project for handling orders from internal hedge fund strategies. No database required for now; file-based persistence is used.

## Features

- C++17
- CMake build system
- Google Test for unit testing
- File-based persistence stub

## Build

```sh
mkdir build
cd build
cmake ..
make
```

## Run

```sh
./internal-order-book
```

## Test

```sh
cd build
ctest
```

# Project Structure

- `src/` - Source files for the order book and related logic.
- `include/` - Public header files for the project.
- `test/` - Unit tests using Google Test.
- `lib/` - (Optional) Third-party libraries, e.g., Google Test as a submodule or source.
