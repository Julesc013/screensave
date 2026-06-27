# Plasma v2 Migration

Plasma v2 migration converts legacy Plasma configuration into direct semantic
controls.

## Rule

Legacy inputs seed a v2 specification once. After that, direct v2 fields own the
runtime request.

```text
legacy preset/theme/config
-> plasma_v2_legacy_config_view
-> plasma_v2_spec
-> future plasma_v2_plan
```

Loading an old preset must not reactivate the hidden preset-authority model.

## Initial Mapping

| Legacy input | v2 meaning |
| --- | --- |
| `effect_mode` | field family |
| `speed_mode` | normalized speed |
| `resolution_mode` | scale, complexity, and softness hints |
| `smoothing_mode` | softness and treatment |
| preset key | starter field/material/control bundle |
| theme key | material key |
| deterministic seed | v2 seed |

## First Stable Migration Anchors

The first direct-control migration anchors are:

- `plasma_lava`
- `aurora_plasma`
- `ocean_interference`
- `museum_phosphor`
- `quiet_darkroom`

Additional old keys may remain importable as compatibility aliases, but they do
not become first-class v2 product claims until separate visual and proof review
admits them.
