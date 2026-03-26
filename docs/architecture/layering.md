# Layering

This document explains dependency direction and responsibility boundaries.

## Layers

- `platform/src/host/` owns screensaver host behavior, mode dispatch, and Win32 integration.
- `platform/src/core/` owns reusable runtime services that are not renderer-specific.
- `platform/src/render/` owns backend implementations behind the renderer contract.
- `platform/include/screensave/` is the future public platform header surface.
- `products/` owns saver-specific and app-specific behavior.
- `tests/` owns verification logic.
- `validation/` owns evidence and notes.
- `tools/`, `build/`, and `packaging/` own support workflows, not runtime behavior.

## Dependency Rules

- Products may depend on the public platform surface.
- Products may not depend on private platform internals unless a later spec explicitly permits it.
- Platform internals must not depend on product code.
- Tests may depend on platform and products as appropriate for verification.
- Tools may assist platform, products, tests, validation, or packaging, but they are not baseline runtime dependencies.
- Packaging may consume built artifacts and release metadata, but it must not define runtime behavior.

## Public Versus Private Platform Surface

The public platform surface now begins under `platform/include/screensave/`.
Implementation detail lives under `platform/src/`.

Code should move into the public surface only when it is stable, cross-product, and intended for reuse.
Everything else should remain private until experience proves the abstraction is durable.

Series 04 keeps the host implementation private while exposing only a narrow version/config/diagnostics/renderer/saver contract set.
Series 05 adds the first real backend under `platform/src/render/gdi/` and removes the need for a host-local placeholder draw path.

## Product Boundary

Products own:

- manifests or equivalent metadata once introduced
- product defaults
- presets
- themes
- product-local content

These concerns should not leak into shared platform layers without a clear reuse case and a corresponding spec or doc update.
