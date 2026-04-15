# Plasma U07 Stable Combination Matrix

## Purpose

This note records the stable reachable combination space that `U07` now treats
as an explicit proof target.

## Reachable Stable Combination Space

The current matrix is based on the actual first-class stable content surface
left after `U05` and retuned by `U06`.

Stable preset set: `classic_core`

- `plasma_lava`
- `aurora_plasma`
- `ocean_interference`
- `museum_phosphor`
- `quiet_darkroom`

Stable theme set: `classic_core`

- `plasma_lava`
- `aurora_cool`
- `oceanic_blue`
- `museum_phosphor`
- `quiet_darkroom`

Stable lanes covered here:

- `gdi`
- `gl11`

That yields `5 x 5 x 2 = 50` exhaustive stable rows.

## Exhaustive Proof In This Matrix

Every row in the current stable matrix is exercised by the deterministic smoke
matrix validator and reported in:

- `validation/captures/u07/stable-combination-matrix.txt`

The smoke harness treats these rows as real reachable product combinations,
not as an idealized grammar cross-product.

## Result Classes Used Here

This matrix uses the shared U07 result classes.

- `validated`: the row was exercised by the smoke matrix harness
- `partial`: the row is real but only partially proved in this phase
- `blocked`: the row would require an environment that is not currently present

In the current matrix, all 50 stable rows are `validated`.

## Deliberate Exclusions

The stable matrix deliberately excludes:

- advanced grammar cross-products
- premium-only presentation requests
- compatibility-only preset and theme keys
- hidden settings that survive only for compatibility
- broader experimental preset, theme, output, treatment, and presentation
  combinations

Those surfaces are handled by the bounded experimental coverage strategy rather
than by pretending they belong to the stable matrix.

## Current Interpretation

This matrix is the truthful stable proof floor for later phases.

If a later phase changes the first-class stable preset or theme surface, it
must either:

- update the matrix and the smoke validator, or
- explicitly demote the affected content out of this matrix

No silent drift is allowed.
