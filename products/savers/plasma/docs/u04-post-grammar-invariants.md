# Plasma U04 Post-Grammar Invariants

## Purpose

This note freezes the invariants that must hold after the `U04` output,
treatment, and presentation rewiring checkpoint.

Later phases may use these invariants as the canonical grammar-truth baseline.

## Output, Treatment, And Presentation Invariants

After `U04`, all of the following must remain true:

- the admitted output families are exactly:
  - `raster`
  - `banded`
  - `contour`
  - `glyph`
- the admitted output modes are exactly:
  - `native_raster`
  - `posterized_bands`
  - `contour_only`
  - `contour_bands`
  - `ascii_glyph`
  - `matrix_glyph`
- output family `surface` and its surface-oriented output modes remain outside
  the admitted current subset
- the admitted sampling treatment subset is exactly `none`
- the admitted filter subset is exactly:
  - `none`
  - `blur`
  - `glow_edge`
  - `halftone_stipple`
  - `emboss_edge`
- the admitted emulation subset is exactly:
  - `none`
  - `phosphor`
  - `crt`
- the admitted accent subset is exactly:
  - `none`
  - `overlay_pass`
  - `accent_pass`
- the first-class user-facing filter subset after `U05` is:
  - `none`
  - `glow_edge`
  - `halftone_stipple`
  - `emboss_edge`
- the first-class user-facing accent subset after `U05` is:
  - `none`
  - `accent_pass`
- unsupported sampling, novelty filters, and unsupported presentation names are
  not treated as silently supported grammar
- the admitted presentation subset is exactly:
  - `flat`
  - `heightfield`
  - `curtain`
  - `ribbon`
  - `contour_extrusion`
  - `bounded_surface`
- the first-class user-facing presentation subset after `U05` is exactly:
  - `flat`
  - `heightfield`
  - `ribbon`
- `contour_extrusion` is only valid when the resolved output family is
  `contour`

## Settings, Runtime, Validation, And Reporting Invariants

After `U04`, all of the following must remain aligned:

- config clamping uses the same admitted grammar subset as runtime validation
- plan validation rejects unsupported output, treatment, and presentation
  combinations rather than merely accepting older taxonomy names
- BenchLab token rendering reports unsupported surfaces as `"unsupported"`
  instead of preserving product-like names for non-claims
- BenchLab presentation forcing passes through the same binding logic as normal
  runtime plan binding
- requested versus resolved versus degraded presentation truth remains visible
  in BenchLab captures

## Degrade-Behavior Invariants

After `U04`, degrade behavior must stay explicit:

- unsupported output families clamp to `raster`
- unsupported or family-mismatched output modes clamp to the default mode for
  the resolved family
- unsupported treatments clamp to `none`
- advanced-only `blur` and `overlay_pass` clamp to `none` outside advanced
  lanes
- compatibility-only `blur`, `overlay_pass`, `curtain`, `contour_extrusion`,
  and `bounded_surface` remain importable or preset-resolvable, but they are
  no longer first-class dialog choices after `U05`
- unsupported presentation modes clamp to `flat`
- premium-only presentation modes clamp to `flat` outside premium
- `contour_extrusion` clamps to `flat` whenever the resolved output family is
  not `contour`
- clamp summaries remain visible in BenchLab when forcing requests a mode that
  resolves differently

## Stable Default Path Invariants

The stable default baseline remains unchanged:

- `default_preset=plasma_lava`
- `default_theme=plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- stable default output remains `raster / native_raster`
- stable default treatments remain `none | none | none | none`
- stable default presentation remains `flat`

`Plasma Classic` still survives as content identity and compatibility surface,
not as a separate runtime regime.

## What Later Phases May Assume

Later phases may now assume:

- the admitted output, treatment, and presentation subset is centralized in the
  shared helper functions introduced or tightened in `U04`
- the smoke harness already requires representative deterministic render
  signatures to differ for the admitted non-baseline output, treatment, and
  premium-presentation subset
- BenchLab captures can prove presentation requested-versus-resolved truth for
  the admitted forcing subset

## What Later Phases Must Still Not Assume

Later phases must still not assume:

- every historical taxonomy name is now a real shipped grammar surface
- the current admitted subset has already been through the final keep, merge,
  hide, or remove pass
- the current subset has already been visually retuned
- BenchLab can independently force every output or treatment slot
- the repo now has a universal screenshot or perceptual-diff framework
