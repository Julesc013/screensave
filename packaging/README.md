# Packaging

Purpose: release delivery structure for installers, portable bundles, and release-note assembly.
Belongs here: packaging docs, metadata, and later packaging implementation.
Does not belong here: runtime code or unsupported release claims.
Type: release support.

## Current State

- `packaging/portable/` now holds the portable bundle manifest, layout note, assembly script, and related release-support docs refreshed for the final `C14` release-candidate pass.
- Generated portable staging and zip artifacts belong under `out/portable/`, not under this source-controlled tree.
- `packaging/installer/` now holds the current-user installer manifest, scripted install and uninstall logic, layout note, and installer assembly path refreshed for the final `C14` release-candidate pass.
- Generated installer staging and zip artifacts belong under `out/installer/`, not under this source-controlled tree.
- Portable and installed distribution are now explicit parallel delivery modes for the saver line.
- `anthology` is part of the canonical saver line and belongs in those delivery paths when its real binary is available.
- `suite` and `benchlab` are real separate app products, but they remain outside the end-user saver bundle and installer payload.
