# C09 Suite App Matrix

This note records the first real `suite` app baseline created in `C09`.

## Scope

- Real app product: `products/apps/suite/`
- Role: suite-level browser, launcher, preview, and configuration surface
- Explicit non-goals in `C09`: replacing standalone `.scr` products, replacing `anthology`, replacing BenchLab, refreshing packaging or installer payloads, marketplace behavior, network fetch, or SDK/plugin work

## Product Identity

| Field | Value |
| --- | --- |
| Product key | `suite` |
| Product kind | `app` |
| Default saver | `nocturne` |
| Build targets | `suite.vcxproj`, `suite.exe` in the MinGW i686 lane |
| Discovery model | Manifest-driven catalog plus linked saver-module getters |
| Config model | Shared settings surface plus saver-owned `Settings...` handoff |

## Saver Coverage

| Saver | Listed In Browser | Linked Module | Local `.scr` Artifact Seen In Current Output Roots | Notes |
| --- | --- | --- | --- | --- |
| `nocturne` | yes | yes | yes | Canonical saver with local output currently present |
| `ricochet` | yes | yes | yes | Canonical saver with local output currently present |
| `deepfield` | yes | yes | yes | Canonical saver with local output currently present |
| `plasma` | yes | yes | no | Browser reports module-only status honestly |
| `phosphor` | yes | yes | no | Browser reports module-only status honestly |
| `pipeworks` | yes | yes | yes | Canonical saver with local output currently present |
| `lifeforms` | yes | yes | yes | Canonical saver with local output currently present |
| `signals` | yes | yes | no | Browser reports module-only status honestly |
| `mechanize` | yes | yes | no | Browser reports module-only status honestly |
| `ecosystems` | yes | yes | no | Browser reports module-only status honestly |
| `stormglass` | yes | yes | no | Browser reports module-only status honestly |
| `transit` | yes | yes | no | Browser reports module-only status honestly |
| `observatory` | yes | yes | no | Browser reports module-only status honestly |
| `vector` | yes | yes | no | Browser reports module-only status honestly |
| `explorer` | yes | yes | no | Browser reports module-only status honestly |
| `city` | yes | yes | no | Browser reports module-only status honestly |
| `atlas` | yes | yes | no | Browser reports module-only status honestly |
| `gallery` | yes | yes | no | Browser reports module-only status honestly |
| `anthology` | yes | yes | no | Meta-saver is surfaced beside the standalone line without recursive self-discovery |

## UI And Launch Surface

| Surface | Status | Notes |
| --- | --- | --- |
| Browser list | real | Lists the full canonical saver line plus `anthology` |
| Metadata pane | real | Shows product identity, family/role, renderer support, settings availability, pack summary, and artifact path/status |
| Embedded preview | real | Runs one selected saver module at a time through the shared saver/module contract |
| Windowed run | real | Opens a detached bounded run window for the selected saver |
| Saver `Settings...` handoff | real | Routes to the selected saver's existing config surface |
| Preset/theme visibility | real | Displays current saver preset and theme lists through shared config state |
| Randomization visibility | real | Surfaces saver-local randomization mode and toggle state where available |
| Pack visibility | real | Reports built-in pack manifests discovered for the selected saver |

## Role Boundaries

- Standalone savers remain first-class `.scr` products.
- `anthology` remains the suite meta-saver for cross-saver randomization inside one `.scr` product.
- BenchLab remains the diagnostics harness and still exposes deeper renderer and runtime inspection.
- `suite` stays bounded to browsing, previewing, launching, and saver-settings handoff.

## Current Limits

- Evidence for `C09` is static only in this environment; native MSVC and MinGW toolchains were unavailable for a fresh suite build/run checkpoint.
- Local `.scr` artifact visibility is intentionally partial and reflects the real current output roots plus the staged `C06` and `C07` payloads.
- Portable and installer artifacts under `out/portable/` and `out/installer/` predate both `anthology` and `suite`; `C09` does not refresh those packaging outputs.

## Next Continuation Step

- `C10` SDK / contributor surface
