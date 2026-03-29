# Post-S15 Continuation Plan

This document defines the continuation program that begins after the implemented `S15` baseline.

## Current Baseline

The repository already contains a real implemented history through `S15`.
That baseline currently includes:

- one shared platform with the required `gdi` floor, optional `gl11`, real optional `gl21`, explicit `gl33` and `gl46` placeholders, and the internal `null` backend
- eighteen real saver products in the tree under the canonical post-`C01` slugs, with explicit old-to-new migration aliases where recoverability requires them
- one true `.scr` output per saver across the current eighteen-product line, with product-specific entry binding and version/resource identity on the shared host path
- one shared settings architecture with versioned common config, preset/theme import-export, within-saver session randomization, pack manifest discovery/validation helpers, and representative per-saver adoption without collapsing products into one runtime blob
- BenchLab as the real developer diagnostics harness
- the canonical `products/apps/suite/` placeholder for the future suite app plus a superseded `products/apps/player/` legacy stub
- the locked `C02` post-rename checkpoint, including the migration-baseline note and recovery tag for the canonical naming regime

This document supersedes the older public assumption that the remaining work fit inside `S16` and `S17`.
That endpoint is retired for future planning.

## Continuation Decisions

1. `C00` is planning-only and performs no runtime, build-target, packaging, or product implementation work.
2. `C01` completed the rename and taxonomy normalization pass and now defines the canonical naming baseline.
3. `C02` established and locked the explicit post-rename checkpoint before wider continuation work proceeds.
4. `C03` completed all-saver `.scr` productization before suite/meta products, idea ingestion, or polish waves.
5. `C04` completed the shared settings, presets, randomization, and pack architecture baseline before distribution work.
6. `C05` completed Windows picker, config, preview, and fullscreen validation before release/distribution work is treated as credible.
7. `C06` portable distribution happens next and before `C07` installer, registration, and uninstall work.
8. `C08` suite-level meta saver and `C09` `suite` app happen after saver productization and Windows-path validation.
9. `C10` SDK and contributor surface stabilization happens before backlog ingestion.
10. `C11` idea backlog ingestion happens before the major polish waves.
11. `C12` cross-cutting polish precedes `C13` per-family polish waves.
12. `C14` final rerelease hardening closes the continuation line.

## Renderer Tier Taxonomy Note

Use the following canonical ladder in roadmap, architecture, changelog, and future implementation planning:

| Tier | Status | Meaning | Planning Rule |
| --- | --- | --- | --- |
| `gdi` | current | Guaranteed renderer floor for the universal saver set | Always present and always preserved |
| `gl11` | current | Conservative universal accelerated tier | Optional, capability-gated, and subordinate to `gdi` |
| `gl21` | current | First real advanced tier | Optional, capability-gated, and the current real later-capability backend |
| `gl33` | future | Later advanced tier placeholder | Do not describe as real until a backend exists beyond placeholder state |
| `gl46` | future | Later advanced tier placeholder | Do not describe as real until a backend exists beyond placeholder state |
| `null` | internal-only | Emergency no-op backend for host stability | Never describe as a normal product capability or release tier |

Interpretation rules:

- `gdi` is the guaranteed floor.
- `gl11` is the conservative universal accelerated tier.
- `gl21` is the first real advanced tier.
- `gl33` and `gl46` remain future tiers only when real.
- `null` remains internal and emergency-only.

## Execution Program

| Phase | Goal | Exit Condition |
| --- | --- | --- |
| `C00` | Reset the roadmap and reconcile naming/taxonomy planning | Roadmap, rename map, taxonomy note, and changelog all agree on the post-`S15` program |
| `C01` | Normalize saver/app naming and renderer terminology | Legacy saver slugs, placeholder app names, and tier wording are reconciled in code and docs |
| `C02` | Freeze a stable renamed baseline | Repo builds, docs, manifests, and validation notes reflect the renamed baseline without ambiguity |
| `C03` | Make every current saver a truthful standalone `.scr` product | Every implemented saver has an honest productized `.scr` path and product metadata |
| `C04` | Add shared settings/preset/randomization/pack architecture | Shared reuse exists where justified without collapsing products into one runtime blob |
| `C05` | Validate the real Windows screensaver lifecycle | Picker, config, preview, fullscreen, and fallback behavior are verified on the actual host path |
| `C06` | Produce a portable suite bundle | Portable distribution structure and metadata are defined and validated honestly |
| `C07` | Add installer and uninstall flow | Registration, install, and uninstall behavior is explicit and reviewable |
| `C08` | Introduce the suite-level meta saver | Meta saver exists without replacing separate saver products |
| `C09` | Introduce the `suite` control app | One canonical suite app replaces the legacy `gallery`/`player` placeholder plan |
| `C10` | Stabilize SDK and contributor materials | Templates, examples, and docs align with the renamed, productized surface |
| `C11` | Ingest deferred idea backlog | New saver ideas are admitted onto the stable normalized surface instead of the pre-normalization tree |
| `C12` | Run cross-cutting polish | Shared UX, metadata, validation, and distribution polish is reconciled repo-wide |
| `C13` | Run per-family polish waves | Family-specific polish happens after the shared surface is stable |
| `C14` | Harden for rerelease | Final compatibility, validation, and release-readiness checks are complete |

## Immediate Next Step

The rename, renderer-taxonomy, checkpoint, saver productization, shared settings, and Windows integration work from `C01` through `C05` is complete.
The next continuation prompt should be `C06` for the portable distribution bundle.
Further continuation work should not need to renegotiate saver/app ownership, migration aliases, renderer-tier terminology, per-saver `.scr` product identity, the shared settings/preset/theme/pack contract, or the bounded Win32 `.scr` lifecycle behavior validated in `C05`.
