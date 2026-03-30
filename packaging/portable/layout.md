# Portable Bundle Layout

This note defines the portable bundle architecture as refreshed in `C14`.

## Source-Controlled Inputs

- `packaging/portable/bundle_manifest.ini`
- `packaging/portable/assemble_portable.py`
- `packaging/portable/README.md`
- `packaging/release_notes/portable-release-notes.md`
- validation notes and license notes copied into the staged bundle

## Generated Outputs

- Staged folder: `out/portable/screensave-portable-c14-rc/`
- Zip artifact: `out/portable/screensave-portable-c14-rc.zip`

Generated outputs are not committed source content.

## Staged Layout

The staged bundle uses this layout:

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

- `SAVERS/` contains only canonical saver binaries discovered in configured output roots.
- `MANIFESTS/` contains only the manifests for staged saver binaries.
- `PACKS/` contains only file-backed packs whose owning saver binary is staged.
- `PRESETS/` and `THEMES/` remain note-only in the `C06` baseline unless a staged saver ships standalone preset/theme export files outside pack directories.
- `OPTIONAL/` is reserved for clearly labeled extras; BenchLab and Suite are excluded from the end-user saver bundle.

## Current Limits

- The portable bundle may be partial when some current saver outputs are unavailable.
- The bundle does not perform installation, registration, or uninstall work.
- Installed distribution now exists separately under `packaging/installer/`.
- The bundle excludes the separate `suite` and `benchlab` app products.
- `anthology` is a normal saver product, but it is staged only when its real binary is present.
