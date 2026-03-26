# AGENTS.md

This file applies to `src/`.

- Keep shared code C89-friendly.
- Preserve the Win32 `.scr` baseline.
- Keep GDI as a first-class renderer, not a degraded fallback.
- Treat product-specific needs as local unless a shared abstraction is clearly justified.
