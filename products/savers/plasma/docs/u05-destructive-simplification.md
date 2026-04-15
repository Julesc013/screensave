# Plasma U05 Destructive Simplification

## Purpose

This note records the keep, hide, and non-claim decisions made during the salvage pass.

## Current U05 Notes

The detailed U05 records now live in:

- [`u05-simplification-decisions.md`](./u05-simplification-decisions.md)
- [`u05-dead-surface-cut.md`](./u05-dead-surface-cut.md)
- [`u05-post-simplification-invariants.md`](./u05-post-simplification-invariants.md)
- [`../tests/u05-simplification-proof.md`](../tests/u05-simplification-proof.md)

## Summary

`U05` turns the wider admitted runtime grammar into a smaller first-class
product surface:

- Basic now keeps only preset, theme, speed, intensity, and safe defaults
- content pool and transition gating move to Advanced
- `favorites_only` remains compatibility-real but stops pretending to be a
  supportable dialog toggle
- blur, overlay-pass, curtain, contour-extrusion, bounded-surface, substrate,
  and arc paths remain compatibility-only instead of first-class product
  breadth
- `midnight_interference`, `amber_terminal`, `aurora_curtain`,
  `substrate_relief`, and `filament_extrusion` remain resolvable but no longer
  sit in the first-class stable or experimental pools

## Boundary

`U05` is subtractive correction, not broad retuning.
It cuts the surface down to something worth tuning in `U06+`.
