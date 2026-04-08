# PL00 Substrate Handoff

This document is the operational handoff from the closed `SX` series to `PL00`.
It tells `plasma` and later flagship work what the substrate now guarantees, what remains optional, and what must not be assumed.

## What PL Starts From

`SX09` closes with a real public ladder and a real private substrate:

- `gdi` is the guaranteed universal floor
- `gl11` is the conservative compat lane
- `gl21` is the first real advanced lane
- `gl33` is the first real modern lane
- `gl46` is the real premium lane
- `null` remains internal only

The public renderer contract is still the narrow one under [`platform/include/screensave/renderer_api.h`](../../platform/include/screensave/renderer_api.h).
Private routing, backend capability bundles, present-path state, and future service seams remain internal.

## Current Guarantees By Band

| Band | Public tier | PL may rely on | Current bounded proof |
| --- | --- | --- | --- |
| `universal` | `gdi` | software floor, explicit fallback landing, conservative present path, routing visibility | `gallery` forced `gdi` report in [validation/notes/sx09-tier-proof-matrix.md](../../validation/notes/sx09-tier-proof-matrix.md) |
| `compat` | `gl11` | parity-first acceleration above the GDI floor, conservative context ownership, clean fallback to `gdi` | `gallery` forced `gl11`, plus `nocturne` and `plasma` auto reports |
| `advanced` | `gl21` | first real advanced lane with explicit private capability bundle and degradation to `gl11` and `gdi` | `gallery` forced `gl21` |
| `modern` | `gl33` | optional modern lane with explicit capability modeling and bounded proof-surface use | `gallery` auto and forced `gl33` |
| `premium` | `gl46` | optional premium lane with explicit capability modeling and bounded proof-surface use | `gallery` forced `gl46` |

## What PL Should Target

- Keep `plasma` truthful on the universal and compat bands first. A stronger product is not allowed to erase the compatibility floor that the public doctrine still promises.
- Treat `gl21` as the first safe place for genuinely advanced behavior that still has a disciplined degrade path.
- Treat `gl33` and `gl46` as optional enhancement lanes, not as the new center of gravity for the whole suite.
- Use saver routing metadata and the centralized routing policy rather than open-coding backend trivia inside product logic.
- Use BenchLab and the SX09 captures as substrate evidence, not as a substitute for product-owned PL validation.

## What PL Must Not Assume

- Do not assume `gl33` or `gl46` availability.
- Do not assume a future non-GL family exists just because private seams now exist.
- Do not branch on private backend kinds or private seam bindings from product code.
- Do not assume `gallery` proof equals universal proof for `plasma` or any later flagship work.
- Do not treat BenchLab report mode as a product feature or as part of the public saver contract.
- Do not reopen renderer architecture during early `PL` unless evidence shows a real substrate contradiction.

## What Remains Bounded

- Live capability-denial fallback for unavailable `gl46`, `gl33`, or `gl21` was not freshly captured on the current hardware because the validation machine supported the exercised ladder.
- The SX09 evidence is bounded BenchLab windowed-harness proof plus shared host diagnostics wiring, not a fresh exhaustive fullscreen `.scr` sweep across every saver and every tier.
- Future image, text, present-policy, and non-GL family seams remain private preparation, not adopted runtime families.

## Evidence And Starting Points

- Substrate validation summary: [../../validation/notes/sx09-substrate-validation-summary.md](../../validation/notes/sx09-substrate-validation-summary.md)
- Tier proof matrix: [../../validation/notes/sx09-tier-proof-matrix.md](../../validation/notes/sx09-tier-proof-matrix.md)
- SX09 captures: [../../validation/captures/sx09/README.md](../../validation/captures/sx09/README.md)
- Routing doctrine: [../../specs/routing_policy.md](../../specs/routing_policy.md)
- Backend and seam doctrine: [../../specs/backend_policy.md](../../specs/backend_policy.md)
- SX constitution: [sx-series.md](./sx-series.md)

## PL00 Starting Advice

Start `PL00` by deciding, explicitly and product-locally:

- the minimum honest `plasma` band
- whether `plasma` needs a real `gl21` path before any modern or premium work
- which `plasma` behaviors remain identical across the ladder
- which behaviors may scale upward only when routing selects `advanced`, `modern`, or `premium`

That keeps `PL` focused on product decisions while preserving the now-finished SX substrate contract.
