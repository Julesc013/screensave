# PL09 Premium Lane

Status: bounded optional showcase lane above the PL08 modern path.

## Purpose

`PL09` makes `premium -> gl46` a real Plasma execution lane.
It adds a bounded premium-only behavior set and the first real dimensional presentation path while keeping the public stable baseline centered on:

- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

## Relationship To PL00 Through PL08

- `PL00` froze the rule that higher lanes must stay optional, truthful, and subordinate to the lower-band floor.
- `PL01` froze the vocabulary for premium routing, dimensional presentation, degrade policy, and content metadata.
- `PL02` preserved Plasma Classic as the compatibility slice that later richer lanes must not erase.
- `PL03` created the runtime-plan seam that now carries premium state.
- `PL04` made output, treatment slots, and presentation explicit internal stages.
- `PL05` proved the lower-band baseline on `gdi` and `gl11`.
- `PL06` created the product-local content registry and selection layer that now carries premium-capable content metadata.
- `PL07` created the first richer lane on `gl21`.
- `PL08` created the refined `gl33` modern lane that PL09 now builds on instead of bypassing.

`PL09` therefore adds an optional showcase lane above the existing stack rather than redefining Plasma's center of gravity.

## What “Premium” Means For Plasma

For Plasma, `premium` means:

- the renderer lane is actually `gl46`
- the selected content is marked premium-capable
- the advanced-derived and modern-derived base is already active
- Plasma may enable a bounded premium-only refinement set
- Plasma may use a bounded dimensional presentation mode when that mode is genuinely implemented

It does not mean:

- Plasma now prefers `gl46` by default
- Plasma has become a generic 3D scene engine
- Plasma has adopted scenic, ecological, or world-simulated identity
- Plasma has gained transitions, journeys, BenchLab depth, or new settings surfaces

## How Premium Differs From Universal, Compat, Advanced, And Modern

`universal` and `compat` remain the preserved stable floor:

- raster output
- native raster mode
- no premium-only modifiers or presentation
- flat presentation

`advanced` remains the first richer lane:

- bounded feedback/history
- bounded domain warp
- bounded turbulence influence
- bounded blur and overlay refinement

`modern` remains the refined lane above advanced:

- refined field shaping above the advanced field state
- refined filter composition
- dedicated modern presentation buffer
- still raster plus flat

`premium` builds on that existing stack and adds a bounded showcase refinement:

- an extra premium-only field refinement pass layered on top of the modern field state
- a premium-only post chain after the existing treatment path
- dedicated premium treatment and presentation buffers
- a bounded dimensional presentation subset rooted in `heightfield`

The product identity remains Plasma.
The premium lane is an optional showcase uplift of the same saver, not a separate product and not a hidden new minimum.

## Bounded Premium-Behavior Set Implemented In PL09

The PL09 premium subset is intentionally small and explicit.

Implemented now:

- premium-capable plan binding in `plasma_premium.h/.c`
- premium-only component flags and degrade-policy flags in the runtime plan
- premium-only extended field refinement above the modern field state
- premium-only post refinement after the existing treatment chain
- dedicated premium treatment and presentation buffers
- a small additional field-size uplift when the premium lane is active
- premium validation that requires the full `gl46` modern-derived stack

Not implemented here:

- multiple premium showcase chains
- free-camera scene rendering
- generic 3D object/world simulation
- transition or journey behavior
- public premium settings/UI
- BenchLab-facing premium inspection tools

## Bounded Dimensional Presentation Modes Implemented In PL09

`PL09` originally makes exactly one bounded dimensional presentation mode real:

- `heightfield`

The `heightfield` path:

- is derived directly from the simulated scalar field
- stays abstract and non-scenic
- remains bounded inside the current bitmap presentation pipeline
- uses vertical lift and bounded highlight/shadow shaping rather than free-camera geometry

Current repo reality is broader because later `PX32` work now also implements:

- `curtain`
- `ribbon`
- `contour_extrusion`
- `bounded_surface`

Still unsupported in the current repo:
- `bounded_billboard_volume`

Those unsupported modes remain rejected honestly rather than silently mapped to fake premium behavior.

## Premium Capability And Selection Rules

PL09 uses these rules:

- the saver now advertises `gl46` support in compiled policy and manifest metadata
- the saver still prefers `gl11`
- premium behavior is enabled only when the active renderer kind is `gl46`
- premium behavior also requires the selected preset content to be marked premium-capable
- premium behavior is layered on top of the already-valid modern-derived path

Current content posture:

- the preserved built-in classic preset inventory is marked premium-capable in the Plasma content registry
- no new public preset inventory is required for PL09
- the default preset `plasma_lava` can therefore participate in the bounded premium path when `gl46` is explicitly selected

## Degrade Policy Back To Modern, Then Lower Bands

PL09 freezes explicit degrade behavior for the premium subset.

When premium-capable content is requested on `gl46` but only `gl33` is active, Plasma drops:

- premium-only field refinement
- premium-only post refinement
- premium-only treatment and presentation buffers
- non-flat premium presentation when the requested bounded subset cannot be honored

What remains:

- the PL08 modern-derived behavior set
- preset identity
- theme identity
- raster output family
- native raster mode
- flat presentation

When the request falls further to `gl21`, Plasma drops the premium and modern refinements and keeps the PL07 advanced-derived path.

When the request falls further to `compat` or `universal`, Plasma also drops the advanced-only components and returns to the already-proven lower-band baseline.

PL09 does not silently fake premium behavior on unsupported lanes.
It degrades explicitly to modern, then advanced, then lower if needed.

## Scope-Boundary Rules For Dimensional Presentation

PL09 keeps Plasma inside its constitutional identity.

The premium dimensional path must remain:

- field-derived
- abstract
- bounded
- non-scenic
- non-ecological
- non-world-simulated

PL09 therefore does not introduce:

- a free camera
- world/object simulation
- landscape, city, weather, or ecology scenes
- a generic 3D engine

## Current Premium-Capable Content Surface

PL09 does not add a new public preset list.

The current premium-capable surface is:

- the preserved built-in classic preset inventory, now marked premium-capable in the Plasma content registry

This is intentionally conservative.
It gives later phases a real premium seam without redefining the public default content surface.

## Preserved Lower-Lane And Classic Invariants

PL09 explicitly preserves:

- product name `Plasma`
- `Plasma Classic` as the preserved compatibility slice
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- the lower-band default path as `raster` + `native_raster` + `flat`
- the PL07 advanced lane on `gl21`
- the PL08 modern lane on `gl33`

## Current Known Limitations

- The premium lane now implements a bounded dimensional subset rooted in `heightfield` and extended by `PX32`.
- The premium presentation path remains bitmap-presented rather than a free-camera geometry pipeline.
- No live transition, journey, settings-surface, or BenchLab behavior is introduced here.
- No scenic or world-simulated behavior is claimed.
- The proof surface added in PL09 validates premium execution and dimensional presentation through a deterministic fake-renderer harness rather than a live interactive GL46 context run.

## What Remained Intentionally Unchanged

PL09 intentionally does not change:

- Plasma's public stable routing posture
- lower-band default behavior
- the PL07 advanced lane contract
- the PL08 modern lane contract
- the preserved classic preset/theme inventory
- the current settings dialog surface
- shared platform law

## What PL10 May Build On Next

PL10 may extend this foundation by adding transition and journey behavior above the now-real premium seam.
It no longer needs to invent the showcase lane from scratch because PL09 now provides:

- premium-capable plan state
- explicit premium selection and degrade binding
- a bounded premium-only field and post refinement seam
- a first real dimensional presentation mode
- explicit non-scenic scope boundaries for future premium work

## Scope Statement

`PL09` is an optional premium showcase lane for Plasma.
It is not the new default stable baseline, and it does not smuggle in `PL10+` transition, settings, BenchLab, or release-cut work.
