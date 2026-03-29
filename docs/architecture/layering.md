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
Series 06 proves the public saver/config boundary with Nocturne while keeping product defaults, presets, themes, and config UI inside the product tree.
Series 07 proves the same public contracts can also support a separate windowed diagnostics app without turning `platform/` into a player or UI framework.
Series 08 proves the renderer contract can support both the mandatory GDI floor and an optional GL11 path without pushing backend-selection logic into products.
Series 09 proves the same layering can support multiple real saver products while keeping only narrow selection helpers in shared code and leaving product motion logic inside each saver.
Series 10 proves the same layering can also support richer framebuffer and vector products while promoting only a narrow shared visual-buffer helper and keeping palette, formula, persistence, and theme choices inside the owning products.
Series 11 proves the same layering can also support stateful grid and cellular products while promoting only a narrow shared byte-grid helper and keeping growth, reseed, rule, and simulation choices inside the owning products.
Series 12 proves the same layering can also support layered system-interface, kinetic-assembly, and ambient-behavior products without promoting a broad widget, animation, or ecology framework into shared code.
Series 13 proves the same layering can also support atmospheric weather, transit-infrastructure, and celestial exhibit products without promoting a broad weather, route, astronomy, or scenic framework into shared code.
Series 14 proves the same layering can also support heavyweight software-3D, traversal, urban-scenic, and fractal-voyage products while keeping projection, route, scene, and refinement behavior inside the owning products rather than promoting a broad engine.
Series 15 proves the same layering can also support a capability-gated GL21 backend, explicit GL33 and GL46 placeholders, an internal null safety fallback, and a renderer-showcase product while keeping advanced context logic backend-private, renderer selection narrow in shared core code, and Gallery product behavior inside the owning product.

## Product Boundary

Products own:

- manifests or equivalent metadata once introduced
- product defaults
- presets
- themes
- product-local content

These concerns should not leak into shared platform layers without a clear reuse case and a corresponding spec or doc update.
