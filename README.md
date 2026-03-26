# ScreenSave

ScreenSave is a monorepo for a retro-compatible Windows screensaver platform and a suite of products built on top of it.

The platform target is intentionally conservative:

- C89 is the language baseline.
- x86 Win32 `.scr` binaries are the baseline product format.
- Windows 95 / 98 / ME and NT 4.0 through Windows 11 are the support band to preserve.
- The host model is the classic ANSI Win32 screensaver path.
- GDI is mandatory.
- OpenGL 1.1 is optional and must be capability-detected at runtime.
- Any later accelerated backend must remain optional and must not weaken the universal baseline.

## Current Status

The repository is in Series 07.
Series 00 established repository law, structure, roadmap, and agent guidance.
Series 01 added contribution, review, governance, and CI foundations.
Series 02 added the checked-in build scaffold and toolchain lane structure.
Series 03 added the first real Win32 `.scr` host skeleton.
Series 04 added the first shared runtime substrate and narrow public platform API.
Series 05 added the mandatory reusable GDI renderer backend.
Series 06 added Nocturne as the first real saver product:

- a real Nocturne saver module with product-owned config, presets, and themes
- restrained dark-room-safe rendering through the shared GDI backend
- real config dialog wiring and long-run fade/reseed behavior on the shared host path

Series 07 adds BenchLab as the first real developer-facing app product:

- a real windowed harness for the active saver module outside the `.scr` host path
- deterministic restart and reseed controls for repeatable debugging
- a modest runtime and renderer diagnostics overlay for validating Nocturne and the GDI baseline

OpenGL, packaging, and multi-product saver selection are still deferred.

## Planned Product Shape

ScreenSave is structured as one shared platform with many releasable products:

- Individual savers under `products/savers/`
- Suite-level applications such as Gallery, Player, and BenchLab under `products/apps/`
- SDK material under `products/sdk/`

`nocturne` is now the first real saver product in the tree.
`benchlab` is now the first real non-saver app in the tree.
Additional saver families and suite-level products are described in `docs/roadmap/products-and-lineup.md`.

## Repository Authority

For normative project law, start with:

- `specs/compatibility.md`
- `specs/saver_api.md`
- `specs/renderer_contract.md`
- `specs/build_targets.md`
- `specs/release_contract.md`
- `specs/config_schema.md`

For explanatory architecture and execution order, start with:

- `docs/architecture/overview.md`
- `docs/architecture/layering.md`
- `docs/architecture/repo-structure.md`
- `docs/roadmap/prompt-program.md`
- `docs/roadmap/series-map.md`

## Directory Summary

- `.codex/` contains project-scoped Codex configuration and custom agent definitions.
- `.github/` contains community-health files, ownership metadata, and stage-appropriate workflows.
- `platform/` is the shared runtime and public API area.
- `products/` is the product tree for savers, applications, and SDK material.
- `assets/` is for shared non-code content and license records.
- `tests/` and `validation/` hold verification logic and evidence.
- `tools/`, `build/`, and `packaging/` hold support material, not baseline runtime dependencies.

## Build Status

The checked-in build scaffold now integrates the real Series 07 saver-plus-harness path on top of the Series 05 renderer, Series 04 shared core runtime, and Series 03 host:

- a concrete MSVC VS2022 solution lane under `build/msvc/vs2022/`
- a concrete MinGW i686 make lane under `build/mingw/i686/`
- documentation-only VS6 and VS2008 lanes that preserve long-term intent without claiming present build completeness

The shared platform target now compiles the reusable non-renderer core runtime, the mandatory GDI backend, and the Win32 host.
The `.scr` target now runs the real Nocturne saver through the shared saver and GDI paths, with the validation scene retained only as a fallback for non-product sessions.
The app target now runs BenchLab as a real windowed harness for Nocturne through the same shared contracts.

Read the specs before adding code.
This repository should stay truthful, reconstructable, and conservative about compatibility.
