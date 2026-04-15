# Plasma U04 Output, Treatment, And Presentation Truth

## Purpose

This short note remains as the umbrella summary for the `U04` grammar-truth
checkpoint.

The detailed canonical notes now live in:

- [`u04-output-truth.md`](./u04-output-truth.md)
- [`u04-treatment-truth.md`](./u04-treatment-truth.md)
- [`u04-presentation-truth.md`](./u04-presentation-truth.md)
- [`u04-post-grammar-invariants.md`](./u04-post-grammar-invariants.md)

## Summary

`U04` does three things:

- narrows output truth to the admitted `raster`, `banded`, `contour`, and
  `glyph` subset
- narrows treatment truth to the admitted sampling, filter, emulation, and
  accent subset that can now be reported and validated coherently
- narrows presentation truth to the admitted flat-plus-bounded-premium subset
  with explicit premium and output-family gating

The proof note for this checkpoint is
[`../tests/u04-visual-grammar-proof.md`](../tests/u04-visual-grammar-proof.md).

The focused BenchLab captures for this checkpoint live in
[`../../../../validation/captures/u04/README.md`](../../../../validation/captures/u04/README.md).

## Boundary

This umbrella note is a compatibility summary only.

`U04` is the checkpoint that makes the admitted grammar subset more real, not
broader.
Later phases still own:

- destructive simplification
- preset and theme retuning
- broader proof harness work
