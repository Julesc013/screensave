## Purpose

`PX32` broadens Plasma's bounded dimensional subset beyond `heightfield` while keeping the product abstract, field-derived, and non-scenic.

## Actual Dimensional Capabilities Landed Beyond `heightfield`

The implemented premium-only subset is now:

- `heightfield`
- `curtain`
- `ribbon`
- `contour_extrusion`
- `bounded_surface`

These presentation modes remain downstream of the field, output, and treatment pipeline.
They do not turn Plasma into a free-camera scene engine.

## Which Presentation Modes Were Actually Implemented

Implemented now:

- `curtain`
- `ribbon`
- `contour_extrusion`
- `bounded_surface`

Still unsupported:

- `bounded_billboard_volume`
- scenic or world-simulated presentation
- unrestricted camera behavior

## Settings And BenchLab Truth For The Supported Subset

The dimensional subset is still bounded and mostly preset-driven.
The current truthful control and reporting surface is:

- preset-owned presentation defaults for the new experimental PX32 slice
- BenchLab forcing support for `auto`, `heightfield`, `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface`
- BenchLab snapshot and report truth through `presentation_mode`, `requested_lane`, `resolved_lane`, `degraded_from_lane`, `degraded_to_lane`, and clamp fields

There is still no broad end-user dimensional-mode picker in the normal stable settings surface.

## Degrade Policy For The Supported Subset

- Non-flat presentation remains premium-gated.
- If premium presentation cannot be honored, the plan degrades honestly to `flat`.
- `contour_extrusion` is admitted only for contour output plans; unsupported combinations fail validation instead of silently mapping to another mode.
- Unsupported broader presentation requests remain explicit clamps or rejects, not fake execution.

## Lane And Capability Gating

- `gdi`, `gl11`, `gl21`, and `gl33` remain non-flat-ineligible.
- `gl46` remains the only truthful lane for the bounded non-flat subset.
- Premium presentation still requires the plan to be premium-capable and the requested mode to be supported by the active plan.

## What Remains Bounded Or Unsupported

- no free-camera rendering
- no scenic world or object simulation
- no generic 3D engine behavior
- no billboard-volume execution
- no claim of exhaustive rendered proof across all dimensional combinations in this checkout

## Scope Boundary

`PX32` dimensional expansion remains field-derived and non-scenic.
It is a bounded premium-only subset, not a scene-engine rewrite and not Wave 4 integration work.
