# BLUSH

Blush puts a little beauty on top of FACE. Someone had to — raw FACE Technical Standard 3.2 IDL
is not a good look on anyone.

BLUSH is a set of C++17 libraries for building [FACE](https://www.opengroup.org/face) (Future
Airborne Capability Environment) aligned software: the standard interfaces, the plumbing UoPs are
built on, and the CMake wiring to turn a `.face` model into real, linkable code — so your
application gets to skip the concealer and start from something that already looks presentable.

## The libraries

| Subproject | What it is | CMake target |
|---|---|---|
| [`face-core`](face-core/README.md) | Header-only C++ binding of the FACE 3.2 interfaces (TSS, LCM, IOSS) | `FACE::Core` |
| [`face-utils`](face-utils/README.md) | UoP support: connection wrappers, injection storage, task scheduling | `FACE::Util` |
| [`data-model`](data-model/README.md) | Templates + a CMake function that turn a `.face` model's data types and transport interfaces into a shared library via [FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser) | `face_data_model` (via `blush_add_data_model_library()`) |
| [`uop-generator`](uop-generator/README.md) | Templates + a CMake function that turn a `.face` model into per-UoP libraries, on top of `data-model` | n/a (provides `blush_add_uop_libraries()`) |
| [`sample-project`](sample-project/README.md) | Worked example: one implementation library per UoP in a sample `.face` model, plus an executable that wires all of them together and runs the FACE lifecycle | `sample_integration` (+ one `<UoP>_impl` per UoP) |

Each subproject's own README covers its API and internals in more depth; this one covers building
BLUSH and pulling it into another project. If you want to see the pieces below used together
end-to-end before reading the reference material, start with
[`sample-project/README.md`](sample-project/README.md).

## Requirements

- CMake 3.16+
- A C++17 compiler
- To use `data-model` or `uop-generator`: `face-idl-gen` and `face-codegen` from
  [FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser) on `PATH`. `face-core` and
  `face-utils` don't need these.

## Building

```sh
cmake -S . -B build
cmake --build build -j
ctest --test-dir build
```

Tests (GoogleTest, fetched automatically) are on by default; turn them off with
`-DBLUSH_BUILD_TESTS=OFF` if you just want the libraries.

The [`sample-project`](sample-project/README.md) demo builds by default too (it needs
`face-idl-gen`/`face-codegen` on `PATH`, same as `data-model`/`uop-generator`); turn it off with
`-DBLUSH_BUILD_SAMPLE_PROJECT=OFF`. Try it with:

```sh
cmake --build build --target sample_integration
./build/sample-project/integration/sample_integration
```

## Using BLUSH in your own project

Install it, or vendor it with `add_subdirectory()`, then:

```cmake
find_package(BLUSH REQUIRED)

target_link_libraries(your_target PRIVATE FACE::Core)   # FACE interfaces only
target_link_libraries(your_target PRIVATE FACE::Util)    # + UoP connection/task helpers

# Turn your own .face model into one library per UoP it defines (this also
# pulls in the shared FACE::DM / FACE::TSS headers from data-model for you):
blush_add_uop_libraries(
    FACE_FILE      myapp.face
    OUT_LIBRARIES  MY_UOP_LIBS
)
target_link_libraries(your_target PRIVATE ${MY_UOP_LIBS})

# Or, if you just need the data model itself (e.g. a transport service with no UoPs):
blush_add_data_model_library(FACE_FILE myapp.face)
target_link_libraries(your_target PRIVATE face_data_model)
```

See [`data-model/README.md`](data-model/README.md) and
[`uop-generator/README.md`](uop-generator/README.md) for the full function references, including
the config-file (`CONFIG`) alternative to `FACE_FILE`.
