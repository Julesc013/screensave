# PX30 Stable Vs Experimental Notes

## Purpose

This note records the stable-versus-experimental posture for the new PX30 output and treatment surfaces.

## Newly Introduced PX30 Surfaces

- bounded experimental preset pool for contour, banded, and treatment studies
- `contour` output family subset:
  - `contour_only`
  - `contour_bands`
- `banded` output family subset:
  - `posterized_bands`
- treatment subset:
  - `glow_edge`
  - `halftone_stipple`
  - `emboss_edge`
  - `phosphor`
  - `crt`
  - `accent_pass`

## Stable Classification

Stable remains unchanged:

- default `plasma_lava` plus `plasma_lava`
- `raster` plus `native_raster` plus `flat`
- no-op treatment default
- truthful `gdi` floor and `gl11` preferred stable lane
- stable-only content filter by default

## Experimental Classification

Experimental in `PX30`:

- all new contour presets and contour output modes
- all new banded presets and banded output mode
- all new treatment families landed in this tranche
- the bounded experimental preset pool that carries those surfaces

## Why No Stable Widening Occurred

- the new surfaces are implemented and supportable, but still bounded
- proof remains narrower than the stable core baseline
- the new output and treatment grammar is explicitly opt-in through the experimental content slice

## Narrow Posture Update

`PX30` changes the current release posture only by making a bounded experimental content slice real.
It does not widen the stable default promise.

## Scope Boundary

If every new PX30 surface remains experimental, that is the truthful outcome.
This note records exactly that outcome.
