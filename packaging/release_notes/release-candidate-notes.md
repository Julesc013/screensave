# ScreenSave Release Candidate Notes

This note records the final `C14` release-candidate state for the current ScreenSave product line.

## Included Product Surface

- Nineteen standalone saver products in source, including the canonical meta-saver `anthology`
- One separate diagnostics app: `benchlab`
- One separate browse/launch/config app: `suite`
- One contributor surface under `products/sdk/`

## Distribution Surface

- Portable saver bundle: refreshed under `out/portable/screensave-portable-c14-rc/`
- Current-user installer package: refreshed under `out/installer/screensave-installer-c14-rc/`
- `C15` doctrine interpretation: the portable bundle is the current Core candidate surface
- `C15` doctrine interpretation: the installer package is the optional companion Installer candidate built from that surface
- App policy: `benchlab` and `suite` stay outside the primary Core bundle and the companion Installer payload

## Current Local Output Reality

- Canonical saver binaries discovered locally during `C14`: `nocturne`, `ricochet`, `deepfield`, `pipeworks`, `lifeforms`
- Meta-saver binary discovered locally during `C14`: none
- App binaries discovered locally during `C14`: `benchlab.exe`
- `suite.exe` was not discovered in the configured local output roots during `C14`

## Compatibility And Evidence

- GDI remains the guaranteed floor.
- GL11 remains optional and capability-gated.
- GL21 remains optional and capability-gated.
- Final `C14` evidence in this environment is static and packaging-oriented; no supported MSVC or MinGW toolchain was available for a fresh full-suite rebuild.

## Release-Candidate Checkpoint

- Preferred canonical checkpoint tag for this repo state: `c14-release-candidate`
- The tag is intended to point at the final release-candidate commit that includes the refreshed packaging outputs and `C14` release-readiness notes.

## Known Limits

- Current local portable and installer payloads remain partial because the configured output roots do not currently contain the full canonical saver line.
- Existing local output binaries were not rebuilt during `C14`; embedded version/about resources inside those binaries were not revalidated here.
- Live installer verification remains mock-mode only in this environment.
- `C16` still has to apply the Core inclusion gate and freeze the actual Core baseline.
