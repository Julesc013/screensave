# ScreenSave Core Baseline Notes

This note records the frozen `C16` ScreenSave baseline.

## Frozen Product

- Primary product: `ScreenSave Core`
- Frozen Core artifact root: `out/portable/screensave-core-c16-baseline/`
- Frozen Core zip: `out/portable/screensave-core-c16-baseline.zip`
- Canonical checkpoint tag: `c16-core-baseline`

## Included Core Surface

- Nineteen standalone saver products are included in the frozen Core baseline:
  - `nocturne`
  - `ricochet`
  - `deepfield`
  - `plasma`
  - `phosphor`
  - `pipeworks`
  - `lifeforms`
  - `signals`
  - `mechanize`
  - `ecosystems`
  - `stormglass`
  - `transit`
  - `observatory`
  - `vector`
  - `explorer`
  - `city`
  - `atlas`
  - `gallery`
  - `anthology`
- The detailed gate decision lives in `validation/notes/c16-core-inclusion-matrix.md`.

## Companion Channels

- `ScreenSave Installer` is the companion installed-distribution package built from the frozen Core payload.
- `ScreenSave Suite` remains a separate companion app and is not part of Core.
- `ScreenSave BenchLab` remains a separate diagnostics harness and is not part of Core.
- `ScreenSave SDK` remains a source-controlled contributor surface and is not part of Core.
- `ScreenSave Extras` remains a separate holdback channel with no frozen payload in `C16`.
- The companion-artifact summary lives in `validation/notes/c16-companion-channel-matrix.md`.

## Evidence Basis

- On April 6, 2026, `build/msvc/vs2022/ScreenSave.sln` built successfully for `Release|Win32`.
- That build produced all nineteen canonical saver `.scr` targets plus `benchlab.exe` and `suite.exe` under `out/msvc/vs2022/Release/`.
- Core and Installer staging in `C16` are refreshed from those real x86 release outputs.

## Honest Limits

- `C16` did not rerun live Control Panel or `desk.cpl` screen saver smoke.
- `C16` did not add machine-wide install support.
- `C16` did not freeze any Extras payload.
- Known limitations remain tracked in `validation/notes/c16-known-issues.md`.

## Next Phase

`C16` closes the post-`S15` continuation bridge.
`SS01` now publishes this frozen baseline at `https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline`.
`SS02` remains pending for refreshed release evidence and support-matrix clarity.
