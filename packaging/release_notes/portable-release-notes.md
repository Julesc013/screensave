# ScreenSave Core Release Notes

This note is the user-facing release-support source for the frozen `C16` `ScreenSave Core` ZIP.

## What This Bundle Is

- The primary end-user ScreenSave product.
- A manual-distribution ZIP of the standalone `.scr` saver line.
- A bundle assembled from the current real x86 release outputs plus product metadata and release-support docs.

## What This Bundle Includes

- Nineteen standalone saver `.scr` products, including `anthology`
- Per-saver manifests for staged savers
- File-backed pack content only where the owning saver binary is staged
- Release notes, baseline notes, compatibility notes, and known-issues notes needed to understand the frozen Core baseline

## What This Bundle Excludes

- The Installer channel
- `suite`
- BenchLab
- SDK material
- Extras content

## Compatibility Summary

- Baseline artifact: x86 Win32 `.scr`
- Required renderer floor: `gdi`
- Optional capability tiers: `gl11`, `gl21`
- `gl33` and `gl46` remain placeholders, not release requirements

## Current Known Limits

- Core is a standalone saver ZIP and does not provide installation, registration, or uninstall automation.
- Fresh live Control Panel and `desk.cpl` runtime smoke were not rerun during `C16`.
- Extras remains empty in `C16`; no bonus payload is bundled into Core.
