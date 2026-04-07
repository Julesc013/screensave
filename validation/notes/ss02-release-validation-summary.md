# SS02 Release Validation Summary

This note records the fresh validation evidence gathered in `SS02` for the frozen `c16-core-baseline` release.
It does not widen the `C16` product scope.
It refreshes the evidence and support language around the already-published `ScreenSave Core` baseline.

## Release Anchor

- Tag: `c16-core-baseline`
- Core asset: `out/portable/screensave-core-c16-baseline.zip`
- Installer asset: `out/installer/screensave-installer-c16-baseline.zip`
- Capture timestamp: `2026-04-07T16:30:44Z`
- Detailed capture log: [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md)
- Support matrix: [ss02-support-matrix.md](./ss02-support-matrix.md)

## Evidence Classes

- `FACT`: directly observed in `SS02`
- `CARRIED FORWARD`: inherited from earlier documented evidence and not rerun in `SS02`
- `TARGET`: intended support or behavior, not freshly verified in `SS02`
- `KNOWN LIMITATION`: observed caveat or restriction
- `UNVERIFIED`: not tested, or tested inconclusively, in `SS02`
- `UNSUPPORTED`: explicitly outside the frozen baseline

## SS02 Environment

| Field | Value |
| --- | --- |
| OS | `Microsoft Windows 10 Enterprise` |
| Windows version | `2009` |
| Build | `10.0.19041.6456` |
| Architecture | `64-bit` on `x64-based PC` |
| PowerShell | `5.1.19041.7058` |

## Fresh Facts Observed In SS02

- The published Core and Installer ZIP files were rehashed locally and matched the release manifest exactly.
- The extracted Core ZIP contained all nineteen frozen saver products.
- The extracted Installer ZIP carried the same nineteen-saver payload as Core.
- Fullscreen `/s` smoke with the host forced to `gdi` passed for all nineteen savers from the extracted Core ZIP.
- Representative `Settings...` `/c` smoke with the host forced to `gdi` passed for `nocturne`, `plasma`, `gallery`, and `anthology`.
- `anthology` behaved as a real standalone saver product in both fullscreen and representative settings checks.
- Same-commit BenchLab renderer probes confirmed `gdi` on representative savers, `gl11` on representative savers, and `gl21` on `gallery` and `anthology`.
- BenchLab requests for `gl33` and `gl46` on `gallery` fell back to `gl21`, confirming that `gl33` and `gl46` are not release tiers in this baseline.
- Installer mock-mode install and uninstall passed against the extracted Installer ZIP, including current-user selection assistance for `anthology` and cleanup of the managed install root.

## Carried-Forward Context

- The classic Windows host-path hardening recorded in [c05-windows-integration-matrix.md](./c05-windows-integration-matrix.md) remains the code-level basis for preview-parent validation, fullscreen virtual-desktop sizing, and shared settings routing.
- The compatibility preservation band in [../../specs/compatibility.md](../../specs/compatibility.md) remains the project target, but `SS02` did not rerun machines across that broader OS range.
- The frozen saver lineup still comes from [c16-core-inclusion-matrix.md](./c16-core-inclusion-matrix.md); `SS02` refreshed evidence for that lineup rather than changing it.

## Remaining Limits After SS02

- Fresh `desk.cpl` discovery and Screen Saver Settings listing evidence is still not recorded in `SS02`.
- Preview `/p` was attempted for `nocturne` and `anthology`, but child-window parenting was not confirmed; preview remains `UNVERIFIED` rather than pass/fail.
- `gl11` and `gl21` now have representative `FACT` evidence, but the full per-saver accelerated-renderer matrix was not rerun in `SS02`.
- Installer evidence is now real for mock-mode current-user install and uninstall, but live registry writes and actual Control Panel registration remain `UNVERIFIED`.
- Machine-wide install remains `UNSUPPORTED`.
- `suite`, BenchLab, SDK, and Extras remain outside the primary Core release; BenchLab was used only as an internal diagnostic companion during `SS02`.

## Outcome

`SS02` establishes real current evidence behind the frozen Core release without pretending that every historical target or Windows host path was rerun.
`gdi` now has direct frozen-release smoke evidence on the full included saver line in the available environment.
Optional renderer tiers and installer behavior are now described with a stricter split between fresh facts, carried-forward context, and unverified surfaces.
