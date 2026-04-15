# U03 UI And Runtime Truth

## Purpose

This note records how the `U03` dialog now maps onto the real Plasma runtime
and settings model.

## UI State Now Follows Runtime Truth

`U03` changes the controller in three important ways:

1. each surface is exact rather than cumulative
2. routine refreshes capture only the controls visible on the current surface
3. the info panel now surfaces deeper hidden state and major gating reasons

That means the dialog no longer rebuilds the working config from defaults plus
the preset bundle every time a visible control changes.

## Mapping From UI To Runtime Truth

- `Basic` writes the stable first-pass controls directly into their current
  settings-owned fields
- `Advanced` writes the active grammar fields directly into the current
  settings-owned runtime grammar
- `Author/Lab` writes the bounded curation, transition, seed-value, and
  diagnostics fields directly into the current product settings
- preset selection remains the one intentional bundle re-seeding path
- hidden surfaces are preserved intentionally in the working config instead of
  being silently reread from invisible widgets

## Degraded, Clamped, And Unsupported States

The dialog now exposes current truth more honestly:

- `Content Pool` and `Allow Transitions` now stay on `Advanced`, not `Basic`,
  because they materially alter selection and motion policy rather than acting
  like simple stable defaults
- `favorites_only` is now hidden from the dialog entirely; the filter remains
  real for imported config and BenchLab forcing, but the UI no longer pretends
  it is supportable without a truthful favorites editor
- transition policy, journey, fallback, seed continuity, and timing controls
  stay disabled until transitions are enabled and the transition surface exists
- `Journey` stays disabled unless authored journeys exist and the current policy
  allows journey-driven selection
- `Fixed Seed` stays disabled unless deterministic mode is currently enabled
- `Output Mode` stays disabled when the selected output family only has one
  supported mode

## Stable Versus Experimental Truth In The UI

The dialog now names broader grammar more honestly:

- non-raster output families and their non-native modes are labeled
  `Experimental`
- premium presentation modes are labeled `Premium`
- the info panel adds a lane-sensitive note when the current grammar requests
  broader effect, output, or presentation paths that may clamp on `gdi` or
  `gl11`

`Basic` itself remains the stable-first surface.
`U03` does not widen the stable cut just because `Advanced` and `Author/Lab`
are more honest now.

## Hidden Deeper State

The info panel now explicitly calls out:

- active preset and theme identity
- hidden Advanced overrides when the current surface is not `Advanced`
- hidden Author/Lab state when the current surface is not `Author/Lab`

That keeps the dialog from pretending the currently visible surface is the only
state that matters.

## Current Limitations

`U03` does not solve:

- visual distinctness between broader grammar combinations
- preset and theme retuning
- any hidden editor for favorites, exclusions, or BenchLab forcing
- screenshot-backed dialog automation

The UI now follows runtime truth instead of legacy assumptions, but later phases
still need to improve the product beneath that truth.
