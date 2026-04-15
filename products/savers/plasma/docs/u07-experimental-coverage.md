# Plasma U07 Experimental Coverage

## Purpose

This note records the bounded U07 proof strategy for the broader experimental
surface.

## Coverage Strategy

The current repo does not honestly support exhaustive proof for the full
experimental cross-product.

`U07` therefore uses a bounded covering-slice strategy:

- exhaustive proof for the stable reachable matrix
- bounded named slices for broader experimental grammar and transition surfaces

The current experimental collection anchor is `wave3_experimental_sampler`.

## Current Covered Slices

Preset slice:

- `ascii_reactor`
- `matrix_lattice`
- `cellular_bloom`
- `quasi_crystal_bands`
- `caustic_waterlight`
- `ribbon_aurora`

Theme slice:

- `aurora_cool`
- `oceanic_blue`
- `museum_phosphor`

Lane slice:

- `gdi`
- `gl11`
- `gl46`
- `auto`

Grammar slices:

- output families and modes:
  `banded/posterized_bands`,
  `contour/contour_only`,
  `contour/contour_bands`,
  `glyph/ascii_glyph`,
  `glyph/matrix_glyph`
- filter treatments:
  `glow_edge`,
  `halftone_stipple`,
  `emboss_edge`
- emulation treatments:
  `phosphor`,
  `crt`
- accent treatments:
  `accent_pass`
- presentation modes:
  `heightfield` and `ribbon` on premium-capable paths plus explicit lower-lane
  degrade captures
- transition slice:
  bounded `classic_cycle` journey behavior on the surviving stable core

## What Is Deliberately Not Covered

The current experimental coverage does not claim:

- exhaustive proof for every experimental preset and theme pairing
- exhaustive proof for every experimental grammar cross-product
- screenshot-grade proof for every renderer path
- first-class proof expectations for compatibility-only or hidden controls

Those are explicit non-claims for `U07`.

## Why This Boundary Is Honest

This boundary is honest because it prefers:

- strong proof for a smaller surface

over

- weak or fake exhaustive claims for a much broader one

Later phases may widen this coverage only if the repo can still keep the
boundary explicit and repeatable.
