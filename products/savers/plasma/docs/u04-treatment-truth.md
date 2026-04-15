# Plasma U04 Treatment Truth

## Purpose

This note freezes the admitted treatment-family truth after the `U04`
visual-grammar rewire.

It records which treatment slots are genuinely supported now, how they differ,
and which historical treatment names remain outside the admitted subset.

## Pre-U04 Treatment Truth

Before `U04`, Plasma had a bounded treatment subset in code, but the truth was
still scattered:

- support checks lived in multiple places
- unsupported treatment names could still sound more real than they were
- several surviving treatments were technically present but visually too mild
  to justify the breadth suggested by the docs

## Post-U04 Treatment Truth

The canonical admitted treatment subset is now:

| Slot | Admitted subset | Notes |
| --- | --- | --- |
| sampling | `none` | current shipped sampling treatments remain intentionally non-claim beyond no-op |
| filter | `none`, `blur`, `glow_edge`, `halftone_stipple`, `emboss_edge` | `blur` remains advanced-only |
| emulation | `none`, `phosphor`, `crt` | lower-band-safe bounded display identities |
| accent | `none`, `overlay_pass`, `accent_pass` | `overlay_pass` remains advanced-only |

These slot claims are now frozen behind shared helper functions:

- `plasma_sampling_treatment_is_supported`
- `plasma_filter_treatment_is_supported`
- `plasma_filter_treatment_requires_advanced`
- `plasma_emulation_treatment_is_supported`
- `plasma_accent_treatment_is_supported`
- `plasma_accent_treatment_requires_advanced`
- the matching token helpers for all four slots

## How Supported Treatments Differ Now

`U04` strengthens the admitted treatment subset rather than widening it.

The concrete changes are:

- `glow_edge` lifts edges more aggressively instead of reading as a faint wash
- `halftone_stipple` darkens dropped samples more decisively
- `emboss_edge` widens the relief envelope so its structural read is clearer
- `phosphor` now reads as a stronger scanlined monochrome display identity
- `crt` now uses a stronger mask, scanline, and vignette profile
- `accent_pass` now catches more of the bright structure instead of only the
  highest peaks

The deterministic smoke harness now requires representative treatment variants
to differ materially from the untreated baseline.

## Unsupported Or Bounded Treatment Claims

The following treatment names remain outside the admitted shipped subset:

- sampling:
  - `nearest`
  - `soft`
  - `dither`
- filters:
  - `kaleidoscope_mirror`
  - `restrained_glitch`

Those names still exist as historical taxonomy or later-phase possibilities, but
they are not claimed as truthful current treatment breadth.
Their canonical token helpers now report `"unsupported"` rather than preserving
product-like labels in reporting.

## Slot Truth And Degrade Behavior

Treatment truth is now explicit:

- unsupported treatments clamp to `none`
- `blur` clamps to `none` outside advanced-capable lanes
- `overlay_pass` clamps to `none` outside advanced-capable lanes
- other admitted lower-band-safe treatments remain available on the truthful
  lower subset

This preserves the lower-band safe baseline without pretending every treatment
exists everywhere.

## Settings, Validation, And BenchLab Truth

After `U04`, the same admitted treatment subset is used by:

- config clamping
- plan validation
- BenchLab token rendering
- smoke proof assertions

BenchLab forcing still does not expose independent treatment-slot overrides in
this phase.
That means treatment distinctness is proved primarily by deterministic smoke
signatures, while BenchLab remains responsible for truthful reporting when the
resolved settings plan already includes an admitted treatment combination.

## U04 Boundary

`U04` makes treatment-family claims more real, not broader.

It does not:

- create a large effect zoo
- promote unsupported sampling or novelty filters into the admitted subset
- perform the later keep, merge, or remove pass for the broader treatment
  taxonomy
