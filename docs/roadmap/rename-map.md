# Rename Map

This document records the canonical post-`S15` saver and app naming baseline that landed in `C01` and was locked as the post-rename checkpoint in `C02`.
It remains the durable reference for legacy-to-canonical migration behavior.

## Saver Canonical Slugs

| Legacy Saver Slug | Canonical Saver Slug | Notes |
| --- | --- | --- |
| `ember` | `plasma` | One-word canonical saver slug for the framebuffer-plasma product |
| `oscilloscope_dreams` | `phosphor` | One-word canonical saver slug for the phosphor/vector product |
| `signal_lab` | `signals` | One-word canonical saver slug for the system-instrument product |
| `mechanical_dreams` | `mechanize` | One-word canonical saver slug for the kinetic assembly product |
| `night_transit` | `transit` | One-word canonical saver slug for the nocturnal infrastructure product |
| `retro_explorer` | `explorer` | One-word canonical saver slug for the traversal product |
| `vector_worlds` | `vector` | One-word canonical saver slug for the software-3D and wireframe product |
| `fractal_atlas` | `atlas` | Canonical fractal-voyage saver slug; absorbs any future Infinity Atlas ideas |
| `city_nocturne` | `city` | One-word canonical saver slug for the urban night-world product |
| `gl_gallery` | `gallery` | Canonical renderer-showcase saver slug; reserves `gallery` for the saver, not the app layer |

Unless a later product-specific branding decision is documented explicitly, the canonical public product name should follow the canonical slug in title case.

## App Naming Decisions

- `suite` is the canonical future suite-level control app.
- `products/apps/suite/` now holds the canonical suite-app placeholder location.
- `products/apps/player/` is superseded by the `suite` plan rather than surviving as a separate enduring app product.
- `benchlab` remains the diagnostics harness and does not merge into `suite`.
- `sdk` remains the contributor-facing surface and does not become the suite app.

## Infinity Atlas Decision

There is no separate planned saver named `Infinity Atlas` or `infinity_atlas`.
Future atlas-expansion ideas, presets, routes, or scene families belong under `atlas`.
Do not create a parallel atlas-family saver unless a later roadmap update explicitly reopens that decision.

## Execution Rules For `C01`

- Rename directories, manifests, build references, docs, and runtime identifiers in one reviewable pass.
- Preserve saver product ownership while applying the canonical slug map.
- Preserve BenchLab as diagnostics-only while retiring the placeholder `gallery` and `player` app naming.
- Apply the renderer-tier ladder `gdi -> gl11 -> gl21 -> gl33 -> gl46 -> null` consistently in docs and identifiers where the public naming is visible.

## Migration Rules

- Old saver selection keys continue to resolve through the shared old-to-new alias table.
- Renamed savers load from legacy per-product registry roots and resave under canonical roots.
- The standalone saver line now emits one true `.scr` output per canonical saver slug; do not reintroduce a shared multi-saver distribution target as the public saver artifact model.
- `plasma` accepts the legacy `ember_lava` preset/theme key as a narrow migration alias.
- BenchLab retains `gl_plus` and `glplus` only as legacy command-line aliases that resolve to canonical `gl21`.
