# Plasma U01 Post-Unification Invariants

## Purpose

This note freezes the invariants that must hold after `U01`.

Later phases may build on these.
They must not silently weaken them.

## Runtime-Plan Invariants

After `U01`, Plasma must continue to satisfy all of these:

- the product compiles one runtime plan, not a Classic-mode plan plus side
  paths
- runtime validity depends on actual grammar, capability, lane, and degrade
  policy, not on whether a plan is called Classic
- lower-band baseline validation is grounded in resolved plan behavior
- requested, resolved, and degraded state remains explicit

## Default Stable Path Invariants

The preserved default stable path must continue to satisfy:

- default preset key is `plasma_lava`
- default theme key is `plasma_lava`
- default lower-band grammar remains `raster` + `native_raster` + `flat`
- routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`,
  `quality_class=safe`
- the default path remains valid on both `gdi` and `gl11`

## Alias And Content Compatibility Invariants

These compatibility surfaces must remain intact unless a later phase records an
explicit migration:

- `ember_lava -> plasma_lava`
- Classic-content set keys such as `classic_core`
- Classic-journey identity such as `classic_cycle`
- pack and provenance claims that preserve Classic identity
- legacy Classic-named helper and validation lookup aliases

## Validation Invariants

The canonical validation story after `U01` is:

- `default_stable_path` is the canonical lower-band default-path area key
- `default_path_gdi` and `default_path_gl11` are the canonical lower-band
  performance envelope keys
- legacy `classic_default`, `classic_gdi`, and `classic_gl11` survive only as
  compatibility aliases for lookup callers
- unsupported paths must still fail honestly

## What Later Phases May Assume

`U02+` may assume:

- there is no architectural `classic_execution` gate to remove first
- canonical runtime helpers describe one product runtime
- Classic is preserved as content and compatibility, not as engine identity
- the smoke harness already asserts both canonical and compatibility lookup
  surfaces for the preserved default path

## What Later Phases Must Not Assume

`U02+` must still not assume:

- presets have fully lost all bundle authority
- the settings UI is already ideal
- every output, treatment, or presentation distinction is equally strong
- stable widening is justified

## Scope Boundary

These invariants freeze runtime shape and compatibility.

They do not replace the later U02 through U09 corrective obligations.
