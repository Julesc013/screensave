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

## Canonical TOML Shape

```toml
visual_intent_version = 1

[appearance]
brightness = 0.40
contrast = 0.72
density = 0.58
palette = ["#090604", "#5c1f09", "#d46b1b"]
texture = "soft-grain"

[motion]
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
