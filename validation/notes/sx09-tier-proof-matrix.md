# SX09 Tier Proof Matrix

This matrix records the bounded proof surfaces exercised during `SX09`.
Status values are used literally:

- `passed`
- `passed with limitations`
- `unverified`

## Tier Ladder

| Band | Public tier | Proof surface | Request | Selected | Status | Evidence | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `universal` | `gdi` | `gallery` | forced `gdi` | `gdi` | `passed` | [benchlab-gallery-gdi.txt](../captures/sx09/benchlab-gallery-gdi.txt) | Hidden deterministic report now presents through an offscreen GDI target and records stable capture detail. |
| `compat` | `gl11` | `gallery` | forced `gl11` | `gl11` | `passed` | [benchlab-gallery-gl11.txt](../captures/sx09/benchlab-gallery-gl11.txt) | Confirms the conservative accelerated tier is active and reported honestly. |
| `advanced` | `gl21` | `gallery` | forced `gl21` | `gl21` | `passed` | [benchlab-gallery-gl21.txt](../captures/sx09/benchlab-gallery-gl21.txt) | Confirms the first real advanced tier is selectable and observable. |
| `modern` | `gl33` | `gallery` | forced `gl33` | `gl33` | `passed` | [benchlab-gallery-gl33.txt](../captures/sx09/benchlab-gallery-gl33.txt) | Confirms the bounded modern lane is real rather than declarative. |
| `premium` | `gl46` | `gallery` | forced `gl46` | `gl46` | `passed` | [benchlab-gallery-gl46.txt](../captures/sx09/benchlab-gallery-gl46.txt) | Confirms the bounded premium lane is real rather than declarative. |

## Policy And Auto-Routing Checks

| Surface | Request | Policy target | Selected | Status | Evidence | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `gallery` | `auto` | `gl33` | `gl33` | `passed` | [benchlab-gallery-auto.txt](../captures/sx09/benchlab-gallery-auto.txt) | Proves that auto-routing follows saver policy rather than silently preferring the highest available tier on a machine that also supports `gl46`. |
| `nocturne` | `auto` | `gl11` | `gl11` | `passed` | [benchlab-nocturne-auto.txt](../captures/sx09/benchlab-nocturne-auto.txt) | Confirms an ordinary saver still routes through the compat lane honestly. |
| `plasma` | `auto` | `gl11` | `gl11` | `passed` | [benchlab-plasma-auto.txt](../captures/sx09/benchlab-plasma-auto.txt) | Confirms `plasma` still begins from the compat lane instead of inheriting showcase-tier assumptions. |
| `nocturne` | forced `gl46` | `gl11` | `gl11` | `passed` | [benchlab-nocturne-gl46.txt](../captures/sx09/benchlab-nocturne-gl46.txt) | Proves requested-versus-policy-target visibility on a saver whose policy caps above-floor requests back to compat. |
| `plasma` | forced `gl46` | `gl11` | `gl11` | `passed` | [benchlab-plasma-gl46.txt](../captures/sx09/benchlab-plasma-gl46.txt) | Proves the same policy-cap visibility on the first future `PL` product track. |

## Bounded And Unverified Areas

| Area | Status | Notes |
| --- | --- | --- |
| Capability-driven fallback caused by unavailable `gl46`, `gl33`, or `gl21` on lower hardware | `unverified` | The current validation machine supports the exercised ladder. The code paths remain wired and diagnosable, but SX09 did not claim a live denial capture it did not obtain. |
| Exhaustive fullscreen `.scr` host proof for every saver and every tier | `unverified` | SX09 is a bounded substrate-validation pass. The exercised evidence is BenchLab windowed-harness proof plus shared host diagnostics wiring, not a fresh exhaustive saver-host sweep. |
| BenchLab as a general end-user launcher or product shell | not applicable | BenchLab remains diagnostics-only. SX09 did not widen it into the real product surface. |
