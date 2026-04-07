# ScreenSave Core Release Notes

This note is the user-facing release-support source for the frozen `C16` `ScreenSave Core` ZIP.

## What This Bundle Is

- The primary end-user ScreenSave product.
- A manual-distribution ZIP of the standalone `.scr` saver line.
- A bundle assembled from the current real x86 release outputs plus product metadata and release-support docs.
- The primary asset on the public `c16-core-baseline` release page.

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

## Evidence Snapshot

- `SS02` rehashed the published Core ZIP and confirmed the frozen nineteen-saver lineup from the extracted release payload.
- `SS02` passed fullscreen `gdi` smoke on all nineteen included savers from the extracted Core ZIP.
- `SS02` passed representative `Settings...` smoke on `nocturne`, `plasma`, `gallery`, and `anthology`.
- Support details now live in `validation/notes/ss02-release-validation-summary.md` and `validation/notes/ss02-support-matrix.md`.

## Current Known Limits

- Core is a standalone saver ZIP and does not provide installation, registration, or uninstall automation.
- Fresh Screen Saver Settings discovery and `desk.cpl` listing evidence is still not recorded in `SS02`.
- Preview child-window confirmation remains inconclusive after representative `/p` attempts.
- `gl11` and `gl21` now have representative evidence, but the full accelerated-renderer matrix was not rerun across every saver.
- Extras remains empty in `C16`; no bonus payload is bundled into Core.
