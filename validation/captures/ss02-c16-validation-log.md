# SS02 C16 Validation Log

This log lifts the concrete `SS02` observations into a durable repo-side record.
It summarizes the local validation capture generated at `2026-04-07T16:30:44Z` for the frozen `c16-core-baseline` release.

## Environment

| Field | Value |
| --- | --- |
| OS | `Microsoft Windows 10 Enterprise` |
| Version | `2009` |
| Build | `10.0.19041.6456` |
| Architecture | `64-bit` on `x64-based PC` |
| PowerShell | `5.1.19041.7058` |

## Method Notes

- Validation used the published `c16-core-baseline` ZIP assets, not a renamed or regenerated package.
- Fullscreen and representative settings smoke ran from the extracted Core ZIP.
- Fullscreen and representative settings smoke forced the host renderer request to `gdi` through `HKCU\\Software\\Julesc013\\ScreenSave\\Host\\RendererRequest=1` during the run and restored that registry subtree afterward.
- BenchLab probes used the same-commit `Release|Win32` build outputs only as an internal diagnostic companion.
- Installer validation used mock mode from the extracted Installer ZIP so the run could verify current-user install and uninstall behavior without inventing live registry claims.

## Release Assets

| Role | Path | Size | SHA-256 |
| --- | --- | --- | --- |
| Core ZIP | `out/portable/screensave-core-c16-baseline.zip` | `940045` bytes | `5a228500b055d3802768206a0dcb67dad30b602fb4df7a3ea13993367fa9e6e0` |
| Installer ZIP | `out/installer/screensave-installer-c16-baseline.zip` | `1000080` bytes | `cab94b3f64548fc268b6dcbf7fc2baa4b57f87888155d7653d0d1944926b9db5` |

## Extracted Payload Roots

- Core root: `C:\\Users\\Jules\\AppData\\Local\\Temp\\screensave-ss02-20260407-163040\\core\\screensave-core-c16-baseline`
- Installer root: `C:\\Users\\Jules\\AppData\\Local\\Temp\\screensave-ss02-20260407-163040\\installer\\screensave-installer-c16-baseline`

## Frozen Saver Coverage

The extracted Core ZIP and the extracted Installer payload both contained these nineteen savers:

`anthology`, `atlas`, `city`, `deepfield`, `ecosystems`, `explorer`, `gallery`, `lifeforms`, `mechanize`, `nocturne`, `observatory`, `phosphor`, `pipeworks`, `plasma`, `ricochet`, `signals`, `stormglass`, `transit`, `vector`

## Fullscreen `/s` GDI Smoke

All fullscreen runs below passed from the extracted Core ZIP with `gdi` forced as the requested renderer.

| Saver | Started | Main Window | Graceful Close | Exit Code |
| --- | --- | --- | --- | --- |
| `anthology` | yes | yes | yes | `0` |
| `atlas` | yes | yes | yes | `0` |
| `city` | yes | yes | yes | `0` |
| `deepfield` | yes | yes | yes | `0` |
| `ecosystems` | yes | yes | yes | `0` |
| `explorer` | yes | yes | yes | `0` |
| `gallery` | yes | yes | yes | `0` |
| `lifeforms` | yes | yes | yes | `0` |
| `mechanize` | yes | yes | yes | `0` |
| `nocturne` | yes | yes | yes | `0` |
| `observatory` | yes | yes | yes | `0` |
| `phosphor` | yes | yes | yes | `0` |
| `pipeworks` | yes | yes | yes | `0` |
| `plasma` | yes | yes | yes | `0` |
| `ricochet` | yes | yes | yes | `0` |
| `signals` | yes | yes | yes | `0` |
| `stormglass` | yes | yes | yes | `0` |
| `transit` | yes | yes | yes | `0` |
| `vector` | yes | yes | yes | `0` |

## Representative Settings `/c` GDI Smoke

| Saver | Started | Main Window | Graceful Close | Exit Code |
| --- | --- | --- | --- | --- |
| `nocturne` | yes | yes | yes | `0` |
| `plasma` | yes | yes | yes | `0` |
| `gallery` | yes | yes | yes | `0` |
| `anthology` | yes | yes | yes | `0` |

## Preview `/p` Attempts

Preview was attempted with a synthetic parent window on representative savers.
The processes started and closed cleanly, but child-window parenting was not confirmed.

| Saver | Started | Child Found | Main Window Present | Main Window Parent | Graceful Close | Exit Code |
| --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | yes | no | yes | `0` | yes | `0` |
| `anthology` | yes | no | yes | `0` | yes | `0` |

## BenchLab Renderer Probes

BenchLab remained a separate companion diagnostic harness and was not part of the release payload.
It was used here only to confirm requested-versus-active renderer behavior on the same frozen commit.

| Saver | Requested | Observed Title | Result |
| --- | --- | --- | --- |
| `nocturne` | `gdi` | `BenchLab - Nocturne [gdi -> gdi]` | pass |
| `gallery` | `gdi` | `BenchLab - Gallery [gdi -> gdi]` | pass |
| `anthology` | `gdi` | `BenchLab - Anthology [gdi -> gdi]` | pass |
| `nocturne` | `gl11` | `BenchLab - Nocturne [gl11 -> gl11]` | pass |
| `plasma` | `gl11` | `BenchLab - Plasma [gl11 -> gl11]` | pass |
| `gallery` | `gl11` | `BenchLab - Gallery [gl11 -> gl11]` | pass |
| `gallery` | `gl21` | `BenchLab - Gallery [gl21 -> gl21]` | pass |
| `anthology` | `gl21` | `BenchLab - Anthology [gl21 -> gl21]` | pass |
| `gallery` | `gl33` | `BenchLab - Gallery [gl33 -> gl21]` | fallback to `gl21` |
| `gallery` | `gl46` | `BenchLab - Gallery [gl46 -> gl21]` | fallback to `gl21` |

## Installer Mock-Mode Results

| Check | Result | Notes |
| --- | --- | --- |
| State file written | pass | Mock backend recorded install state |
| Installed saver count | `19` | Matches the frozen Core lineup |
| Selected saver | `anthology` | Selection assistance was exercised explicitly |
| Backend | `mock` | No live registry claims were made from this run |
| Install root removed after uninstall | pass | Managed install root was cleaned up |

### Mock State After Install

- Desktop `SCRNSAVE_EXE` pointed to the installed `anthology.scr`.
- Desktop `ScreenSaveActive` was `1`.
- A current-user uninstall record was written under `ScreenSave.CurrentUser`.

### Mock State After Uninstall

- Desktop `SCRNSAVE_EXE` returned to an empty value.
- Desktop `ScreenSaveActive` returned to `0`.
- The uninstall state was emptied.

## Residual Gaps Recorded Here

- No fresh `desk.cpl` listing or Screen Saver Settings discovery evidence was captured in this run.
- Preview child-window confirmation remains inconclusive.
- Live installer registry writes and real Control Panel registration remain unverified.
- The accelerated renderer evidence is representative rather than a full per-saver matrix.
