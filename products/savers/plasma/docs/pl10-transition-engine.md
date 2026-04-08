# PL10 Transition Engine

Status: bounded runtime transition engine above the PL06 content system and PL07-PL09 lane stack.

## Purpose

`PL10` makes Plasma transitions real as a compiled runtime subsystem.
It adds bounded morph-capable transitions, explicit fallback behavior, and journey progression while keeping the public stable baseline centered on:

- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

## Relationship To PL00 Through PL09

- `PL00` froze the rule that Plasma transitions belong inside Plasma's field-derived identity and must not erase the lower-band floor.
- `PL01` froze the vocabulary for transition types, compatibility classes, fallback policy, and journeys.
- `PL02` preserved Plasma Classic as the compatibility slice that transition work must not overwrite.
- `PL03` created the runtime-plan seam transitions now extend.
- `PL04` made output, treatment, and presentation explicit stages that transition logic can now carry honestly.
- `PL05` proved the preserved lower-band baseline on `gdi` and `gl11`.
- `PL06` created the content registry, selection state, preset sets, and theme sets that transitions now consume.
- `PL07` through `PL09` created explicit lane-aware runtime profiles that transition support now respects and degrades across.

`PL10` therefore adds a product-local transition subsystem on top of the real content and lane model instead of inventing a disconnected animation layer.

## What “Transition Engine” Means For Plasma

For Plasma, the transition engine means:

- transitions are compiled into the runtime plan
- transition state lives explicitly in per-session execution state
- supported transitions use real Plasma concepts:
  - preset identity
  - theme identity
  - field-derived runtime state
  - treatment and presentation state where already honest
- unsupported pairs use explicit fallback policy instead of silently pretending every pair morphs

It does not mean:

- every preset/theme pair now morphs
- every transition is available on every renderer lane
- Plasma has gained public settings surfaces for the full transition matrix
- Plasma has gained BenchLab inspection depth or a general animation scripting system

## Supported Transition Types Implemented In PL10

Implemented now:

- `theme_morph`
  - real palette/theme morphing across compatible theme morph classes
- `preset_morph`
  - real direct preset morphing for compatible preset morph classes
  - current implementation morphs live theme colors plus speed and smoothing state through the transition window, then commits the target preset identity
- `bridge_morph`
  - bounded richer-lane bridge path for curated cross-family preset pairs that share a bridge class
  - bridge morphs commit the target preset at the bounded midpoint rather than pretending an incompatible pair can fully direct-morph
- `fallback_transition`
  - explicit fallback path when the requested preset pair is not directly morph-compatible but a compatible companion theme morph exists
- explicit `hard_cut`
- explicit `rejected`

Not implemented here:

- unrestricted arbitrary preset-pair morphing
- transition graphs across the full content registry
- generic fade overlays sold as “morphs”
- settings-surface exposure for every transition knob

## Unsupported Transition Types Or Pairings

PL10 does **not** claim:

- real preset morphs across incompatible preset morph classes
- bridge morphs on the lower bands
- theme morphs across incompatible theme morph classes
- premium-only dependency for all meaningful transition behavior

Unsupported requests fail honestly through explicit policy:

- `theme_morph` fallback when a companion compatible theme path exists
- `hard_cut` when the chosen fallback policy says to cut
- `rejected` when the chosen fallback policy says to reject

## Runtime-Plan Integration

The runtime plan now carries:

- whether transitions are requested
- whether transitions are enabled after content-surface and renderer-lane checks
- selected transition policy
- selected fallback policy
- selected seed continuity policy
- supported transition types for the active renderer lane
- transition interval
- transition duration
- optional resolved journey entry

The execution state now carries:

- active transition type
- requested and fallback transition type state
- supported-type flags
- elapsed and idle timing
- journey pointer and current journey-step index
- source and target preset/theme entries
- bridge midpoint state

## Transition Timing And Fallback Policy

PL10 uses two timing values:

- `transition_interval_millis`
- `transition_duration_millis`

For journey-driven transitions, the active journey step may override the plain interval with its own dwell.

Fallback policy is explicit:

- `hard_cut`
- `theme_morph`
- `reject`

Seed continuity is explicit:

- `keep_stream`
- `reseed_target`

Current `reseed_target` behavior is bounded and honest:

- if the target preset has a fixed seed, the transition commit reseeds against the target preset and current stream seed
- otherwise the live stream continues

## Per-Band Support And Degrade Policy

`universal -> gdi` and `compat -> gl11` support:

- theme morphs
- direct preset morphs
- explicit fallback handling

They do **not** support:

- bridge morphs

`advanced -> gl21`, `modern -> gl33`, and `premium -> gl46` support:

- theme morphs
- direct preset morphs
- bridge morphs
- explicit fallback handling
- journeys using the same bounded transition types

Degrade behavior is explicit:

- if a direct morph is unsupported but a compatible fallback theme morph exists, use `fallback_transition`
- if bridge morphing is requested on a lower band, drop to the declared fallback policy
- if the requested lane is unavailable, keep the previously established lane degrade path first, then apply transition support rules on the active lane

PL10 does not silently invent a premium dependency for transitions.

## Preserved Default And Classic Invariants

PL10 explicitly preserves:

- product name `Plasma`
- `Plasma Classic` as the preserved compatibility slice
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- non-transition default behavior when transitions are disabled
- the existing lower-band, advanced, modern, and premium lane contracts

When transitions are disabled or unsupported, the classic/default path remains materially unchanged:

- output family `raster`
- output mode `native_raster`
- presentation mode `flat`

## Current Known Limitations

- Theme morphing currently operates through theme color roles rather than a broader material-role system.
- Direct preset morphing is bounded to theme, speed, and smoothing continuity plus a target commit.
- Bridge morphing is intentionally limited to curated bridge-class pairs and uses a midpoint commit.
- Journeys are bounded ordered/weighted content progressions, not a generic choreography language.
- No public settings surface or BenchLab transition inspector is introduced here.
- The proof surface uses deterministic fake-renderer routing for richer-lane validation rather than live multi-context capture proof.

## What Remained Intentionally Unchanged

PL10 intentionally does not change:

- Plasma's public stable routing posture
- the preserved lower-band default behavior
- the PL07 advanced lane contract
- the PL08 modern lane contract
- the PL09 premium lane contract
- the current settings dialog surface
- shared platform law

## What PL11 May Build On Next

PL11 may expose bounded transition controls through settings surfaces and later PL12 may expose deeper inspection, but they no longer need to invent the engine substrate from scratch because PL10 now provides:

- transition plan fields
- transition runtime state
- compatibility-driven transition selection
- explicit fallback policy
- journey dwell and step progression
- real supported theme morphs, preset morphs, and bridge morphs for the implemented subset

## Scope Statement

`PL10` is a bounded transition-engine phase for Plasma.
It is not a settings-surface phase, not a BenchLab-depth phase, and not a generic animation-system rewrite.
