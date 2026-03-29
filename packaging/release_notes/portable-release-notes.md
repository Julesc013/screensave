# ScreenSave Portable Bundle Release Notes

This note is the user-facing release-support source for the `C06` portable bundle.

## What This Bundle Is

- A portable end-user bundle for the current ScreenSave saver line.
- A manual-distribution layout for standalone `.scr` products.
- A bundle assembled from real discovered saver outputs plus product metadata and supporting docs.

## What This Bundle Is Not

- Not an installer.
- Not a registration or uninstall workflow.
- Not the future `suite` app.
- Not the future suite meta-saver.
- Not a developer-tools bundle; BenchLab remains excluded from the end-user payload.

## Included Content Policy

- Include only canonical saver `.scr` outputs that actually exist in configured output roots.
- Include per-saver manifests for staged savers.
- Include file-backed pack content only when the owning saver binary is staged.
- Include user-facing bundle notes, current compatibility guidance, and known limitations.

## Compatibility And Renderer Notes

- The baseline artifact remains x86 Win32 `.scr`.
- GDI remains the guaranteed renderer floor.
- GL11 remains optional and capability-gated.
- GL21 remains optional and only matters for savers and systems that support it.
- GL33 and GL46 are not treated as real user-facing portable-bundle requirements in `C06`.

## Current Known Limits

- The `C06` portable stage may be partial when some canonical saver outputs are unavailable in the local output roots.
- The bundle assembly may rely on pre-existing binaries when a supported toolchain is unavailable.
- Installer, registration, and uninstall flow are deferred to `C07`.
