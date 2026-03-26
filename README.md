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

The repository is in Series 02.
Series 00 established repository law, structure, roadmap, and agent guidance.
Series 01 added contribution, review, governance, and CI foundations.
Series 02 adds the checked-in build scaffold and toolchain lane structure.
There is still no real runtime implementation yet.

## Planned Product Shape

ScreenSave is structured as one shared platform with many releasable products:

- Individual savers under `products/savers/`
- Suite-level applications such as Gallery, Player, and BenchLab under `products/apps/`
- SDK material under `products/sdk/`

The first named saver placeholder is `nocturne`.
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

Series 02 introduces a truthful build scaffold:

- a concrete MSVC VS2022 solution lane under `build/msvc/vs2022/`
- a concrete MinGW i686 make lane under `build/mingw/i686/`
- documentation-only VS6 and VS2008 lanes that preserve long-term intent without claiming present build completeness

The checked-in build graph currently uses tiny build-only stubs.
It does not implement the real Win32 `.scr` host, real saver behavior, or real rendering yet.

Read the specs before adding code.
This repository should stay truthful, reconstructable, and conservative about compatibility.
