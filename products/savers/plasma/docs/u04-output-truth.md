# Plasma U04 Output Truth

## Purpose

This note freezes the admitted output-family truth after the `U04` visual-grammar
rewire.

`U04` does not broaden Plasma's output grammar.
It narrows the truthful claim to the subset that now changes both the compiled
plan and the rendered result in a supportable way.

## Why U04 Exists

Pre-`U04`, the repo had output-family vocabulary spread across older `PX30` and
`PX31` docs, product settings, BenchLab text, and private helper logic.
The product could name more output breadth than it could enforce coherently.

The main problems were:

- support checks were split across local call sites instead of one canonical
  admitted subset
- unsupported output families and modes could still sound product-like in some
  reporting paths
- the surviving non-raster families were real enough to keep, but not yet
  documented as the only admitted subset

## Pre-U04 Output Truth

Before this checkpoint, the practical subset was already narrower than the
broader taxonomy:

- `raster`
- `banded`
- `contour`
- `glyph`

But that truth was not yet frozen in one canonical contract.
`surface` still existed in the taxonomy and enum space even though it was not a
truthful first-class shipped output family.

## Post-U04 Output Truth

The canonical admitted output subset is now:

| Family | Admitted mode subset | Post-U04 truth |
| --- | --- | --- |
| `raster` | `native_raster` | stable continuous-field baseline |
| `banded` | `posterized_bands` | harder quantized strata with stronger alternating accent and darkening |
| `contour` | `contour_only`, `contour_bands` | explicit isoline-only and band-plus-edge reads |
| `glyph` | `ascii_glyph`, `matrix_glyph` | bounded text-cell render subset with distinct ASCII and Matrix modes |

These families and modes now route through shared support helpers:

- `plasma_output_family_is_supported`
- `plasma_output_mode_is_supported`
- `plasma_output_family_supports_mode`
- `plasma_output_default_mode_for_family`
- `plasma_output_family_token`
- `plasma_output_mode_token`

That helper set is now the canonical admitted-subset contract for output truth.

## How Supported Output Families Differ Now

The surviving output families are expected to read differently:

- `raster` keeps the continuous scalar-field read
- `banded` now uses fewer, harder posterized steps with alternating bright and
  dark shaping so it does not collapse into a weak palette-only variant
- `contour_only` now reads as bright edge-only linework against black
- `contour_bands` now keeps stronger separation between band fill and contour
  edge accent
- `glyph` keeps the bounded cell-driven ASCII and Matrix modes rather than
  pretending to be a broad text renderer

The smoke harness now treats these as real visual-grammar switches and requires
their deterministic render signatures to differ materially from the raster
baseline.

## Unsupported Or Weak Output Claims

The following output claims remain explicitly out of the admitted subset:

- output family `surface`
- output mode `dithered_raster`
- surface-oriented output modes:
  - `heightfield_surface`
  - `curtain_surface`
  - `ribbon_surface`
  - `extruded_contour_surface`

Those names still exist in taxonomy and enum history, but `U04` does not claim
them as truthful output-family surfaces.
They resolve to `"unsupported"` tokens in the canonical output-token helpers and
they are not treated as supported output grammar.

## Degrade And Clamp Behavior

Output degrade is now intentionally blunt:

- unsupported output families clamp back to `raster`
- unsupported or family-mismatched output modes clamp to the default mode for
  the surviving family
- no silent fallback should preserve an unsupported family or mode label in the
  resolved plan

This is a smaller claim than the older taxonomy, but it is a more honest one.

## BenchLab, Settings, And Validation Truth

After `U04`, the same admitted subset is used by:

- settings/config clamping
- plan validation
- BenchLab token rendering
- smoke proof assertions

BenchLab still does not provide a standalone forcing surface for output-family
overrides in this phase.
That means output-family distinctness is proved primarily by the deterministic
smoke signature harness, while BenchLab remains responsible for reporting the
resolved output family and mode when settings drive the plan.

## U04 Boundary

`U04` makes output-family claims more real, not broader.

It does not:

- promote `surface` into the admitted shipped output subset
- add new output families
- retune every preset around the new output truth
- make final keep-or-remove decisions for later destructive simplification
