# Plasma Spec v2 Contract

Status: active product contract for the PAW-DX reference preview slice.

This contract is product-owned by Plasma. It is not a shared platform schema,
not a public SDK ABI, and not a VisualIntent runtime.

## Purpose

`plasma_spec_v2` is the bounded semantic description consumed by the Plasma v2
software reference path. It carries enough meaning to reproduce the first
reference preview profile while preserving the U09 stable-cut truth:

- default preset `plasma_lava`
- default theme `plasma_lava`
- minimum renderer `gdi`
- preferred renderer `gl11`
- quality class `safe`
- raster/native-raster and flat presentation as the stable claim

## Schema Identity

- schema id: `screensave.plasma.spec.v2`
- schema version: `2`
- source of migration truth: `products/savers/plasma/docs/u09-stable-recut.md`

The C representation begins with `struct_size` and `schema_version` so readers
can reject undersized or wrong-version data deterministically.

## Fields

The first slice admits these fields:

| Field | Meaning |
| --- | --- |
| `field_family` | reference field generator family |
| `scale` | field scale, clamped to the bounded preview range |
| `complexity` | octave/detail density for the reference field |
| `motion_speed` | fixed-step timeline speed |
| `warp_amount` | bounded domain-warp amount |
| `feedback_amount` | bounded previous-frame feedback |
| `output_style` | reference output mapping |
| `material_id` | product-owned material palette |
| `brightness` | bounded output brightness |
| `contrast` | bounded output contrast |
| `treatment_flags` | restrained treatment selection |
| `quality_intent` | stable quality posture |
| `seed_policy` | session, fixed, or randomized seed policy |

All durable numeric fields use `ss_u32` from the portable v2 base header.

## Admitted Values

Field family:

- `classic_interference`
- `radial_warped`

Output style:

- `continuous`
- `banded`
- `contour`

Material:

- `plasma_lava`
- `aurora_cool`
- `oceanic_blue`
- `museum_phosphor`
- `quiet_darkroom`

Presentation:

- `flat`

Treatment:

- `none`
- `restrained_dither`
- `restrained_crt`

Quality intent:

- `safe`

Seed policy:

- `session`
- `fixed`
- `randomize`

## U09 Migration Law

The deterministic U09 migration table admits only the stable preset and theme
pool:

- presets: `plasma_lava`, `aurora_plasma`, `ocean_interference`,
  `museum_phosphor`, `quiet_darkroom`
- themes: `plasma_lava`, `aurora_cool`, `oceanic_blue`,
  `museum_phosphor`, `quiet_darkroom`

The compatibility alias `ember_lava` resolves to `plasma_lava`.

U09 migration intentionally maps stable content to the flat reference
presentation and lower-band-safe quality intent. The richer implemented Plasma
surface remains outside this contract until later reviewed slices.

## Boundaries

This contract does not:

- implement a renderer,
- widen stable Plasma beyond U09,
- claim artistic acceptance,
- promote a release,
- admit executable packs,
- provide a public SDK ABI,
- make AIDE a runtime dependency.
