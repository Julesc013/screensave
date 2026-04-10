# PL10 Journeys And Compatibility

Status: bounded compatibility and journey model for the implemented PL10 transition subset.

## Purpose

This document freezes the PL10 compatibility model and the bounded journey surface that the transition engine actually uses now.
It separates what is implemented in the current runtime from broader future transition ambitions.

## Compatibility Class Model

PL10 uses two compatibility layers:

- preset morph class
- theme morph class

Current preset morph classes:

- `fire`
- `plasma`
- `interference`

Current theme morph classes:

- `warm`
- `cool`
- `phosphor`

Implemented rule:

- direct preset morphs require matching preset morph class
- direct theme morphs require matching theme morph class

Current built-in preset mapping:

- `plasma_lava` -> `fire`
- `quiet_darkroom` -> `fire`
- `aurora_plasma` -> `plasma`
- `museum_phosphor` -> `plasma`
- `amber_terminal` -> `plasma`
- `ocean_interference` -> `interference`
- `midnight_interference` -> `interference`

Current built-in theme mapping:

- `plasma_lava` -> `warm`
- `quiet_darkroom` -> `warm`
- `amber_terminal` -> `warm`
- `aurora_cool` -> `cool`
- `oceanic_blue` -> `cool`
- `midnight_interference` -> `cool`
- `museum_phosphor` -> `phosphor`

## Bridge Class Model

PL10 adds a second compatibility layer for bounded cross-family bridge transitions.

Current bridge classes:

- `warm_classic`
- `cool_field`

Implemented rule:

- bridge morphs require matching bridge class
- bridge morphs also require different preset morph classes
- bridge morphs are only available on richer lanes that actually advertise bridge support

Current built-in bridge mapping:

- `plasma_lava` -> `warm_classic`
- `quiet_darkroom` -> `warm_classic`
- `museum_phosphor` -> `warm_classic`
- `amber_terminal` -> `warm_classic`
- `aurora_plasma` -> `cool_field`
- `ocean_interference` -> `cool_field`
- `midnight_interference` -> `cool_field`

This means:

- `quiet_darkroom -> museum_phosphor` is bridge-capable on richer lanes
- `midnight_interference -> amber_terminal` is **not** bridge-capable because the bridge classes differ

## Preset, Theme, And Set Relationship Model

PL10 transition selection operates over existing PL06 content structures:

- selected preset
- selected theme
- optional active preset set
- optional active theme set
- optional journey

Implemented product-local preset sets relevant to PL10:

- `fire_classics`
- `plasma_classics`
- `interference_classics`
- `dark_room_classics`
- `classic_core`

Implemented product-local theme sets relevant to PL10:

- `warm_classics`
- `cool_classics`
- `dark_room_classics`
- `classic_core`

How the engine currently uses them:

- `theme_set` policy advances within the active theme set and keeps the current preset
- `preset_set` policy advances within the active preset set
- when `preset_set` policy also has an active companion theme set, the engine may use that theme set as the companion theme target for morph or fallback decisions
- `journey` policy resolves the active step to a concrete preset-set or theme-set policy

## Journey Model

PL10 journeys are bounded ordered sequences, not a scripting language.

Implemented journey entries:

- `classic_cycle`
- `dark_room_cycle`

Implemented journey-step fields:

- preset set key
- theme set key
- step policy
- step dwell

Current semantics:

- the journey selects one active step at a time
- that step supplies the concrete set keys and policy used for the next transition
- step dwell overrides the plain transition interval when present
- journey-step index advances only when the transition target is committed

This keeps journeys deterministic and content-local.

## Seed Continuity Model

PL10 supports:

- `keep_stream`
- `reseed_target`

Current implemented semantics:

- `keep_stream` leaves the current RNG stream intact across the transition
- `reseed_target` reseeds only when the target preset has a fixed seed

PL10 does not yet claim a richer continuity model for:

- field-history migration between arbitrary preset pairs
- cross-journey deterministic replay tooling
- BenchLab transition-seed inspection

## Stable Vs Experimental Journey Considerations

The default repo content pool remains stable-only, but `PX30` adds a bounded experimental preset slice for output and treatment studies.
PL10 therefore does not add an experimental journey pool or experimental-only transition matrix.

The current implemented subset stays inside stable classic content and stable classic-adjacent sets:

- no fabricated experimental content was added just to exercise journeys
- no experimental-only transition behavior is claimed

## Implemented Subset Vs Future Intended Surface

Implemented now:

- direct theme morphs for compatible theme-class pairs
- direct preset morphs for compatible preset-class pairs
- bounded bridge morphs for richer-lane pairs that share a bridge class
- explicit fallback transition policy
- explicit hard-cut and reject outcomes
- ordered journeys over curated preset/theme sets

Prepared for later but not implemented now:

- broader compatibility metadata across a larger content surface
- transition-set files as a distinct product-local content type
- richer bridge representations
- more advanced seed/history continuity handling
- public settings and BenchLab inspection depth

## Scope Statement

PL10 makes the compatibility and journey model runtime-real for a bounded supported subset.
It does not claim every set, every preset pair, or every theme pair is now morph-compatible, and it does not claim the later PL11+ control surfaces already exist.
