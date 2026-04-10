# PL14 Release Cut

## Purpose

`PL14` defines the original end-of-program Plasma release cut.
It turned the PL13 evidence surface into the first explicit stable, experimental, and deferred shipping posture without adding new feature scope.

Current note:
the current post-`PX41` recut now lives in [`px50-stable-recut.md`](./px50-stable-recut.md) and [`px50-experimental-and-deferred.md`](./px50-experimental-and-deferred.md).
`PX50` preserves the narrow stable runtime core and adds only a narrow stable support-surface widening.

## Relationship To PL00-PL13

- `PL00-PL02` froze Plasma identity and the preserved Classic surface.
- `PL03-PL05` established the refactored engine and the truthful lower-band baseline.
- `PL06-PL10` added the content layer, richer lanes, dimensional presentation subset, and transition engine.
- `PL11-PL12` added layered settings and BenchLab support.
- `PL13` added the proof matrix, performance envelopes, and known-limit register.
- `PL14` uses that evidence to define what ships now, what ships only as experimental, and what is not part of this release cut.

## Stable Core Ship Scope

The stable Core cut is intentionally narrow and centered on the preserved Plasma Classic identity.

| Area | Stable Core Decision | Evidence / Basis |
| --- | --- | --- |
| Product identity | Ship as `Plasma`, preserving `Plasma Classic` inside it | [`pl02-plasma-classic.md`](./pl02-plasma-classic.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) |
| Default content | Ship the stable built-in classic preset/theme inventory, `ember_lava -> plasma_lava`, and the current `lava_remix` pack compatibility surface | [`pl02-classic-migration-map.md`](./pl02-classic-migration-map.md), [`../tests/pl06-content-proof.md`](../tests/pl06-content-proof.md) |
| Default runtime path | Ship the preserved `raster` + `native_raster` + `flat` + no-op treatment path as the stable default | [`pl04-output-framework.md`](./pl04-output-framework.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) |
| Lower-band posture | Ship `gdi` as the truthful floor and `gl11` as the truthful preferred lane | [`pl05-universal-compat.md`](./pl05-universal-compat.md), [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt), [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) |
| Default settings posture | Ship the safe default selection posture with stable-only content filtering and transitions disabled by default | [`pl11-settings-surfaces.md`](./pl11-settings-surfaces.md), [`../tests/pl11-settings-proof.md`](../tests/pl11-settings-proof.md) |
| Support proof surface | Ship the implemented Plasma-specific BenchLab reporting surface as support and validation infrastructure | [`pl12-benchlab.md`](./pl12-benchlab.md), [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md) |

## Experimental Ship Scope

These areas may ship in the same product binary, but only as explicit opt-in experimental material.
They are not part of the default stable Core promise.

| Area | Experimental Decision | Why Experimental Now |
| --- | --- | --- |
| `advanced -> gl21` | Ship as optional experimental uplift | Implemented and validated, but not required for the stable baseline and not yet broad enough for unconditional stable support claims |
| `modern -> gl33` | Ship as optional experimental uplift | Implemented and validated, but still a richer optional lane rather than a default support target |
| `premium -> gl46` with the bounded dimensional subset | Ship as optional experimental uplift | Implemented and validated for a bounded premium dimensional subset only; premium breadth remains intentionally narrow |
| Bounded contour and banded output subset | Ship as opt-in experimental behavior | Implemented for a bounded preset-driven subset, but not broad enough or proved enough for stable default claims |
| Bounded treatment-family subset | Ship as opt-in experimental behavior | Implemented and supportable for a curated slot-disciplined subset, but still intentionally narrower than the full taxonomy |
| Bounded glyph output subset | Ship as opt-in experimental behavior | Implemented for a bounded preset-driven subset, but still too narrow and too newly proved for stable default claims |
| Bounded field-families-I subset | Ship as opt-in experimental behavior | Implemented as bounded chemical, lattice, and caustic studies, but still intentionally narrow and evidence-bounded |
| Bounded field-families-II subset | Ship as opt-in experimental behavior | Implemented as bounded aurora, substrate, and arc studies, but still intentionally narrow and evidence-bounded |
| Bounded dimensional subset beyond `heightfield` | Ship as opt-in experimental behavior | Implemented only for a bounded premium subset and still too narrow for stable-default claims |
| Bounded experimental preset pool | Ship as opt-in experimental content | Real shipped experimental content now exists, but it is a narrow study slice rather than a broad alternate default catalog |
| Transition engine subset | Ship as opt-in experimental behavior | Real morphs and journeys are implemented, but transition pair coverage and soak proof remain bounded rather than exhaustive |
| Advanced and Author-Lab control surfaces | Ship as opt-in experimental control surfaces | Meaningful and implemented, but they expose richer or riskier controls beyond the default stable support center |
| BenchLab forcing overrides | Ship as support-facing experimental tooling | Implemented and validated for the bounded subset only, not as an end-user stability promise |

## Deferred / Non-Ship Scope

These areas are outside this release cut and should not be presented as shipping features.

| Area | Deferred / Non-Ship Decision | Why |
| --- | --- | --- |
| Additional dimensional presentation modes beyond the bounded subset | Do not ship | `bounded_billboard_volume` and scenic presentation remain unsupported in the repo today |
| Broad experimental content pool | Do not ship | `PX30` through `PX32` only add a narrow experimental preset slice, not a broad alternate content catalog |
| Later field-families II breadth beyond the admitted subset | Do not ship | Reserved for later work |
| Surface output family work | Do not ship | Unsupported in the repo today |
| Exhaustive transition pair coverage | Do not ship as a claim | Only the curated implemented subset is proved |
| Broad cross-hardware lane guarantees | Do not ship as a claim | PL13 proof came from one capable validation machine |
| Numeric performance SLAs | Do not ship as a claim | PL13 records qualitative envelopes only |
| Scenic, world-simulated, or free-camera premium presentation | Do not ship | Outside Plasma's identity and unsupported in the repo |
| Full BenchLab forcing console or deep telemetry debugger | Do not ship | Out of scope for the implemented product-local BenchLab surface |

## Cut Rationale By Major Area

- Identity integrity: strong enough for stable Core because the preserved Classic/default path remains intact and still defines the product.
- Lower-band truth: strong enough for stable Core because `gdi` and `gl11` are separately proved and remain the routing center of gravity.
- Content curation: stable enough for Core because the default content pool remains stable-only even though a bounded experimental preset slice now exists.
- Richer lanes: suitable to ship only as experimental because they are real, but still optional and not broadly validated across environments.
- Output and treatment widening: suitable to ship only as experimental because the new PX30 subset is real but still bounded, preset-driven, and evidence-limited.
- Glyph and field-family-I widening: suitable to ship only as experimental because the new PX31 subset is real but still bounded, preset-driven, and evidence-limited.
- Field-family-II and dimensional widening: suitable to ship only as experimental because the new PX32 subset is real but still bounded, premium-aware, and evidence-limited.
- Transitions: suitable to ship only as experimental because the implemented subset is meaningful and real, but pair coverage remains bounded.
- Settings and BenchLab: suitable to ship with a split posture, where safe defaults and reporting are part of stable support, while deeper forcing and lab surfaces remain experimental/support-facing.

## Preserved Default And Classic Invariants

This release cut preserves:

- product name `Plasma`
- `Plasma Classic` preservation
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- routing posture `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- default classic runtime path as `raster` + `native_raster` + `flat`

## Lower-Band Truth Statement

Stable Core Plasma is anchored on the truthful lower-band baseline.
The release cut assumes users must get a good, honest experience on `gdi` and `gl11` without depending on `gl21`, `gl33`, `gl46`, premium presentation, or transition-heavy behavior.

## How Higher Lanes Are Positioned

- `gl21` is the first richer lane, not the hidden minimum.
- `gl33` is a refined optional lane, not the default support center.
- `gl46` is a bounded showcase lane with a bounded dimensional presentation subset, not a broad premium platform claim.

## What This Release Intentionally Does Not Promise

- a wider premium-dimensional family than the bounded current subset
- exhaustive transition compatibility coverage
- cross-driver parity across multiple hardware labs
- hard numeric frame-time, memory, or startup guarantees
- a non-default promotion of richer lanes into the stable baseline

## Packaging Posture

Plasma ships as one saver product, not as separate stable and experimental binaries.
The stable-versus-experimental split is enforced by the existing default posture, content-channel posture, capability gating, and explicit opt-in control paths.

## Scope Boundary

This cut is current-repo-grounded and evidence-backed.
It does not promote unimplemented or insufficiently validated work into the shipping promise.
