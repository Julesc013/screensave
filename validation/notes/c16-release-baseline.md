# C16 Release Baseline

This note records the frozen `C16` ScreenSave release baseline.

## What C16 Froze

- `ScreenSave Core` as the primary saver ZIP
- The Core inclusion decision for the full current saver line
- The companion Installer package built from that Core payload
- The separate companion artifact status for `suite`, BenchLab, the SDK, and Extras

## Source Of Truth

- Channel doctrine: `docs/roadmap/release-channels.md`
- Core gate doctrine: `docs/roadmap/core-zip-doctrine.md`
- Core inclusion decision: `validation/notes/c16-core-inclusion-matrix.md`
- Companion artifact state: `validation/notes/c16-companion-channel-matrix.md`

## Artifact Roots

- Core staging root: `out/portable/screensave-core-c16-baseline/`
- Core zip: `out/portable/screensave-core-c16-baseline.zip`
- Installer staging root: `out/installer/screensave-installer-c16-baseline/`
- Installer zip: `out/installer/screensave-installer-c16-baseline.zip`
- Suite companion artifact: `out/msvc/vs2022/Release/suite/suite.exe`
- BenchLab companion artifact: `out/msvc/vs2022/Release/benchlab/benchlab.exe`

## Evidence Summary

- On April 6, 2026, `build/msvc/vs2022/ScreenSave.sln` built successfully for `Release|Win32`.
- That build produced the full canonical saver line, `anthology.scr`, `benchlab.exe`, and `suite.exe`.
- The Core and Installer staged artifacts were refreshed from those release outputs in `C16`.

## Checkpoint

- Canonical baseline tag: `c16-core-baseline`

## Next Phase

The `C00` through `C16` continuation bridge is closed after this freeze.
`SS01` now provides the public baseline release surface at `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`.
`SS02` remains pending for refreshed release evidence and support-matrix clarity and is not implied complete by this note.
`PL` still follows only after the short `SS` bridge closes.
