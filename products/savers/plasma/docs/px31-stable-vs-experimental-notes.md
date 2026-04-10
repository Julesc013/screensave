# PX31 Stable Vs Experimental Notes

## Purpose

This note records the stable-versus-experimental posture for the new PX31 glyph and field-family surfaces.

## Newly Introduced PX31 Surfaces

- bounded experimental preset additions:
  - `ascii_reactor`
  - `matrix_lattice`
  - `cellular_bloom`
  - `quasi_crystal_bands`
  - `caustic_waterlight`
- bounded `glyph` output family subset:
  - `ascii_glyph`
  - `matrix_glyph`
- bounded field-families-I subset:
  - `chemical_cellular_growth`
  - `lattice_quasi_crystal`
  - `caustic_marbling`

## Stable Classification

Stable remains unchanged:

- default `plasma_lava` plus `plasma_lava`
- `raster` plus `native_raster` plus `flat`
- no-op treatment default
- truthful `gdi` floor and `gl11` preferred stable lane
- stable-only content filter by default

## Experimental Classification

Experimental in `PX31`:

- all new glyph presets and glyph output modes
- all new field-families-I presets
- the new generator-family values admitted for those presets
- the growth of the bounded experimental preset slice that carries those surfaces

## Why No Stable Widening Occurred

- the new surfaces are implemented and supportable, but still bounded
- proof remains narrower than the stable core baseline
- the new glyph and field-family surfaces remain preset-driven and opt-in

## Narrow Posture Update

`PX31` changes the current release posture only by broadening the bounded experimental content and generator slice.
It does not widen the stable default promise.

## Scope Boundary

If every new PX31 surface remains experimental, that is the truthful outcome.
This note records exactly that outcome.
