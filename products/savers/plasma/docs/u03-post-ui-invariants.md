# U03 Post-UI Invariants

## Purpose

This note freezes the invariants that must hold after the `U03` settings UI
recut.

## Default Stable Path Invariants

After `U03`, later phases may still assume:

- product name remains `Plasma`
- `Plasma Classic` remains preserved as content identity and compatibility
  surface
- `default_preset=plasma_lava`
- `default_theme=plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- the default stable path still resolves through the raster plus flat classic
  baseline with transitions disabled and stable-only content filtering

## Control-Surface Truth Invariants

After `U03`, the dialog must keep all of these true:

- each visible control belongs to the exact surface assigned by the settings
  catalog
- `Advanced` is not a second copy of `Basic`
- `Author/Lab` is not a second copy of `Basic` plus `Advanced`
- routine refreshes do not reread hidden controls
- preset selection remains the only intentional preset-bundle reseeding action
- `Output Mode` must disable when the current output family only admits one
  supported mode
- unavailable controls may remain visible, but they must disable honestly

## UI And Runtime Agreement Invariants

After `U03`, the dialog and runtime must agree on these rules:

- visible controls write into the actual settings-owned fields used by the
  current runtime plan
- hidden deeper state remains preserved intentionally, not accidentally
- the info panel must surface active preset/theme identity
- the info panel must surface hidden Advanced overrides when `Advanced` is not
  visible
- the info panel must surface hidden Author/Lab state when `Author/Lab` is not
  visible
- the info panel must surface major gating reasons for unavailable transition,
  content-pool, favorites, journey, seed, or lane-sensitive requests

## What Later Phases May Now Assume

Later corrective phases may now assume:

- the Plasma dialog has a real Basic, Advanced, and Author/Lab split
- the UI no longer relies on cumulative-surface behavior
- hidden-control rereads are no longer the default refresh behavior
- the currently hidden settings surface is explicit enough to support later keep
  versus hide versus remove decisions

## What Later Phases Must Still Not Assume

Later phases must still not assume:

- broader grammar is visually distinct enough
- current presets and themes are fully retuned
- every currently surfaced Advanced or Author/Lab control deserves to survive
  `U04` through `U06`
- the absence of screenshot automation means the dialog layout itself is fully
  capture-proven
