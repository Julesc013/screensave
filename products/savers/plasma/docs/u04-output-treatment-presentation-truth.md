# Plasma U04 Output, Treatment, And Presentation Truth

## Purpose

This note records the first salvage pass that makes Plasma's claimed visual grammar more truthful in practice.

## Output Families Kept

The user-facing output families that remain first-class are:

- `raster`
- `banded`
- `contour`
- `glyph`

Each surviving family now has a clearer visual read:

- `raster` stays the continuous-field baseline
- `banded` now pushes stronger posterized strata instead of looking like a weak palette variation
- `contour_only` now reads as brighter edge-only linework
- `contour_bands` now keeps a clearer separation between the band fill and the contour accent
- `glyph` remains the ASCII and Matrix subset already proven in PX31

`surface` remains a non-claim surface.
It is not promoted into the settings story.

## Treatment Slots Kept

The bounded treatment slots that remain real are:

- filter: `none`, `blur`, `glow_edge`, `halftone_stipple`, `emboss_edge`
- emulation: `none`, `phosphor`, `crt`
- accent: `none`, `overlay_pass`, `accent_pass`

The salvage pass retuned the currently supported lower-band-safe treatments so they stop collapsing into subtle almost-no-ops:

- `glow_edge` now lifts edges more aggressively
- `halftone_stipple` now darkens the dropped samples more decisively
- `emboss_edge` now has a wider relief envelope
- `phosphor` now reads more like a scanlined monochrome display instead of a mild tint
- `crt` now reads as a stronger mask, scanline, and vignette treatment
- `accent_pass` now affects more of the bright structure instead of only the topmost peaks

## Presentation Truth

Presentation truth remains explicit:

- `flat` is the stable lower-band and `gl11` truth
- `heightfield`, `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface` remain premium-gated
- unsupported richer presentation requests still degrade explicitly back to `flat`

This pass does not widen the truthful presentation subset.
It only keeps the current subset explicit and aligned across plan compilation, BenchLab, and smoke coverage.

## BenchLab And Report Truth

For every surviving output, treatment, and presentation surface, BenchLab remains responsible for reporting:

- requested versus resolved versus degraded state
- generator family
- output family and mode
- sampling, filter, emulation, and accent treatment state
- presentation mode
- active clamps and lane degradations

The salvage work keeps those fields in place and updates smoke assertions where the preset retunes changed the honest resolved values.
