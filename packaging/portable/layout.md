# Portable Bundle Layout

This note defines the frozen `C16` `ScreenSave Core` bundle layout.

## Source-Controlled Inputs

- `packaging/portable/bundle_manifest.ini`
- `packaging/portable/assemble_portable.py`
- `packaging/portable/README.md`
- `packaging/release_notes/portable-release-notes.md`
- `packaging/release_notes/core-baseline-notes.md`
- `packaging/release_notes/core-compatibility-notes.md`
- validation notes and license notes copied into the staged bundle

## Generated Outputs

- Staged folder: `out/portable/screensave-core-c16-baseline/`
- Zip artifact: `out/portable/screensave-core-c16-baseline.zip`

Generated outputs are not committed source content.

## Staged Layout

- `README.txt`
- `SAVERS/`
- `MANIFESTS/`
- `PRESETS/`
- `THEMES/`
- `PACKS/`
- `DOCS/`
- `LICENSES/`
- `OPTIONAL/`

## Inclusion Policy

- `SAVERS/` contains only canonical Core saver binaries discovered in configured output roots.
- `MANIFESTS/` contains only the manifests for staged saver binaries.
- `PACKS/` contains only file-backed packs whose owning saver binary is staged.
- `PRESETS/` and `THEMES/` stay note-only unless a staged saver ships standalone export files outside pack directories.
- `OPTIONAL/` remains reserved for Extras material; BenchLab, Suite, SDK material, and installer assets are excluded.

## Current Limits

- The bundle is only as complete as the real canonical saver outputs present in configured output roots.
- The bundle does not perform installation, registration, or uninstall work.
- Installed distribution exists separately under `packaging/installer/`.
- The bundle excludes `suite`, BenchLab, SDK material, and unfrozen Extras content.
