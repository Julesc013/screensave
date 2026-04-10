# Plasma PL11 Settings Surfaces

## Purpose

`PL11` makes Plasma's layered settings surfaces real at the product level.
It turns the previously flat Plasma config dialog and scattered product-local config assumptions into a bounded settings architecture with:

- a real `basic` surface
- a real `advanced` surface
- a real `author_lab` surface
- a real settings catalog and availability model behind those surfaces

This phase is deliberately bounded.
It is not full BenchLab integration, not a generic suite-wide settings law rewrite, and not a release-curation phase.

## Relationship To PL00-PL10

- `PL00` established that Plasma needed layered settings surfaces later.
- `PL01` froze the vocabulary and schema direction those settings would eventually point at.
- `PL02` preserved Plasma Classic identity and defaults.
- `PL03` through `PL05` made the core engine and lower-band baseline honest.
- `PL06` made content, sets, favorites, and exclusions real product-local concepts.
- `PL07` through `PL09` made the richer renderer lanes real.
- `PL10` made transition policy, journeys, and compatibility classes real.

`PL11` builds on that substrate by giving Plasma a real product-owned control surface and a real settings-resolution layer without claiming the deeper forcing and inspection work reserved for `PL12`.

## What “Settings Surfaces” Means For Plasma

For Plasma, a settings surface is not just a UI label.
It is a product-local contract that says:

- which setting belongs on which surface
- when that setting is actually available
- whether it is persisted or session-derived
- what runtime subsystem it affects
- whether the setting is intended to be BenchLab-addressable later

The implementation now has both sides:

- a `plasma_settings` descriptor catalog with surface and gating metadata
- a real settings dialog surface selector that shows and hides controls by surface

## Surface Model

### Basic

The Basic surface is intentionally small and safe.
The current implemented Basic subset is:

- preset selection
- theme selection
- speed selection

Basic is meant to keep the stable Plasma Classic experience simple.
It does not expose transition policy, content-channel filtering, or deeper authoring controls.

### Advanced

Advanced exposes stable power-user controls that remain honest for normal product use.
The current implemented Advanced subset adds:

- render detail
- effect family, including the bounded experimental PX31 field-family-I and PX32 field-family-II studies
- field resolution
- smoothing
- preset set
- theme set
- transitions enabled
- transition policy
- deterministic seed mode

This surface is still capability-aware and content-aware.
It does not claim every future output, treatment, presentation, or lane-forcing control already exists.

### Author/Lab

Author/Lab exposes the rest of the currently meaningful product-local controls without pretending to be BenchLab.
The current implemented Author/Lab subset adds:

- content pool filter
- favorites-only selection posture
- journey selection
- transition fallback policy
- transition seed continuity policy
- transition interval
- transition duration
- deterministic seed value
- diagnostics overlay

This is the deepest PL11 surface, but it is still bounded:

- no full telemetry inspector
- no lane forcing console
- no full BenchLab override stack
- no generic animation debugger

## Availability And Gating Rules

Plasma now gates settings honestly through the settings catalog and dialog refresh logic.

The current implemented rules include:

- `preset_set_key` is only available when the Plasma content registry actually has preset sets.
- `theme_set_key` is only available when the Plasma content registry actually has theme sets.
- `transitions_enabled` and `transition_policy` are only available when there is at least one real transition surface from sets or journeys.
- `journey_key` is only available when transitions are enabled and the selected transition policy can actually consume a journey.
- `deterministic_seed` is only available when deterministic seed mode is enabled.
- `favorites_only` is only available when favorite keys are actually configured.
- `content_filter` is only available when the registry actually contains experimental content.

The default Plasma pool remains stable-only, but `PX30` through `PX32` add bounded experimental preset slices, so the experimental content-pool control is now present in the model and available on the current Author/Lab surface.

## Persistence And Resolution Overview

PL11 makes Plasma's resolution order explicit in code:

1. product defaults
2. content defaults via the selected preset
3. persisted product-local user preferences
4. current dialog/session selections
5. settings-catalog availability checks and bounded clamping
6. runtime-plan compilation

The new `plasma_settings` resolution layer now feeds the runtime plan instead of leaving these decisions spread across dialog code and plan compilation.

Persistence is now intentionally split:

- user/product-local persisted state:
  - surface selection
  - current effect/speed/resolution/smoothing selections
  - set and transition preferences
  - deterministic seed mode/value
  - diagnostics overlay
- content-derived defaults:
  - preset and theme defaults
  - shared detail defaults that come from presets unless the user overrides them
- product-local preset import/export:
  - content selection metadata
  - transition metadata
  - existing product-local preset entries already supported before PL11
- session-only future seam:
  - later BenchLab overrides

PL11 does not claim that every persisted setting now exports through shared preset files.
For example, the last-used settings surface is product-local UI state and remains local to the product registry path rather than becoming fake shared config law.

## Preserved Default And Classic Invariants

PL11 keeps the default stable baseline intact:

- product name remains `Plasma`
- Plasma Classic remains preserved
- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- the default classic path remains `raster` + `native_raster` + `flat`

If users do nothing, the product still resolves to the same classic/default path as before this phase.

## Current Implemented Subset Vs Future Surface

Implemented now:

- layered surfaces in the product dialog
- settings descriptor catalog
- availability/gating helpers
- persisted last-used surface
- runtime settings resolution into the compiled plan
- bounded surface coverage for content, transition, deterministic-seed, and diagnostics settings

Intentionally deferred:

- full BenchLab UI and forcing
- full lane forcing
- every future output/treatment/presentation control
- deep performance/telemetry surfaces
- suite-wide settings-law generalization

## Known Limitations

- The current dialog uses one layered product dialog with show/hide behavior rather than separate per-surface windows.
- Favorites and exclusions remain product-local real settings, but PL11 only surfaces `favorites_only`; full favorite/exclusion list editing remains later work.
- Experimental content filtering is implemented in the model, and `PX30` through `PX32` now make it available through a bounded experimental preset slice while the default pool still remains stable-only.
- Advanced/modern/premium lane behavior remains capability-gated by the runtime engine; PL11 does not add a full lane-forcing UI.

## What Remained Intentionally Unchanged

- shared renderer and routing law
- shared config schema outer contract
- lower-band truthful baseline
- Plasma Classic defaults and aliases
- BenchLab depth and inspection tooling

## What PL12 May Build On Next

`PL12` can now build on a real product-owned settings substrate instead of inventing one.
The main new seams are:

- stable `setting_key` descriptors
- explicit surface metadata
- category and persistence metadata
- availability predicates already used by the product dialog
- runtime settings resolution already feeding the compiled plan
- BenchLab-exposable hints already attached to implemented settings

## Scope Boundary

`PL11` is the settings-surface phase for Plasma.
It is not full BenchLab integration, not a release-cut phase, and not a generic cross-suite settings rewrite.
