# PX31 Glyph Family

## Purpose

`PX31` lands the first real glyph-family subset for Plasma.
It makes `glyph` an implemented output family for a bounded experimental slice without turning Plasma into a text renderer or a separate terminal-style saver.

## Why PX31 Exists Now

- `PX20` made the authored and validation substrate real enough for later-wave content truth.
- `PX30` made non-raster output seams real for contour and banded work.
- `glyph` was already part of the frozen taxonomy, but it remained taxonomy-only until this tranche.

## Relationship To PX30 And Later PX32 Work

- `PX30` remains the output-and-treatment widening tranche underneath this work.
- `PX31` uses those seams to make a bounded glyph subset real.
- `PX32` may build on this bounded glyph truth, but it should not have to invent basic glyph-family wiring again.

## Actual Glyph Capabilities Landed

The implemented subset is:

- `glyph` output family
- `ascii_glyph`
- `matrix_glyph`

Those surfaces are now:

- compiled into the runtime plan from a bounded experimental preset slice
- validated explicitly instead of being rejected as taxonomy-only
- rendered through real glyph-cell mapping rather than silent raster fallback
- visible in BenchLab snapshot and report truth

## How `glyph`, `ascii`, And `matrix` Differ

- `glyph` is the family name for symbol-driven output derived from the scalar field.
- `ascii_glyph` is density-ramp-first. It uses a static ASCII-like glyph ramp so brighter or more contoured cells resolve to denser symbols.
- `matrix_glyph` is motion-first. It uses a bounded columnar trail grammar so the field resolves through moving glyph columns instead of a static ASCII ramp.

`matrix_glyph` is not just “ASCII but green”, and neither mode is treated as a generator family.

## Settings And BenchLab Truth

`PX31` does not add a new glyph-specific settings UI.
The current supported subset is preset-driven:

- stable defaults remain `raster` plus `native_raster`
- the new glyph subset lives in the bounded experimental preset slice
- the existing `content_filter` surface is still the truthful gate for reaching that slice
- the existing `effect_mode` setting now includes the bounded experimental field-family-I values that feed some of those glyph presets

BenchLab now reports the supported subset through:

- `generator_family`
- `output_family`
- `output_mode`
- existing treatment-slot fields alongside them

## Degrade Policy For The Supported Subset

- The bounded glyph subset is honest on the lower bands and does not require premium-only presentation.
- Unsupported glyph combinations still fail validation instead of snapping back to raster silently.
- `ascii_glyph` and `matrix_glyph` are admitted only for the bounded implemented subset; broader glyph claims remain out of scope.

## What Remains Bounded Or Unsupported

- no broad text-rendering subsystem
- no large glyph alphabet inventory
- no Unicode or font-driven glyph work
- no glyph-specific transition expansion
- no surface-family or later dimensional work

## Scope Boundary

`PX31` is glyph and field expansion.
It is not later dimensional work, not a broader content-system redesign, and not a stable-default rewrite.
