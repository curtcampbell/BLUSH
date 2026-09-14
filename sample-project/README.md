# sample-project

A worked example showing how to consume BLUSH's generated UoP libraries and
face-utils components from application code. It builds one static library
per Unit of Portability (UoP) defined in
[`face-models/GROCERY_with_IM.face`](face-models/GROCERY_with_IM.face),
each implementing that UoP by deriving from its generated `<UoP>Base`
class, plus a single `integration` executable that links all seven,
constructs one instance of each, drives it through the FACE lifecycle, and
exits.

**No real TSS is injected anywhere in this example.** That's deliberate --
see [What this demo does and doesn't show](#what-this-demo-does-and-doesnt-show)
below.

## Layout

```
sample-project/
  CMakeLists.txt          # runs codegen, add_subdirectory()s everything below
  face-models/
    GROCERY_with_IM.face  # the .face model this whole tree is generated from
  common/                 # DemoUtil.h -- console-logging helpers, demo-only
  Alpha_Comp/             # one directory per UoP:
    CMakeLists.txt        #   <UoP>_impl static library
    include/<UoP>Impl.h   #   class <UoP>::<UoP>Impl : public <UoP>::<UoP>Base
    src/<UoP>Impl.cpp
  Beta_Comp/
  Charlie_Comp/
  CustomerEngagement/
  Delta_Comp/
  PointOfSaleTerminal/
  ReplenishmentService/
  integration/
    CMakeLists.txt        # sample_integration executable
    src/main.cpp          # constructs + runs all seven UoPs, then exits
```

The `integration` executable links six of the seven UoP impl libraries --
Charlie_Comp is excluded from it; see
[Why Charlie_Comp isn't in the integration binary](#why-charlie_comp-isnt-in-the-integration-binary)
below. `Charlie_Comp_impl` still builds normally on its own.

Each `<UoP>Impl` class adds no new lifecycle behavior beyond what the
generated `<UoP>Base` already implements -- it exists purely to demonstrate
calling that UoP's generated connection-table API
(`<UoP>*ConnectionTable::send___()` / `register___Handler()`) from within
`Initialize()`. Which specific connections each impl calls is noted at the
top of its `.cpp` file; UoPs with more than one Integration Context (and
therefore more than one connection table) call one representative method
per table rather than every connection -- see the generated
`ConnectionTable/<UoP><IntegrationContext>ConnectionTable.h` headers for
each table's full method set.

## Building and running

From the BLUSH repo root:

```sh
cmake -B build -DBLUSH_BUILD_SAMPLE_PROJECT=ON
cmake --build build --target sample_integration
./build/sample-project/integration/sample_integration
```

`BLUSH_BUILD_SAMPLE_PROJECT` defaults to `ON`; it exists so a downstream
packager building only the libraries can turn the demo off. The sample also
requires `face-idl-gen` and `face-codegen` on `PATH` (the same tools the
rest of the BLUSH build depends on) -- `sample-project/CMakeLists.txt`
skips itself with a `message(STATUS ...)` if they aren't found.

## Why Charlie_Comp isn't in the integration binary

`Beta_Comp` (the REQUESTER/client side) and `Charlie_Comp` (the
RESPONDER/server side) are the two ends of the same CLIENT_SERVER
connection, `NewStockAgentRequest` / `NewStockAgentResponse`. Both touch
the DM type `NewStockAgent_Request`, but each needs a *different*
`Traits<NewStockAgent_Request>` binding:

- Beta_Comp's requester side needs the combined Extended TypedTS module
  (`Send_Message_Blocking` / `Send_Message_Async`), per FACE TS 3.2
  Appendix E.3.3.
- Charlie_Comp's responder side needs the plain Standard TypedTS module
  (`Register_Callback`), per Appendix E.3.2 -- servers never use the
  Extended interface at all.

`Traits<T>` (`face-utils/include/FaceTypeTraits.h`) is keyed globally on
the bare DM type, with no notion of "which role is asking." That's fine as
long as a type's requester-side and responder-side UoPs never end up
linked into the same process -- which matches how FACE deployments
normally look (each UoP in its own partition), and is true for every UoP
pair in this sample **except** Beta_Comp/Charlie_Comp. Linking both into
one binary produces two incompatible specializations of
`Traits<NewStockAgent_Request>`, which is a hard compile error, not
something an include guard can paper over (the two definitions genuinely
disagree, unlike the same-UoP-shared-type case below).

Resolving this properly means decoupling `Traits<T>` from the bare DM type
(e.g. threading an explicit requester/responder tag through
`ConnectionResolver`, `RequesterConnection`, and `ResponderConnection`
instead of deriving everything from `Traits<RequestType>`) -- real,
worthwhile future work, but a face-utils redesign, not a sample-project
task. For now, Charlie_Comp is built as its own standalone library and
left out of the combined `sample_integration` executable. Run it on its
own by writing a one-UoP `main()` against `Charlie_Comp_impl`, following
the same pattern as `integration/src/main.cpp`.

A related but *fixable* problem this sample did fix: several UoPs that
don't have this requester/responder conflict still legitimately share a
plain pub/sub DM type (e.g. `Receipt`, used by `CustomerEngagement`,
`PointOfSaleTerminal`, and `ReplenishmentService`). Each UoP's generated
`<UoP>TypeTraits.h` independently declares `Traits<Receipt>` with
*identical* content, so linking several such UoPs into one binary is safe
in principle -- but naively, `template<> struct Traits<Receipt>` appearing
twice in one translation unit is still a hard redefinition error, and the
out-of-line `Traits<Receipt>::Name` string was a duplicate-symbol error at
link time. `uop-generator/templates/UoPTypeTraits.h.vm` now wraps each
`DECLARE_FACE_TYPE_TRAITS` invocation in an include guard keyed on the DM
type's namespace+name (not the UoP name), and `Name[]`
(`face-utils/include/FaceTypeTraits.h`) is now a C++17 `constexpr` (inline)
variable instead of requiring a separate out-of-line `.cpp` definition --
so identical declarations from different UoPs now coexist correctly. This
sample's `sample_integration` binary is what exposed the gap; it's a real
fix to the generator, not a demo-only workaround.

## How a real (out-of-tree) project would do this

`sample-project/CMakeLists.txt` is, line for line, what a downstream
application repo writes once BLUSH is installed -- the only difference is
that this one runs in-tree, before `make install`, so it must pass
`TEMPLATE_DIR` explicitly instead of getting it from `find_package(BLUSH)`:

```cmake
find_package(BLUSH REQUIRED)

blush_add_uop_libraries(
    FACE_FILE     path/to/YourModel.face
    OUT_LIBRARIES MY_UOP_LIBS       # e.g. MyUoP_lib, OtherUoP_lib, ...
)

add_subdirectory(MyUoP)             # defines MyUoP_impl, linking MyUoP_lib
```

Everything else -- deriving an impl class from the generated `<UoP>Base`,
calling its connection-table methods, linking the result into an
executable -- is identical to what's in this directory.

## What this demo does and doesn't show

Every generated `<UoP>Base::Initialize()` constructs that UoP's connection
table objects, but leaves every connection inside them null -- wiring a
connection to a live TSS is `Framework_Connect()`'s job, and
`Framework_Connect()` in turn needs a `FACE::Configuration` service to have
been injected via `Set_Reference()` before it can read
`uop-connections.xml` and do that wiring (see
`face-utils/include/ConnectionResolver.h` and each generated
`<UoP>Base.cpp`). This demo never injects one, so:

- `Initialize()` always returns `NO_ERROR` -- it only builds local objects.
- Every `send___()` call in each `<UoP>Impl::Initialize()` override returns
  `NOT_AVAILABLE`.
- Every `register___Handler()` call returns a null handle.
- `Framework_Connect()` itself returns `NOT_AVAILABLE` (it catches the
  exception `GetInjected()` throws when nothing was injected).

All four are the correct, documented behavior of a UoP that hasn't been
wired into a real executive yet -- not errors this demo is failing to
handle. `sample_integration`'s console output labels each one accordingly.
Wiring in a real TSS (injecting a `FACE::Configuration` implementation,
providing `uop-connections.xml`, and injecting each connection's
`TypedTS*`) is future work, not something this example attempts.
