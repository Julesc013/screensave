# C07 Installer Matrix

This note records the first real installer, registration, and uninstall baseline created in `C07`.

## Installer Architecture

- Source-controlled installer inputs live under `packaging/installer/`.
- Generated staging output lives under `out/installer/screensave-installer-c07/`.
- Generated zip artifact lives under `out/installer/screensave-installer-c07.zip`.
- The installer consumes the real `C06` portable payload instead of redefining a second saver payload.
- Supported install mode in `C07`: current-user only.
- Machine-wide install remains deferred after `C07`.

## Install, Update, And Cleanup Policy

| Area | `C07` Policy | Notes |
| --- | --- | --- |
| Install root | `%LOCALAPPDATA%\\ScreenSave\\Installed\\CurrentUser\\` by default | Dedicated current-user managed tree |
| Payload source | `out/portable/screensave-portable-c06/` | Installer coverage is only as complete as the current portable payload |
| Active saver selection | Optional only | `install_screensave.ps1 -SetActiveSaver <slug>` updates current-user desktop settings |
| Update behavior | Conservative overlay | Reinstall does not prune payload-absent savers automatically |
| Uninstall record | Current-user uninstall entry | `HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ScreenSave.CurrentUser` in live mode |
| Uninstall cleanup | Remove managed install root and uninstall record | Preserve user configuration and future user-pack roots outside the install tree |
| Saver restoration | Best-effort restore | Restore previous current-user saver path when captured and still relevant |

## Current Payload Coverage

The `C07` installer package is built from the currently staged `C06` portable payload.

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

## Excluded Products

- `benchlab` remains excluded from the end-user installer payload.
- `suite` remains excluded from the installer payload.
- The future suite meta-saver remains excluded from this prompt.

## Evidence Level

- The installer payload, staging folder, and zip are generated from real repository outputs only.
- Registration and uninstall behavior are validated in mock mode during `C07` so repository verification does not modify live user registry state.
- `C07` validation exercised:
  - first install with `-SetActiveSaver nocturne`
  - conservative overlay reinstall without changing the active saver selection request
  - uninstall from the installed `INSTALLER\` copy
  - mock-state result returning to empty `SCRNSAVE.EXE`, `ScreenSaveActive=0`, and no uninstall entries
- Validation logs are emitted to `out/installer/validation/` during local verification and are not committed as repository truth.
- Real machine-wide install, Control Panel integration beyond current-user saver selection assistance, and installer-driven repair workflows remain unverified and deferred.

## Known Limits

- The current installer package is partial because the upstream portable payload is partial.
- The current installer package does not claim machine-wide deployment.
- Reinstall overlays existing managed files and intentionally avoids payload-absent pruning in `C07`.

## Next Continuation Step

- `C08` suite meta-saver for cross-saver randomization
