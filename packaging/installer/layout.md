# C07 Installer Layout

This note defines the Installer-channel architecture as refreshed in `C14` and interpreted by `C15`.

## Source-Controlled Inputs

- `packaging/installer/installer_manifest.ini`
- `packaging/installer/build_installer.py`
- `packaging/installer/installer_common.ps1`
- `packaging/installer/install_screensave.ps1`
- `packaging/installer/uninstall_screensave.ps1`
- `packaging/installer/README.md`
- `packaging/release_notes/installer-release-notes.md`

## Generated Outputs

- Staged folder: `out/installer/screensave-installer-c14-rc/`
- Zip artifact: `out/installer/screensave-installer-c14-rc.zip`

Generated outputs are derived from source-controlled installer inputs plus the current staged portable payload under `out/portable/screensave-portable-c14-rc/`.
Under the `C15` doctrine that portable payload is the current Core candidate surface.

## Supported Install Mode

- Supported in `C07`: current-user install only
- Deferred after `C07`: machine-wide install, shell-wide registration beyond current-user desktop settings, and richer upgrade migration tooling

The installer defaults to the current-user install root:

- `%LOCALAPPDATA%\\ScreenSave\\Installed\\CurrentUser\\`

That root is installer-managed and dedicated to ScreenSave savers plus their support files.

## Installed Layout

The installer copies the portable payload into the dedicated install root and adds an installer-managed support directory:

- `README.txt`
- `SAVERS\\*.scr`
- `MANIFESTS\\*.manifest.ini`
- `PRESETS\\`
- `THEMES\\`
- `PACKS\\`
- `DOCS\\`
- `LICENSES\\`
- `OPTIONAL\\`
- `INSTALLER\\`
  - `install_screensave.ps1`
  - `uninstall_screensave.ps1`
  - `installer_common.ps1`
  - `installer_manifest.ini`
  - `install-state.json`

## Registration And Selection Assistance

- Default install does not silently select an active saver.
- Optional selection assistance is available through `install_screensave.ps1 -SetActiveSaver <slug>`.
- When that option is used, the installer writes current-user desktop settings only:
  - `HKCU\\Control Panel\\Desktop\\SCRNSAVE.EXE`
  - `HKCU\\Control Panel\\Desktop\\ScreenSaveActive`
- The installer captures the previous current-user saver path and active flag before replacing them so uninstall can restore them conservatively when possible.

## Update And Reinstall Rules

- Reinstalling into an existing managed ScreenSave install root is allowed.
- Default reinstall behavior overlays files from the current installer payload.
- Files absent from the current payload are not pruned automatically in `C07`.
- This conservative overlay policy avoids deleting savers simply because the current payload is partial.
- A future series may add explicit prune-or-repair modes once full output coverage and installer evidence are stronger.

## Uninstall And Cleanup Rules

- Uninstall removes the dedicated ScreenSave install root after verifying that the directory contains an installer-managed state file.
- Uninstall removes the current-user uninstall record created by this installer.
- Uninstall restores the previously captured current-user active saver state when the current active saver still points into the ScreenSave install root.
- If the current active saver still points into the install root and no previous saver was captured, uninstall clears the current-user saver path and disables screen saver activation conservatively.
- Uninstall does not remove user configuration or future user-pack roots outside the managed install tree.

## Portable Versus Installed

- Core portable mode: the primary product candidate, manual distribution, manual selection, no uninstall record, no install root, no automated rollback.
- Installed mode: the companion Installer channel, dedicated current-user install root, uninstall record, optional current-user selection assistance, explicit uninstall behavior, and a package-level README for end users.

## Current Known Limits

- The installer payload is only as complete as the real staged portable bundle it consumes.
- `C07` does not add machine-wide installation.
- The installer excludes the separate `suite` and `benchlab` app products.
- The installer remains a companion convenience layer and does not redefine Core inclusion.
- `anthology` is treated as a normal saver product but is installed only when the current portable payload contains `anthology.scr`.
