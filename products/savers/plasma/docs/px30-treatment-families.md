# PX30 Treatment Families

## Purpose

`PX30` widens Plasma through a bounded treatment-family subset with explicit slot placement and explicit degrade rules.
It does not turn Plasma into an unordered effect stack.

## Actual Treatment Families Landed

The implemented subset now includes:

- filter slot:
  - `glow_edge`
  - `halftone_stipple`
  - `emboss_edge`
  - existing advanced-only `blur`
- emulation slot:
  - `phosphor`
  - `crt`
- accent slot:
  - `accent_pass`
  - existing advanced-only `overlay_pass`

Unsupported and still rejected:

- `kaleidoscope_mirror`
- `restrained_glitch`
- any broader treatment inventory outside the bounded subset above

## Slot Placement

Each treatment remains tied to one fixed slot:

- `glow_edge`, `halftone_stipple`, `emboss_edge`, and `blur` are filter treatments
- `phosphor` and `crt` are emulation treatments
- `accent_pass` and `overlay_pass` are accent treatments

No treatment is allowed to float between slots and no effect-stack grammar is introduced here.

## Stable Vs Experimental Posture

- Stable:
  - `none`
  - the preserved stable default raster path with no-op treatment posture
- Experimental:
  - the new PX30 treatment subset
  - existing richer-lane `blur` and `overlay_pass` uplift behavior

No new treatment family is promoted into the stable default promise in this tranche.

## Degrade And Constraint Rules

- `blur` remains richer-lane-only and still degrades away honestly when advanced execution is unavailable.
- `overlay_pass` remains richer-lane-only and still tracks the advanced-lane uplift path.
- `glow_edge`, `halftone_stipple`, `emboss_edge`, `phosphor`, `crt`, and `accent_pass` are admitted on the bounded current subset without requiring a richer lane.
- Unsupported treatment values fail validation instead of being silently reinterpreted.

## What Remains Bounded Or Unsupported

- no broad post-processing zoo
- no open-ended treatment stacking rules
- no exhaustive lane-by-lane proof for every theoretical treatment combination
- no glyph-specific or new-field-specific treatment grammar

## Scope Boundary

These treatment families are bounded and curated.
`PX30` does not claim that every named taxonomy treatment now ships, and it does not redefine Plasma as a generic effect host.
