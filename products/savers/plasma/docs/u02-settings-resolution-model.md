# Plasma U02 Settings Resolution Model

## Purpose

This note freezes the canonical settings-resolution order after `U02`.

The order below is now the authoritative answer to "which layer wins" when
Plasma compiles a runtime plan.

## Canonical Resolution Order

The implemented order is:

1. shared saver defaults from `screensave_common_config_set_defaults`
2. Plasma product defaults from `plasma_config_set_product_defaults`
3. preset-bundle defaults from `plasma_apply_preset_bundle_to_config` for the
   active preset identity
4. explicit persisted or imported values that override the bundle field by
   field
5. session-local overrides, including BenchLab config forcing for supported
   selection and transition fields
6. config clamping and content canonicalization
7. `plasma_settings_resolve`, which produces the canonical requested settings
   record
8. `plasma_plan_apply_settings_resolution`, which copies that requested settings
   record into the plan as the pre-bind resolved grammar
9. capability, lane, transition, and BenchLab plan forcing, which may degrade
   unsupported requests while preserving the requested fields for reporting

No later phase should treat this order as implicit or optional.

## Layer Semantics

### Product Defaults

Product defaults provide the non-preset Plasma floor:

- default settings surface
- default selection and transition structures
- default lower-band grammar floor before any preset bundle is applied
- default BenchLab forcing state

These defaults are now separate from preset defaults on purpose.

### Preset Or Theme Defaults

The active preset contributes a curated starting bundle:

- preset identity
- default theme pairing
- deterministic-seed default and fixed-seed value when the preset carries one
- generator, output, treatment, presentation, speed, resolution, smoothing, and
  other bundled visual defaults

The active theme may still replace the theme key explicitly.
Preset and theme defaults are a starting point, not final runtime authority.

### Persisted User Values

Registry-backed or imported values override the preset bundle field by field.

That includes explicit user choices for:

- theme identity
- detail and diagnostics
- generator, output, treatment, and presentation fields
- selection, favorites, and transition preferences

If a persisted field exists, it wins over the preset bundle for that field.

### Session Overrides

Session-local overrides sit above persisted values.

In current `U02` scope, that mainly means:

- BenchLab selection and transition forcing through config state
- shared renderer request and deterministic-seed session state
- product-local BenchLab presentation forcing at plan level

Not every forcing surface imports a preset bundle.
`/plasma-preset:` currently changes selection identity, not the full authored
bundle.

### Capability And Lane Clamps

After resolution, Plasma still clamps invalid or unsupported requests.

Examples:

- invalid content keys fall back through normal canonicalization
- unsupported richer-lane requests degrade to the truthful lower lane
- unsupported presentation uplifts degrade back to supported presentation
- `sampling_treatment` remains a bounded clamp-to-`none` surface

The clamp does not erase the request.
It changes the resolved runtime state.

### Runtime Plan

The compiled plan is now fed from the canonical resolved settings record.

`plasma_plan_apply_settings_resolution` is the explicit handoff point where the
settings model becomes the runtime plan grammar.

## Conflict Rules

The conflict rules after `U02` are:

- later layers win over earlier layers
- invalid values are clamped or canonicalized, not left ambiguous
- unsupported requests remain visible in requested fields even when the resolved
  plan must degrade them
- presets may seed values, but they do not silently reassert ownership after an
  explicit override

## What Remains Intentionally Deferred

This order does not claim that every exposed setting is equally valuable.

Still deferred:

- UI-surface recut and control hiding in `U03`
- later destructive simplification for weak settings
- stronger output and treatment differentiation
- broader proof harness work for rendered distinctness

`sampling_treatment` remains the clearest bounded non-claim surface inside this
otherwise canonical order.

## Scope Boundary

The order above is now canonical for `U02`.

Later phases may simplify controls or hide weak surfaces, but they should not
reintroduce vague preset-first precedence.
