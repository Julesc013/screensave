# PX50 Experimental And Deferred

## Purpose

`PX50` records the current experimental cut and the current deferred, non-ship, and non-claim cut after the completed Waves 1 through 4 work now present in the repo.

## Experimental Cut

Experimental remains real, implemented, and ship-adjacent.
It does not become stable merely because it ships in the same product binary or now has better docs and proof notes.

### Implemented And Supportable But Still Experimental

| Area | Current Experimental Decision | Evidence / Basis | Why It Stays Experimental |
| --- | --- | --- | --- |
| `gl21` advanced lane | Keep experimental | [`pl13-validation-matrix.md`](./pl13-validation-matrix.md), [`pl13-performance-envelopes.md`](./pl13-performance-envelopes.md) | Real and validated, but still an optional uplift rather than the stable support center. |
| `gl33` modern lane | Keep experimental | [`pl13-validation-matrix.md`](./pl13-validation-matrix.md), [`pl13-performance-envelopes.md`](./pl13-performance-envelopes.md) | Real and validated, but still optional and not broadly proved enough for stable-default claims. |
| `gl46` premium lane and the admitted dimensional subset | Keep experimental | [`pl13-validation-matrix.md`](./pl13-validation-matrix.md), [`px32-dimensional-expansion.md`](./px32-dimensional-expansion.md), [`../tests/px32-dimensional-proof.md`](../tests/px32-dimensional-proof.md) | Real and bounded, but still premium-aware, narrower, and evidence-limited. |
| Transition subset | Keep experimental | [`px11-transition-coverage.md`](./px11-transition-coverage.md), [`px11-promotion-review-1.md`](./px11-promotion-review-1.md), [`../tests/px11-transition-proof.md`](../tests/px11-transition-proof.md) | Real and useful, but pair coverage and rerun breadth remain bounded. |
| PX30 contour, banded, and treatment subset | Keep experimental | [`px30-contour-and-banded.md`](./px30-contour-and-banded.md), [`px30-treatment-families.md`](./px30-treatment-families.md), [`../tests/px30-output-proof.md`](../tests/px30-output-proof.md), [`../tests/px30-treatment-proof.md`](../tests/px30-treatment-proof.md) | Implemented, preset-driven, and supportable for a narrow slice only. |
| PX31 glyph and field-families-I subset | Keep experimental | [`px31-glyph-family.md`](./px31-glyph-family.md), [`px31-field-families-i.md`](./px31-field-families-i.md), [`../tests/px31-glyph-proof.md`](../tests/px31-glyph-proof.md), [`../tests/px31-field-families-i-proof.md`](../tests/px31-field-families-i-proof.md) | Real and supportable for a bounded slice only. |
| PX32 field-families-II and broader dimensional subset | Keep experimental | [`px32-field-families-ii.md`](./px32-field-families-ii.md), [`px32-dimensional-expansion.md`](./px32-dimensional-expansion.md), [`../tests/px32-field-families-ii-proof.md`](../tests/px32-field-families-ii-proof.md), [`../tests/px32-dimensional-proof.md`](../tests/px32-dimensional-proof.md) | Real and supportable for a bounded slice only. |
| Bounded experimental preset pool | Keep experimental | [`px30-stable-vs-experimental-notes.md`](./px30-stable-vs-experimental-notes.md), [`px31-stable-vs-experimental-notes.md`](./px31-stable-vs-experimental-notes.md), [`px32-stable-vs-experimental-notes.md`](./px32-stable-vs-experimental-notes.md) | The pool is real, but it remains intentionally narrow and opt-in. |
| Advanced and Author-Lab control surfaces | Keep experimental | [`pl11-settings-surfaces.md`](./pl11-settings-surfaces.md), [`px40-plasma-lab.md`](./px40-plasma-lab.md) | These remain deeper control surfaces outside the simple stable default path. |
| BenchLab forcing overrides | Keep experimental support tooling | [`pl12-benchlab-fields-and-forcing.md`](./pl12-benchlab-fields-and-forcing.md), [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md) | Bounded support tooling only, not a stable end-user product promise. |

### Implemented But Still Too Weakly Proved Or Too Narrow For Stable Promotion

| Area | Current Experimental Decision | Evidence / Basis | Why It Stays Experimental |
| --- | --- | --- | --- |
| Capture-backed `degrade-report --capture` | Keep experimental | [`px40-stable-vs-experimental-notes.md`](./px40-stable-vs-experimental-notes.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md) | Depends on existing BenchLab text captures and bounded semantic interpretation, not broad rerun or rendered proof. |
| `capture-diff` | Keep experimental | [`px40-stable-vs-experimental-notes.md`](./px40-stable-vs-experimental-notes.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md) | Real and useful, but still bounded to current text captures rather than a universal deterministic comparison lab. |
| PX41 experimental bridge and sampler metadata | Keep experimental | [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These surfaces intentionally expose experimental Wave 3 material and remain metadata-first rather than live-consumer proved. |
| `experimental_matrix` control profile | Keep experimental | [`px41-sdk-and-automation.md`](./px41-sdk-and-automation.md), [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md) | The profile deliberately opts into experimental content and does not belong in the stable control surface. |

## Deferred / Non-Ship / Non-Claim Cut

| Area | Deferred Or Non-Claim Decision | Why |
| --- | --- | --- |
| Scenic, world-simulated, or free-camera presentation | Do not ship or claim | Outside Plasma identity and still unsupported. |
| `bounded_billboard_volume` and broader premium-dimensional breadth | Do not ship or claim | Not implemented and not proved. |
| Surface output family work | Do not ship or claim | Still unsupported in repo reality. |
| Broad experimental content catalog | Do not ship or claim | The current experimental preset slice remains intentionally narrow. |
| Exhaustive transition pair coverage | Do not ship as a claim | Proof still covers the implemented curated subset only. |
| Broad cross-hardware lane guarantees | Do not ship as a claim | Proof still comes from one capable validation machine. |
| Numeric performance SLAs | Do not ship as a claim | No hard numeric measurement surface exists. |
| Live `suite` or `anthology` consumer behavior | Do not ship as a claim | `PX41` is metadata-first and report-first only. |
| Broad automation bus, daemon, or replay framework | Do not ship or claim | `PX41` landed bounded local control profiles only. |
| Community platform behavior, ratings, dislikes, or online index | Do not ship or claim | `PX41` landed bounded local curation and one-pack provenance only. |
| Full authored preset/theme migration or GUI editor claims | Do not ship or claim | `PX40` remains a CLI-first, report-first Lab surface. |

## Enforceable Stable / Experimental / Deferred Boundaries

The current repo enforces these boundaries materially rather than only rhetorically:

- stable defaults still point to `plasma_lava` and `plasma_lava`
- stable-only content filtering remains the default posture
- transitions remain off by default
- richer lanes remain capability-gated and non-default
- the bounded experimental preset slice is explicitly marked experimental in docs and content posture
- `PX40` capture-backed compare surfaces are explicitly marked experimental
- `PX41` stable-visible profiles reject experimental content, while experimental profiles must opt in explicitly
- shared `SY40` vocabulary remains thin and does not silently centralize product-local behavior

## Opt-In Only Statement

The following remain opt-in only in the current cut:

- richer runtime lanes beyond `gl11`
- the bounded transition subset
- the bounded Wave 3 experimental preset slice
- capture-backed compare and degrade inspection over existing text captures
- experimental local bridge, curation, and control surfaces from `PX41`

## Scope Boundary

`PX50` does not reclassify experimental material as stable by momentum.
It keeps implemented but bounded surfaces visible, explicit, and opt-in, and it keeps unsupported or weakly proved areas out of the current ship promise.
