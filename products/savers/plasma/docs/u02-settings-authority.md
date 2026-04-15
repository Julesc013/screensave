# Plasma U02 Settings Authority

## Purpose

This note records the `U02` settings-authority correction for Plasma.

`U02` exists to make one thing explicit and enforceable:
the compiled runtime grammar must come from one resolved settings model, not
from preset identity quietly acting as a second hidden plan.

This is a settings and runtime authority rewrite.
It is not the later UI redesign.

## Why U02 Exists Now

`U00` and `U01` left the product with one runtime, but the authority chain was
still too blurry.

Before this checkpoint:

- preset application still looked like the place where the real grammar became
  "true"
- product defaults and preset-default seeding were fused together
- the runtime plan consumed resolved settings, but that canonical handoff was
  not named as the single settings-to-plan authority boundary
- BenchLab mostly showed resolved grammar, which made it too easy to miss where
  a request had been clamped or degraded

`U02` turns that fuzzy middle into a named, documented, and reported authority
chain.

## Pre-U02 Settings Truth

Before this corrective pass, the repo already had material settings work in
place:

- `plasma_config` carried the major visual grammar fields
- `plasma_settings_resolve` already assembled those fields into one resolved
  settings record
- `plasma_plan_compile` already consumed that resolved record rather than
  looking the preset up again at compile time

What still remained was ambiguity:

- "apply preset" still sounded like runtime authority instead of curated bundle
  seeding
- product defaults and preset defaults were not described as separate layers
- requested-versus-resolved reporting for the full grammar subset was still too
  thin
- BenchLab presentation forcing could change the resolved plan without updating
  the requested presentation field that proof notes should inspect

## Post-U02 Settings Truth

After `U02`, Plasma has one explicit settings-authority chain:

1. shared and product defaults establish the floor
2. the active preset contributes a curated default bundle
3. persisted or imported values override the bundle field by field
4. session-local overrides and BenchLab config forcing override the persisted
   state where supported
5. `plasma_settings_resolve` produces the canonical requested settings record
6. `plasma_plan_apply_settings_resolution` copies that resolved settings record
   into the compiled plan
7. lane and capability binders may degrade unsupported requests, but the
   requested fields remain visible

That means the runtime no longer behaves like "preset identity plus whatever the
settings happened to change."
It behaves like one resolved settings model, with presets acting as curated
starting bundles.

## What It Means For Settings To Own The Visual Grammar

Concretely, `U02` now means all of the following:

- generator or effect family, output family, output mode, sampling treatment,
  filter treatment, emulation treatment, accent treatment, presentation mode,
  speed, detail, resolution, smoothing, and transition policy all pass through
  the same resolved-settings boundary before plan binding
- preset loading, dialog preset selection, and default-product startup all use
  an explicitly named preset-bundle helper instead of pretending that preset
  application is the final runtime grammar decision
- BenchLab snapshots and reports now export requested and resolved truth for the
  shipped grammar subset instead of only showing the final resolved state
- presentation forcing keeps the requested presentation visible even when the
  resolved lower-lane plan must fall back to `flat`

## What Preset Authority Was Removed Or Reduced

This checkpoint reduces preset-hidden authority by:

- splitting product defaults from preset-bundle seeding in
  `plasma_config_set_defaults`
- renaming the preset application helper to
  `plasma_apply_preset_bundle_to_config`, while preserving the older helper name
  as a compatibility wrapper
- routing module, config-load, and dialog preset-selection paths through that
  explicit bundle helper
- centralizing the settings-to-plan copy step in
  `plasma_plan_apply_settings_resolution`
- making BenchLab report the requested and resolved grammar side by side

Presets still matter.
They still choose identity, theme pairing, seed defaults, and a starting visual
bundle.
They no longer get to masquerade as the final compiled grammar authority.

## What Remains Intentionally Deferred To U03+

`U02` does not claim that later salvage work is finished.

It leaves these areas explicitly deferred:

- the user-facing settings-surface redesign in `U03`
- stronger keep, hide, or lab-only control decisions for weak settings surfaces
- broader output, treatment, and presentation strengthening
- preset and theme retuning
- later visual distinctness proof and recut work

Current bounded caveats still matter:

- `sampling_treatment` still resolves to `none` and remains a non-claim surface
- BenchLab `/plasma-preset:` forcing still overrides selection identity rather
  than reapplying the full preset bundle
- the current dialog still predates the salvage recut, so the runtime is now
  ahead of the UI

## Scope Boundary

`U02` is the settings-authority rewrite and preset-authority demotion phase.

It should not be read as:

- a UI redesign
- a broad output, treatment, or presentation retune
- a stable-widening decision
- a new feature tranche
