# Architecture Overview

ScreenSave is organized as one shared platform plus many separate products.

The platform side exists to solve the common problems once:

- classic Win32 `.scr` hosting
- preview and full-screen mode handling
- config resolution
- renderer selection
- shared runtime services

The product side exists to keep each saver or suite-level application separate:

- savers define their own identity, presets, themes, and content
- applications such as the future `suite` control app and `benchlab` serve different purposes than the `.scr` products
- the current `gallery` and `player` app placeholders are transitional naming artifacts, not the durable post-`S15` app model
- SDK material supports later contributors without becoming part of the runtime baseline

The architecture is intentionally conservative.
Baseline functionality must remain available on a GDI-only path and must not depend on modern-only Windows assumptions.

Series 03 made the private Win32 host real.
Series 04 makes the reusable non-renderer runtime and first public platform surface real.
Series 05 makes the mandatory reusable GDI backend real and routes the host through the shared renderer path.
Series 06 makes Nocturne the first real saver product on those shared contracts.
Series 07 makes BenchLab the first real app product and proves the current saver/renderer contracts in a windowed diagnostics harness.
Series 08 makes the optional GL11 backend, explicit renderer selection, and explicit fallback-to-GDI behavior real while preserving the compatibility and ownership rules defined in `specs/`.
Series 09 makes the first real multi-product saver family live on those same contracts by adding Ricochet and Deepfield without turning the platform into a gallery, plugin loader, or scene framework.
Series 10 proves the same contracts can also host richer framebuffer and vector products by adding Ember and Oscilloscope Dreams with only a narrow shared visual-buffer helper instead of a broad effects or plotting framework.
Series 11 proves the same contracts can also host richer grid and state-driven products by adding Pipeworks and Lifeforms with only a narrow shared grid-buffer helper instead of a broad simulation or editor framework.
Series 12 proves the same contracts can also host layered system-interface, kinetic-assembly, and ambient-behavior products by adding Signal Lab, Mechanical Dreams, and Ecosystems without promoting a broad widget, animation, or ecology framework into the platform.
Series 13 proves the same contracts can also host layered weather, transit-infrastructure, and celestial exhibit products by adding Stormglass, Night Transit, and Observatory without promoting a broad weather, route, astronomy, or scenic framework into the platform.
Series 14 proves the same contracts can also host heavyweight software-3D, traversal, urban night-world, and fractal-voyage products by adding Vector Worlds, Retro Explorer, City Nocturne, and Fractal Atlas without promoting a broad 3D, map, city, or fractal framework into the platform.
Series 15 proves the same contracts can also host a versioned renderer ladder by adding the optional capability-gated GL21 backend, explicit GL33 and GL46 placeholders, the internal null safety fallback, and GL Gallery without weakening the GDI baseline, the conservative GL11 tier, or the product/local ownership boundaries.
Post-`S15` continuation planning is now tracked in `docs/roadmap/post-s15-plan.md`, and the next implementation step is the rename/taxonomy normalization pass rather than a direct jump to packaging or suite handoff.
