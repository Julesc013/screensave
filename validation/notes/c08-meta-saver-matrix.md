# C08 Meta-Saver Matrix

This note records the first real suite meta-saver baseline created in `C08`.

## Product Identity

- Canonical meta-saver slug: `anthology`
- Product kind: standalone `.scr` saver
- Scope: bounded in-process cross-saver randomization across the other eighteen canonical saver products
- Explicit non-goals in `C08`: the `suite` app, packaging refresh, installer refresh, external `.scr` orchestration, plugin loading, or a broad content browser

## Selection And Orchestration Rules

- `anthology` never selects itself.
- One inner saver is active at a time.
- Selection modes are:
  - `random_uniform`
  - `weighted_random`
  - `favorites_first`
- Candidate filtering applies, in order:
  - explicit self-exclusion
  - include mask
  - family mask
  - favorites-only gating when that mode is active
  - renderer-aware filtering when explicit-tier policy is enabled
- If no candidate survives strict favorites or strict renderer filtering, `anthology` falls back explicitly and records that in diagnostics text.
- If a chosen inner saver cannot start, `anthology` rejects it for the current switch attempt and falls back to another candidate or a bounded black-frame idle path.
- Transition behavior in `C08` is intentionally simple: clean teardown, clean init, and no broad transition framework.

## Eligible Inner Saver Set

| Saver | Family | Eligible | Notes |
| --- | --- | --- | --- |
| `nocturne` | quiet | yes | Calm default weight. |
| `ricochet` | motion | yes | Motion-family participant. |
| `deepfield` | motion | yes | Motion-family participant. |
| `plasma` | vector | yes | Representative framebuffer/vector participant. |
| `phosphor` | vector | yes | Vector-tier participant. |
| `pipeworks` | grid | yes | Grid/simulation participant. |
| `lifeforms` | grid | yes | Grid/simulation participant. |
| `signals` | ambient | yes | Systems/ambient participant. |
| `mechanize` | ambient | yes | Systems/ambient participant. |
| `ecosystems` | ambient | yes | Systems/ambient participant. |
| `stormglass` | places | yes | Places/atmosphere participant. |
| `transit` | places | yes | Places/atmosphere participant. |
| `observatory` | places | yes | Places/atmosphere participant. |
| `vector` | worlds | yes | Heavyweight worlds participant. |
| `explorer` | worlds | yes | Heavyweight worlds participant. |
| `city` | worlds | yes | Heavyweight worlds participant. |
| `atlas` | worlds | yes | Heavyweight worlds participant. |
| `gallery` | worlds + showcase | yes | Showcase participant and weighted high in the showcase preset. |

## Renderer-Tier Behavior

- `anthology` itself stays baseline-safe on `gdi`.
- Inner-saver eligibility is filtered against the active renderer tier when explicit-tier policy requires it.
- `gl33` and `gl46` are treated as `gl21`-class capability for current anthology filtering because the saver capability contract still tops out at `gl21`.
- If strict tier filtering would strand the session, `anthology` falls back explicitly instead of failing silently.
- No inner saver becomes GL-dependent because of `anthology`.

## Host And BenchLab Status

- Win32 host status: `anthology.scr` is a normal saver product with the standard saver configuration path.
- BenchLab status: the harness can select `anthology`, and the overlay surfaces current inner saver, previous saver, selection reason, family summary, candidate count, and renderer context.
- Build status: the checked-in VS2022 and MinGW lanes now include `anthology` as a first-class saver target with explicit inner-saver linkage.

## Evidence Level

- Static validation completed for:
  - product tree and manifest presence
  - explicit self-exclusion
  - weighted/favorites/family filtering code paths
  - renderer-aware filtering and fallback text
  - BenchLab registration and overlay summary wiring
  - build-lane inclusion in VS2022 and MinGW metadata
- Toolchain limitation:
  - supported MSVC and MinGW toolchains were not available in this environment, so `C08` evidence is static and source-level rather than a fresh native build/run checkpoint

## Known Limits

- `anthology` uses bounded in-process teardown/restart switching, not a broader transition or scheduling engine.
- Packaging artifacts under `out/portable/` and `out/installer/` predate `anthology` and were intentionally not regenerated in `C08`.
- The future `suite` app remains the next step for broader browse, launch, and management UX.

## Next Step

- `C09` `suite` app
