# SS02 Support Matrix

This matrix records the support picture for the frozen `c16-core-baseline` release after the `SS02` evidence refresh.
Status and evidence class are intentionally separate.

## Status Key

- `Tested / Pass`
- `Tested / Pass with limitations`
- `Tested / Inconclusive`
- `Not tested in SS02`
- `Target only`
- `Unsupported`

## Evidence Class Key

- `FACT`
- `CARRIED FORWARD`
- `TARGET`
- `KNOWN LIMITATION`
- `UNVERIFIED`
- `UNSUPPORTED`

## Matrix

| Surface | Environment | Mode / Scope | Status | Evidence Class | Notes | Source |
| --- | --- | --- | --- | --- | --- | --- |
| Release asset integrity | Windows 10 Enterprise `2009` x64 | Core ZIP plus Installer ZIP | Tested / Pass | `FACT` | Local SHA-256 digests matched the published release manifest exactly | [ss02-release-validation-summary.md](./ss02-release-validation-summary.md) |
| Frozen saver inventory | Windows 10 Enterprise `2009` x64 | Extracted Core ZIP and extracted Installer payload | Tested / Pass | `FACT` | Both extracted payloads contained the same nineteen-saver frozen lineup | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Fullscreen saver host path | Windows 10 Enterprise `2009` x64 | `/s` with host request forced to `gdi`, all nineteen savers from the extracted Core ZIP | Tested / Pass | `FACT` | All included savers started, created a main window, closed gracefully, and returned exit code `0` | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| `anthology` as a real saver | Windows 10 Enterprise `2009` x64 | `anthology.scr` `/s` plus representative `/c` under `gdi` | Tested / Pass | `FACT` | `anthology` behaved as a standalone saver product rather than a `suite` app surface | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Settings host path | Windows 10 Enterprise `2009` x64 | `/c` with host request forced to `gdi` on `nocturne`, `plasma`, `gallery`, and `anthology` | Tested / Pass | `FACT` | Representative settings dialogs opened and closed cleanly | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Preview host path | Windows 10 Enterprise `2009` x64 | Synthetic `/p` parent on `nocturne` and `anthology` | Tested / Inconclusive | `UNVERIFIED` | The processes started and closed cleanly, but child-window parenting was not confirmed | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Screen Saver Settings discovery and listing | Windows 10 Enterprise `2009` x64 | `desk.cpl` and picker listing | Not tested in SS02 | `CARRIED FORWARD` | `C05` documented host-path hardening, but `SS02` did not rerun Control Panel discovery or selection UI | [c05-windows-integration-matrix.md](./c05-windows-integration-matrix.md) |
| GDI renderer floor | Windows 10 Enterprise `2009` x64 | Fullscreen release smoke plus representative same-build BenchLab probes | Tested / Pass | `FACT` | `gdi` remains the guaranteed release floor for the frozen saver line | [ss02-release-validation-summary.md](./ss02-release-validation-summary.md) |
| GL11 optional tier | Windows 10 Enterprise `2009` x64 | Same-build BenchLab probes on `nocturne`, `plasma`, and `gallery` | Tested / Pass | `FACT` | Representative evidence only; this is not a full nineteen-saver accelerated-renderer matrix | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| GL11 across the full saver line | Windows 10 Enterprise `2009` x64 | All included savers | Not tested in SS02 | `UNVERIFIED` | `SS02` reran representative GL11 probes, not the full saver set | [ss02-release-validation-summary.md](./ss02-release-validation-summary.md) |
| GL21 current real later-capability tier | Windows 10 Enterprise `2009` x64 | Same-build BenchLab probes on `gallery` and `anthology` | Tested / Pass | `FACT` | Fresh evidence exists where the current frozen baseline actually advertises `gl21` | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| GL33 and GL46 | Windows 10 Enterprise `2009` x64 | BenchLab requests on `gallery` | Unsupported | `UNSUPPORTED` | Requests fell back to `gl21`; `gl33` and `gl46` remain future placeholders, not release tiers | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Internal `null` renderer | n/a | Host safety path | Unsupported | `UNSUPPORTED` | Internal-only fallback, not a user-facing support claim | [../../specs/compatibility.md](../../specs/compatibility.md) |
| Installer current-user flow | Windows 10 Enterprise `2009` x64 | Mock-mode install plus uninstall from the extracted Installer ZIP | Tested / Pass with limitations | `FACT`, `KNOWN LIMITATION` | Mock mode wrote expected current-user state, selected `anthology`, and cleaned up correctly; this does not prove live registry integration | [../captures/ss02-c16-validation-log.md](../captures/ss02-c16-validation-log.md) |
| Installer live registry writes and Control Panel selection | Windows 10 Enterprise `2009` x64 | Real current-user registry integration | Not tested in SS02 | `UNVERIFIED` | `SS02` used mock mode to avoid inventing live install claims | [c16-installer-matrix.md](./c16-installer-matrix.md) |
| Machine-wide install | n/a | System-wide install and uninstall | Unsupported | `UNSUPPORTED` | The frozen Installer baseline remains current-user only | [c16-installer-matrix.md](./c16-installer-matrix.md) |
| Broader Windows compatibility band | Windows 95 / 98 / ME and NT 4.0 through Windows 11 | Project preservation target | Target only | `TARGET` | The compatibility band remains normative, but `SS02` only reran Windows 10 x64 locally | [../../specs/compatibility.md](../../specs/compatibility.md) |
| Multi-monitor policy | Shared host path | Virtual-desktop fullscreen sizing, not per-monitor sessions | Not tested in SS02 | `CARRIED FORWARD` | `C05` documented the one-window virtual-desktop policy; `SS02` did not rerun multi-monitor hardware | [c05-windows-integration-matrix.md](./c05-windows-integration-matrix.md) |
