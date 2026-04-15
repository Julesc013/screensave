# Plasma U06 Post-Retuning Invariants

## Purpose

This note freezes the invariants that must hold after the `U06` retune.

Later proof and QA work may build on these invariants.
They must not silently change them.

## Default Stable Path Invariants

The following still hold after `U06`:

- product name remains `Plasma`
- `Plasma Classic` remains preserved as content identity and compatibility
  surface
- `ember_lava` still canonicalizes to `plasma_lava`
- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- minimum kind remains `gdi`
- preferred kind remains `gl11`
- quality class remains `safe`
- default stable grammar remains `fire` + `gentle` + `standard` + `soft` +
  `raster/native_raster` + no treatments + `flat`

## Retuned Content Invariants

- the first-class stable preset pool remains:
  `plasma_lava`, `aurora_plasma`, `ocean_interference`,
  `museum_phosphor`, and `quiet_darkroom`
- `midnight_interference` and `amber_terminal` remain compatibility-real by key
  even though they stay outside the first-class stable pools
- all surviving built-in themes remain resolvable by their existing keys
- no new preset families, theme families, or major controls are introduced by
  `U06`

## Distinctness Invariants

- the compiled preset audit must not regress back to any near-duplicate pairs at
  threshold `<= 2`
- the theme catalogue must stay materially more separated than the pre-U06 warm
  and monochrome clustering
- authored stable sets and journeys must continue to reflect the smaller stable
  catalogue instead of reintroducing demoted content as if it were first-class

These are retuning invariants, not the later formal screenshot-grade QA bar.

## Preview, Fullscreen, And Long-Run Invariants

- preview remains intentionally cheaper than fullscreen, but it must stay closer
  to the fullscreen intent than the pre-U06 preview path
- long-run pacing must remain calmer than the pre-U06 cadence for the surviving
  supported subset
- `deterministic_classic` and `dark_room_support` remain stable deterministic
  review profiles
- the default transition timing remains `14000ms / 1800ms` unless a later phase
  explicitly retunes it again with proof

## Lower-Band Degrade Invariants

- `gdi` remains a real floor, not a marketing fallback
- lower-band paths must stay honest about what they cannot do
- lower-band retuning must come from parameter and pacing work, not from hidden
  stable-scope widening

## What Later Phases May Assume

Later phases may now assume:

- the surviving preset and theme catalogue has been materially retuned
- the stable default path is stronger than it was at the end of `U05`
- preview-safe and dark-room support profiles have current intentional tuning
- the repo has enough capture and control-profile evidence to begin the later
  proof-harness pass

## What Later Phases Must Still Not Assume

Later phases must still not assume:

- final screenshot-grade acceptance has happened
- every surviving theme or experimental preset is guaranteed to stay
- BenchLab can force every preset bundle field independently
- preview, BenchLab, and fullscreen `.scr` host behavior are already proved
  equivalent
