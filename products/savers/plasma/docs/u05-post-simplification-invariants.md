# Plasma U05 Post-Simplification Invariants

## Purpose

This note freezes the invariants that must hold after the `U05` subtractive cut.

Later phases may now assume the smaller first-class surface below.

## Default Stable Path Invariants

After `U05`, all of the following must still hold:

- product name remains `Plasma`
- `Plasma Classic` remains preserved as content identity and compatibility
  surface
- `default_preset=plasma_lava`
- `default_theme=plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- stable default output remains `raster / native_raster`
- stable default presentation remains `flat`

## First-Class Surface Invariants

After `U05`, the first-class product surface is explicitly smaller:

- Basic keeps:
  - `preset_key`
  - `theme_key`
  - `speed_mode`
  - `detail_level`
  - safe defaults
- Advanced keeps:
  - `content_filter`
  - `transitions_enabled`
  - `effect_mode`
  - `output_family`
  - `output_mode`
  - `resolution_mode`
  - `smoothing_mode`
  - `filter_treatment`
  - `emulation_treatment`
  - `accent_treatment`
  - `presentation_mode`
  - `use_deterministic_seed`
- Author/Lab keeps:
  - `preset_set_key`
  - `theme_set_key`
  - `transition_policy`
  - `journey_key`
  - `transition_fallback_policy`
  - `transition_seed_policy`
  - `transition_interval_millis`
  - `transition_duration_millis`
  - `deterministic_seed`
  - `diagnostics_overlay_enabled`

The dialog no longer treats `favorites_only` as a first-class control.

## First-Class Grammar Invariants

After `U05`, the first-class grammar surface is:

- effects:
  - fire
  - plasma
  - interference
  - chemical
  - lattice
  - caustic
  - aurora
- outputs:
  - raster
  - banded
  - contour
  - glyph
- first-class filter treatments:
  - none
  - glow_edge
  - halftone_stipple
  - emboss_edge
- first-class emulation treatments:
  - none
  - phosphor
  - crt
- first-class accent treatments:
  - none
  - accent_pass
- first-class presentation modes:
  - flat
  - heightfield
  - ribbon

## Compatibility Invariants

After `U05`, all of the following must still remain true:

- `ember_lava -> plasma_lava` still resolves
- explicit preset and theme keys for demoted classics still resolve
- hidden compatibility values can still survive imported config
- the dialog shows the current hidden compatibility value explicitly instead of
  silently losing it
- requested versus resolved versus degraded reporting remains explicit when a
  hidden compatibility value survives into the resolved plan

## Authored Content Invariants

After `U05`, stable-facing authored sets and collections must not route through
the demoted first-class cuts:

- stable classic pools no longer include `midnight_interference` or
  `amber_terminal`
- the Wave 3 experimental sampler no longer promotes `aurora_curtain`,
  `substrate_relief`, or `filament_extrusion`
- the dark-room support profile no longer routes through the demoted amber
  classic preset

## What Later Phases May Assume

Later phases may now assume:

- the first-class surface is smaller than the full supported compatibility
  grammar
- compatibility-only values are now explicit rather than silently coequal with
  first-class values
- the stable first-class preset and theme pool is narrow enough for serious
  retuning work

## What Later Phases Must Still Not Assume

Later phases must still not assume:

- every compatibility-only value should survive permanently
- U05 has already retuned the surviving presets or themes
- the current first-class surface is already the final stable or experimental
  recut
- the repo now has a universal screenshot or perceptual-diff proof layer
