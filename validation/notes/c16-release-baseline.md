# C16 Release Baseline

This note records the frozen `C16` ScreenSave release baseline.
`SS01` published that baseline, and `SS02` refreshed the evidence and support wording around it without changing the frozen saver payload.

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
- `SS01` published the frozen Core and Installer ZIP assets at `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`.
- `SS02` rehashed the published ZIP assets, verified the frozen nineteen-saver lineup, passed fullscreen `gdi` smoke on all nineteen included savers from the extracted Core ZIP, passed representative `Settings...` smoke on `nocturne`, `plasma`, `gallery`, and `anthology`, and passed mock-mode current-user install and uninstall against the extracted Installer ZIP.
- `SS02` also refreshed the support language so `gdi`, `gl11`, `gl21`, installer behavior, and remaining unverified host paths are now classified explicitly instead of implied.

## Checkpoint

- Canonical baseline tag: `c16-core-baseline`

## Next Phase

The `C00` through `C16` continuation bridge is closed after this freeze.
`SS01` now provides the public baseline release surface at `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`.
`SS02` now provides the refreshed release evidence, capture log, and support matrix under `validation/notes/ss02-release-validation-summary.md`, `validation/notes/ss02-support-matrix.md`, and `validation/captures/ss02-c16-validation-log.md`.
Optional `SS03` may still define the maintenance/support policy and patch discipline before `PL` begins.
`PL` still follows only after the short `SS` bridge closes.
