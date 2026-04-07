# ScreenSave Core C16 Baseline

`ScreenSave Core` is the frozen `C16` public baseline for ScreenSave.
It is the primary ScreenSave product and the main download for normal users.

## Release Anchor

- Tag: `c16-core-baseline`
- Tag commit: `3fbce98845472a67dad799f381d53fab7266e446`
- Public release page: `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`
- Baseline line: completed `C00` through `C16` continuation bridge
- Current release step: `SS01` public release surfacing
- Next step: `SS02` release-evidence refresh and support-matrix clarification

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

## Known Limits

- Fresh live Control Panel and `desk.cpl` screen saver smoke were not rerun during `C16`.
- `gl11` and `gl21` remain optional capability tiers and were not re-smoke-tested on every saver during `C16`.
- The Installer remains current-user only; machine-wide install is not part of this frozen baseline.
- `SS02` still remains for refreshed release evidence and support-matrix clarity.

## Source Notes

- Inclusion matrix: [../../validation/notes/c16-core-inclusion-matrix.md](../../validation/notes/c16-core-inclusion-matrix.md)
- Known issues: [../../validation/notes/c16-known-issues.md](../../validation/notes/c16-known-issues.md)
- Compatibility notes: [../../packaging/release_notes/core-compatibility-notes.md](../../packaging/release_notes/core-compatibility-notes.md)
- Frozen baseline note: [../../validation/notes/c16-release-baseline.md](../../validation/notes/c16-release-baseline.md)
- Release-channel doctrine: [../roadmap/release-channels.md](../roadmap/release-channels.md)
