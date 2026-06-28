# VisualIntent v1

Status: draft authoring contract, not executable runtime behavior.

## Purpose

VisualIntent is a small cross-product description of desired appearance,
motion, composition, and safety constraints.

It exists to help choose or configure an existing ScreenSave product. It is
not a universal graphics language, not a product-local IR, and not a permission to
generate unrestricted native code.

## Boundary

VisualIntent may describe:

- brightness, contrast, density, palette hints, and texture hints
- motion character, speed, continuity, and loop duration
- symmetry, depth, focus, and spatial balance
- safety constraints such as deterministic output and flashing avoidance

VisualIntent must not describe:

- Win32, macOS, X11, Wayland, WebAssembly, or renderer APIs
- shader programs or executable scripts
- direct platform resource paths
- native code patches
- compatibility claims
- final artistic acceptance

VisualIntent cannot execute, cannot mutate runtime directly, and cannot become
a hidden product specification. VisualIntent cannot bypass product-specific schemas.

## Two-Level Authoring Rule

The authoring pipeline is:

```text
text / image / video reference
  -> VisualIntent
  -> product-family resolver
  -> product-specific specification
  -> safe preset or pack candidate
  -> deterministic capture
  -> proof
  -> human promotion decision
```

The product-specific specification owns the real meaning. For example, Plasma
may translate VisualIntent into a field/material/treatment specification, while
Ricochet may translate the same intent into particle and geometry parameters.

No shared VisualIntent field may silently redefine product semantics.

## Product Reduction Law

VisualIntent is descriptive and cross-product. It must be reduced through a
product-family resolver before it can affect any ScreenSave product.

For Plasma v2, the admitted path is:

```text
VisualIntent
  -> product-family resolver
  -> bounded candidate set
  -> explicit plasma_v2_spec
  -> plasma_v2_plan
  -> plasma_v2_runtime / proof path
  -> capture / proof bundle / review
```

Every Plasma v2 candidate must produce an explicit `plasma_v2_spec`. Every
candidate `plasma_v2_spec` must be compilable to `plasma_v2_plan` through the
existing product-owned plan compiler and safe pack/spec flow.

VisualIntent candidates are review inputs, not accepted product truth. Candidate
rationale is textual evidence only. Candidate generation must not generate C
source, shader programs, scripts, executable payloads, renderer commands,
platform resource paths, final artistic acceptance, stable release claims, or
compatibility certification.

Each candidate must carry a claim boundary that keeps ScreenSave product truth
owned by the product-specific spec, plan, runtime, proof, review, and promotion
gates.

## Required Fields

VisualIntent v1 admits these common fields at minimum:

- `mood`
- `brightness`
- `contrast`
- `density`
- `motion_character`
- `motion_speed`
- `palette_hint`
- `warmth`
- `softness`
- `complexity`
- `avoid_flashing`
- `deterministic`
- `seed`
- `target_product`
- `candidate_count`

Products may ignore unsupported descriptive fields, but a resolver must record
that choice as a constraint or warning rather than silently treating the field
as executable truth.

## Candidate Policy

Candidate generation is bounded and deterministic:

- `candidate_count` defaults to `3`.
- `candidate_count` must never exceed `5`.
- candidate IDs must be stable for a fixed input.
- candidate order must be stable for a fixed input.
- all candidates must be deterministic for a fixed input.
- candidate rationale must be textual evidence only.
- generated candidates are review inputs, not accepted product truth.

## Canonical TOML Shape

```toml
visual_intent_version = 1
target_product = "plasma"
candidate_count = 3
mood = "warm-room"
seed = 4096

[appearance]
brightness = 0.40
contrast = 0.72
density = 0.58
palette_hint = "warm-lava"
palette = ["#090604", "#5c1f09", "#d46b1b"]
texture = "soft-grain"
warmth = 0.78
softness = 0.62
complexity = 0.58

[motion]
motion_character = "slow-drifting"
motion_speed = 0.18
character = "slow-drifting"
speed = 0.18
continuity = 0.92
loop_seconds = 90

[composition]
symmetry = "low"
depth = "medium"
focus = "distributed"

[constraints]
avoid_flashing = true
deterministic = true
maximum_brightness = 0.55
```

## Validation Rules

- `visual_intent_version` must be `1`.
- Numeric intent values are normalized floats in `[0.0, 1.0]` unless a field
  states otherwise.
- `loop_seconds` must be positive when present.
- Palette entries must be CSS-style `#RRGGBB` strings.
- Unknown sections are invalid until explicitly admitted.
- Unknown keys are invalid until explicitly admitted.
- The result is descriptive only; it must be resolved through a product schema
  and ScreenSave proof before promotion.
- VisualIntent must not bypass product schemas, proof, or human promotion.

## Initial Non-Goals

VisualIntent v1 does not:

- generate C source
- VisualIntent does not generate C source
- define a universal node graph
- replace product-specific presets or packs
- qualify visual quality
- certify compatibility
- bypass proof bundles
- make AIDE part of product runtime
