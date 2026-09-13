# face-core

`face-core` is a header-only C++ binding of the **FACE Technical Standard 3.2** interfaces:
Transport Services Segment (TSS), Life-Cycle Management (LCM), and the I/O Services Segment
(IOSS). It's pure interface/data-type code with no build step of its own — CMake exposes it as
the `FACE::Core` target so other libraries and applications can depend on it.

## Using it

```cmake
find_package(BLUSH REQUIRED)   # or add_subdirectory() if vendoring BLUSH directly
target_link_libraries(your_target PRIVATE FACE::Core)
```

```cpp
#include "FACE.hpp"   // pulls in every module below
// or include individual headers, e.g. "FACE/IOSS/Discrete.hpp", to minimize
// compilation dependencies.
```

## What's in it

| Namespace | Contents |
|---|---|
| `FACE` | Common types (`RETURN_CODE_TYPE`, `STRING_TYPE`, time/GUID types) and `Configuration` |
| `FACE::TSS` | Transport Services: `Base` (connection lifecycle), `CSP`, `TPM`, `TypeAbstraction`, `Primitive_Marshalling`, `Serialization`, `Message_Type_Utilities` |
| `FACE::LCM` | Life-cycle interfaces: `Configurable`, `Connectable`, `Initializable` |
| `FACE::IOSS` | I/O services: `Generic`, `Analog`, `Discrete`, `Serial`, `ARINC429`, `ARINC825`, `M1553`, `M1553_Mk2`, `MultiChannelAnalog`, `MultiChannelDiscrete`, `Synchro`, `PrecisionSynchro`, `I2C` |

Every module that has an FACE-defined Injectable interface also has a `<Module>_Injectable.hpp`
(e.g. `FACE::IOSS::Discrete::IO_Service_Injectable`) — the interface the external executive calls
to inject a live connection reference into a UoP at startup. See `face-utils`' `FaceInjectable<T>`
for the mixin that implements one of these on a UoP's behalf.

### Uninstantiated IDL template modules

Three IDL template modules have no instantiation in the FACE 3.2 standard IDL and are represented
here only as C++ templates, for reference:

- `FACE::TSS::Typed<T>` (`TSS/TypedTS.hpp`) — single-type pub/sub transport shape
- `FACE::TSS::Extended<Req, Resp>` (`TSS/Extended.hpp`) — request/response transport shape
- `FACE::LCM::Stateful<Req, Rep>` (`LCM/Stateful.hpp`)

These are **not** what real generated code uses. A conformant FACE UoP instantiates the IDL
template in its own `.face`/`.idl` model and gets a concrete, expanded namespace back — that's
what `uop-generator`'s templates + `face-codegen` produce. Treat the C++ templates here as a
reference for the interface shape only, e.g. when hand-writing a test fake.

## Building and testing

`face-core` builds as part of the top-level BLUSH CMake project — see the repo root README for
full build instructions. `test/` contains compile-only smoke tests (`static_assert`s over the
namespace structure and type shapes, no runtime behavior to exercise since this library has none)
registered with CTest:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build -R "face_smoke_test|minimal_test"
```
