# Plasma PL11 Settings Catalog

## Purpose

This document freezes the canonical PL11 settings descriptor model and records the bounded settings subset that Plasma actually exposes now.
It separates:

- implemented settings descriptors
- implemented surface assignment and persistence rules
- later BenchLab-facing seams that now exist but are not fully integrated yet

## Canonical Descriptor Model

Each implemented Plasma setting is now representable with descriptor metadata including:

- `setting_key`
- `display_name`
- `summary`
- `surface`
- `category_key`
- `value_type`
- `default_value_text`
- `domain_summary`
- `persistence_scope`
- `affects_mask`
- `benchlab_exposable`

This model lives in the product-local `plasma_settings` module.

## Category Model

The current implemented categories are:

- `content`
- `motion`
- `render`
- `generator`
- `treatment`
- `selection`
- `transition`
- `seed`
- `diagnostics`

These are product-local categories.
PL11 does not claim they are already a shared suite-wide catalog law.

## Surface Assignment Rules

Current rules:

- `basic` is for safe day-to-day control
- `advanced` is for stable power-user control
- `author_lab` is for deeper product-local authoring control

Surface assignment is cumulative in the current dialog:

- `basic` shows only Basic settings
- `advanced` shows Basic + Advanced settings
- `author_lab` shows Basic + Advanced + Author/Lab settings

## Persistence-Scope Model

The current implemented persistence scopes are:

- `user`
  - persisted product-local user state
- `session`
  - reserved for later session-only settings or overrides
- `content`
  - content-derived defaults or content-owned meaning

Current shipped PL11 usage is centered on `user` persistence.
The dialog surface selection itself is also persisted product-locally, but it is UI metadata rather than a runtime-affecting catalog setting.

## Availability And Gating Model

Availability is resolved product-locally from:

- content registry shape
- whether favorites are configured
- whether transition surfaces exist
- whether a journey surface exists
- deterministic seed mode
- current surface selection

Current important gates:

- `content_filter` is available only when experimental content exists; `PX30`, `PX31`, and `PX32` make that true through bounded experimental preset slices while the default pool still remains stable-only.
- `favorites_only` is unavailable until favorite keys are actually configured.
- `journey_key` is unavailable unless transitions are enabled and the transition policy can consume a journey.
- `deterministic_seed` is unavailable until deterministic seed mode is enabled.

## Reset And Default Semantics

Current PL11 reset semantics:

- the dialog `Defaults` action restores the safe product defaults
- the currently selected settings surface is preserved so users remain on the same layered view
- the runtime default baseline remains the preserved Plasma Classic path

## Future BenchLab Seam

PL11 does not implement full BenchLab integration.
It does leave a real seam for it through:

- stable `setting_key` values
- explicit `surface` metadata
- explicit `category_key`
- explicit availability rules
- explicit persistence scope
- `benchlab_exposable` metadata on the implemented settings subset

## Implemented Settings Catalog

| Setting Key | Surface | Category | Type | Default | Persistence | Availability Rule | Affects |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `preset_key` | `basic` | `content` | `content_key` | `plasma_lava` | `user` | always | content selection, plan |
| `theme_key` | `basic` | `content` | `content_key` | `plasma_lava` | `user` | always | content selection, plan |
| `speed_mode` | `basic` | `motion` | `enum` | `gentle` | `user` | always | plan, execution |
| `detail_level` | `advanced` | `render` | `enum` | `standard` | `user` | always | plan |
| `effect_mode` | `advanced` | `generator` | `enum` | `fire` | `user` | always | plan, execution, bounded experimental generator-family selection |
| `resolution_mode` | `advanced` | `render` | `enum` | `standard` | `user` | always | plan, execution |
| `smoothing_mode` | `advanced` | `treatment` | `enum` | `soft` | `user` | always | plan, presentation |
| `preset_set_key` | `advanced` | `selection` | `content_key` | empty | `user` | requires real preset sets | content selection, transition |
| `theme_set_key` | `advanced` | `selection` | `content_key` | empty | `user` | requires real theme sets | content selection, transition |
| `transitions_enabled` | `advanced` | `transition` | `bool` | `false` | `user` | requires at least one transition surface | plan, transition |
| `transition_policy` | `advanced` | `transition` | `enum` | `disabled` | `user` | requires transitions surface | plan, transition |
| `use_deterministic_seed` | `advanced` | `seed` | `bool` | `false` | `user` | always | plan, execution |
| `content_filter` | `author_lab` | `selection` | `enum` | `stable_only` | `user` | requires experimental content in registry | content selection |
| `favorites_only` | `author_lab` | `selection` | `bool` | `false` | `user` | requires configured favorite keys | content selection |
| `journey_key` | `author_lab` | `transition` | `content_key` | empty | `user` | requires enabled journey-capable transitions | plan, transition |
| `transition_fallback_policy` | `author_lab` | `transition` | `enum` | `hard_cut` | `user` | requires enabled transitions | plan, transition |
| `transition_seed_policy` | `author_lab` | `transition` | `enum` | `keep_stream` | `user` | requires enabled transitions | plan, transition |
| `transition_interval_millis` | `author_lab` | `transition` | `unsigned` | `14000` | `user` | requires enabled transitions | plan, transition |
| `transition_duration_millis` | `author_lab` | `transition` | `unsigned` | `1800` | `user` | requires enabled transitions | plan, transition |
| `deterministic_seed` | `author_lab` | `seed` | `unsigned` | `0` | `user` | requires deterministic seed mode | plan, execution |
| `diagnostics_overlay_enabled` | `author_lab` | `diagnostics` | `bool` | `false` | `user` | always | diagnostics |

## Implemented Now Vs Prepared For Later

Implemented now:

- the descriptor model above
- surface assignment and gating
- runtime settings resolution
- real product dialog surfaces for the implemented subset

Prepared for later but not claimed as fully implemented:

- deeper BenchLab-specific forcing and inspection
- broader lane-selection controls
- wider output/treatment/presentation control coverage beyond the current preset-driven non-raster subset
- richer favorite/exclusion list editing
- future settings that depend on later content or performance work

## Scope Boundary

This catalog freezes the implemented PL11 subset.
It does not claim every future Plasma parameter is already surfaced, and it does not claim full BenchLab integration already exists.
