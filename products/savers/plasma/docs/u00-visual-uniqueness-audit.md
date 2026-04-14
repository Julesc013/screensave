# Plasma U00 Visual Uniqueness Audit

## Purpose

This note freezes the current visual-distinctness diagnosis for Plasma.

It uses the current bounded proof surface:

- current preset and theme descriptors
- current runtime grammar
- current smoke-backed influence checks
- current `plasma_lab.py preset-audit` output

It does not pretend to be a universal perceptual-diff lab.

## Current Distinctness Posture

The current repo is not in the old state where almost all variety was fake.

Current truthful summary:

- stable presets are materially more distinct than they were before the salvage
  pass
- surfaced output, treatment, and presentation families are real enough to keep
- some preset and theme crowding still remains
- some breadth still survives only as unsupported or non-claim vocabulary

## Likely Near-Duplicate Presets And Themes

The current preset audit reports no exact duplicate compiled preset signatures.
It still reports close pairs that should stay under scrutiny.

### Closest Stable Pairs

- `amber_terminal` <-> `museum_phosphor`
- `midnight_interference` <-> `plasma_lava`
- `plasma_lava` <-> `quiet_darkroom`

These are not exact duplicates, but they are close enough that later tuning or
promotion work should not treat them as self-evidently distinct.

### Closest Experimental Or Cross-Family Pairs

- `aurora_curtain` <-> `cellular_bloom`

This pair is close at the compiled-signature level even though the named study
families differ.

### Theme Crowding Signals

The current theme-palette audit does not show exact duplicates, but it does show
nearer palette pairs such as:

- `amber_terminal` <-> `plasma_lava`
- `midnight_interference` <-> `oceanic_blue`
- `aurora_cool` <-> `oceanic_blue`

That is acceptable for a bounded palette family.
It is not acceptable as proof that theme identity alone creates major visual
separation.

## Output, Treatment, And Presentation Distinctions

### Keep

These distinctions are real enough to survive:

- output families `raster`, `banded`, `contour`, and `glyph`
- output modes `native_raster`, `posterized_bands`, `contour_only`,
  `contour_bands`, `ascii_glyph`, and `matrix_glyph`
- filter treatments `blur`, `glow_edge`, `halftone_stipple`, and `emboss_edge`
- emulation treatments `phosphor` and `crt`
- accent treatment `accent_pass`
- presentation modes `flat`, `heightfield`, `curtain`, `ribbon`,
  `contour_extrusion`, and `bounded_surface`

### Keep But Retune Or Gate Carefully

- `overlay_pass` is real, but it remains a more bounded advanced accent than the
  clearer `accent_pass`
- premium presentation modes are real, but they remain lane-sensitive and should
  stay explicitly bounded
- stable preset separation is improved, but some pairs still need continued
  retuning rather than automatic promotion to "obviously distinct"

### Hide Or Remove

The following breadth is still too weak or too fake to survive as a first-class
product claim:

- `PLASMA_OUTPUT_FAMILY_SURFACE`
- `PLASMA_OUTPUT_MODE_DITHERED_RASTER`
- `PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR`
- `PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH`
- `PLASMA_PRESENTATION_MODE_BOUNDED_BILLBOARD_VOLUME`
- `sampling_treatment` beyond `none`

These are not current product distinctions.
They are leftover vocabulary.

## Where Current Breadth Is Still Too Weak

- stable preset identity still leans partly on mood, pacing, and palette rather
  than on large grammar jumps
- theme uniqueness is bounded; palettes support mood separation more than they
  create a second independent grammar
- transition-facing visual differences are real but long-run and context
  dependent rather than immediate
- premium presentation differences remain supportable only as bounded
  experimental posture, not as broad stable truth

## Recommended Actions

| Surface | Recommendation | Why |
| --- | --- | --- |
| Stable preset family | Retune continuously | It is improved, but it still contains close pairs. |
| Stable themes | Keep | No exact duplicates, but do not overclaim theme-only distinctness. |
| Experimental study presets | Keep | They carry real bounded grammar differences and should stay explicit. |
| Supported output families and modes | Keep | They are real enough to justify their current surfaced status. |
| Supported treatment slots | Keep | They now produce meaningful distinctness. |
| Premium presentation modes | Keep experimental | Real, but still bounded by lane and proof depth. |
| Unsupported enum breadth | Hide or remove | It is paper breadth, not current product value. |

## Later Proof Requirements

Later proof work should not rely on naming or docs alone.
It should require, at minimum:

- continued smoke-backed influence checks for surfaced grammar controls
- continued `preset-audit` checks for exact duplicates and close pairs
- capture-backed review for stable presets under preview, fullscreen, dark-room,
  and long-run conditions
- explicit human visual review before any later stable widening

## U00 Conclusion

Current Plasma has real visual breadth, but it is still a bounded and curated
product rather than a limitless grammar space.
Later work should simplify and retune before it expands again.
