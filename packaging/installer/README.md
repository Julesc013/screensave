# Installer Packaging

Purpose: source-controlled definition and assembly logic for the optional installed-distribution companion to `ScreenSave Core`.
Belongs here: installer manifests, install and uninstall scripts, layout notes, and Installer-channel release-support docs.
Does not belong here: runtime platform code, suite-app work, or fabricated install success claims.
Type: release support.

## Historical Baseline

`C07` added the first real installer path for the saver line.
`C15` formalized Installer as a companion channel.
`C16` refreshes the package against the frozen Core baseline.

Source-controlled installer inputs live here:

- `installer_manifest.ini`: explicit Installer package identity, install policy, Core-payload source, and doc inputs
- `layout.md`: durable install layout, registration model, update rules, uninstall rules, and portable-versus-installed distinction
- `build_installer.py`: stdlib-only assembly script that stages and zips an Installer package from the current frozen Core payload
- `installer_common.ps1`: bounded shared PowerShell helpers for manifest parsing, mock-state validation, registry writes, and uninstall-record handling
- `install_screensave.ps1`: current-user installer entry point with optional active-saver selection assistance
- `uninstall_screensave.ps1`: conservative uninstall entry point for removing the dedicated install root and uninstall record without deleting user config

Generated installer staging and zip artifacts do not belong here.
The assembly script writes generated staging and zip artifacts under `out/installer/`.

## C16 Frozen State

- Frozen Installer staging root: `out/installer/screensave-installer-c16-baseline/`
- Frozen Installer zip: `out/installer/screensave-installer-c16-baseline.zip`
- The Installer channel is built from the frozen Core payload.
- `suite`, BenchLab, SDK material, and Extras remain outside the Installer payload.

## Current Installer Policy

- Current-user install mode is supported.
- Machine-wide install mode remains deferred.
- The Installer consumes the frozen Core payload instead of inventing a second saver definition.
- Active saver selection assistance is opt-in and does not silently replace the current saver during a default install.
- Uninstall removes the dedicated install root and current-user uninstall record, but preserves user configuration and future user-pack roots outside the install tree.
- Update and reinstall overlay existing managed files by default and do not prune payload-absent savers automatically.

## Current Prompt Boundary

This directory defines the Installer channel only.
Core ZIP behavior remains under `packaging/portable/`.
`suite` and BenchLab remain separate companion app products outside the saver-only Installer payload.
