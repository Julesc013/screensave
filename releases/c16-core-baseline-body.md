# ScreenSave Core C16 Baseline

`ScreenSave Core` is the frozen `C16` ScreenSave baseline and the primary public product.
The main download is the standalone saver ZIP.
The Installer ZIP is attached as a secondary current-user convenience package built from the same frozen Core payload.

## Downloads

- `screensave-core-c16-baseline.zip` - primary download; frozen Core ZIP of 19 standalone `.scr` savers
- `screensave-installer-c16-baseline.zip` - secondary download; optional current-user installer for the same frozen Core payload

## Included Saver Set

`nocturne`, `ricochet`, `deepfield`, `plasma`, `phosphor`, `pipeworks`, `lifeforms`, `signals`, `mechanize`, `ecosystems`, `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, `anthology`

`anthology` is included as a real saver product and does not replace the rest of the saver line.

## Not Included In The Primary Release

- `suite`
- BenchLab
- SDK
- Extras

These remain separate companion channels and are not bundled into the primary Core release.

## Validation Snapshot

- `SS02` rehashed the published Core and Installer ZIP assets and matched the recorded SHA-256 manifest exactly.
- `SS02` passed fullscreen `/s` smoke with `gdi` forced on all nineteen included savers from the extracted Core ZIP.
- `SS02` passed representative `Settings...` `/c` smoke on `nocturne`, `plasma`, `gallery`, and `anthology`.
- `SS02` confirmed representative `gl11` behavior on `nocturne`, `plasma`, and `gallery`, plus representative `gl21` behavior on `gallery` and `anthology`, using same-build BenchLab probes only as an internal diagnostic companion.
- `SS02` passed mock-mode current-user install and uninstall against the extracted Installer ZIP without turning that into a live registry or Control Panel claim.

## Known Limits

- Fresh Screen Saver Settings discovery and `desk.cpl` listing evidence is still not recorded in `SS02`.
- Preview `/p` child-window parenting remains unverified after representative attempts on `nocturne` and `anthology`.
- `gl11` and `gl21` now have representative evidence, but the full accelerated-renderer matrix was not rerun across every saver.
- The Installer remains current-user only; `SS02` validated mock-mode current-user install and uninstall, while live registry integration remains unverified.
- Machine-wide install is not part of this baseline.

## Integrity

- `screensave-core-c16-baseline.zip`
  - size: `940045` bytes (`0.896` MiB)
  - SHA-256: `5a228500b055d3802768206a0dcb67dad30b602fb4df7a3ea13993367fa9e6e0`
- `screensave-installer-c16-baseline.zip`
  - size: `1000080` bytes (`0.954` MiB)
  - SHA-256: `cab94b3f64548fc268b6dcbf7fc2baa4b57f87888155d7653d0d1944926b9db5`

## Release Anchor

- tag: `c16-core-baseline`
- commit: `3fbce98845472a67dad799f381d53fab7266e446`
- baseline line: completed `C00` through `C16`
- release step: `SS01` publication plus `SS02` evidence refresh complete
- next step: optional `SS03`

## Notes

- repo release page: `https://github.com/Julesc013/screensave/blob/main/docs/releases/c16-core-baseline.md`
- SS02 validation summary: `https://github.com/Julesc013/screensave/blob/main/validation/notes/ss02-release-validation-summary.md`
- SS02 support matrix: `https://github.com/Julesc013/screensave/blob/main/validation/notes/ss02-support-matrix.md`
- SS02 capture log: `https://github.com/Julesc013/screensave/blob/main/validation/captures/ss02-c16-validation-log.md`
- known issues: `https://github.com/Julesc013/screensave/blob/main/validation/notes/c16-known-issues.md`
- compatibility notes: `https://github.com/Julesc013/screensave/blob/main/packaging/release_notes/core-compatibility-notes.md`
- inclusion matrix: `https://github.com/Julesc013/screensave/blob/main/validation/notes/c16-core-inclusion-matrix.md`
- checksums: `https://github.com/Julesc013/screensave/blob/main/releases/c16-core-baseline-sha256.txt`
