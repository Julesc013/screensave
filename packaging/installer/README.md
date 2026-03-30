# Installer Packaging

Purpose: source-controlled definition and assembly logic for the end-user installed saver distribution.
Belongs here: installer manifests, install and uninstall scripts, layout notes, and installer-specific release-support docs.
Does not belong here: runtime platform code, suite-app work, or fabricated install success claims.
Type: release support.

## Historical Baseline

`C07` adds the first real installer path for the current ScreenSave saver line.

Source-controlled installer inputs now live here:

- `installer_manifest.ini`: explicit installer package identity, install policy, portable-payload source, and doc inputs
- `layout.md`: durable install layout, registration model, update rules, uninstall rules, and portable-versus-installed distinction
- `build_installer.py`: stdlib-only assembly script that stages and zips an installer package from the current portable payload
- `installer_common.ps1`: bounded shared PowerShell helpers for manifest parsing, mock-state validation, registry writes, and uninstall-record handling
- `install_screensave.ps1`: current-user installer entry point with optional active-saver selection assistance
- `uninstall_screensave.ps1`: conservative uninstall entry point for removing the dedicated install root and uninstall record without deleting user config

Generated installer staging and zip artifacts do not belong here.
The assembly script writes generated staging and zip artifacts under `out/installer/`.

## C14 Refresh

`C14` refreshes the installed-distribution surface for the final release-candidate pass:

- installer staging now points at the refreshed `C14` portable payload
- staged output now moves to the current `C14` installer root under `out/installer/`
- installer docs now include final release-readiness, known-issues, and config-integrity notes
- `suite` and `benchlab` remain separate app products outside the end-user saver installer payload
- `anthology` is treated as part of the canonical saver line and is installed when it exists in the current portable payload

## Current Installer Policy

- Current-user install mode is supported in `C07`.
- Machine-wide install mode remains deferred.
- The installer consumes the current portable payload instead of inventing a second saver-payload definition.
- Active saver selection assistance is opt-in and does not silently replace the current saver during a default install.
- Uninstall removes the dedicated install root and current-user uninstall record, but preserves user configuration and any future user-pack roots outside the install tree.
- Update and reinstall overlay existing managed files by default and do not prune payload-absent savers automatically.

## Current Prompt Boundary

This directory defines installed delivery only.
Portable bundle behavior remains under `packaging/portable/`.
`suite` and `benchlab` remain separate app products and stay outside the saver-only installer payload.
