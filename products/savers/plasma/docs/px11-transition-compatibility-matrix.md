# PX11 Transition Compatibility Matrix

## Purpose

This document names the supported transition classes and fallback grammar that are genuinely implemented after `PX11`.
It is a support matrix, not a promise that every theoretical pair now morphs.

## Compatibility Classes Used

Preset morph classes:

- `fire`: `plasma_lava`, `quiet_darkroom`
- `plasma`: `aurora_plasma`, `museum_phosphor`, `amber_terminal`
- `interference`: `ocean_interference`, `midnight_interference`

Theme morph classes:

- `warm`: `plasma_lava`, `quiet_darkroom`, `amber_terminal`
- `cool`: `aurora_cool`, `oceanic_blue`, `midnight_interference`
- `phosphor`: `museum_phosphor`

Bridge classes:

- `warm_classic`: `plasma_lava`, `quiet_darkroom`, `museum_phosphor`, `amber_terminal`
- `cool_field`: `aurora_plasma`, `ocean_interference`, `midnight_interference`

## Supported Direct-Morph Classes

Direct preset morphs are supported when the source and target presets share one preset morph class:

- `fire`: `plasma_lava <-> quiet_darkroom`
- `plasma`: `aurora_plasma <-> museum_phosphor <-> amber_terminal`
- `interference`: `ocean_interference <-> midnight_interference`

Direct theme morphs are supported when the source and target themes share one theme morph class:

- `warm`: `plasma_lava <-> quiet_darkroom <-> amber_terminal`
- `cool`: `aurora_cool <-> oceanic_blue <-> midnight_interference`

`museum_phosphor` is a real theme, but it has no second `phosphor` peer, so it does not create a broader direct theme morph family today.

## Supported Bridge-Morph Classes

Bridge morphs are supported only on richer lanes that advertise bridge support.
They also require matching bridge class and different preset morph classes.

Implemented warm bridge-capable crossings:

- `plasma_lava <-> museum_phosphor`
- `plasma_lava <-> amber_terminal`
- `quiet_darkroom <-> museum_phosphor`
- `quiet_darkroom <-> amber_terminal`

Implemented cool bridge-capable crossings:

- `aurora_plasma <-> ocean_interference`
- `aurora_plasma <-> midnight_interference`

The new curated consumers for those paths are:

- `warm_bridge_classics`
- `cool_bridge_classics`
- `warm_bridge_cycle`
- `cool_bridge_cycle`

## Unsupported Pair Classes

The following still remain unsupported as real morph claims:

- preset pairs that do not share either one preset morph class or one bridge class
- lower-band bridge-morph claims
- direct theme morphs across different theme morph classes
- any exhaustive all-to-all preset or theme matrix claim

Examples of unsupported cross-class or cross-bridge claims:

- `midnight_interference -> amber_terminal`
- `ocean_interference -> museum_phosphor`
- `museum_phosphor -> aurora_cool` as a direct theme morph

## Fallback And Hard-Cut Mapping

When a requested preset change is not directly or bridge-morph compatible:

- if the configured fallback policy is `theme_morph` and the target theme is still honestly direct-theme-compatible, the engine resolves to `fallback_transition`
- if the configured fallback policy is `hard_cut`, the engine resolves to `hard_cut`
- if the configured fallback policy is `reject`, the engine resolves to `rejected`

`PX11` now records:

- requested transition type
- resolved transition type
- fallback transition type

separately in BenchLab reporting so unsupported morphs do not masquerade as direct support.

## Lane And Degrade Notes

- `gdi` and `gl11` support direct theme morphs, direct preset morphs, and explicit fallback behavior.
- `gl21`, `gl33`, and `gl46` support the same lower-band surface plus bridge morphs.
- Lower-band bridge-capable journey steps remain honest fallback or hard-cut cases, not hidden bridge support.

## Implemented Versus Not Supported

Implemented now:

- named direct preset classes
- named direct theme classes
- named warm and cool bridge classes
- explicit fallback, hard-cut, and reject grammar
- curated bridge-capable journeys

Not supported now:

- all-to-all preset morphing
- all-to-all theme morphing
- lower-band bridge morphs
- any claim that the transition engine now covers every classic pair
