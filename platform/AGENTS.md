# AGENTS.md

This file applies to `platform/`.

- `platform/include/screensave/` is the future public API surface; keep it stable, small, and cross-product.
- `platform/src/` is private implementation space until a boundary is intentionally promoted.
- Move code into the platform only when multiple products need it or when it is part of the host, renderer, or shared config contract.
- Do not expose product-specific presets, themes, content rules, or branding through public headers.
- When a change affects a public boundary, update the relevant files in `specs/` and `docs/` in the same series.
