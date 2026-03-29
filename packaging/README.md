# Packaging

Purpose: release delivery structure for installers, portable bundles, and release-note assembly.
Belongs here: packaging docs, metadata, and later packaging implementation.
Does not belong here: runtime code or unsupported release claims.
Type: release support.

## Current State

- `packaging/portable/` now holds the real `C06` portable bundle manifest, layout note, assembly script, and related release-support docs.
- Generated portable staging and zip artifacts belong under `out/portable/`, not under this source-controlled tree.
- Installer, registration, and uninstall behavior remain deferred to `C07`.
