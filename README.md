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
| [`uop-generator`](uop-generator/README.md) | Templates + a CMake function that turn a `.face` model into per-UoP libraries via [FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser) | n/a (provides `blush_add_uop_libraries()`) |

Each subproject's own README covers its API and internals in more depth; this one covers building
BLUSH and pulling it into another project.

## Requirements

- CMake 3.16+
- A C++17 compiler
- To use `uop-generator`: `face-idl-gen` and `face-codegen` from
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

## Using BLUSH in your own project

Install it, or vendor it with `add_subdirectory()`, then:

```cmake
find_package(BLUSH REQUIRED)

target_link_libraries(your_target PRIVATE FACE::Core)   # FACE interfaces only
target_link_libraries(your_target PRIVATE FACE::Util)    # + UoP connection/task helpers

# Turn your own .face model into one library per UoP it defines:
blush_add_uop_libraries(
    FACE_FILE      myapp.face
    OUT_LIBRARIES  MY_UOP_LIBS
)
target_link_libraries(your_target PRIVATE ${MY_UOP_LIBS})
```

See [`uop-generator/README.md`](uop-generator/README.md) for the full `blush_add_uop_libraries()`
reference.
