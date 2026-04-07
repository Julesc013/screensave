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

## Known Limits

- Fresh live Control Panel and `desk.cpl` screen saver smoke were not rerun during `C16`.
- `gl11` and `gl21` remain optional capability tiers and were not re-smoke-tested on every saver during `C16`.
- The Installer remains current-user only; machine-wide install is not part of this baseline.
- `SS02` still remains for refreshed release evidence and support-matrix clarity.

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
- release step: `SS01`
- next step: `SS02`
