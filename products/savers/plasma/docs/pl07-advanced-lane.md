# PL07 Advanced Lane

Status: first richer-lane phase after the lower-band and content-system groundwork.

## Purpose

`PL07` makes `advanced -> gl21` a real Plasma execution lane.
It adds a bounded richer-behavior profile on top of the preserved Plasma Classic content surface while keeping the public stable baseline centered on:

- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

## Relationship To PL00 Through PL06

- `PL00` froze the rule that `advanced -> gl21` is the first real richer lane, not the new center of gravity.
- `PL01` froze the vocabulary for treatments, presentation, degrade policy, and content metadata.
- `PL02` preserved Plasma Classic as the content slice that must survive future work.
- `PL03` created the runtime-plan seam PL07 now extends.
- `PL04` made output, treatment slots, and presentation explicit internal stages.
- `PL05` proved the lower-band baseline on `gdi` and `gl11`.
- `PL06` created the product-local content registry and selection system that now carries advanced-capable content metadata.

`PL07` builds on those phases without changing the default stable routing posture.

## What “Advanced” Means For Plasma

For Plasma, `advanced` means:

- the renderer lane is actually `gl21`
- the selected content is marked advanced-capable
- Plasma may enable a bounded richer-behavior set that is not available on the lower bands

It does not mean:

- Plasma now prefers `gl21` by default
- Plasma has become a modern-lane or premium-lane product
- Plasma has adopted a new public settings surface
- Plasma has gained transition, journey, or BenchLab feature work

## How Advanced Differs From Universal And Compat

`universal` and `compat` remain the preserved baseline:

- raster output
- native raster mode
- explicit no-op treatment slots
- flat presentation
- no advanced-only feedback, warp, turbulence, or post glow

`advanced` keeps the same output family and presentation family but enables a bounded richer profile:

- bounded history feedback in the scalar field
- bounded domain warp
- bounded turbulence influence
- bounded blur filter
- bounded overlay glow accent
- slightly richer internal field resolution budgeting

The product identity remains Plasma.
The richer lane is an uplift of the same saver, not a second product.

## Bounded Richer-Behavior Set Implemented In PL07

The PL07 advanced subset is intentionally small and explicit.

Implemented now:

- advanced-capable plan binding in `plasma_advanced.h/.c`
- advanced-only component flags in the runtime plan
- advanced-only field feedback/history
- advanced-only domain warp and turbulence influence
- advanced-only blur treatment
- advanced-only overlay glow accent
- advanced-only field-size uplift through a smaller divisor when the advanced lane is active

Not implemented here:

- glyph or surface advanced output families
- non-flat advanced presentation
- modern-lane (`gl33`) orchestration
- premium-lane (`gl46`) showcase chains
- transitions or journeys

## Advanced Capability And Selection Rules

PL07 uses these rules:

- the saver now advertises `gl21` support in compiled policy and manifest metadata
- the saver still prefers `gl11`
- advanced behavior is enabled only when the active renderer kind is `gl21`
- advanced behavior also requires the selected preset content to be marked advanced-capable

Current content posture:

- all preserved built-in Plasma Classic presets are marked advanced-capable
- no new public preset inventory is required for PL07
- the default preset `plasma_lava` is therefore capable of richer execution when `gl21` is explicitly selected

This keeps the content surface honest while avoiding a premature PL06+ content explosion.

## Degrade Policy Back To Lower Bands

PL07 freezes explicit degrade behavior for the advanced subset.

When advanced-capable content is forced down to `compat` or `universal`, Plasma drops:

- history feedback
- domain warp
- turbulence influence
- blur and overlay glow

What remains preserved:

- preset identity
- theme identity
- raster output family
- native raster mode
- flat presentation
- the current lower-band stable product posture

PL07 does not silently map unsupported advanced behavior to fake lookalikes.
It drops the advanced-only components and falls back to the already-proven lower-band baseline.

## Current Advanced-Capable Content Surface

PL07 does not add a new public preset list.

The current advanced-capable surface is:

- the preserved built-in classic preset inventory, now marked advanced-capable in the Plasma content registry

This is intentionally conservative.
It gives later phases a real advanced foundation without redefining the public default content surface.

## Preserved Lower-Band Invariants

PL07 explicitly preserves:

- product name `Plasma`
- `Plasma Classic` as the preserved compatibility slice
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- the lower-band default path as `raster` + `native_raster` + `flat`

## Current Known Limitations

- The advanced lane currently stays within raster output and flat presentation.
- There is no public UI for choosing advanced profiles or viewing degrade state.
- No live morph, journey, or transition behavior is introduced here.
- No modern or premium lane behavior is claimed.
- The proof surface added in PL07 validates advanced execution through a deterministic fake-renderer harness rather than a live interactive GL21 window/context run.

## What Remained Intentionally Unchanged

PL07 intentionally does not change:

- Plasma's public stable routing posture
- lower-band default behavior
- the preserved classic preset/theme inventory
- the current settings dialog surface
- the current pack model
- shared platform law

## What PL08 May Build On Next

PL08 may extend this foundation by refining richer-lane behavior above PL07's first bounded uplift.
It no longer needs to invent the first advanced seam from scratch because PL07 now provides:

- advanced-capable plan state
- advanced selection and degrade binding
- advanced field modifiers
- advanced post-treatment hooks

## Scope Statement

`PL07` is the first richer lane for Plasma.
It is not the new default stable baseline, and it does not smuggle in `PL08+` modern, premium, transition, settings, or BenchLab work.
