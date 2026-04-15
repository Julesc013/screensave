# Plasma U06 Preset Retuning

## Purpose

This note records the first aggressive retune of Plasma's surviving preset
catalogue after the `U05` subtractive cut.

`U06` exists because `U05` left a smaller and more honest product surface, but
that smaller surface still needed stronger defaults, clearer identity, and
better long-run behavior.

`U06` is retuning, not feature expansion.

## Stable Presets Retuned

The stable slice keeps the same preserved keys and compatibility posture while
retuning the surviving built-ins into clearer roles.

| Preset | U06 role | Retuning work |
| --- | --- | --- |
| `plasma_lava` | preserved default baseline | Keeps the classic fire grammar but now reads as a hotter ember field with cleaner contrast, steadier fire-floor pacing, and longer-held structure. |
| `aurora_plasma` | cool plasma stable variant | Pushes the stable cool path into clearer edge-lit plasma instead of a soft blue recolor. |
| `ocean_interference` | crisp interference stable variant | Drops back to standard speed and disables soft smoothing so the interference structure stays legible instead of drifting into another soft raster wash. |
| `museum_phosphor` | monochrome phosphor stable variant | Moves to a steadier phosphor raster with standard resolution and calmer museum-floor motion. |
| `quiet_darkroom` | dark-room-safe stable variant | Keeps the low-detail calm role while removing extra diffusion so the preset stays subdued during long unattended runs. |

## Compatibility-Preserved Stable Presets Retuned

These presets remain resolvable by explicit key but stay outside the first-class
stable pools.

| Preset | Why it still exists | U06 retune |
| --- | --- | --- |
| `midnight_interference` | preserved classic compatibility identity | Retuned into a darker contour-band and embossed-relief path so it no longer collapses back toward `plasma_lava`. |
| `amber_terminal` | preserved classic compatibility identity | Retuned into a warmer edge-glow terminal identity so it no longer reads as a weak museum-phosphor duplicate. |

## Experimental Presets Retuned

The experimental slice stays bounded, but the surviving studies now read more
deliberately.

- contour and band studies:
  `lava_isolines`, `aurora_bands`, `wire_glow`, `phosphor_topography`,
  `stipple_bands`, `emboss_current`, and `crt_signal_bands`
- glyph and field-family studies:
  `ascii_reactor`, `matrix_lattice`, `cellular_bloom`,
  `quasi_crystal_bands`, and `caustic_waterlight`
- bounded dimensional studies:
  `aurora_curtain`, `ribbon_aurora`, `substrate_relief`, and
  `filament_extrusion`

The retune tightens speed, smoothing, output-mode, and treatment choices so the
surviving studies are less likely to collapse into one or two generic looks.

## Distinctness Improvements

The deterministic preset audit now gives a better starting point for later
proof and QA work:

- no compiled preset pairs remain within the audit's near-duplicate threshold
  of `<= 2`
- the closest stable pair is now `museum_phosphor <-> plasma_lava` at distance
  `3`
- the previous crowding around `plasma_lava`, `midnight_interference`,
  `amber_terminal`, and `museum_phosphor` no longer survives as distance-2
  collisions

This is not a claim that the preset catalogue is visually final.
It is a claim that the surviving catalogue is materially less crowded than it
was before `U06`.

## Authored Set Follow-Through

The authored stable sets now lean harder into the five first-class stable
preset identities:

- `classic_core` weights favor `plasma_lava`, `aurora_plasma`, and
  `ocean_interference`
- `dark_room_classics` now orbits `quiet_darkroom`
- warm and cool bridge sets now bias more strongly toward their warm and cool
  anchors instead of diluting the bridge pools with weak near-duplicates

This keeps authored selection aligned with the smaller, more supportable stable
surface established in `U05`.

## Intentionally Deferred To U07+

`U06` does not claim:

- final screenshot-grade quality ranking for every surviving preset
- a universal perceptual-diff harness
- the final keep or remove decision for every bounded experimental preset
- the final stable or experimental recut

Those remain later-phase work.
