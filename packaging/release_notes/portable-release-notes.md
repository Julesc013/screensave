# ScreenSave Core Portable Release Notes

This note is the user-facing release-support source for the current `ScreenSave Core` portable ZIP candidate refreshed in `C14` and named formally in `C15`.

## What This Bundle Is

- The current portable ZIP candidate for `ScreenSave Core`.
- A manual-distribution layout for standalone `.scr` products.
- A bundle assembled from real discovered saver outputs plus product metadata and supporting docs.

## What This Bundle Is Not

- Not an installer.
- Not a registration or uninstall workflow.
- Not the separate `suite` app.
- Not the separate BenchLab diagnostics harness, the SDK, or unfrozen Extras content.

## Included Content Policy

- Include only canonical saver `.scr` outputs that actually exist in configured output roots.
- Treat `anthology` as a normal saver product and include it when its binary exists in configured output roots.
- Include per-saver manifests for staged savers.
- Include file-backed pack content only when the owning saver binary is staged.
- Include user-facing bundle notes, current compatibility guidance, and known limitations.
- Use `OPTIONAL/` only for clearly labeled Extras material when later work chooses to stage it.

## Compatibility And Renderer Notes

- The baseline artifact remains x86 Win32 `.scr`.
- GDI remains the guaranteed renderer floor.
- GL11 remains optional and capability-gated.
- GL21 remains optional and only matters for savers and systems that support it.
- GL33 and GL46 are not treated as real user-facing portable-bundle requirements in `C14`.

## Current Known Limits

- The `C14` portable stage may still be partial when some canonical saver outputs are unavailable in the local output roots.
- The bundle assembly may rely on pre-existing binaries when a supported toolchain is unavailable.
- `suite`, BenchLab, SDK material, and Extras remain separate companion channels and are not part of Core by default.
- Installed distribution exists separately through the refreshed current-user Installer channel.
- `C16` will decide the actual frozen Core lineup by applying the Core inclusion gate.
