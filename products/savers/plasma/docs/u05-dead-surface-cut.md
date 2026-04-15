# Plasma U05 Dead Surface Cut

## Purpose

This note records which dead, weak, misleading, or near-duplicate surfaces were
cut from first-class exposure during `U05`.

## Controls Cut Or Hidden

| Control / Surface | U05 action | Why |
| --- | --- | --- |
| `favorites_only` dialog toggle | Hidden | Real only as a compatibility filter until the product has a truthful favorites editor. |
| favorite and exclusion key-list editors | Keep hidden | Underlying state is real, but the UI still cannot expose it honestly. |
| `content_filter` on Basic | Moved to Advanced | Too consequential to live on the tiny stable surface. |
| `transitions_enabled` on Basic | Moved to Advanced | Real, but still deeper than the tiny stable surface should carry. |

## Grammar Surfaces Cut From First-Class Exposure

| Surface | U05 action | Compatibility story |
| --- | --- | --- |
| `PLASMA_EFFECT_SUBSTRATE` | Hidden from normal picker | Presets and imported config can still resolve it. |
| `PLASMA_EFFECT_ARC` | Hidden from normal picker | Presets and imported config can still resolve it. |
| `PLASMA_FILTER_TREATMENT_BLUR` | Hidden from normal picker | Still resolves as a compatibility-only advanced path. |
| `PLASMA_ACCENT_TREATMENT_OVERLAY_PASS` | Hidden from normal picker | Still resolves as a compatibility-only advanced path. |
| `PLASMA_PRESENTATION_MODE_CURTAIN` | Hidden from normal picker | Still resolves for explicit imported settings and preset compatibility. |
| `PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION` | Hidden from normal picker | Still resolves where contour-output compatibility rules admit it. |
| `PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE` | Hidden from normal picker | Still resolves for explicit imported settings and preset compatibility. |

## Presets And Themes Cut From First-Class Pools

### Stable pools

Demoted from first-class stable pools:

- `midnight_interference`
- `amber_terminal`

### Experimental pools

Demoted from first-class experimental pools:

- `aurora_curtain`
- `substrate_relief`
- `filament_extrusion`

### Themes

Demoted from first-class theme pools:

- `midnight_interference`
- `amber_terminal`

## Authored Sets And Collections Cut Down

The authored selection layer now stops advertising demoted content through:

- `classic_core`
- `dark_room_classics`
- `plasma_classics`
- `interference_classics`
- `warm_bridge_classics`
- `cool_bridge_classics`
- `warm_classics`
- `cool_classics`
- `dark_room_classics` themes
- `wave3_experimental_sampler`
- `dark_room_support`

## Compatibility-Preserving Strategy

`U05` deliberately preserves compatibility wherever it could do so honestly:

- explicit preset and theme keys still resolve
- `ember_lava -> plasma_lava` still resolves
- imported config can still carry hidden compatibility values
- the dialog surfaces currently selected hidden values as explicit
  `(Compatibility)` entries instead of silently dropping them
- BenchLab and runtime reporting still expose requested versus resolved truth
  when a hidden compatibility value survives into the resolved plan

## What Remains Internally Present But No Longer User-Facing

The following remain real internally, but are no longer first-class product
breadth:

- compatibility-only blur and overlay treatment paths
- compatibility-only curtain, contour-extrusion, and bounded-surface
  presentation paths
- compatibility-only substrate and arc studies
- demoted stable and experimental presets or themes that still need key
  compatibility

## Compatibility Caveats

`U05` intentionally changes visibility and pool membership.
It does not promise that every hidden compatibility surface still deserves to
survive a later phase unchanged.

Later phases may still remove or migrate some of these paths if stronger proof
or retuning work does not justify them.
