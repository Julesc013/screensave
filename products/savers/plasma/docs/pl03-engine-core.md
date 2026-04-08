# Plasma PL03: Engine Core Refactor

Status: internal engine-core refactor after `PL02`.

## Purpose

`PL03` reshapes Plasma internals around three layers:

- authored content
- compiled runtime plan
- execution state

It does this without changing the public Plasma product, without expanding the feature surface, and without relaxing the `Plasma Classic` preservation rules frozen by `PL02`.

## Relationship To PL00 / PL01 / PL02

- [`pl00-constitution.md`](./pl00-constitution.md) froze the identity, lower-band truth, and future direction Plasma must keep.
- [`pl01-taxonomy.md`](./pl01-taxonomy.md) and [`pl01-schema.md`](./pl01-schema.md) froze the product vocabulary and logical schema direction.
- [`pl02-plasma-classic.md`](./pl02-plasma-classic.md) and [`pl02-classic-migration-map.md`](./pl02-classic-migration-map.md) froze the current real saver surface as `Plasma Classic`.

`PL03` consumes those rules internally.
It does not reopen them.

## Current Pre-Refactor Shape

Before `PL03`, the current Plasma session owned a flat mix of:

- resolved content choices
- current preset and theme selection
- routing assumptions
- mutable simulation fields
- mutable timing and palette state

That shape worked for the current classic saver, but it left authored content, one-time plan decisions, and hot-loop state too tightly coupled inside one session blob.

## Target PL03 Architecture

After `PL03`, the internal shape is:

1. Authored content
   - built-in preset descriptors
   - built-in theme descriptors
   - classic aliases and classic inventory helpers
   - current shared config binding inputs

2. Compiled runtime plan
   - one resolved `plasma_plan` per session
   - canonical preset and theme identity
   - resolved classic effect, speed, resolution, and smoothing choices
   - resolved detail level
   - resolved seed facts needed by the current engine
   - current routing policy facts
   - explicit classic-execution and classic output assumptions

3. Execution state
   - drawable and field sizes
   - preview-state-dependent runtime sizing
   - framebuffer and field buffers
   - RNG state
   - timing, palette, and source phases
   - other mutable per-frame state only

## Authored Content Vs Compiled Plan Vs Execution State

The key rule introduced by `PL03` is:

- authored content stays in descriptors, config, packs, and classic helpers
- the plan compiles that authored content into one resolved session contract
- the execution state mutates frame-to-frame without needing to re-resolve preset/theme/content decisions every step

This keeps the classic saver honest while making later work easier to extend.

## Modules Introduced Or Clarified

### New module: `plasma_plan`

[`../src/plasma_plan.h`](../src/plasma_plan.h) and [`../src/plasma_plan.c`](../src/plasma_plan.c) now own:

- plan struct definition
- plan initialization
- plan compilation from the current config binding and current classic authored content
- plan validation for the current classic runtime contract

The plan is intentionally narrow.
It only resolves what current Plasma already needs plus a small amount of future-safe structure that is already truthful today:

- classic output family: `raster`
- classic output mode: `native_raster`
- classic presentation mode: `flat`

No new treatments, output families, or dimensional runtime behaviors are implemented here.

### Clarified session shape

[`../src/plasma_internal.h`](../src/plasma_internal.h) now defines the session as:

- `plasma_plan plan`
- `plasma_execution_state state`

That split is the main architectural result of `PL03`.

### Sim and render now consume the plan

[`../src/plasma_sim.c`](../src/plasma_sim.c) and [`../src/plasma_render.c`](../src/plasma_render.c) now read resolved classic behavior from the compiled plan instead of keeping authored-content choices in mutable hot-loop state.

## What Remained Intentionally Unchanged

`PL03` intentionally leaves these areas alone:

- the Plasma product name and public identity
- the classic preset and theme inventory
- the classic effect families
- the current alias policy
- current routing posture: `gdi` floor, `gl11` preferred lane, `quality_class=safe`
- the current compact raster presentation
- current smoothing control semantics
- current import/export and pack surfaces

## Classic-Preservation Guarantees

`PL03` preserves the following classic guarantees:

- public product remains `Plasma`
- `Plasma Classic` remains an internal preserved slice, not a separate saver
- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- classic preset and theme keys remain resolvable
- `ember_lava -> plasma_lava` remains valid
- current visible classic behavior remains materially unchanged
- current lower-band truth remains `gdi` plus conservative `gl11`

## Current Limits Of The Runtime Plan

The `PL03` runtime plan is deliberately limited.

It does not yet model:

- expanded generator families beyond the current classic runtime
- treatment slots as real runtime systems
- non-classic output families
- non-flat presentation modes
- transition or journey execution
- pack/set/favorites systems
- higher-band uplift-specific execution branches

That limitation is intentional.
The plan is a current-classic execution plan with future-safe seams, not a fake flagship implementation.

## What PL04 May Build On Next

`PL04` can now build on real internal seams instead of the old flat session shape.

The next phase may extend:

- plan compilation for additional output assumptions
- translation from legacy classic controls into broader taxonomy-aware runtime choices
- bounded treatment or presentation expansion

It must do so without breaking the classic plan contract introduced here.

## Scope Statement

`PL03` is an internal refactor, not a public feature expansion.
It establishes the engine-core shape later Plasma phases can extend while keeping the preserved classic saver honest today.
