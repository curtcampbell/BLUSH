# Traits<T> role conflict (Beta_Comp / Charlie_Comp) -- resolved

Status: **fixed** (2026-09-14). This document originally recorded the
problem as a planned future refactor; it now records the design that was
actually implemented, for anyone touching `Traits<>`/codegen later.

## The problem (as originally found)

`Traits<T>` (`face-utils/include/FaceTypeTraits.h`) used to be specialized
once per DM type, globally, via `DECLARE_FACE_TYPE_TRAITS` / `_COMPOSITE` /
`_REQRESP` / `_REQRESP_COMPOSITE`. That's fine as long as a DM type is only
ever used one way. It broke when the same DM type played *different roles*
in different UoPs:

- `Beta_Comp` is the REQUESTER (client) side of the `NewStockAgentRequest`
  CLIENT_SERVER connection. Its generated `Beta_CompTypeTraits.h` declared
  `Traits<NewStockAgent_Request>` via `DECLARE_FACE_TYPE_TRAITS_REQRESP`,
  binding `TypedTS` to the COMBINED Extended TypedTS module (FACE TS 3.2
  Appendix E.3.3 -- `Send_Message_Blocking`/`Send_Message_Async`).
- `Charlie_Comp` is the RESPONDER (server) side of the same connection.
  Its generated `Charlie_CompTypeTraits.h` declared
  `Traits<NewStockAgent_Request>` via plain `DECLARE_FACE_TYPE_TRAITS`,
  binding `TypedTS` to the plain Standard TypedTS module (Appendix E.3.2 --
  servers never use the Extended interface at all).

Same DM type, two mutually incompatible specializations. Linking both UoPs
into one binary was a hard compile error (two conflicting `template<>
struct Traits<NewStockAgent_Request>` bodies). `sample-project`'s
`sample_integration` executable was the first thing in this codebase to
attempt linking a requester UoP and a responder UoP for the same connection
into one process, which is what surfaced this.

A second, related but distinct problem existed alongside it: DM types
shared by two UoPs *without* a role conflict (e.g. `Receipt`, used by
`CustomerEngagement`, `PointOfSaleTerminal`, and `ReplenishmentService` --
all plain pub/sub, no CLIENT_SERVER involved) got the *same, identical*
`DECLARE_FACE_TYPE_TRAITS` invocation redundantly, once per UoP. Textually
identical, but still a hard redefinition error if two such UoPs' generated
headers ever landed in the same translation unit, plus a duplicate-symbol
link error from the old out-of-line `Traits<T>::Name` definition.

## The fix: two independent, composed changes

### 1. `Traits<T, Role>` -- a role tag, defaulted so nothing else changes

```cpp
struct StandardRole {};
struct RequesterRole {};

template <typename T, typename Role = StandardRole>
struct Traits {};
```

Only ONE role is ever "special": a RESPONDER-role type's request and
response are both ordinary Standard TypedTS types (Appendix E.3.2), exactly
like a plain pub/sub type -- so `StandardRole` (the default) covers both
plain pub/sub *and* RESPONDER-role types with a single specialization.
Only a REQUESTER-role connection's request type needs something different,
so it's the only case that gets an explicit non-default tag:
`DECLARE_FACE_TYPE_TRAITS_REQRESP(_COMPOSITE)` now specializes
`Traits<RequestType, RequesterRole>` instead of the bare `Traits<RequestType>`.

Because `Traits<T>` (bare) and `Traits<T, RequesterRole>` are two different
template instantiations, not competing definitions of the same one, a
REQUESTER UoP and a RESPONDER UoP for the same connection's request type
now coexist in one binary with zero conflict.

**Blast radius was small** because almost everything already used the bare
form and stays untouched:

| File | Change |
|---|---|
| `face-utils/include/FaceTypeTraits.h` | Primary template gets the `Role` param + default; `DECLARE_FACE_TYPE_TRAITS_REQRESP`/`_COMPOSITE` specialize the tagged form. Plain `DECLARE_FACE_TYPE_TRAITS`/`_COMPOSITE` untouched. |
| `face-utils/include/RequesterConnection.h` | 2 default template args: `Traits<RequestType>::...` → `Traits<RequestType, RequesterRole>::...` |
| `face-utils/include/ConnectionResolver.h` | 1 line, inside `CreateReqConnection` only |
| `face-utils/include/ResponderConnection.h`, `PublisherConnection.h`, `SubscriberConnection.h`, `FaceInjectable.h` | **Untouched** -- already used the bare form, which is exactly what they still want |
| `face-utils/test/support/FakeFaceModel.h` | The hand-rolled `Traits<Request>` specialization (combined form, for `RequesterConnection_test.cpp`) became `Traits<Request, RequesterRole>` |

The `RspRequest`/`RspResponse` duplicate struct pair in `FakeFaceModel.h`
(added earlier specifically to work around the pre-tag conflict) is no
longer strictly necessary -- `Traits<Request, RequesterRole>` and a bare
`Traits<Request>` could now coexist in that same test file -- but it was
left in place; removing it is optional future cleanup, not required for
correctness.

### 2. Hoist the Standard/Composite declarations to data-model

Orthogonal to the role tag, and worth doing regardless: `Traits<T>` (bare)
for a plain/RESPONDER-role type is a property of the *type*, not of any
particular UoP, so it should be declared exactly once, not once per UoP
that happens to touch it.

New template `data-model/templates/ModelNamespaceTraits.hpp.vm`
(`for_each: MODEL_NAMESPACE`, same scope as the existing
`ModelNamespace.hpp.vm`) generates `FACE/DM/<ns>/<ns>_Traits.hpp`, declaring
bare `Traits<T>` for every type in that namespace that actually has a
Standard TypedTS module. (Not every DM type does -- a struct that's only
ever used as a nested field, never a connection's own message type, has no
TypedTS module at all. The template filters `$namespaceTypes` down to types
referenced by a plain connection or by either side of a RESPONDER-role
CLIENT_SERVER connection, computed across **all** UoPs in the model via the
`$uops` base-context variable -- see the template's own comments for why a
REQUESTER-only request type is deliberately excluded from this filter.)

`ModelNamespace.hpp.vm` (the existing DM-only convenience header) now
`#include`s the new `<ns>_Traits.hpp` at the bottom, so any caller who
already includes `FACE/DM/<ns>/<ns>.hpp` gets `Traits<>` for free. This was
a **deliberate layering choice**, not a technical requirement: it means the
DM convenience header is no longer transport-free (it transitively pulls in
`FACE::TSS` headers now). If that trade-off ever needs revisiting, the two
headers can be decoupled again -- `<ns>_Traits.hpp` doesn't have to be
included from `<ns>.hpp`, callers could include it explicitly instead.

`uop-generator/templates/UoPTypeTraits.h.vm` shrank accordingly: it no
longer declares anything for plain/RESPONDER-role types at all (they come
from data-model transitively, via the same namespace `#include` it already
needed for DM-type visibility) and only handles the `$reqRespPairs`
(REQUESTER-role) case. The `FACE_TRAITS_DECLARED_*`/`FACE_TRAITS_REQRESP_DECLARED_*`
include guards this file used to need are gone entirely -- with exactly one
declaration site per (type, role) pair, there's nothing left to guard
against:

- Plain/RESPONDER-role: declared once, in data-model, never duplicated.
- REQUESTER-role: a connection has exactly one requester in a well-formed
  model, so only one UoP ever declares `Traits<T, RequesterRole>` for a
  given `T`, and it can never collide with data-model's bare `Traits<T>`
  for the same type since they're different specializations.

## Verification

Full clean regeneration (`rm -rf` all generated dirs), full rebuild, full
`ctest` (98/98 passing), and `sample-project`'s `sample_integration`
executable rebuilt linking **all seven** UoPs -- including both
`Beta_Comp` and `Charlie_Comp` together for the first time. See
`sample-project/README.md`, "Why Charlie_Comp is back in the integration
binary", for the user-facing writeup.

## Related, separate future idea (not part of this fix)

Curt floated building a project for a **default/reference transport
service (TSS) implementation** at some point after this. Right now every
UoP in `sample-project` runs with nothing injected (`Framework_Connect()`
always reports `NOT_AVAILABLE`, as documented in `sample-project/
README.md`'s "What this demo does and doesn't show"). A default TSS would
be what actually exercises the injected-`TypedTS*`/`FACE::Configuration`
path end-to-end instead of just documenting the graceful-degradation
behavior. No design work done on this yet.
