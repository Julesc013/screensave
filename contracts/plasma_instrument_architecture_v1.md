# Plasma v2 Instrument Architecture Contract v1

Status: active PAW-I instrument-architecture audit contract.

This contract folds the 25-27 June Plasma product doctrine into PAW-I. It does
not create a new roadmap layer. It makes the stable-promotion review ask
whether `plasma-v2-rc1` is truly a direct-control deterministic field
instrument.

The product thesis is:

```text
Plasma is not a preset picker. Plasma is a visual instrument.
```

PAW-I may promote Plasma v2 only if the release candidate proves it is a
direct-control deterministic field instrument, not a cleaned-up preset shell.

## Product Spine

The ScreenSave doctrine remains:

```text
Portable meaning.
Native delivery.
Deterministic proof.
Optional automation.
```

For Plasma, the product-owned meaning is:

```text
direct controls first
presets second
software/reference truth first
optional accelerated realizations second
```

Presets may be examples, bookmarks, curated moods, migration anchors, and
starting points. They must not be hidden masters of the runtime once a Plasma
v2 spec has been resolved.

## Portfolio Rule

ScreenSave must not adopt one universal visual graph. Plasma owns a
field-instrument semantic core. Other savers keep their own semantics.

The governing architecture remains:

```text
one stable platform contract
many product-owned semantic cores
multiple hosts / presenters / renderers
```

This is the product-architecture rule:

```text
Share mechanics.
Preserve meaning.
```

## Canonical Plasma v2 Path

The PAW-I audit must judge the release candidate against this product path:

```text
stored config / preset / pack / VisualIntent
-> plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> providers / analytic sources
-> field generators
-> field modifiers
-> output transform
-> material mapping
-> treatment stack
-> presentation
-> surface / renderer / presenter
-> proof bundle
```

The first stable realization must remain conservative:

```text
fixed-point field
-> material map to screensave_visual_buffer
-> optional treatment
-> flat present buffer
-> screensave_renderer_blit_bitmap
-> GDI or optional GL11
```

Field kernels must be deterministic, fixed-point, allocation-free per frame,
and free of renderer APIs, file I/O, and hot-loop floating trigonometry.

## Required Product Surface

Basic mode is an instrument panel. Every first-class Basic control must be
plan-visible, render-visible, intentionally structural, or unsupported and
hidden.

Required Basic controls are:

```text
Field
Scale
Complexity
Motion
Speed
Warp
Feedback
Material
Contrast
Brightness
Softness
Treatment
Seed
```

Advanced mode exposes the pipeline:

```text
Sources
Generators
Modifiers
Output
Material
Treatment
Presentation
Routing
```

Author/Lab mode may later expose graph-like editing, but a graph is not runtime
truth. Any author graph must lower through validated semantic graph, typed
graph, compiled execution plan, and runtime plan before execution.

## Stable Slice

The minimum stable Plasma v2 slice is intentionally narrow:

- field families: classic, interference, fire
- sources: sine/wave, radial, deterministic noise
- modifiers: domain warp, feedback
- output: continuous raster
- materials: at least four distinct materials
- treatments: none, soft, phosphor
- presentation: flat bitmap
- renderer path: software RGBA to `screensave_renderer_blit_bitmap`
- renderers: GDI stable, GL11 optional only if bitmap blit works
- proof: deterministic signatures and captures

Stable promotion must not include heightfield, ribbon, glyphs, GL46,
ShaderToy-style shader code, arbitrary graph runtime, or more presets to hide
weak controls.

## Pack, VisualIntent, And AIDE Boundaries

Packs remain data-only. They must not contain DLLs, scripts, macros,
executable payloads, path traversal, unbounded expansion, network behavior, or
unchecked provenance.

VisualIntent remains broad and approximate. It is descriptive authoring intent,
not executable behavior. It may propose product-family candidates, but Plasma
truth starts when candidates reduce to `plasma_v2_spec` and then pass pack,
capture, proof, review, and promotion gates.

AIDE may coordinate bounded development work, repair proposals, EvidencePacket
projection, drift detection, and release-gate evidence. AIDE must not become
the saver runtime, visual authority, release authority, compatibility
certifier, automatic merge authority, or final artistic judge.

## PAW-I Blocking Gates

PAW-I stable promotion must require each of these gates as distinct facts:

- plasma_v2_spec_contract_passes
- plasma_v2_plan_contract_passes
- plasma_v2_runtime_contract_passes
- legacy_preset_authority_removed
- direct_controls_influence_passes
- field_pipeline_boundaries_pass
- material_mapping_distinctness_passes
- treatment_boundaries_pass
- software_reference_is_canonical
- gl11_is_not_hidden_minimum
- packc_data_only_passes
- visualintent_candidates_reduce_to_plasma_spec
- aide_not_runtime_or_truth

If any gate is not satisfied, PAW-I must hold:

```toml
[plasma_v2]
status = "release-candidate-hold"
stable = false
release_promotion = "blocked"
opened_next = "plasma-v2-stable-repair"
```

Instrument-specific repair WorkUnits may be opened under the stable-repair
program, including direct-control influence repair, material response repair,
plan/degradation repair, and GL11 hidden-minimum repair.

## Claim Boundary

An instrument-architecture audit pass does not publish Plasma, certify broad OS
support, freeze a public SDK, admit Labs features to stable, or make AIDE a
runtime dependency. A failed or incomplete audit is a product architecture
hold, not a mechanical validator failure.
