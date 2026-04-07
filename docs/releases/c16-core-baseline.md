# ScreenSave Core C16 Baseline

`ScreenSave Core` is the frozen `C16` public baseline for ScreenSave.
It is the primary ScreenSave product and the main download for normal users.

## Release Anchor

- Tag: `c16-core-baseline`
- Tag commit: `3fbce98845472a67dad799f381d53fab7266e446`
- Public release page: `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`
- Baseline line: completed `C00` through `C16` continuation bridge
- Release surface state: `SS01` publication complete and `SS02` evidence refresh complete
- Next step: optional `SS03` maintenance/support policy and patch discipline before `PL`

## Downloads

| Role | Asset | Size | Notes |
| --- | --- | --- | --- |
| primary | [`screensave-core-c16-baseline.zip`](https://github.com/Julesc013/screensave/releases/download/c16-core-baseline/screensave-core-c16-baseline.zip) | `940045` bytes (`0.896` MiB) | ZIP of the frozen standalone saver line |
| secondary | [`screensave-installer-c16-baseline.zip`](https://github.com/Julesc013/screensave/releases/download/c16-core-baseline/screensave-installer-c16-baseline.zip) | `1000080` bytes (`0.954` MiB) | Optional current-user installer built from the same frozen Core payload |

Checksums and the machine-readable asset manifest live in [../../releases/c16-core-baseline-sha256.txt](../../releases/c16-core-baseline-sha256.txt) and [../../releases/c16-core-baseline-manifest.ini](../../releases/c16-core-baseline-manifest.ini).

## What Core Is

- The primary public ScreenSave product
- A ZIP of polished standalone `.scr` savers
- A frozen baseline anchored to the `c16-core-baseline` tag

## What Core Includes

`nocturne`, `ricochet`, `deepfield`, `plasma`, `phosphor`, `pipeworks`, `lifeforms`, `signals`, `mechanize`, `ecosystems`, `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology`

All nineteen savers are included in the frozen Core baseline.
`anthology` is included as a real saver product and does not replace the rest of the saver line.

## What Core Does Not Include

- `ScreenSave Installer` as part of the primary ZIP
- `suite`
- BenchLab
- SDK material
- Extras content

The Installer remains a separate secondary convenience channel.
`suite`, BenchLab, SDK, and Extras remain separate companion channels.

## Evidence Snapshot

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
- Machine-wide install is not part of this frozen baseline.

## Source Notes

- `SS02` validation summary: [../../validation/notes/ss02-release-validation-summary.md](../../validation/notes/ss02-release-validation-summary.md)
- `SS02` support matrix: [../../validation/notes/ss02-support-matrix.md](../../validation/notes/ss02-support-matrix.md)
- `SS02` capture log: [../../validation/captures/ss02-c16-validation-log.md](../../validation/captures/ss02-c16-validation-log.md)
- Inclusion matrix: [../../validation/notes/c16-core-inclusion-matrix.md](../../validation/notes/c16-core-inclusion-matrix.md)
- Known issues: [../../validation/notes/c16-known-issues.md](../../validation/notes/c16-known-issues.md)
- Compatibility notes: [../../packaging/release_notes/core-compatibility-notes.md](../../packaging/release_notes/core-compatibility-notes.md)
- Frozen baseline note: [../../validation/notes/c16-release-baseline.md](../../validation/notes/c16-release-baseline.md)
- Release-channel doctrine: [../roadmap/release-channels.md](../roadmap/release-channels.md)
