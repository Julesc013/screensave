# Release Channels

This document defines the canonical ScreenSave release doctrine introduced in `C15`.

## Doctrine Summary

- The primary product is `ScreenSave Core`.
- `ScreenSave Core` is a ZIP of the most complete and polished standalone `.scr` saver products.
- Core is the main thing normal users should download.
- Core is independent of the installer, the `suite` app, BenchLab, the SDK, and experimental extras.
- All other deliverables are secondary companion channels: `Installer`, `Suite`, `BenchLab`, `SDK`, and `Extras`.

## Naming Rule

- `Core` is the primary end-user saver product.
- `suite` is the optional companion app channel name, not the umbrella name for every shipped artifact.
- `anthology` remains a saver product. It belongs in Core only when it satisfies the same release gate as every other Core saver.

## Channel Matrix

| Channel | Audience | Canonical Role | Includes | Explicitly Excludes |
| --- | --- | --- | --- | --- |
| `Core` | normal end users | primary product | release-grade standalone `.scr` savers, the docs needed to use them, and packs or support files required for the standalone saver experience | installer logic, `suite`, BenchLab, SDK material, unstable extras |
| `Installer` | end users who want convenience | optional convenience layer | a scripted installed-distribution path built on top of the Core saver line | any authority to redefine Core membership, `suite`, BenchLab, SDK material, unstable extras |
| `Suite` | end users who want browse and launch help | optional companion app | the `suite` browser, launcher, preview, and saver-settings control surface | authority to define Core, BenchLab diagnostics scope, SDK authoring scope |
| `BenchLab` | developers and advanced users | diagnostics and validation harness | renderer, runtime, deterministic, and multi-product inspection tooling | primary end-user bundle status, installer authority, SDK authoring identity |
| `SDK` | contributors | authoring and validation surface | template saver, examples, pack authoring guidance, contributor validators | end-user bundle status, installer status, BenchLab identity |
| `Extras` | opt-in experimenters | unstable or lower-confidence bonus channel | experimental packs, showcase material, renderer-heavy or lower-confidence content | anything required for the primary Core experience |

## Channel Boundary Rules

- Core owns the primary end-user saver ZIP only.
- Installer is derived from Core and cannot silently broaden Core.
- Suite is optional and cannot become a dependency for the standalone saver line.
- BenchLab remains diagnostics-only.
- SDK remains contributor-only.
- Extras exists to hold back material that is interesting but not yet strong enough for Core.

## Current C16 State

- `C16` froze `ScreenSave Core` at `out/portable/screensave-core-c16-baseline/` with a matching zip beside it.
- `C16` froze the companion Installer package at `out/installer/screensave-installer-c16-baseline/` with a matching zip beside it.
- `suite` and BenchLab remain separate companion app artifacts under `out/msvc/vs2022/Release/`.
- The SDK remains source-only and outside Core.
- Extras remains a separate empty holdback channel in `C16`.
- The `C00` through `C16` continuation bridge is now closed; future work should start from the frozen baseline in a new post-release `S`-series program.
