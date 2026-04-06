# ScreenSave Installer Release Notes

This note is the user-facing release-support source for the optional `ScreenSave Installer` channel refreshed in `C14` and named formally in `C15`.

## What This Installer Is

- A scripted current-user installer package for the current ScreenSave Core candidate surface.
- A complement to the Core ZIP, not a replacement for it.
- A package that installs the currently staged saver payload into a dedicated current-user install root and writes a current-user uninstall record.

## What This Installer Is Not

- Not a machine-wide installer.
- Not the separate `suite` app.
- Not a broad deployment platform.
- Not the authority that decides what belongs in Core.
- Not a guarantee that every canonical saver is present when the upstream portable payload is partial.

## Install And Registration Policy

- The supported `C07` install mode is current-user only.
- Default install does not silently replace the current active saver.
- Optional selection assistance is available through `install_screensave.ps1 -SetActiveSaver <slug>`.
- Uninstall removes the dedicated install root and current-user uninstall record while preserving user configuration outside that managed tree.
- `anthology` is treated like any other saver product and is installed only when the current payload includes `anthology.scr`.

## Update And Cleanup Policy

- Reinstall overlays existing managed files by default.
- `C07` does not automatically prune files absent from the current payload.
- This conservative update policy avoids deleting savers when the current installer payload is partial.

## Current Known Limits

- The installer package is only as complete as the real portable payload it consumes.
- Machine-wide install, shell-wide registration helpers, and richer upgrade repair modes remain deferred after `C07`.
- `suite`, BenchLab, SDK material, and Extras remain separate companion channels and are not installed by this saver-only package.
- Portable distribution remains available as a separate delivery mode.
