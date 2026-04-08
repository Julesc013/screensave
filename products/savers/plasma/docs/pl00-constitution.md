# Plasma PL00 Constitution

Status: product-local doctrine freeze after `SX09`.

This document is the product-local law for Plasma at `PL00`.
It freezes identity, scope, current-versus-target framing, and the doctrinal answers later `PL` work must inherit.
It does not claim that later `PL` content, transitions, schema migration, or renderer uplift already exist.

## Purpose

`PL00` exists to:

- freeze what Plasma is
- freeze what Plasma is not
- record the current repo-supported subset honestly
- record the intended flagship direction without treating it as already implemented
- preserve the lower-band and Plasma Classic obligations that later `PL` work must not erase

## Current Repo Reality

Today, the checked-in Plasma product is still the compact legacy saver described in [`../README.md`](../README.md):

- a focused framebuffer-and-palette saver, not a general pixel lab or shader showcase
- centered on three current effect modes: plasma, fire, and interference waves
- driven by a CPU-generated low-resolution field that is then presented through the shared visual-buffer path
- exposed through modest product controls: effect, speed, resolution, and smoothing
- shipped with curated presets and themes rather than an open-ended content system
- explicitly marked preview-safe and long-run-stable at the saver level

Current routing truth also remains narrow and honest:

- compiled and manifest policy both state `minimum_kind=gdi`
- compiled and manifest policy both state `preferred_kind=gl11`
- compiled and manifest policy both state `quality_class=safe`
- the checked-in product currently supports `gdi` and `gl11`, not a higher public tier

Current metadata and migration reality is also bounded:

- the product already uses the shared preset/theme/config surfaces
- preset application already supports deterministic fixed seeds where a preset carries one
- current preset and theme aliases already exist and must remain part of migration truth where they are supported today
- no checked-in product-local content system, transition engine, set system, layered settings surface, or full Plasma-specific BenchLab integration exists yet

## Target Flagship Direction

The intended flagship Plasma shape is a larger product, but not a different species.

Flagship Plasma remains:

- field-derived first
- abstract rather than scenic
- palette-and-output driven rather than object-demo driven
- truthful on lower bands before claiming higher-band uplift
- expanded mainly through presets, themes, packs, sets, randomization or journeys, and explicit transitions

Flagship Plasma may grow into richer 2D, 2.5D, and bounded 3D presentation, stronger output families, layered settings, BenchLab integration, curated stable content, isolated experimental content, and real morph transitions.
Those are future `PL` commitments, not current implementation claims.

## Identity Boundaries

Plasma's identity is frozen as follows:

- Plasma is an abstract field-derived saver first.
- Plasma is not defined by any one backend, private substrate seam, or renderer-private implementation detail.
- Plasma may present as raster, banded, contour, glyph, or bounded surface-oriented output later, but the underlying identity remains field-first rather than scene-first.
- Chemical, cellular, and growth-like visuals are allowed only when they remain abstract, field-derived, non-literal, and non-ecological.
- Dimensional presentation may extend to 2.5D and bounded 3D, but Plasma must not become scenic, free-camera, or object-demo driven.
- Public preset naming may use familiar descriptive words, but internal doctrinal meanings stay precise.

## Exclusions / Anti-Scope-Creep Rules

Plasma must not become:

- a generic visual sandbox
- a catch-all home for every atmospheric or scenic idea in the suite
- a palette editor product
- a shader showcase whose identity depends on `gl33` or `gl46`
- a place where backend-private details leak into product vocabulary
- a product that silently redefines the frozen ScreenSave public renderer doctrine

Later `PL` work may widen content and presentation, but it must do so without abandoning these exclusions.

## Ontology

At `PL00`, the following terms are frozen conceptually.
`PL01` will freeze the exact taxonomy and logical schema that use them.

| Term | Meaning at PL00 |
| --- | --- |
| generator | the field-producing source behavior that defines the base motion family |
| modifier | a bounded transformation applied to a field before final output selection |
| output family | the main form used to interpret a field result for display |
| output mode | a named variant within an output family |
| theme | the mapping and appearance layer that controls palette or material expression without replacing the base generator identity |
| treatment | an optional bounded presentation adjustment applied through a fixed slot |
| treatment slot | one named stage where a treatment may occur; not an arbitrary effect pile |
| dimensional presentation | the bounded 2D, 2.5D, or 3D-style interpretation applied after core field/output work |
| profile class | the stable-versus-experimental posture of content |
| quality class | the declared cost and uplift posture of content or routing intent |
| band intent | the product-local expectation for which internal band a behavior targets or enhances |
| degrade policy | explicit rules for what may be reduced, dropped, or declared unsupported when a lower band is selected |
| transition or journey | an explicit content-to-content movement contract, not a hidden side effect |

Two meaning freezes are already settled:

- `contour` is an internal output-family concept or quantization result, not a base generator family
- `ascii` and `matrix` are output modes under a glyph-oriented output family, not generator families

## Composition Rules

Plasma composition is frozen to the following canonical order:

1. generator
2. modifiers
3. output family
4. output mode
5. theme or palette mapping
6. sampling treatment
7. filter or post treatment
8. emulation treatment
9. accent or overlay treatment
10. dimensional presentation
11. renderer presentation

Rules:

- Later work may refine the contents of each stage, but it must preserve this order unless a later doctrine update says otherwise.
- Multiple treatments are allowed only through fixed slots.
- Plasma must not adopt arbitrary unordered effect stacks.
- Output-family selection is product identity; it is not a disposable last-second renderer trick.
- Dimensional presentation is downstream of the field and output decisions, not a replacement for them.

## Field Model

The field model is frozen as:

- required primary scalar field
- optional secondary or history scalar field
- optional vector flow field
- optional mask or edge field
- optional depth or height derivative field
- optional glyph or index field
- derived presentation buffers

Rules:

- Every later Plasma form must still have a truthful field-first interpretation.
- Optional fields may enrich presentation, feedback, or dimensional work, but they do not replace the required primary scalar field.
- Derived buffers are presentation consequences, not the product's primary identity.

## Band Contract

The finished `SX09` internal band model is binding on Plasma:

| Internal band | Public tier | PL00 rule |
| --- | --- | --- |
| `universal` | `gdi` | mandatory honest baseline |
| `compat` | `gl11` | conservative accelerated lane above the baseline |
| `advanced` | `gl21` | first real advanced lane |
| `modern` | `gl33` | optional modern uplift |
| `premium` | `gl46` | optional highest uplift |

Product rules:

- Plasma must keep `universal` and `compat` truthful first.
- `gl21` is the first safe place for genuinely advanced Plasma behavior.
- `gl33` and `gl46` remain optional enhancement lanes rather than Plasma's center of gravity.
- Higher-band work must degrade honestly through the canonical fallback walk defined by `SX`.
- Plasma product doctrine must describe bands and public tiers honestly without pretending private backend trivia is product identity.

The guaranteed universal-band baseline for Plasma is frozen as:

- honest usable GDI Plasma
- CPU field generation
- low internal resolution fallback
- theme or palette mapping
- deterministic seed support
- stable classic content
- no dependence on advanced feedback or premium-only presentation

Repo reality note:
the current compiled Plasma saver still tops out at the `compat` lane.
`advanced`, `modern`, and `premium` are available substrate doctrine, not current Plasma shipping claims.

## Degradation Semantics

Lower-band operation is normal, not failure.
Degradation must be explicit and truthful.

Plasma must never silently degrade:

- preset identity
- profile class
- output family
- theme family
- transition truth
- capture determinism contract
- unsupported-path truthfulness

Allowed future classes of degradation include:

- dropping premium presentation
- dropping heavy post treatment
- dropping advanced-only modifiers
- dropping emulation extras
- reducing filter quality
- reducing internal resolution
- declaring a path unsupported in the active band

Rules:

- Degradation may simplify presentation cost.
- Degradation may not quietly convert one kind of content into another while pretending nothing changed.
- If a lower band cannot preserve the declared identity honestly, Plasma must fail honestly for that path instead of faking compatibility.

## Stable Vs Experimental Doctrine

Profile classes are frozen as `stable` and `experimental`.

Stable Core posture is frozen as:

- stable defaults must be honest on `universal` and `compat`
- stable content must not hide a premium-only baseline
- preserved classic content is part of the stable anchor

Experimental posture is frozen as:

- chemical, cellular, and growth-like visuals begin experimental-first
- heavier distortion or feedback begins experimental-first
- more aggressive dimensional presentation begins experimental-first
- other identity-edge content begins experimental-first until later promotion is justified

Safety-tag meanings are frozen in product terms:

| Tag | Meaning |
| --- | --- |
| `preview_safe` | remains legible, stable, and operational in the small embedded preview path without depending on fullscreen-only composition or costly startup behavior |
| `long_run_safe` | remains operationally stable and visually bounded over unattended runs without runaway resource growth or intentionally fatiguing motion spikes |
| `dark_room_safe` | keeps brightness, flash character, and contrast aggression restrained enough for intended dark-room use rather than treating eye-searing output as the default posture |
| `capture_safe` | can be used for deterministic or otherwise controlled validation and representative capture on supported paths without relying on hidden nondeterministic tricks |

Repo reality note:
current saver-level metadata already claims preview safety and long-run stability.
Per-content safety tagging is future product-local metadata, not a current parser or runtime claim.

## Plasma Classic Migration Doctrine

The current saver must survive as `Plasma Classic`.

Migration rules:

- the current compact framebuffer-and-palette saver remains the preserved classic baseline
- current classic identities, aliases, and import or export compatibility must remain supported
- the current effect families, classic presets, and classic themes form the first stable content corpus that later Plasma work must preserve
- later flagship Plasma may wrap, extend, or reclassify Classic content, but it must not erase it or rewrite it dishonestly

## Metadata / Schema Direction

Shared outer schema law remains in [`../../../../specs/config_schema.md`](../../../../specs/config_schema.md).
`PL00` does not rewrite that law.

Plasma may add product-local metadata direction for:

- generator family
- modifier set
- output family and output mode
- treatment slots
- dimensional presentation mode
- profile class
- quality class
- band intent
- degrade policy
- safety tags
- transition compatibility
- set membership
- seed policy

Additional doctrine:

- presets may carry parameter ranges and deterministic seed policy
- seed policy conceptually supports `inherit`, `fixed`, and `range-derived`
- favorites and exclusions are first-class at the Plasma product level
- `PL00` does not promote favorites or exclusions into shared suite law

Repo reality note:
today's checked-in Plasma text forms only expose a smaller subset through the shared outer preset/theme schema, routing fields, common deterministic-seed fields, and product-local effect or speed or resolution or smoothing keys.
The larger Plasma-local metadata model remains future logical direction until later docs and implementation phases land it honestly.

## Settings And BenchLab Future Commitments

All meaningful Plasma options should eventually exist somewhere, but not in one giant flat dialog.

The intended settings-surface stack is:

- Basic
- Advanced
- Author or Lab
- BenchLab-facing inspection or forcing

Full Plasma-specific BenchLab integration is a required later-program target.
That does not change the suite doctrine that BenchLab remains diagnostics-only rather than a normal end-user product shell.

## Transitions / Journeys Future Commitments

Theme-set selection, explicit transitions, morph chains, and journeys are in flagship Plasma scope.
They are not implemented by `PL00`.

Doctrine:

- these capabilities are future commitments, not current product claims
- transition truth must always be explicit
- unsupported morphs must fail honestly rather than pretending to be seamless
- later transition work must preserve preset identity, theme truth, and deterministic-capture honesty where those contracts are claimed

## Release Intent

Flagship Plasma should ultimately ship as a Core-grade standalone saver with:

- preserved Plasma Classic
- truthful lower-band operation
- explicit higher-band uplift
- layered settings surfaces
- full Plasma BenchLab integration
- curated stable content
- isolated experimental content
- real morph transitions for supported content

This release intent does not authorize `PL00` to claim those later capabilities are already implemented.

## Non-Goals For PL00

`PL00` does not own:

- engine refactors
- renderer or backend expansion work
- settings UI implementation
- BenchLab implementation work
- transition-engine implementation
- content-system implementation
- parser or runtime schema migration
- code or performance refactors
- reopening `SS` or `SX` public renderer doctrine

## What PL01 Must Freeze Next

`PL01` must freeze the next layer beneath this constitution:

- exact product vocabulary for generators, modifiers, outputs, treatments, theme families, quality or profile classes, degrade terms, and transition classes
- exact naming and keying conventions
- the canonical product-local logical schema for presets, themes, packs, sets, transitions, and journeys
- the explicit distinction between current repo-supported metadata and future logical schema direction

That work must inherit this constitution rather than renegotiating it implicitly.
