# ScreenSave

ScreenSave is a Windows screensaver project built around a conservative compatibility contract:

- C89 as the code baseline.
- x86 Win32 `.scr` 32-bit binaries as the baseline product format.
- GDI as the universal renderer floor.
- OpenGL 1.1 as the optional conservative accelerated tier.
- Advanced OpenGL reserved for later, strictly optional, and capability-gated.

## Current State

The repository is at bootstrap series `S00`.
The control plane, documentation, and directory layout now exist.
No saver host, renderer implementation, or product code has been added yet.

## Repository Layout

- `docs/` holds architecture, validation, and series mapping documents.
- `specs/` holds normative project rules and compatibility requirements.
- `src/` is reserved for shared C89 code, Win32 host code, and renderer implementations.
- `products/` is reserved for product-local assets and code.
- `tools/` holds local validation and developer scripts.
- `.codex/` holds project-scoped Codex control files and reusable agent briefs.

## Validation

Run the bootstrap check from the repository root:

```powershell
.\tools\verify_bootstrap.ps1
```

See `docs/validation.md` for current toolchain limits and validation notes.

## Status Notes

Bootstrap work is intentionally narrow.
Future queued series should add implementation without weakening the compatibility baseline documented in `specs/compatibility.md`.
