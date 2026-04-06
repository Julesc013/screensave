# Packaging

Purpose: release delivery structure for Core, Installer, and related release-support docs.
Belongs here: packaging docs, metadata, and bounded assembly logic.
Does not belong here: runtime code or unsupported release claims.
Type: release support.

## Current State

- `packaging/channel_manifest.ini` records the canonical `C16` channel split and the frozen artifact roots.
- `packaging/portable/` holds the frozen `ScreenSave Core` bundle manifest, layout note, assembly script, and release-support docs for `out/portable/screensave-core-c16-baseline/`.
- `packaging/installer/` holds the companion Installer manifest, scripts, layout note, and assembly logic for `out/installer/screensave-installer-c16-baseline/`.
- Generated staging and zip artifacts belong under `out/`, not under this source-controlled tree.
- `suite` and BenchLab remain separate companion app artifacts under `out/msvc/vs2022/Release/`.
- The SDK remains a source-controlled contributor surface, and Extras remains a separate empty holdback channel in `C16`.
