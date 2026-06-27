# Plasma v2 Instrument Constitution

Status: active product constitution for PAW-I-R.

Plasma is not a preset picker. Plasma is a visual instrument.

This constitution is product law for the Plasma v2 repair wave. It keeps the
useful ScreenSave scaffolding around Plasma while moving product authority into
the direct-control v2 instrument core.

## Product Identity

Plasma is a deterministic modular field instrument for ambient visuals.

Direct controls are primary. Presets are secondary. Software/reference truth is
primary. Optional accelerated realizations are secondary.

Presets remain:

- examples
- bookmarks
- curated moods
- migration anchors
- starting points

Presets must not be hidden runtime authority after a direct v2 specification is
resolved.

## Product Truth

The product-owned truth path is:

```text
stored config / preset / pack / VisualIntent
-> plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> field generators and modifiers
-> material mapping
-> treatment
-> presentation
-> surface / renderer / presenter
```

`plasma_v2_spec` is product meaning. It records the user's direct semantic
intent in a portable, clampable, serializable form.

`plasma_v2_plan` is resolved executable truth. It records what this session
will actually do after capability checks, size decisions, renderer requests,
fallbacks, and degradation.

`plasma_v2_runtime` owns deterministic state and buffers. It owns fields,
material output, treatment output, present output, frame index, elapsed time,
phase streams, and RNG state. The renderer is an output path, not the source of
product truth.

## Compatibility Boundary

The legacy Plasma files may remain as migration evidence, compatibility
wrappers, old data mappings, and host integration scaffolding. They must not be
the default product center for new Plasma v2 behavior.

Useful scaffolding may stay:

- Win32 host integration
- GDI and optional GL paths
- `screensave_visual_buffer`
- config and diagnostics APIs
- manifests
- presets, themes, and packs as data inputs
- BenchLab and Workbench inspection surfaces
- requested/resolved/degraded reporting

The old product center must be replaced or bypassed by direct v2 semantics, not
polished by adding more preset conditionals.

## Reference And Acceleration

The software/reference path is canonical. Optional acceleration must implement
the same resolved plan, compare honestly against reference evidence, and degrade
without becoming a hidden minimum.

GDI remains the stable floor. GL11 remains optional. GL46, heightfields,
ribbons, glyphs, arbitrary graph runtime, and new renderer families are outside
the PAW-I-R stable repair scope.

## Authoring And AIDE

Workbench may author, inspect, capture, compare, profile, and review evidence.
Workbench does not own Plasma semantics and must not become a duplicate runtime.

AIDE may coordinate WorkUnits, preflight context, proposal-only sessions,
EvidencePackets, repair summaries, and drift reports. AIDE does not own product
truth, final artistic acceptance, compatibility certification, publication, or
promotion.

AIDE does not own product truth.

## Stable Gate

Stable promotion remains blocked until Plasma v2 proves:

- the spec is direct semantic authority;
- the plan is resolved executable truth;
- the runtime owns deterministic buffers and state;
- field, material, treatment, and presentation are distinct stages;
- Basic controls materially affect either plan or render output;
- legacy presets migrate into specs instead of hidden runtime authority;
- software/reference execution is canonical;
- GL11 is optional, not a hidden minimum;
- Workbench inspects the real pipeline;
- AIDE records evidence without owning truth.

Until those facts are proven, Plasma v2 remains:

```toml
[plasma_v2]
status = "release-candidate-hold"
stable = false
release_promotion = "blocked"
opened_next = "plasma-v2-instrument-repair"
```
