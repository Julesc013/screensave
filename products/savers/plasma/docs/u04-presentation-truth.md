# Plasma U04 Presentation Truth

## Purpose

This note freezes the admitted presentation-mode truth after the `U04`
visual-grammar rewire.

It records which presentation modes remain truthful now, how they are gated,
and how incompatible or lower-lane requests degrade.

## Pre-U04 Presentation Truth

Before `U04`, Plasma already had a bounded premium presentation subset, but the
truth still had rough edges:

- support checks were not fully centralized
- unsupported presentation names could still look more real than they were
- output-family compatibility for `contour_extrusion` was not enforced through
  one shared binding step across runtime and BenchLab forcing

## Post-U04 Presentation Truth

The canonical admitted presentation subset is now:

| Mode | Admission rule | Current truth |
| --- | --- | --- |
| `flat` | always admitted | stable lower-band and compat baseline |
| `heightfield` | premium only | bounded dimensional uplift |
| `curtain` | premium only | bounded premium presentation subset, compatibility-only after U05 |
| `ribbon` | premium only | bounded premium presentation subset |
| `contour_extrusion` | premium only and contour output only | bounded contour-only dimensional variant, compatibility-only after U05 |
| `bounded_surface` | premium only | bounded premium presentation subset, compatibility-only after U05 |

`bounded_billboard_volume` remains outside the admitted current subset and now
resolves to `"unsupported"` in the canonical token helper.

## Canonical Gating Rules

Presentation truth is now centralized in:

- `plasma_presentation_mode_is_supported`
- `plasma_presentation_mode_requires_premium`
- `plasma_presentation_mode_supports_output_family`
- `plasma_presentation_mode_token`
- `plasma_presentation_bind_plan`

That binding step is now part of both:

- normal plan binding
- BenchLab plan forcing

So the resolved plan no longer depends on presentation checks being duplicated
in separate call sites.

## How Supported Presentation Modes Differ

`U04` does not claim scenic or broad 3D breadth.
It keeps a bounded presentation subset and makes its truth tighter:

- `flat` remains the truthful stable baseline
- the premium non-flat subset remains explicitly premium-gated
- `contour_extrusion` now has a clear additional compatibility rule: it is only
  valid when the resolved output family is `contour`

The deterministic smoke harness now requires a premium presentation variant to
produce a materially different presented signature from its `flat` baseline.

`U05` narrows the first-class dialog subset to `flat`, `heightfield`, and
`ribbon`.
`curtain`, `contour_extrusion`, and `bounded_surface` remain real compatibility
paths for explicit imported settings and preset compatibility, but they are no
longer promoted as first-class product breadth.

## Degrade And Clamp Behavior

Presentation degrade is now explicit:

- unsupported presentation names resolve to `flat`
- premium-only presentation requests on non-premium lanes resolve to `flat`
- `contour_extrusion` on non-contour output resolves to `flat`
- requested presentation state remains visible in BenchLab even when the
  resolved plan clamps back to `flat`

This gives Plasma an explicit degrade ladder instead of silent collapse.

## BenchLab And UI Truth

`U04` keeps requested versus resolved presentation truth explicit:

- `validation/captures/u04/benchlab-plasma-presentation-ribbon.txt` shows a
  premium `ribbon` request resolving cleanly to `ribbon`
- `validation/captures/u04/benchlab-plasma-degrade-ribbon-auto.txt` shows a
  premium `ribbon` request degrading back to `flat` on the lower-band auto path
- `validation/captures/u04/benchlab-plasma-degrade-contour-extrusion-raster.txt`
  shows an incompatible `contour_extrusion` request resolving back to `flat`
  while keeping the requested mode visible

The settings dialog also now hides `contour_extrusion` when the effective output
family is not `contour`, which keeps the surface claim aligned with runtime
truth.

## U04 Boundary

`U04` makes presentation claims more real, not broader.

It does not:

- add new presentation families
- widen the admitted premium subset
- promote billboard-style volume or scenic presentation into the current truth
- retune all premium presets around presentation identity
