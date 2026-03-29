# Build

This directory holds the checked-in build scaffold for ScreenSave.
It documents the toolchain lanes, keeps their project files reviewable, and defines the output conventions that later implementation series must preserve.

## Build Philosophy

The build system is intentionally explicit:

- checked-in per-toolchain lanes under `build/` are the canonical checked-in build entry points
- generated output belongs under `out/`, never under `build/` or source directories
- modern concrete lanes exist to support near-term implementation work
- legacy lanes stay visible as honest scaffold and documentation until later series can validate them properly

The build scaffold is not the constitutional source of truth.
The normative compatibility and target rules remain in `specs/build_targets.md` and related specs.
This directory translates those rules into reviewable build lanes.

## Current State In Series 15

Real now:

- a concrete MSVC VS2022 solution and project pair
- a concrete MinGW i686 make-based lane
- a real Win32 `.scr` host skeleton wired into both concrete modern lanes
- a real shared core runtime under `platform/src/core/`
- a narrow public platform header set under `platform/include/screensave/`
- a real mandatory GDI backend under `platform/src/render/gdi/`
- a real optional OpenGL 1.1 backend under `platform/src/render/gl11/`
- a real optional GL21 capability path under `platform/src/render/gl21/`
- explicit GL33 and GL46 placeholder tiers under `platform/src/render/gl33/` and `platform/src/render/gl46/`
- a real internal null safety backend under `platform/src/render/null/`
- a narrow shared software visual-buffer helper under `platform/src/core/visual/`
- a narrow shared grid-state helper under `platform/src/core/grid/`
- real saver products under `products/savers/nocturne/`, `products/savers/ricochet/`, `products/savers/deepfield/`, `products/savers/plasma/`, `products/savers/phosphor/`, `products/savers/pipeworks/`, `products/savers/lifeforms/`, `products/savers/signals/`, `products/savers/mechanize/`, `products/savers/ecosystems/`, `products/savers/stormglass/`, `products/savers/transit/`, `products/savers/observatory/`, `products/savers/vector/`, `products/savers/explorer/`, `products/savers/city/`, `products/savers/atlas/`, `products/savers/gallery/`, and `products/savers/anthology/`
- a real BenchLab diagnostics app under `products/apps/benchlab/`
- product-owned configuration, presets, and themes wired through the shared saver contract
- a windowed harness path that runs the active saver through the shared saver and renderer contracts outside the `.scr` host
- explicit versioned renderer selection and fallback reporting through the shared renderer path
- one true `.scr` output per saver across the current nineteen-product saver line
- `anthology.scr` staying explicit as the suite meta-saver target that links its own product code plus the current eighteen inner saver modules in-process instead of hiding cross-saver orchestration behind external process launch
- BenchLab remaining separate as the direct multi-saver diagnostics harness
- a tiny renderer-driven validation scene retained only as a non-product fallback path
- build-layout validation in `tools/scripts/check_build_layout.py`

Deferred:

- later real renderer tiers beyond the current GDI, GL11, GL21, GL33-placeholder, GL46-placeholder, and null safety stack
- later-series work beyond Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, Atlas, Gallery, and Anthology
- the future `suite` app or a suite-level meta-product
- full legacy project files for VS6 and VS2008
- packaging and release production

## Toolchain Lanes

- `build/msvc/vs2022/` is the practical modern Windows lane for near-term implementation work.
- `build/mingw/i686/` is the practical GCC-family x86 lane.
- `build/msvc/vs6/` documents the long-term legacy validation lane for early Windows targets.
- `build/msvc/vs2008/` documents the intermediate-era MSVC lane.
- `build/ci/` documents how CI validates the scaffold at the current stage.

## Output Conventions

Generated output belongs under `out/` using a stable pattern:

- binaries: `out/<toolchain>/<lane>/<profile>/<target>/`
- intermediates: `out/intermediate/<toolchain>/<lane>/<profile>/<target>/`
- logs or generated reports: `out/logs/<toolchain>/<lane>/` or other explicitly named subtrees under `out/`

Examples:

- `out/msvc/vs2022/Debug/nocturne/nocturne.scr`
- `out/intermediate/mingw/i686/release/screensave_platform/`

These conventions are documented now so later series can extend them without reshaping the tree.

## How Later Series Should Extend This

- keep the shared core, GDI backend, conservative GL11 tier, saver contracts, and host/app lifecycle intact while later series add more saver sessions and optional higher renderer tiers against the public contracts
- keep checked-in project files small and explicit
- add real legacy project files only when they can be maintained honestly
- expand CI from static scaffold validation to real builds only when the runtime is ready

`build/` should remain readable without generated files or hidden generators becoming the only truth.
