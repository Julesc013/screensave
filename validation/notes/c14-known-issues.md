# C14 Known Issues And Limitations

This note records the explicit non-blocking limitations left in the final `C14` release-candidate state.

## Current Known Issues

- The refreshed portable bundle and installer payloads remain partial because the configured local output roots currently expose only `nocturne`, `ricochet`, `deepfield`, `pipeworks`, and `lifeforms`.
- No local `anthology.scr` or `suite.exe` output was discovered during `C14`, so those products are represented in release-readiness notes but not in the staged payloads.
- Final `C14` validation is static and packaging-oriented in this environment; no supported MSVC or MinGW toolchain was available for a fresh full-suite rebuild.
- Existing local binaries were not rebuilt during `C14`, so embedded version/about resource strings inside those binaries were not revalidated here.
- Installer verification remains mock-mode only in this environment; live registry writes and machine-wide install behavior are still intentionally unverified.

## Compatibility Notes

- GDI remains the guaranteed floor.
- GL11 remains optional and capability-gated.
- GL21 remains optional and capability-gated.
- GL33 and GL46 remain placeholders only and are not release requirements.

## Release Impact

- These issues do not block a truthful release-candidate checkpoint because they are explicitly documented and reflected in the refreshed staging outputs.
- These issues do block any claim that the current local package payload represents the full canonical saver line.
- These issues also block freezing a truthful `ScreenSave Core` baseline until `C16` refreshes the candidate surface against the Core gate.
