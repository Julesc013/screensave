# AGENTS.md

This file applies to `packaging/`.

- Packaging work covers installers, portable bundles, and release-note assembly.
- Packaging must consume built artifacts and documented metadata; it must not redefine runtime behavior.
- Do not hide build logic or compatibility claims inside packaging-only files.
- Release statements must stay aligned with validation evidence and changelog entries.
- Series 00 creates structure only; real packaging logic belongs to later series.
