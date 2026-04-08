# Plasma PL02: Classic Migration Map

Status: migration and compatibility map for the preserved current Plasma surface.

## Purpose

This document records how the current saver surface maps into `Plasma Classic` so later `PL03+` work can change internals without losing identity or compatibility.

## Current Plasma -> Plasma Classic Mapping

At `PL02`, the mapping is direct:

| Current repo surface | Preserved meaning in later Plasma |
| --- | --- |
| saver product `Plasma` | public product remains `Plasma` |
| current compact framebuffer-and-palette saver | preserved compatibility slice `Plasma Classic` |
| current classic presets and themes | preserved classic content corpus |
| current alias `ember_lava` | preserved compatibility alias |
| current lower-band routing posture | preserved classic runtime truth |

`Plasma Classic` is therefore the name of the preserved current slice, not a separate product fork.

## Classic Identities That Must Remain Valid

Required preserved identities:

- product key `plasma`
- display name `Plasma`
- default preset `plasma_lava`
- default theme `plasma_lava`
- classic effect ids `fire`, `plasma`, `interference`
- classic preset keys
- classic theme keys
- classic pack key `lava_remix`

## Alias Preservation Table

| Input key | Canonical preserved key | Applies to |
| --- | --- | --- |
| `ember_lava` | `plasma_lava` | preset lookup |
| `ember_lava` | `plasma_lava` | theme lookup |

Rule:
later phases may widen alias handling if needed, but this existing alias must remain valid.

## Preset / Theme Compatibility Table

| Preset key | Preserved theme key | Later invariant |
| --- | --- | --- |
| `plasma_lava` | `plasma_lava` | remains the default classic pair |
| `aurora_plasma` | `aurora_cool` | remains a resolvable classic preset/theme pairing |
| `ocean_interference` | `oceanic_blue` | remains a resolvable classic preset/theme pairing |
| `museum_phosphor` | `museum_phosphor` | remains a resolvable classic preset/theme pairing |
| `quiet_darkroom` | `quiet_darkroom` | remains a resolvable classic preset/theme pairing |
| `midnight_interference` | `midnight_interference` | remains a resolvable classic preset/theme pairing |
| `amber_terminal` | `amber_terminal` | remains a resolvable classic preset/theme pairing |

## Import / Export Compatibility Notes

The preserved current import/export expectations are:

- shared preset files remain versioned `.preset.ini` files under the current shared outer schema
- shared theme files remain versioned `.theme.ini` files under the current shared outer schema
- current classic product-local preset fields remain the legacy narrow set:
  - `effect_mode` / `effect`
  - `speed_mode` / `speed`
  - `resolution_mode` / `resolution`
  - `smoothing_mode` / `smoothing`
- current preset/theme keys remain the stable compatibility anchors
- current deterministic fixed-seed behavior in compiled preset descriptors remains part of classic compatibility truth

Later internal schema migration is allowed only if classic import/export identity stays honest by key and meaning.

## Pack Compatibility Notes

The preserved current pack expectations are:

- `lava_remix` remains owned by product key `plasma`
- the current pack manifest format and file references remain valid
- the current bundled preset/theme files remain representable honestly
- the pack's current routing notes remain advisory and truthful, not backend-private doctrine

Later phases may remap how pack content is loaded internally, but they must not orphan or silently rewrite the preserved shipped pack surface.

## Migration Risks For PL03+

Primary risks later phases must guard against:

- silently changing or dropping classic keys
- removing the current alias while assuming new canonical names are enough
- preserving visuals approximately but losing preset/theme identity by key
- changing the default classic pair without an explicit compatibility story
- widening higher-band work in a way that rewrites the current classic lower-band truth
- changing import/export fields without preserving classic reader compatibility
- treating the pack surface as expendable because future content systems are broader

## Required Invariants Later Phases Must Keep

- product stays `Plasma`
- `Plasma Classic` stays an internal preserved slice, not a separate saver product
- `plasma_lava` remains the classic default preset
- `plasma_lava` remains the classic default theme
- current classic preset keys remain resolvable
- current classic theme keys remain resolvable
- `ember_lava` remains a valid alias to `plasma_lava`
- current manifest and compiled routing posture for the classic slice remains truthful
- current shipped pack content remains represented honestly

## Preservation By Identity, Not By Internal Layout

What later phases preserve is the classic surface by stable identity:

- keys
- aliases
- defaults
- routing truth
- import/export expectations
- classic content meaning

Internal storage, helper layout, and runtime organization may change later.
Those internal changes remain acceptable only if the preserved classic identities above still resolve honestly.
