# PX30 Contour And Banded

## Purpose

`PX30` lands the first real Wave 3 output-language widening for Plasma.
It makes bounded contour and banded output grammars real without adding new field families, glyph work, or new dimensional modes.

## Why PX30 Exists Now

- `PX20` made the authoring and validation substrate real enough for bounded visual-language growth.
- Pre-`PX30` taxonomy already named `contour` and `banded`, but the runtime still treated non-raster output as unsupported.
- Later Wave 3 work needs honest non-raster output seams before glyph or new field-family expansion can land safely.

## Relationship To Wave 2 And Later Wave 3 Work

- `PX20` remains the authored-substrate foundation underneath this tranche.
- `PX30` is the first output-and-treatment widening tranche only.
- `PX31` may build on these output-family seams, but it should not need to reinvent contour or banded truth.

## Actual Capabilities Landed

The bounded implemented subset is:

- `contour` output family with:
  - `contour_only`
  - `contour_bands`
- `banded` output family with:
  - `posterized_bands`

Those surfaces are now:

- compiled into the runtime plan from bounded built-in presets
- validated explicitly rather than rejected as taxonomy-only
- visible in BenchLab snapshot, overlay, and report truth
- selectable through the bounded experimental content pool when the content filter allows it

## How Contour And Banded Differ

- `contour` is isoline-first. `contour_only` renders line-driven topographic structure on a dark background, while `contour_bands` combines filled bands with explicit contour edges.
- `banded` is quantized-fill-first. `posterized_bands` keeps the field as filled band regions rather than line extraction.

They are not synonyms and are not treated as interchangeable fallback labels.

## Settings And BenchLab Truth

`PX30` does not add a new output-settings surface.
The current supported path is preset-driven:

- the stable default path still resolves to `raster` plus `native_raster`
- the bounded experimental presets carry explicit contour or banded output intent
- `content_filter` in the existing Author/Lab surface now exposes the experimental slice honestly because experimental presets now exist

BenchLab now reports the actual resolved subset through:

- `profile_class`
- `output_family`
- `output_mode`
- the treatment-slot fields alongside them

## Degrade Policy For The Supported Subset

- The bounded `contour` and `banded` subset is lower-band-safe on `gdi` and `gl11`.
- Richer lanes preserve the explicit output family and mode for the admitted subset instead of silently snapping back to raster.
- Unsupported non-raster families or modes, especially `glyph` and `surface`, still fail validation honestly.
- No hidden fallback claims are made for output combinations that are still unsupported.

## What Remains Bounded Or Unsupported

- no glyph-family output
- no surface-family output
- no new field families
- no new dimensional modes
- no exhaustive non-raster mode matrix
- no dedicated end-user output-mode UI beyond the existing preset and content-filter surfaces

## Scope Boundary

`PX30` is output-and-treatment language expansion.
It is not new field-family work, not glyph work, and not a broader content-system redesign.
