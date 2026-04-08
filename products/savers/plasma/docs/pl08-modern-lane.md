# PL08 Modern Lane

Status: first refined richer-lane phase above the bounded PL07 advanced lane.

## Purpose

`PL08` makes `modern -> gl33` a real Plasma execution lane.
It adds a bounded refined-behavior profile on top of the existing advanced-derived path while keeping the public stable baseline centered on:

- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

## Relationship To PL00 Through PL07

- `PL00` froze the rule that higher lanes must stay optional and honest beneath the lower-band floor.
- `PL01` froze the vocabulary for refined treatment, presentation, degrade policy, and content metadata.
- `PL02` preserved Plasma Classic as the content slice that later richer lanes must not erase.
- `PL03` created the runtime-plan seam PL08 now extends.
- `PL04` made output, treatment slots, and presentation explicit internal stages.
- `PL05` proved the lower-band baseline on `gdi` and `gl11`.
- `PL06` created the product-local content registry and selection system that now carries modern-capable content metadata.
- `PL07` created the first real richer lane on `gl21`, which PL08 now builds on instead of bypassing.

`PL08` therefore adds refinement above the existing richer lane rather than redefining Plasma's public center of gravity.

## What “Modern” Means For Plasma

For Plasma, `modern` means:

- the renderer lane is actually `gl33`
- the selected content is marked modern-capable
- the advanced-derived base is already active
- Plasma may enable a bounded refined-behavior subset that is not available on the lower bands or plain `gl21`

It does not mean:

- Plasma now prefers `gl33` by default
- Plasma has become a premium-lane or dimensional-showcase product
- Plasma has adopted new public settings surfaces
- Plasma has gained transitions, journeys, or BenchLab integration

## How Modern Differs From Universal, Compat, And Advanced

`universal` and `compat` remain the preserved stable floor:

- raster output
- native raster mode
- no-op treatment slots
- flat presentation
- no advanced-only or modern-only refinement

`advanced` remains the first richer lane:

- bounded field history feedback
- bounded domain warp
- bounded turbulence influence
- bounded blur
- bounded overlay glow

`modern` keeps the same output family and presentation family but refines the advanced-derived path:

- a modern-only refined field pass layered on top of the advanced field state
- a modern-only second filter refinement pass after the advanced blur
- a dedicated modern presentation buffer before the renderer blit
- slightly richer internal field resolution budgeting than the advanced lane

The product identity remains Plasma.
The modern lane is a refined uplift of the same saver, not a separate product or a hidden new default.

## Bounded Refined-Behavior Set Implemented In PL08

The PL08 modern subset is intentionally small and explicit.

Implemented now:

- modern-capable plan binding in `plasma_modern.h/.c`
- modern-only component flags in the runtime plan
- modern-only refined field shaping on top of the advanced-derived field state
- modern-only refined filter composition after the advanced blur pass
- modern-only dedicated presentation buffer and final flat-presentation refinement
- modern-only field-size uplift through a smaller divisor when the modern lane is active

Not implemented here:

- premium-lane (`gl46`) showcase chains
- non-flat premium presentation
- broad dimensional showcase identity
- transitions or journeys
- public GL33 settings or content browsers

## Modern Capability And Selection Rules

PL08 uses these rules:

- the saver now advertises `gl33` support in compiled policy and manifest metadata
- the saver still prefers `gl11`
- modern behavior is enabled only when the active renderer kind is `gl33`
- modern behavior also requires the selected preset content to be marked modern-capable
- modern behavior is layered on top of the advanced-derived execution profile, so it does not bypass the PL07 seam

Current content posture:

- all preserved built-in Plasma Classic presets are marked modern-capable
- no new public preset inventory is required for PL08
- the default preset `plasma_lava` can therefore participate in the bounded modern refinement path when `gl33` is explicitly selected

This keeps the content surface honest while avoiding premature PL09 showcase expansion.

## Degrade Policy Back To Advanced, Then Lower Bands

PL08 freezes explicit degrade behavior for the modern subset.

When modern-capable content is requested on `gl33` but only `gl21` is active, Plasma drops:

- the refined field pass
- the refined filter pass
- the dedicated modern presentation buffer

What remains:

- the PL07 advanced-derived behavior set
- preset identity
- theme identity
- raster output family
- native raster mode
- flat presentation

When the request falls further to `compat` or `universal`, Plasma also drops the advanced-only components and returns to the already-proven lower-band baseline.

PL08 does not silently fake modern behavior on unsupported lanes.
It degrades explicitly to advanced, then lower if needed.

## Current Modern-Capable Content Surface

PL08 does not add a new public preset list.

The current modern-capable surface is:

- the preserved built-in classic preset inventory, now marked modern-capable in the Plasma content registry

This is intentionally conservative.
It gives later phases a real modern foundation without redefining the public default content surface.

## Preserved Lower-Band And Advanced Invariants

PL08 explicitly preserves:

- product name `Plasma`
- `Plasma Classic` as the preserved compatibility slice
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- the lower-band default path as `raster` + `native_raster` + `flat`
- the PL07 advanced lane as the first richer lane on `gl21`

## Current Known Limitations

- The modern lane currently stays within raster output and flat presentation.
- There is no public UI for choosing modern profiles or viewing modern degrade state.
- No live morph, journey, or transition behavior is introduced here.
- No premium lane behavior is claimed.
- The proof surface added in PL08 validates modern execution through a deterministic fake-renderer harness rather than a live interactive GL33 window/context run.

## What Remained Intentionally Unchanged

PL08 intentionally does not change:

- Plasma's public stable routing posture
- lower-band default behavior
- the preserved classic preset/theme inventory
- the PL07 advanced lane contract
- the current settings dialog surface
- the current pack model
- shared platform law

## What PL09 May Build On Next

PL09 may extend this foundation by adding a bounded premium lane above the modern seam.
It no longer needs to invent a refined `gl33` layer from scratch because PL08 now provides:

- modern-capable plan state
- explicit modern selection and degrade binding
- a refined field pass above advanced
- a refined treatment/post seam above advanced blur
- a dedicated modern presentation buffer

## Scope Statement

`PL08` is a refined optional lane for Plasma.
It is not the new default stable baseline, and it does not smuggle in `PL09+` premium, transition, settings, or BenchLab work.
