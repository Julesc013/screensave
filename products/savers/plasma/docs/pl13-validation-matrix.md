# PL13 Validation Matrix

## Purpose

`PL13` turns Plasma's accumulated implementation work into an explicit proof matrix.
It records what is actually validated, what is only partially covered, what is only documented, what is unsupported, and what remains blocked by environment gaps.

This document is evidence-oriented.
It is not the final release cut.

## Relationship To PL00-PL12

- `PL00-PL02` froze Plasma identity, doctrine, and the preserved Classic slice.
- `PL03-PL05` established the internal engine shape and the truthful lower-band baseline.
- `PL06-PL10` added the content layer, richer lanes, dimensional presentation subset, and transition engine.
- `PL11` made the settings surfaces explicit.
- `PL12` made BenchLab reporting and bounded forcing explicit.
- `PL13` ties those implemented areas to concrete proof states and evidence references.

## Matrix Dimensions

The current matrix keys each entry by:

- `area_key`: the capability family being judged
- `lane_key`: the lane, backend, or product-wide scope being judged
- `status`: `validated`, `partial`, `documented_only`, `unsupported`, or `blocked`
- `evidence_ref`: the primary repo-local proof or documentation reference
- `notes`: the grounded interpretation of that evidence

Meaning of states:

- `validated`: backed by actual smoke or capture evidence in the current repo
- `partial`: implemented and tested, but only for a bounded subset
- `documented_only`: described and wired, but not backed by direct shipped-content proof
- `unsupported`: explicitly not implemented and not claimed
- `blocked`: evidence would require environments or sweeps that were not available in PL13

## Current Matrix

| Area | Lane / Scope | Status | Evidence | Meaning |
| --- | --- | --- | --- | --- |
| `classic_default` | `gdi` | Validated | [`validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt) | Preserved classic baseline proved on the universal floor. |
| `classic_default` | `gl11` | Validated | [`validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | Preserved classic baseline proved on the compat lane. |
| `content_registry` | `product` | Validated | [`../tests/pl06-content-proof.md`](../tests/pl06-content-proof.md) | Registry, sets, favorites, exclusions, and pack preservation are still backed by smoke and proof notes. |
| `advanced_lane` | `gl21` | Validated | [`validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt) | Bounded advanced lane still compiles and selects honestly. |
| `modern_lane` | `gl33` | Validated | [`validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt) | Bounded modern lane still compiles and selects honestly. |
| `premium_heightfield` | `gl46` | Validated | [`validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt) | Bounded premium lane plus current dimensional presentation subset is real. |
| `premium_request_degrade` | `auto` | Validated | [`validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`](../../../../validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt) | Requested premium presentation degrades honestly to the compat baseline and reports the clamp. |
| `transition_subset` | `gl11` | Validated | [`validation/captures/pl13/benchlab-plasma-journey-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt) | Transition routing and live journey-driven `preset_morph` are proved on a non-premium lane. |
| `settings_subset` | `product` | Validated | [`../tests/pl11-settings-proof.md`](../tests/pl11-settings-proof.md) | Layered settings surfaces and current gating rules remain backed by smoke and recorded proof. |
| `benchlab_surface` | `product` | Validated | [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md) | BenchLab field export, forcing, and clamp reporting remain backed by smoke and captures. |
| `transition_soak` | `product` | Partial | [`../tests/pl13-transition-validation.md`](../tests/pl13-transition-validation.md) | Repeated transition stepping is covered for the implemented subset, not all theoretical pairs. |
| `experimental_pool` | `product` | Documented Only | [`pl06-selection-and-sets.md`](./pl06-selection-and-sets.md) | Experimental filtering exists, but the current repo ships stable-only Plasma content. |
| `non_heightfield_presentations` | `gl46` | Unsupported | [`pl09-premium-lane.md`](./pl09-premium-lane.md) | Other premium-dimensional presentation modes are not implemented. |
| `cross_driver_ladder` | `all` | Blocked | [`pl13-known-limits.md`](./pl13-known-limits.md) | PL13 proof came from one capable validation machine, not a multi-driver denial sweep. |

## Evidence Notes For Material Areas

- Lower-band baseline proof is split into forced `gdi` and forced `gl11` BenchLab captures plus the smoke harness.
- Richer-lane proof is represented separately for `gl21`, `gl33`, and `gl46` so later phases do not have to infer lane viability from one merged report.
- Transition proof uses both smoke assertions and a long-enough `gl11` BenchLab journey capture that reaches a live `preset_morph`.
- Settings and BenchLab cells intentionally point back to the earlier phase proof notes because PL13 is validating continued truth, not re-implementing those systems.

## Preserved Default And Classic Invariants

PL13 re-verified that:

- product identity remains `Plasma`
- `Plasma Classic` remains preserved
- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- `ember_lava -> plasma_lava` still resolves
- manifest routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- when validation hooks are inactive, the default classic path remains `raster` + `native_raster` + `flat`

## Intentionally Outside Scope

PL13 does not claim:

- a final ship decision
- a broad cross-machine hardware sweep
- universal pairwise transition proof for every theoretical preset/theme combination
- numeric FPS or memory SLAs
- validation of unimplemented premium presentation modes

## What PL14 May Build On Next

`PL14` can use this matrix as its factual base for release-cut decisions, especially:

- which areas are validated strongly enough for a stable cut
- which partial or blocked areas must stay out of a stable release claim
- which unsupported features need to remain absent from release-facing messaging

## Scope Boundary

`PL13` provides evidence and status labels for the implemented Plasma subset.
It does not make the final release cut by itself.
