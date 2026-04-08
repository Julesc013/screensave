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
- applications such as the real `suite` control app and `benchlab` serve different purposes than the `.scr` products
- `products/apps/suite/` is the canonical suite-app location, and `products/apps/player/` is only a superseded legacy stub
- SDK material supports later contributors without becoming part of the runtime baseline

Release doctrine now treats `ScreenSave Core` as the primary product for the standalone saver line.
`suite`, BenchLab, the Installer, the SDK, and Extras remain separate companion channels.
`anthology` remains a real saver product rather than collapsing into the `suite` app.

The architecture is intentionally conservative.
Baseline functionality must remain available on a GDI-only path and must not depend on modern-only Windows assumptions.

Series 03 made the private Win32 host real.
Series 04 makes the reusable non-renderer runtime and first public platform surface real.
Series 05 makes the mandatory reusable GDI backend real and routes the host through the shared renderer path.
Series 06 makes Nocturne the first real saver product on those shared contracts.
Series 07 makes BenchLab the first real app product and proves the current saver/renderer contracts in a windowed diagnostics harness.
Series 08 makes the optional GL11 backend, explicit renderer selection, and explicit fallback-to-GDI behavior real while preserving the compatibility and ownership rules defined in `specs/`.
Series 09 makes the first real multi-product saver family live on those same contracts by adding Ricochet and Deepfield without turning the platform into a gallery, plugin loader, or scene framework.
Series 10 proves the same contracts can also host richer framebuffer and vector products by adding Plasma and Phosphor with only a narrow shared visual-buffer helper instead of a broad effects or plotting framework.
Series 11 proves the same contracts can also host richer grid and state-driven products by adding Pipeworks and Lifeforms with only a narrow shared grid-buffer helper instead of a broad simulation or editor framework.
Series 12 proves the same contracts can also host layered system-interface, kinetic-assembly, and ambient-behavior products by adding Signals, Mechanize, and Ecosystems without promoting a broad widget, animation, or ecology framework into the platform.
Series 13 proves the same contracts can also host layered weather, transit-infrastructure, and celestial exhibit products by adding Stormglass, Transit, and Observatory without promoting a broad weather, route, astronomy, or scenic framework into the platform.
Series 14 proves the same contracts can also host heavyweight software-3D, traversal, urban night-world, and fractal-voyage products by adding Vector, Explorer, City, and Atlas without promoting a broad 3D, map, city, or fractal framework into the platform.
Series 15 proves the same contracts can also host a versioned renderer ladder by adding the optional capability-gated GL21 backend, explicit GL33 and GL46 ladder entries, the internal null safety fallback, and Gallery without weakening the GDI baseline, the conservative GL11 tier, or the product/local ownership boundaries.
Post-`S15` continuation planning is tracked historically in `docs/roadmap/post-s15-plan.md`, and the continuation path is now complete through the rename, checkpoint, productization, shared-settings, Windows-integration, portable-bundle, installer, suite-meta-saver, suite-app, SDK, backlog-ingestion, cross-cutting polish, `C13` Wave A, Wave B, and Wave C, `C14` rerelease hardening, `C15` release doctrine, and the `C16` Core baseline freeze.
`SS` is now closed, `SX` is the active substrate series, and `PL` still follows later as the first flagship expansion program.
Active post-`C16` sequencing now lives in `docs/roadmap/post-c16-program.md` and `docs/roadmap/sx-series.md`.

During `SX`, the architecture keeps the public renderer ladder and the current narrow saver-facing contracts intact while private substrate layers grow underneath them.
`SX01` makes that split real in code by introducing private substrate headers under `platform/include/screensave/private/` and private routing helpers under `platform/src/core/substrate/`, while savers and companion apps continue to build against the same public `screensave/` headers.
`SX02`, `SX03`, and `SX04` then harden the `gdi`, `gl11`, and `gl21` lanes without widening the public contract.
`SX05` makes `gl33` the first real modern tier by adding a bounded modern backend under the private substrate and proving it on `gallery` without turning that proof into a suite-wide migration.
`SX06` then makes `gl46` the real premium tier with the same bounded-proof discipline and the same explicit degradation path back through `gl33`, `gl21`, `gl11`, and `gdi`.
`SX07` keeps that GL ladder primary while adding private image, text, present-policy, and future-backend-family seams beside it so later optional adapter work can land without another public-contract reset.
`gallery` is the bounded renderer-proof surface during the renderer-tier work, BenchLab remains diagnostics-only, and `plasma` stays the first flagship `PL` track rather than becoming the scope of `SX`.
