# Packaging

Purpose: release delivery structure for installers, portable bundles, and release-note assembly.
Belongs here: packaging docs, metadata, and later packaging implementation.
Does not belong here: runtime code or unsupported release claims.
Type: release support.

## Current State

- `packaging/channel_manifest.ini` records the canonical `C15` channel split and points back to the doctrine docs.
- `packaging/portable/` now holds the Core-candidate bundle manifest, layout note, assembly script, and related release-support docs refreshed for the final `C14` release-candidate pass.
- Generated portable staging and zip artifacts belong under `out/portable/`, not under this source-controlled tree.
- `packaging/installer/` now holds the current-user Installer-channel manifest, scripted install and uninstall logic, layout note, and installer assembly path refreshed for the final `C14` release-candidate pass.
- Generated installer staging and zip artifacts belong under `out/installer/`, not under this source-controlled tree.
- `ScreenSave Core` is the primary end-user saver ZIP.
- `Installer` is an optional convenience channel built on top of Core.
- `anthology` is part of the canonical saver line and belongs in Core only when it satisfies the same release gate as the rest of Core.
- `suite`, BenchLab, the SDK, and experimental Extras are separate companion channels and remain outside the primary Core payload by default.
