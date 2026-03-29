# Portable Bundle Layout

This note defines the `C06` portable bundle architecture.

## Source-Controlled Inputs

- `packaging/portable/bundle_manifest.ini`
- `packaging/portable/assemble_portable.py`
- `packaging/portable/README.md`
- `packaging/release_notes/portable-release-notes.md`
- validation notes and license notes copied into the staged bundle

## Generated Outputs

- Staged folder: `out/portable/screensave-portable-c06/`
- Zip artifact: `out/portable/screensave-portable-c06.zip`

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
- `OPTIONAL/` is reserved for clearly labeled extras; BenchLab is excluded in `C06`.

## Known C06 Limits

- The portable bundle may be partial when some current saver outputs are unavailable.
- The bundle does not perform installation, registration, or uninstall work.
- The bundle does not include the future `suite` app or any suite-level meta saver.
