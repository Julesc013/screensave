# PX11 Transition Coverage

## Purpose

`PX11` is the second Wave 1 Plasma tranche.
It broadens the bounded transition-support surface in honest, supportable ways and records the first stable-promotion review outcome without changing Plasma's default identity.

## Why PX11 Exists Now

Before `PX11`, the repo already had real transition behavior:

- direct theme morphs
- direct preset morphs
- richer-lane bridge morphs
- explicit fallback transitions
- explicit hard cuts and rejects
- curated journeys

That surface was real but still described mostly as a bounded subset.
`PX11` exists to replace vague bounded-language with a clearer supported graph, clearer fallback grammar, and clearer proof boundaries.

## Relationship To PX10 And Later Waves

- `PX10` hardened the support baseline and made the broader proof caveats explicit.
- `PX11` consumes that support posture and tightens the transition compatibility, fallback, and reporting story.
- `PX20` should consume the named compatibility classes, curated bridge-cycle sets, and explicit requested-versus-resolved reporting left here.

## Current Bounded Posture Before PX11

The pre-`PX11` repo already supported:

- direct preset morphs inside one preset morph class
- direct theme morphs inside one theme morph class
- bridge morphs on richer lanes when bridge class matched and preset morph class differed
- lower-band fallback when the target theme still had a real direct theme morph
- explicit hard-cut and reject outcomes
- `classic_cycle` and `dark_room_cycle`

The main remaining problem was not missing architecture.
It was support clarity:

- more of the graph needed to be named explicitly
- bridge-capable sequences needed clearer product-local curation
- hard-cut behavior needed stronger proof
- BenchLab needed clearer requested-versus-resolved transition truth

## Supported Transition Surface After PX11

`PX11` broadens the supported surface by adding curated compatibility consumers on top of the existing engine:

- `warm_bridge_classics`
- `cool_bridge_classics`
- `warm_bridge_cycle`
- `cool_bridge_cycle`

These do not create new transition families.
They curate more of the existing direct and bridge-capable surface into named paths that later work can target.

## Direct, Bridge, Fallback, And Hard-Cut Policy Summary

- Direct theme morph remains the honest path when the preset stays fixed and the source and target themes share one theme morph class.
- Direct preset morph remains the honest path when the source and target presets share one preset morph class.
- Bridge morph remains the honest richer-lane path when the source and target presets share one bridge class and differ in preset morph class.
- Lower-band cross-class steps do not fake bridge support. They either use the configured fallback theme morph when the target theme is still honestly direct-theme-compatible or they hard cut or reject.
- Hard cut remains the explicit truth path when the requested preset change is not honestly morphable and the configured fallback policy is `hard_cut`.

## What Was Actually Broadened

`PX11` actually broadened:

- the named transition compatibility graph through new curated warm and cool bridge-capable sets
- the named journey surface through `warm_bridge_cycle` and `cool_bridge_cycle`
- hard-cut proof coverage
- BenchLab transition truth by exporting requested, resolved, and fallback transition types separately

## What Remains Bounded

`PX11` does not claim:

- every preset pair morphs
- every theme pair morphs
- every bridge-class member can morph on lower bands
- exhaustive pairwise proof
- a new visual language
- new transition families

## Stable Identity Protection

`PX11` does not change:

- product name `Plasma`
- preserved `Plasma Classic`
- default preset `plasma_lava`
- default theme `plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- transitions-off-by-default stable posture

## Scope Boundary

`PX11` is support and posture work.
It does not widen Plasma's visual language, does not redesign the engine, and does not broaden stable scope by implication.
