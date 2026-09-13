# data-model

`data-model` is not a library like `face-core` or `face-utils` either — like `uop-generator`, its
premise is templates + a CMake function around the same
[FACE-IDL-Parser](https://github.com/curtcampbell/FACE-IDL-Parser) pipeline. Where
`uop-generator` turns a `.face` model into one library per UoP, `data-model` turns it into the
**one shared library** every UoP (and any hand-written FACE component, e.g. a transport service
implementation) includes: the `FACE::DM` data-model types and `FACE::TSS` transport interfaces the
model defines.

## Prerequisites

`face-idl-gen` and `face-codegen` (from FACE-IDL-Parser) must be on `PATH`.

## Using this from your own FACE project

After `find_package(BLUSH)`:

```cmake
find_package(BLUSH REQUIRED)

blush_add_data_model_library(FACE_FILE myapp.face)

target_link_libraries(your_target PRIVATE face_data_model)
```

You won't usually call this directly if you're also using `uop-generator` —
`blush_add_uop_libraries()` calls it for you (see below) — but it's here directly for components
that need the data model without any UoPs, e.g. a transport service implementation.

### Config file, instead of more CMake arguments

Rather than growing more CMake keyword arguments as generation options are added (per-language
toggles, per-namespace overrides, ...), `blush_add_data_model_library()` optionally takes a small
declarative config file:

```yaml
# myapp.yaml
face_file: myapp.face
```

```cmake
blush_add_data_model_library(CONFIG myapp.yaml)
```

This is deliberate, not just an alternative spelling of `FACE_FILE`: a config file means the exact
same "what to generate" description could, in principle, be read identically by a future
non-CMake build-system integration (a Gradle plugin, a Cargo build script, ...) for other-language
FACE bindings, whereas a CMake function call obviously cannot be. **CMake treats the file's
contents as close to opaque as practical** — `_blush_read_data_model_config()` in
`cmake/BLUSHDataModelCodegen.cmake` only understands a flat `key: value` subset (no nesting, no
lists), just enough for today's single `face_file:` key. It is deliberately not a general YAML
parser. Once real per-language or per-namespace options are needed, that's the point to either
grow this reader properly or — better, so every future build-system wrapper benefits from one
implementation instead of each reimplementing config parsing — push interpretation of the config
into the FACE-IDL-Parser tooling itself, the same way `face_file:` already just becomes a
`--face-file` argument to that tooling today.

A relative `face_file:` path resolves relative to the config file's own directory, not the calling
`CMakeLists.txt` — again so a hypothetical non-CMake reader would resolve it the same way.

### Function reference

```cmake
blush_add_data_model_library(
    FACE_FILE     <path>   |   CONFIG <path>   # exactly one of these
    [TEMPLATE_DIR <dir>]        # default: BLUSH_DATA_MODEL_TEMPLATE_DIR
    [OUTPUT_DIR   <dir>]        # default: <binary-dir>/data-model-generated/<face-file-name>
    [OUT_LIBRARY  <var>]        # receives the generated library's target name (today: face_data_model)
)
```

Idempotent: calling it more than once for the same model (e.g. once directly and once indirectly
via `blush_add_uop_libraries()`) is a no-op after the first call — it checks `if(NOT TARGET
face_data_model)` before doing anything.

## Running the bundled example

```sh
cmake -S . -B build
cmake --build build --target face_data_model
```

`CMakeLists.txt` here calls `blush_add_data_model_library()` against `examples/GROCERY.yaml` (which
just points at `uop-generator/examples/GROCERY_with_IM.face`, rather than duplicating that file) as
a worked example of the `CONFIG` path specifically.

## How it works

```
your.face  ──►  face-idl-gen generate-tss-idl --cpp  ──►  IDL + per-type FACE::DM / FACE::TSS headers
                                                                     │
                templates/*.vm + codegen.yaml  ──────►  face-codegen generate
                                                                     │
                                                                     ▼
                              <output>/face-model/include/FACE/DM/<namespace>/<namespace>.hpp
                              (one aggregate convenience header per model namespace, re-exporting
                               the per-type headers face-idl-gen already generated)
```

Unlike `uop-generator`, the set of things to generate here doesn't depend on the model's content
(it's always exactly one `face_data_model` INTERFACE library, regardless of how many namespaces or
UoPs the `.face` file defines), so this function defines the CMake target directly rather than
relying on a generated `CMakeLists.txt` the way `uop-generator` does.

Rebuild minimization (CMAKE_CONFIGURE_DEPENDS + MD5 stamp + copy-if-different sync) works the same
way as `uop-generator` and is implemented once, shared by both, in
`cmake/BLUSHFaceCodegenPipeline.cmake`.

## Known limitation

Shares `uop-generator`'s known limitation: the installed `face-idl-gen`'s C++ data-model language
binding generator has at least one bug unrelated to BLUSH (some generated headers `#include` a
sibling type with no relative path). See
[`uop-generator/README.md`](../uop-generator/README.md#known-limitation) for details — it affects
`data-model`'s output too, since both pull from the same `face-idl-gen generate-tss-idl --cpp`
step.
