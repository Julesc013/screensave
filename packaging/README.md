# Packaging

Purpose: release delivery structure for installers, portable bundles, and release-note assembly.
Belongs here: packaging docs, metadata, and later packaging implementation.
Does not belong here: runtime code or unsupported release claims.
Type: release support.

## Current State

- `packaging/portable/` now holds the real `C06` portable bundle manifest, layout note, assembly script, and related release-support docs.
- Generated portable staging and zip artifacts belong under `out/portable/`, not under this source-controlled tree.
- `packaging/installer/` now holds the real `C07` current-user installer manifest, scripted install and uninstall logic, layout note, and installer assembly path.
- Generated installer staging and zip artifacts belong under `out/installer/`, not under this source-controlled tree.
- Portable and installed distribution are now explicit parallel delivery modes; suite-level products remain future work.
