# Installer Layout

This note defines the frozen `C16` Installer companion package.

## Source-Controlled Inputs

- `packaging/installer/installer_manifest.ini`
- `packaging/installer/build_installer.py`
- `packaging/installer/installer_common.ps1`
- `packaging/installer/install_screensave.ps1`
- `packaging/installer/uninstall_screensave.ps1`
- `packaging/installer/README.md`
- `packaging/release_notes/installer-release-notes.md`

## Generated Outputs

- Staged folder: `out/installer/screensave-installer-c16-baseline/`
- Zip artifact: `out/installer/screensave-installer-c16-baseline.zip`

Generated outputs are derived from source-controlled installer inputs plus the current staged Core payload under `out/portable/screensave-core-c16-baseline/`.

## Supported Install Mode

- Supported in `C16`: current-user install only
- Deferred after `C16`: machine-wide install, shell-wide registration beyond current-user desktop settings, and richer upgrade migration tooling

The installer defaults to the current-user install root:

- `%LOCALAPPDATA%\\ScreenSave\\Installed\\CurrentUser\\`

## Installed Layout

- `README.txt`
- `PAYLOAD\\`
- `DOCS\\`
- `INSTALLER\\`
  - `install_screensave.ps1`
  - `uninstall_screensave.ps1`
  - `installer_common.ps1`
  - `installer_manifest.ini`
  - `install-state.json`

The `PAYLOAD\\` subtree mirrors the frozen Core saver payload.

## Registration And Selection Assistance

- Default install does not silently select an active saver.
- Optional selection assistance is available through `install_screensave.ps1 -SetActiveSaver <slug>`.
- When that option is used, the installer writes current-user desktop settings only.
- The installer captures the previous current-user saver path and active flag before replacing them so uninstall can restore them conservatively when possible.

## Update And Reinstall Rules

- Reinstalling into an existing managed ScreenSave install root is allowed.
- Default reinstall behavior overlays files from the current Installer payload.
- Files absent from the current payload are not pruned automatically.

## Uninstall And Cleanup Rules

- Uninstall removes the dedicated ScreenSave install root after verifying that the directory contains an installer-managed state file.
- Uninstall removes the current-user uninstall record created by this installer.
- Uninstall restores the previously captured current-user active saver state when the current active saver still points into the ScreenSave install root.
- Uninstall does not remove user configuration or future user-pack roots outside the managed install tree.

## Portable Versus Installed

- Core portable mode: the primary product, manual distribution, manual selection, no uninstall record, no install root, no automated rollback
- Installed mode: the companion Installer channel, dedicated current-user install root, uninstall record, optional current-user selection assistance, explicit uninstall behavior, and package-level docs

## Current Known Limits

- The Installer payload is only as complete as the real staged Core bundle it consumes.
- `C16` does not add machine-wide installation.
- The Installer excludes `suite`, BenchLab, SDK material, and Extras content.
