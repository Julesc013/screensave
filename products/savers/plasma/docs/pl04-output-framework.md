# Plasma PL04: Output, Treatment, And Presentation Framework

Status: internal framework phase after `PL03`.

## Purpose

`PL04` makes three previously implicit engine areas explicit:

- output family and output mode
- fixed treatment slots
- presentation mode

It does this without broadening the public Plasma feature surface and without changing the preserved `Plasma Classic` defaults.

## Relationship To PL00 / PL01 / PL02 / PL03

- [`pl00-constitution.md`](./pl00-constitution.md) froze the canonical pipeline order and the lower-band truth Plasma must keep.
- [`pl01-taxonomy.md`](./pl01-taxonomy.md) and [`pl01-schema.md`](./pl01-schema.md) froze the vocabulary for output families, output modes, treatment slots, and presentation modes.
- [`pl02-plasma-classic.md`](./pl02-plasma-classic.md) preserved the current real saver as `Plasma Classic`.
- [`pl03-engine-core.md`](./pl03-engine-core.md) established the `authored content -> compiled plan -> execution state` split.

`PL04` consumes those decisions by giving the compiled classic runtime plan a real output/treatment/presentation path.

## Pre-PL04 Vs Post-PL04 Internal Shape

Before `PL04`, the classic engine shape was effectively:

- simulate field
- map the field directly into a raster buffer
- assume flat presentation
- hand that buffer to the renderer

After `PL04`, the internal render path is explicitly:

1. build output frame
2. theme-map and run fixed treatment slots
3. prepare presentation target
4. hand the prepared target to the renderer

The public saver is unchanged, but the internal seams are now real and named.

## Output Family / Mode Framework

`PL04` introduces a dedicated output module:

- [`../src/plasma_output.h`](../src/plasma_output.h)
- [`../src/plasma_output.c`](../src/plasma_output.c)

Framework vocabulary is structurally real for:

- `raster`
- `banded`
- `contour`
- `glyph`
- `surface`

and for output modes including:

- `native_raster`
- `dithered_raster`
- `contour_only`
- `contour_bands`
- `ascii_glyph`
- `matrix_glyph`
- `heightfield_surface`
- `curtain_surface`
- `ribbon_surface`
- `extruded_contour_surface`

Current implemented subset:

- `raster`
- `native_raster`
- bounded experimental `banded`
- bounded experimental `contour`
- bounded experimental `glyph`
- bounded experimental `posterized_bands`
- bounded experimental `contour_only`
- bounded experimental `contour_bands`
- bounded experimental `ascii_glyph`
- bounded experimental `matrix_glyph`

Current policy:

- unsupported output families or modes are rejected explicitly
- Plasma Classic does not silently remap unsupported output requests into fake support

## Treatment-Slot Framework

`PL04` introduces a dedicated treatment module:

- [`../src/plasma_treatment.h`](../src/plasma_treatment.h)
- [`../src/plasma_treatment.c`](../src/plasma_treatment.c)

The fixed slot order is now explicit and stable:

1. sampling treatment
2. filter or post treatment
3. emulation treatment
4. accent or overlay treatment

Framework vocabulary is structurally real for:

- sampling treatments such as `nearest`, `soft`, `dither`
- filter treatments such as `blur`, `glow_edge`, `halftone_stipple`, `kaleidoscope_mirror`, `restrained_glitch`, `emboss_edge`
- emulation treatments such as `phosphor`, `crt`
- accent treatments such as `overlay_pass`, `accent_pass`

Current implemented subset:

- explicit `none` defaults
- richer-lane `blur`
- richer-lane `overlay_pass`
- bounded experimental `glow_edge`
- bounded experimental `halftone_stipple`
- bounded experimental `emboss_edge`
- bounded experimental `phosphor`
- bounded experimental `crt`
- bounded experimental `accent_pass`

Important classic-preservation note:

- current classic `smoothing` behavior remains preserved in the classic simulation path at `PL04`
- `PL04` does not silently reinterpret that legacy control into a new slot behavior if doing so would risk visual drift
- the slot framework is therefore real and used, but the classic/default path currently runs explicit no-op slot values

## Presentation Framework

`PL04` introduces a dedicated presentation module:

- [`../src/plasma_presentation.h`](../src/plasma_presentation.h)
- [`../src/plasma_presentation.c`](../src/plasma_presentation.c)

Framework vocabulary is structurally real for:

- `flat`
- `heightfield`
- `curtain`
- `ribbon`
- `contour_extrusion`
- `bounded_surface`
- `bounded_billboard_volume`

Current implemented subset:

- `flat`
- bounded premium `heightfield`
- bounded experimental `curtain`
- bounded experimental `ribbon`
- bounded experimental `contour_extrusion`
- bounded experimental `bounded_surface`

Current policy:

- unsupported presentation modes are rejected explicitly
- no fake `bounded_billboard_volume` or scenic presentation execution is claimed

## Currently Implemented Subset Vs Future Taxonomy

`PL04` makes the full taxonomy structurally real enough for later phases to target, but only a small truthful subset actually executes today.

Implemented and used now:

- output family `raster`
- output mode `native_raster`
- bounded experimental output families `banded` and `contour`
- bounded experimental output family `glyph`
- bounded experimental output modes `posterized_bands`, `contour_only`, `contour_bands`, `ascii_glyph`, and `matrix_glyph`
- treatment slots with explicit `none` values plus the bounded implemented subset
- presentation mode `flat`
- bounded premium presentation mode `heightfield`
- bounded experimental presentation modes `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface`

Enumerated but not implemented now:

- surface output families and modes
- unsupported treatment values outside the bounded implemented subset
- `bounded_billboard_volume`

That limitation is intentional.
`PL04` is a framework phase, not a broad feature drop.

## Classic Default Path Mapping

The preserved classic/default plan now maps explicitly to:

- output family: `raster`
- output mode: `native_raster`
- sampling treatment: `none`
- filter treatment: `none`
- emulation treatment: `none`
- accent treatment: `none`
- presentation mode: `flat`

This is the current truthful classic mapping.
It keeps the visible classic path materially unchanged while giving later phases a clean place to extend.

## Validation / Unsupported-Mode Policy

`PL04` freezes this internal policy:

- unsupported output families or output modes fail validation
- unsupported treatment values fail validation
- unsupported presentation modes fail validation
- the render path does not silently substitute a different family or mode and pretend support exists

Current runtime effect:

- the compiled classic plan validates cleanly
- unsupported future-facing mutations are rejected directly in the framework modules

## What Remained Intentionally Unchanged

`PL04` intentionally does not change:

- the Plasma product name
- the Plasma Classic inventory
- current defaults and aliases
- current routing posture
- current public settings surface
- current public preset or theme surface
- current pack surface
- current renderer-law posture

It also does not add surface output, later field-family breadth, or broader dimensional showcase behavior yet.

## What PL05 May Build On Next

`PL05` can now build on real seams instead of implicit assumptions.

The next phase may extend:

- truthful universal and compat proof around the framework
- broader framework validation and hardening
- future routing-aware support decisions per band

It must do so without breaking the classic path mapping frozen here.

## Scope Statement

`PL04` is an internal output/treatment/presentation framework phase, not a broad public feature expansion.
It makes the pipeline stages explicit, keeps unsupported future modes honest, and preserves the current classic raster-plus-flat path as the active default.
