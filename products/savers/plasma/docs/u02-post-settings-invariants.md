# Plasma U02 Post-Settings Invariants

## Purpose

This note freezes the invariants that must hold after `U02`.

Later phases may simplify, hide, retune, or recut.
They must not silently undo these authority guarantees.

## Default Stable Path Invariants

The preserved default stable path must continue to satisfy:

- default preset key is `plasma_lava`
- default theme key is `plasma_lava`
- routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`,
  `quality_class=safe`
- the default lower-band grammar remains `raster` + `native_raster` + `flat`
- the default path remains valid on both `gdi` and `gl11`

## Preset, Theme, And Alias Compatibility Invariants

These compatibility surfaces remain required unless a later phase records an
explicit migration:

- `ember_lava -> plasma_lava`
- Classic-content identity and catalog continuity
- preset and theme registry identities
- persisted preset and theme keys in config import or export flows
- preset bundles as curated starting defaults

## Settings-Authority Invariants

After `U02`, Plasma must continue to satisfy all of these:

- product defaults and preset-bundle seeding remain separate steps
- the compiled plan grammar is copied from `plasma_settings_resolution` through
  `plasma_plan_apply_settings_resolution`
- persisted or imported explicit field values override preset-bundle defaults
  before plan compilation
- requested grammar fields remain visible even when lower-lane or capability
  binders must degrade the resolved runtime state
- presets must not silently reassert ownership after explicit settings
  overrides have been applied

## Validation And Reporting Invariants

The validation and reporting truth after `U02` is:

- BenchLab snapshot and report surfaces export both requested and resolved truth
  for detail, generator, speed, resolution, smoothing, output family, output
  mode, treatment slots, and presentation mode
- presentation forcing must preserve requested presentation even when the
  resolved lower-lane plan falls back to `flat`
- settings-authority proof now belongs to the `U02` proof note, not only to the
  older PL11 settings surface note
- unsupported settings requests must still fail or degrade honestly

## What Later Phases May Assume

`U03+` may assume:

- one canonical settings-resolution order exists
- presets are curated defaults and compatibility bundles, not hidden plan
  masters
- the runtime plan exposes a coherent requested-settings subset for the major
  shipped grammar fields
- BenchLab can report requested-versus-resolved grammar truth for that subset

## What Later Phases Must Still Not Assume

`U03+` must still not assume:

- the dialog surface is already truthful
- every exposed control is worth keeping user-facing
- `sampling_treatment` is a real shipped grammar branch
- output, treatment, and presentation distinctness are fully retuned
- stable widening is justified

## Scope Boundary

These invariants freeze settings authority and reporting truth.

They do not replace the later U03 through U09 corrective obligations.
