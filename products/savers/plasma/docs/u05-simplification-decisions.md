# Plasma U05 Simplification Decisions

## Purpose

This note records the exact keep, move, hide, demote, and compatibility-only
decisions made during the `U05` subtractive cut.

`U05` exists because `U04` made more of the grammar real, but the reachable
surface was still wider than the product could honestly support.

`U05` is subtractive correction, not preset or theme retuning.

## Surface Categories Simplified

The `U05` cut acts on five categories:

- settings and control surfaces
- generator, output, treatment, and presentation vocabulary
- stable and experimental preset or theme pools
- authored preset and theme sets plus curated collections
- support-facing compatibility surfaces that stay real but stop pretending to
  be first-class

## Control-Surface Decisions

| Surface | Decision | Result |
| --- | --- | --- |
| Basic | Keep and shrink | Keeps only preset, theme, speed, visual intensity, and safe defaults. |
| Basic `content_filter` | Move | Moves to `Advanced` because it materially changes selection posture. |
| Basic `transitions_enabled` | Move | Moves to `Advanced` because it materially changes motion policy. |
| `favorites_only` dialog control | Hide | The filter stays real for imported config and BenchLab, but stops pretending to be a supportable dialog toggle. |
| Favorites and exclusions list editors | Keep hidden | Lists stay file-first and compatibility-real until a bounded honest editor exists. |

## Grammar-Surface Decisions

| Area | First-class after U05 | Compatibility-only after U05 | Rationale |
| --- | --- | --- | --- |
| effect families | fire, plasma, interference, chemical, lattice, caustic, aurora | substrate, arc | Keep the families that still anchor surviving first-class presets and hide the weaker or more support-heavy studies. |
| output families | raster, banded, contour, glyph | none newly added | `surface` remains a non-claim. |
| filter treatments | none, glow_edge, halftone_stipple, emboss_edge | blur | `blur` still resolves for explicit imports, but it is too weak and lane-sensitive to stay first-class. |
| emulation treatments | none, phosphor, crt | none | The surviving emulation read stays coherent and supportable. |
| accent treatments | none, accent_pass | overlay_pass | `overlay_pass` remains compatibility-real but not first-class. |
| presentation modes | flat, heightfield, ribbon | curtain, contour_extrusion, bounded_surface | Keep the dimensional modes with the clearest truth and demote the rest to compatibility-only. |

## Preset And Theme Decisions

### Stable First-Class Presets Kept

- `plasma_lava`
- `aurora_plasma`
- `ocean_interference`
- `museum_phosphor`
- `quiet_darkroom`

### Stable Presets Demoted From First-Class Pools

- `midnight_interference`
- `amber_terminal`

These presets remain resolvable by explicit key, but they no longer live inside
the first-class stable preset pools.

### Experimental First-Class Presets Kept

- `lava_isolines`
- `aurora_bands`
- `wire_glow`
- `phosphor_topography`
- `stipple_bands`
- `emboss_current`
- `crt_signal_bands`
- `ascii_reactor`
- `matrix_lattice`
- `cellular_bloom`
- `quasi_crystal_bands`
- `caustic_waterlight`
- `ribbon_aurora`

### Experimental Presets Demoted From First-Class Pools

- `aurora_curtain`
- `substrate_relief`
- `filament_extrusion`

### Themes

Stable first-class themes remain:

- `plasma_lava`
- `aurora_cool`
- `oceanic_blue`
- `museum_phosphor`
- `quiet_darkroom`

Themes demoted from first-class pools but preserved for explicit compatibility:

- `midnight_interference`
- `amber_terminal`

## Authored Metadata Decisions

The following authored surfaces now match the smaller first-class product:

- `classic_core`, `dark_room_classics`, `plasma_classics`,
  `interference_classics`, `warm_bridge_classics`, and
  `cool_bridge_classics` no longer route through the demoted stable classics
- matching theme sets no longer route through the demoted warm and cool
  compatibility themes
- `dark_room_support` now points at the surviving calm stable preset instead of
  a demoted warm-amber favorite
- `wave3_experimental_sampler` now keeps the surviving contour, glyph,
  field-family-I, and ribbon study slice instead of the demoted dimensional
  compatibility studies

## Why These Cuts Happened In This Order

The first-class surface is now smaller because the repo already had evidence
that several areas were too crowded:

- `amber_terminal <-> museum_phosphor` was already a near-duplicate pair in the
  deterministic preset audit
- `midnight_interference <-> plasma_lava` was also already a near-duplicate
  pair
- `aurora_curtain <-> cellular_bloom` was close enough that the aurora-curtain
  branch did not deserve first-class exposure before later retuning
- blur, overlay, curtain, contour-extrusion, and bounded-surface behavior all
  remained meaningfully more support-heavy than the smaller first-class subset

## Intentionally Deferred To U06+

`U05` does not claim:

- broad preset or theme retuning
- final aesthetic quality ranking for every surviving preset
- final stable or experimental recut
- a universal screenshot or perceptual-diff proof layer

Those remain later-phase work.
