# ScreenSave Core Compatibility Notes

This note records the compatibility framing for the frozen `C16` ScreenSave Core baseline after the `SS02` evidence refresh.

## Artifact Floor

- Baseline saver artifact: x86 Win32 `.scr`
- Host model: classic ANSI Win32 screen saver path
- Guaranteed renderer floor: `gdi`

## Optional Capability Tiers

- `gl11` remains optional and capability-gated.
- `gl21` remains optional and capability-gated.
- `gl33` and `gl46` remain named placeholders, not real shipped requirements.

## Product-Class Boundaries

- Core is the standalone saver ZIP only.
- Core does not require the Installer channel.
- Core does not require `suite`, BenchLab, the SDK, or Extras content.
- `anthology` remains a standalone saver product inside Core, not a replacement for the separate saver line.

## Evidence Classes

- `FACT`: directly observed in `SS02`
- `CARRIED FORWARD`: inherited from earlier notes and not rerun in `SS02`
- `TARGET`: intended support band, not freshly verified in `SS02`
- `KNOWN LIMITATION`: observed caveat or restriction
- `UNVERIFIED`: not tested, or tested inconclusively, in `SS02`
- `UNSUPPORTED`: explicitly outside the frozen baseline

## Evidence Level

- `FACT`: the published Core and Installer ZIP assets were rehashed in `SS02` and matched the release manifest.
- `FACT`: fullscreen `gdi` smoke passed on all nineteen included savers from the extracted Core ZIP.
- `FACT`: representative `Settings...` smoke passed on `nocturne`, `plasma`, `gallery`, and `anthology`.
- `FACT`: representative same-build BenchLab probes confirmed `gl11` on `nocturne`, `plasma`, and `gallery`, plus `gl21` on `gallery` and `anthology`.
- `UNSUPPORTED`: same-build BenchLab requests for `gl33` and `gl46` fell back to `gl21`, so those tiers remain placeholders rather than release claims.
- `CARRIED FORWARD`: shared Windows host lifecycle expectations beyond the fresh `SS02` checks still rely on the recorded `C05` validation note plus later source-level hardening.
- `TARGET`: the broader Windows 95 / 98 / ME and NT 4.0 through Windows 11 compatibility band remains normative, but `SS02` reran only Windows 10 x64 locally.

## Current Installer Compatibility Notes

- The companion Installer channel remains current-user only.
- `SS02` verified mock-mode current-user install and uninstall behavior against the frozen Installer ZIP.
- Live registry writes, live Control Panel registration, machine-wide install, and richer repair modes remain outside the fresh `SS02` evidence.
