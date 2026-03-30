# C14 Installer Matrix

This note records the final `C14` installer refresh for the current ScreenSave saver line.

## Installer Architecture

- Source-controlled installer inputs live under `packaging/installer/`.
- Generated staging output lives under `out/installer/screensave-installer-c14-rc/`.
- Generated zip artifact lives under `out/installer/screensave-installer-c14-rc.zip`.
- The installer consumes the real `C14` portable payload instead of redefining a second saver payload.
- Supported install mode remains current-user only.
- Machine-wide install remains deferred.

## Install, Update, And Cleanup Policy

| Area | Current Policy | Notes |
| --- | --- | --- |
| Install root | `%LOCALAPPDATA%\\ScreenSave\\Installed\\CurrentUser\\` by default | Dedicated current-user managed tree |
| Payload source | `out/portable/screensave-portable-c14-rc/` | Installer coverage is only as complete as the current portable payload |
| Active saver selection | Optional only | `install_screensave.ps1 -SetActiveSaver <slug>` updates current-user desktop settings |
| Update behavior | Conservative overlay | Reinstall does not prune payload-absent savers automatically |
| Uninstall record | Current-user uninstall entry | `HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ScreenSave.CurrentUser` in live mode |
| Uninstall cleanup | Remove managed install root and uninstall record | Preserve user configuration and future user-pack roots outside the install tree |
| Saver restoration | Best-effort restore | Restore previous current-user saver path when captured and still relevant |

## Current Payload Coverage

The `C14` installer package is built from the currently staged `C14` portable payload.

| Saver | Expected Artifact | Present In Current Installer Payload | Selection Assistance Eligible | Notes |
| --- | --- | --- | --- | --- |
| `nocturne` | `nocturne.scr` | yes | yes | Present through the current portable payload |
| `ricochet` | `ricochet.scr` | yes | yes | Present through the current portable payload |
| `deepfield` | `deepfield.scr` | yes | yes | Present through the current portable payload |
| `plasma` | `plasma.scr` | no | no | Missing upstream from the current portable payload |
| `phosphor` | `phosphor.scr` | no | no | Missing upstream from the current portable payload |
| `pipeworks` | `pipeworks.scr` | yes | yes | Present through the current portable payload |
| `lifeforms` | `lifeforms.scr` | yes | yes | Present through the current portable payload |
| `signals` | `signals.scr` | no | no | Missing upstream from the current portable payload |
| `mechanize` | `mechanize.scr` | no | no | Missing upstream from the current portable payload |
| `ecosystems` | `ecosystems.scr` | no | no | Missing upstream from the current portable payload |
| `stormglass` | `stormglass.scr` | no | no | Missing upstream from the current portable payload |
| `transit` | `transit.scr` | no | no | Missing upstream from the current portable payload |
| `observatory` | `observatory.scr` | no | no | Missing upstream from the current portable payload |
| `vector` | `vector.scr` | no | no | Missing upstream from the current portable payload |
| `explorer` | `explorer.scr` | no | no | Missing upstream from the current portable payload |
| `city` | `city.scr` | no | no | Missing upstream from the current portable payload |
| `atlas` | `atlas.scr` | no | no | Missing upstream from the current portable payload |
| `gallery` | `gallery.scr` | no | no | Missing upstream from the current portable payload |
| `anthology` | `anthology.scr` | no | no | Canonical meta-saver source is present, but no local binary was discovered during `C14` |

## Excluded Products

- `benchlab` remains excluded from the end-user installer payload.
- `suite` remains excluded from the installer payload.
- `sdk` remains source-only and is not part of the installer payload.

## Evidence Level

- The installer payload, staging folder, and zip are generated from real repository outputs only.
- Registration and uninstall behavior are validated in mock mode during `C14` so repository verification does not modify live user registry state.
- `C14` validation exercises:
  - first install with `-SetActiveSaver nocturne`
  - conservative overlay reinstall without changing the active saver selection request
  - uninstall from the installed `INSTALLER\\` copy
- Real machine-wide install, richer repair workflows, and live registry modification remain unverified in this environment.

## Known Limits

- The current installer package is partial because the upstream portable payload is partial.
- The current installer package does not claim machine-wide deployment.
- Reinstall overlays existing managed files and intentionally avoids payload-absent pruning.
- `C14` closes the current continuation line; later work, if any, is post-release follow-on rather than another scheduled continuation prompt.
