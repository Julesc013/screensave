# C16 Installer Matrix

This note records the frozen `C16` Installer companion package built from the frozen Core baseline.

## Installer Architecture

- Source-controlled installer inputs live under `packaging/installer/`.
- Generated staging output lives under `out/installer/screensave-installer-c16-baseline/`.
- Generated zip artifact lives under `out/installer/screensave-installer-c16-baseline.zip`.
- The Installer channel consumes the frozen Core payload under `out/portable/screensave-core-c16-baseline/`.
- Supported install mode remains current-user only.

## Install, Update, And Cleanup Policy

| Area | C16 Policy | Notes |
| --- | --- | --- |
| Install root | `%LOCALAPPDATA%\\ScreenSave\\Installed\\CurrentUser\\` by default | Dedicated current-user managed tree |
| Payload source | `out/portable/screensave-core-c16-baseline/` | Installer mirrors the frozen Core payload instead of redefining it |
| Active saver selection | Optional only | `install_screensave.ps1 -SetActiveSaver <slug>` updates current-user desktop settings |
| Update behavior | Conservative overlay | Reinstall does not prune payload-absent savers automatically |
| Uninstall record | Current-user uninstall entry | Live registry writes remain current-user only |
| Uninstall cleanup | Remove managed install root and uninstall record | Preserve user configuration outside the managed tree |

## Payload Coverage

The frozen `C16` Installer package carries the same saver lineup as frozen Core.

| Saver | Expected Artifact | Present In C16 Installer Payload | Selection Assistance Eligible | Notes |
| --- | --- | --- | --- | --- |
| `nocturne` | `nocturne.scr` | yes | yes | Present through frozen Core |
| `ricochet` | `ricochet.scr` | yes | yes | Present through frozen Core |
| `deepfield` | `deepfield.scr` | yes | yes | Present through frozen Core |
| `plasma` | `plasma.scr` | yes | yes | Present through frozen Core |
| `phosphor` | `phosphor.scr` | yes | yes | Present through frozen Core |
| `pipeworks` | `pipeworks.scr` | yes | yes | Present through frozen Core |
| `lifeforms` | `lifeforms.scr` | yes | yes | Present through frozen Core |
| `signals` | `signals.scr` | yes | yes | Present through frozen Core |
| `mechanize` | `mechanize.scr` | yes | yes | Present through frozen Core |
| `ecosystems` | `ecosystems.scr` | yes | yes | Present through frozen Core |
| `stormglass` | `stormglass.scr` | yes | yes | Present through frozen Core |
| `transit` | `transit.scr` | yes | yes | Present through frozen Core |
| `observatory` | `observatory.scr` | yes | yes | Present through frozen Core |
| `vector` | `vector.scr` | yes | yes | Present through frozen Core |
| `explorer` | `explorer.scr` | yes | yes | Present through frozen Core |
| `city` | `city.scr` | yes | yes | Present through frozen Core |
| `atlas` | `atlas.scr` | yes | yes | Present through frozen Core |
| `gallery` | `gallery.scr` | yes | yes | Present through frozen Core |
| `anthology` | `anthology.scr` | yes | yes | Present through frozen Core as a standalone meta-saver |

## Excluded Companion Products

- `suite` remains excluded from the Installer payload.
- BenchLab remains excluded from the Installer payload.
- The SDK remains source-only and is not part of the Installer payload.
- Extras remains empty in `C16` and is not part of the Installer payload.

## Limits

- `C16` did not add machine-wide install support.
- `C16` did not add payload pruning for reinstall.
- Live installer smoke remains distinct from payload assembly; see `validation/notes/c16-known-issues.md` for the remaining validation limits.
