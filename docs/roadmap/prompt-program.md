# Prompt Program

This document records the implemented `S00` through `S15` line and the active post-`S15` continuation line.
Earlier docs described an intended Series 00 through 17 program; that shorter endpoint is now superseded by the continuation work recorded here.

Implementation currently exists through `S15`.
The older public endpoint that treated `S16` and `S17` as the whole remaining plan is superseded.
Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, and `C12` work plus later continuation prompts.
C13 is the next planned implementation prompt after the cross-cutting polish baseline.

## Implemented Baseline

| Series | Purpose | Baseline Achieved |
| --- | --- | --- |
| S00 | Constitutional foundation and control plane | Specs, docs, AGENTS hierarchy, `.codex` config, and permanent skeleton |
| S01 | Governance, community-health, and CI foundation | Contribution path, review hygiene, workflows, and lightweight repository validation |
| S02 | Build scaffold and toolchain lane foundation | Concrete VS2022 and MinGW i686 lanes plus honest legacy-lane placeholders |
| S03 | Win32 `.scr` host skeleton and classic lifecycle groundwork | Real screen, preview, and config dispatch on the ANSI Win32 path |
| S04 | Shared core runtime and first public platform API layer | Narrow public headers and reusable non-renderer runtime services |
| S05 | Mandatory GDI backend and shared present path | Real reusable GDI renderer backend and baseline present path |
| S06 | `nocturne` first saver product | First real saver module with product-owned config, presets, and themes |
| S07 | BenchLab and validation instrumentation | Real windowed diagnostics harness with deterministic restart and reseed controls |
| S08 | Optional GL11 backend and runtime selection | Capability-gated GL11 backend, explicit selection, and fallback reporting |
| S09 | Motion Family with `ricochet` and `deepfield` | First true multi-product saver family on the shared platform |
| S10 | Framebuffer and vector family with `plasma` and `phosphor` | Product-local framebuffer and vector savers plus a narrow shared visual-buffer helper |
| S11 | Grid and simulation family with `pipeworks` and `lifeforms` | Product-local grid and state-driven savers plus a narrow shared grid helper |
| S12 | Systems and ambient family with `signals`, `mechanize`, and `ecosystems` | Layered system-interface, kinetic, and ambient savers without broad framework sprawl |
| S13 | Places and atmosphere family | Scenic weather, transit, and celestial savers on the same shared contracts |
| S14 | Heavyweight worlds family | Software-3D, traversal, urban, and fractal savers without redefining the platform |
| S15 | `gl21` backend, explicit higher-tier placeholders, and `gallery` | Real optional GL21 backend, explicit `gl33`/`gl46` placeholders, internal `null`, and the eighteen-saver harness |

## Active Continuation Line

| Series | Purpose | Outputs | Depends On | Must Preserve |
| --- | --- | --- | --- | --- |
| C00 | Roadmap reset and normalization planning | Post-`S15` continuation docs, rename map, renderer taxonomy note, and truthful changelog/status updates only | S00-S15 | No runtime, build-target, packaging, or product implementation changes |
| C01 | Rename and taxonomy normalization | Canonical saver/product/app naming, renderer-tier terminology normalization, and replacement of legacy placeholder names across code, docs, manifests, and build references | C00 | C89, x86 Win32 `.scr`, ANSI host path, one shared platform, BenchLab role, GDI floor |
| C02 | Post-rename baseline checkpoint | A stable renamed baseline with reconciled docs, manifests, identifiers, migration-baseline notes, and verification evidence before wider continuation work | C01 | Recoverable one-pass rename history and truthful repo state |
| C03 | All-saver `.scr` productization | Every current saver packaged as an honest standalone `.scr` product with product-owned manifests and bounded metadata | C02 | Separate saver products, no silent suite coupling, no baseline regression |
| C04 | Shared settings, presets, randomization, and pack architecture | Shared-but-bounded settings/preset/randomizer/pack surface that multiple saver products can reuse without collapsing into a meta-product | C03 | Product ownership, stable public contracts only where reuse is proven |
| C05 | Windows picker, config, preview, and fullscreen validation | Explicit validation of saver selection, config dialogs, preview hosting, fullscreen lifecycle, and fallback behavior on the real Windows screensaver path | C03-C04 | Classic Win32 `.scr` behavior, ANSI host path, truthful validation |
| C06 | Portable distribution bundle | Portable saver bundle layout, manifest assembly, release metadata, and validation notes for manual distribution | C03-C05 | Packaging consumes artifacts and metadata; it does not redefine runtime behavior |
| C07 | Installer, registration, and uninstall flow | Installer/uninstaller, saver registration flow, and conservative shell integration guidance | C06 | Truthful installation claims, no hidden runtime logic in packaging |
| C08 | Suite-level meta saver | `anthology.scr` plus bounded in-process cross-saver selection, weighting, renderer-aware filtering, and transition orchestration without replacing individual saver products | C03-C07 | Individual saver products remain first-class `.scr` artifacts |
| C09 | `suite` app | One canonical suite control application for browse, launch, preview, settings, randomization, and pack management; legacy `gallery`/`player` placeholders are superseded here | C03-C08 | BenchLab remains diagnostics-only; suite app does not redefine the compatibility baseline |
| C10 | SDK and contributor surface | Stable contributor-facing SDK, templates, examples, and docs aligned with the renamed product surface | C03-C09 | Small public platform surface and explicit contracts |
| C11 | Idea backlog ingestion | Structured intake of deferred saver ideas and concept families onto the normalized product surface | C03-C10 | No speculative concept silently changes the platform contract |
| C12 | Cross-cutting polish | Shared quality passes across naming, config UX, metadata, validation notes, and distribution details | C03-C11 | Compatibility-first behavior and recoverable change sets |
| C13 | Per-family polish waves | Focused polish passes by saver family after the shared product and suite surfaces have stabilized | C03-C12 | Product-local ownership, no broad framework creep |
| C14 | Final rerelease hardening | End-to-end release readiness, compatibility sweep, validation reconciliation, and rerelease notes | C03-C13 | Truthful release evidence, preserved baseline, no silent tier inflation |

## Superseded Legacy Endpoint

The older two-step post-`S15` endpoint is retired for future planning:

- Former `S16` packaging work is redistributed across `C06` and `C07`.
- Former `S17` release-hardening and suite-handoff work is redistributed across `C08` through `C14`.

Do not schedule new continuation work under `S16` or `S17`.

## Continuation Rules

- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, and `C12` are complete.
- `C13` is the next implementation prompt after the cross-cutting polish baseline.
- Rename and renderer-taxonomy normalization land before any productization or suite/meta work.
- All-saver `.scr` productization lands before suite/meta products, backlog ingestion, or polish waves.
- Shared settings, presets, randomization, and pack architecture land before distribution work.
- Portable distribution lands before installer, registration, and uninstall flow work.
- Installer, registration, and uninstall flow land before suite meta-saver work.
- The suite meta-saver lands before the `suite` app so cross-saver orchestration is real before browse-and-control UI work expands.
- The `suite` app lands before SDK stabilization so contributor-facing templates and examples can target the real browse-and-control surface instead of a placeholder.
- Creative backlog ingestion waits until the renamed product surface, suite surface, and SDK surface are stable enough to absorb new ideas without immediate rework.
- Cross-cutting polish starts only after the backlog, routing, and wave buckets are explicit enough to keep saver-local work from forking shared architecture.
