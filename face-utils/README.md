# face-utils

`face-utils` is a small support library built on top of `face-core`: UoP connection wrappers,
the injection-store base class UoPs derive from, task scheduling, and a couple of general-purpose
concurrency helpers. CMake exposes it as the `FACE::Util` target, which links `FACE::Core`
publicly, so depending on `FACE::Util` alone is enough.

```cmake
find_package(BLUSH REQUIRED)
target_link_libraries(your_target PRIVATE FACE::Util)
```

## What's in it

**UoP injection plumbing**
- `UopBase` — base class for a UoP; implements `FACE::TSS::Base_Injectable::Injectable` and
  stores injected interface pointers by name, retrievable via `GetInjected<T>()`.
- `FaceInjectable<DataType>` — mixin that implements one FACE-generated `Injectable` interface
  for `DataType` on top of `UopBase` (virtual inheritance, so multiple `FaceInjectable<T>` mixins
  on one UoP share a single `UopBase`).
- `FaceTypeTraits.h` — the `Traits<T>` template and the `DECLARE_FACE_TYPE_TRAITS(_COMPOSITE)?`
  macros that generated code (see `uop-generator`) uses to wire a data-model type to its
  `TypedTS`/`Read_Callback`/`InjectableInterface`.

**Connection wrappers** (each templated on `Traits<DataType>` by default, so they work with any
FACE-IDL-generated `TypedTS`/`Read_Callback` pair without modification)
- `PublisherConnection<T>` / `SubscriberConnection<T>` — pub/sub, single data type.
- `RequesterConnection<Req, Resp>` / `ResponderConnection<Req, Resp, ISender>` — CLIENT_SERVER,
  request/response.
- `ConnectionResolver` — bridges a UoP's `uop-connections.xml` runtime config (see
  `include/uop-connections.xml`/`.xsd`) and its `UopBase` injections into constructed connection
  objects; this is what generated UoP `Base` classes call during startup.

**Concurrency / general-purpose**
- `TaskRunner` — thread-pool task queue.
- `TaskScheduler` — one-shot and repeating task scheduling on top of a `TaskRunner`, with
  cancellation via `Cancellable` and overlap control via `OverlapPolicy`.
- `EventDispatcher<Args...>` / `EventRegistration` — a small multicast-delegate implementation used
  internally by the connection wrappers for handler registration/cancellation.
- `ReadWriteLocked<T>` — a value paired with its own reader-writer mutex.
- `IDGenerator<T>` — lock-free monotonically increasing ID generator.

**Vendored**
- `tinyxml2` — third-party XML parser, used internally by `ConnectionResolver` to parse
  `uop-connections.xml`. Not covered by this library's own test suite (see below).

## A note on `FACE::STRING_TYPE`

`FACE::STRING_TYPE` (from `face-core`) is a fixed binding of `FACE/Common.idl` and has no
`operator<`, so it can't be a `std::map`/`std::set` key directly. `UopBase` and
`ConnectionResolver` both need to key/order on names internally; they convert to `std::string` at
the point of storage/lookup rather than adding anything to `FACE::STRING_TYPE` itself, since that
type isn't this project's to change. The public API of both classes still takes
`FACE::STRING_TYPE`, so this is invisible to callers.

## Building and testing

`face-utils` builds as part of the top-level BLUSH CMake project — see the repo root README for
full build instructions. `test/` has a GoogleTest (gtest + gmock) suite, one executable per class
above (tinyxml2 excepted). `PublisherConnection`/`SubscriberConnection`/`RequesterConnection`/
`ResponderConnection`/`ConnectionResolver` are templated on FACE-IDL-generated `TypedTS`/
`Read_Callback`/`Base` types that don't exist in this repo (they're produced by `face-codegen`
against a real application `.face` model); their tests use hand-written gmock fakes in
`test/support/FakeFaceModel.h` that reproduce the exact method shapes those classes call, rather
than a real generated model.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```
