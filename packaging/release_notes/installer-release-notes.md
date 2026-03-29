# ScreenSave Installer Release Notes

This note is the user-facing release-support source for the `C07` installed distribution path.

## What This Installer Is

- A scripted current-user installer package for the current ScreenSave saver line.
- A complement to the `C06` portable bundle, not a replacement for it.
- A package that installs the currently staged saver payload into a dedicated current-user install root and writes a current-user uninstall record.

## What This Installer Is Not

- Not a machine-wide installer.
- Not the future `suite` app.
- Not the future suite meta-saver.
- Not a broad deployment platform.
- Not a guarantee that every canonical saver is present when the upstream portable payload is partial.

## Install And Registration Policy

- The supported `C07` install mode is current-user only.
- Default install does not silently replace the current active saver.
- Optional selection assistance is available through `install_screensave.ps1 -SetActiveSaver <slug>`.
- Uninstall removes the dedicated install root and current-user uninstall record while preserving user configuration outside that managed tree.

## Update And Cleanup Policy

- Reinstall overlays existing managed files by default.
- `C07` does not automatically prune files absent from the current payload.
- This conservative update policy avoids deleting savers when the current installer payload is partial.

## Current Known Limits

- The installer package is only as complete as the real portable payload it consumes.
- Machine-wide install, shell-wide registration helpers, and richer upgrade repair modes remain deferred after `C07`.
- Portable distribution remains available as a separate delivery mode.
