# uop-generator

`uop-generator` is not a library like `face-core` or `face-utils`. It ships:

- **`templates/`** — Velocity (`.vm`) templates and a `codegen.yaml` manifest, fed to the
  [FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser) `face-codegen` tool to turn a
  `.face` architecture model into UoP (Unit of Portability) source code.
- **`cmake/BLUSHUopCodegen.cmake`** (at the BLUSH repo root) — a reusable CMake function,
  `blush_add_uop_libraries()`, that runs the full generation pipeline and turns the result into
  one library target per UoP.

A FACE-aligned application uses these to turn its own `.face` model into a set of `<UoP>_lib`
targets it links into its executable(s) — one library per UoP defined in the model.

## Prerequisites

`face-idl-gen` and `face-codegen` (from FACE-IDL-Parser) must be on `PATH`. Both are required;
`blush_add_uop_libraries()` fails with a clear error at configure time if either is missing.

## Using this from your own FACE application

After `find_package(BLUSH)`, call `blush_add_uop_libraries()` with your own `.face` file:

```cmake
find_package(BLUSH REQUIRED)

blush_add_uop_libraries(
    FACE_FILE      myapp.face   # your .face XMI model
    OUT_LIBRARIES  MY_UOP_LIBS  # receives the generated <UoP>_lib target names
)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ${MY_UOP_LIBS})
```

`find_package(BLUSH)` sets `BLUSH_UOP_TEMPLATE_DIR` to BLUSH's installed templates, which is what
`blush_add_uop_libraries()` uses by default. Pass `TEMPLATE_DIR` explicitly only if you need your
own template set instead.

### Function reference

```cmake
blush_add_uop_libraries(
    FACE_FILE      <path>        # required: your .face XMI model
    [TEMPLATE_DIR  <dir>]        # default: BLUSH_UOP_TEMPLATE_DIR (set by find_package(BLUSH))
    [OUTPUT_DIR    <dir>]        # default: <binary-dir>/uop-generated/<face-file-name>
    [EXCLUDE_FROM_ALL]           # generated <UoP>_lib targets are excluded from the default build
    [OUT_LIBRARIES <var>]        # receives the list of generated <UoP>_lib target names
)
```

Each generated `<UoP>_lib` links `PUBLIC` against a shared `face_data_model` target (defined once
per directory scope: `FACE::Core` + `FACE::Util` + the generated data-model headers), so you don't
need to wire that up yourself.

## Running the bundled example

`CMakeLists.txt` here calls `blush_add_uop_libraries()` against `examples/GROCERY_with_IM.face` as
a worked example / smoke test of the templates, producing one `<UoP>_lib` target per UoP in that
model (`Alpha_Comp`, `Beta_Comp`, `Charlie_Comp`, `CustomerEngagement`, `Delta_Comp`,
`PointOfSaleTerminal`, `ReplenishmentService`). Build one directly to exercise it:

```sh
cmake -S . -B build
cmake --build build --target CustomerEngagement_lib
```

The example is added with `EXCLUDE_FROM_ALL`, so it does not build as part of a plain
`cmake --build build` — see **Known limitation** below.

## How it works

```
your.face  ──►  face-idl-gen generate-tss-idl --cpp  ──►  IDL + C++ data-model bindings
                                                                     │
                templates/*.vm + codegen.yaml  ──────►  face-codegen generate
                                                                     │
                                                                     ▼
                              <output>/<UoPName>/            (one per UoP, each with
                                  include/, src/,              its own generated
                                  CMakeLists.txt                CMakeLists.txt)
                              <output>/face-model/include/   (shared data-model headers)
```

`face-codegen` itself writes a `CMakeLists.txt` into every generated UoP directory (see
`templates/UoPCMakeLists.txt.vm`), so `blush_add_uop_libraries()` never needs to parse the `.face`
file or know UoP names ahead of time: it runs the two tools, then globs the output for directories
containing a `CMakeLists.txt` and `add_subdirectory()`s each one.

### Rebuild minimization

Regenerating a UoP tree involves two JVM-based tool invocations, so the function is careful about
when it actually re-runs them:

- The `.face` file and every file under `TEMPLATE_DIR` are registered as
  `CMAKE_CONFIGURE_DEPENDS`, so a plain `cmake --build` only reconfigures when one of them changes
  — not on every build.
- Even on a reconfigure, an MD5 stamp of those same inputs skips the tool invocations entirely if
  neither the model nor the templates actually changed since the last successful run.
- Generated files are synced into the real output tree copy-if-different (`face-codegen` itself
  rewrites every file's mtime on every run regardless of content), so a change to one UoP's
  connections doesn't force unrelated UoPs to recompile.

## Known limitation

The installed `face-idl-gen`'s C++ data-model language binding generator has at least one bug
unrelated to BLUSH — some generated headers `#include` a sibling type with no relative path (e.g.
`#include <CartLine.hpp>`), so the bundled GROCERY example does not fully compile yet. The CMake
wiring itself is verified correct up to that point (building `CustomerEngagement_lib` fails exactly
at that missing header, nothing earlier). This is why the example above is `EXCLUDE_FROM_ALL`; it
will build cleanly once that's fixed upstream in
[FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser).

## Modifying the templates

Each `.vm` file's header comment documents which `codegen.yaml` entry drives it and what output
path it produces. One Velocity gotcha to know before editing: `\#` only strips the backslash when
the following word is one of Velocity's *own* directive names (`#include`, `#define` are real VTL
directives; C/C++ tokens like `#ifndef`, `#endif`, `#pragma` are not). For those, write a bare
`#ifndef`/`#endif`/`#pragma` with no backslash — Velocity passes an unrecognized `#word` through
literally already, substituting any embedded `${var}` correctly, and a leading `\` in front of it
leaks into the generated file instead of being stripped.
