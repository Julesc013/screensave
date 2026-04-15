# Plasma U09 Experimental And Deferred

## Purpose

`U09` records the final experimental cut and the final deferred, hidden, or
non-claim cut for the current corrective-program state.

It exists so implemented breadth stays visible where honest, but does not leak
into stable by wording or by inertia.

## Explicit Experimental Cut

Experimental remains real, implemented, and opt-in.
It does not become stable merely because it lives in the same binary or now has
better documentation.

### Implemented And Supportable But Still Experimental

| Area | Experimental Decision | Evidence / Basis | Why It Stays Experimental |
| --- | --- | --- | --- |
| `gl21`, `gl33`, and `gl46` lanes | Keep experimental | [`pl13-validation-matrix.md`](./pl13-validation-matrix.md), [`pl13-performance-envelopes.md`](./pl13-performance-envelopes.md), [`px50-experimental-and-deferred.md`](./px50-experimental-and-deferred.md) | They remain optional uplifts rather than the lower-band stable center. |
| Advanced settings surface | Keep experimental | [`u03-settings-surfaces.md`](./u03-settings-surfaces.md), [`u07-settings-influence-proof.md`](../tests/u07-settings-influence-proof.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | The controls are real, but they expose broader grammar than the stable promise. |
| Banded and contour output families | Keep experimental | [`u04-output-truth.md`](./u04-output-truth.md), [`u07-experimental-coverage.md`](./u07-experimental-coverage.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | These surfaces are supportable, but broader than the stable raster claim. |
| Direct filter, emulation, and accent controls | Keep experimental | [`u04-treatment-truth.md`](./u04-treatment-truth.md), [`u07-settings-influence-proof.md`](../tests/u07-settings-influence-proof.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | Real enough to keep, but still not part of the narrow stable user promise. |
| `wave3_experimental_sampler` and experimental metadata slice | Keep experimental | [`../curation/curated_collections.ini`](../curation/curated_collections.ini), [`../integration/projection_surface.ini`](../integration/projection_surface.ini), [`u07-experimental-coverage.md`](./u07-experimental-coverage.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | The sampler is intentionally opt-in and remains the admitted Wave 3 experimental gallery. |
| BenchLab forcing overrides and bounded support forcing | Keep experimental support tooling | [`pl12-benchlab-fields-and-forcing.md`](./pl12-benchlab-fields-and-forcing.md), [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md), [`px50-experimental-and-deferred.md`](./px50-experimental-and-deferred.md) | Useful support tooling, but not part of the stable end-user promise. |
| `capture-diff` and capture-backed `degrade-report --capture` | Keep experimental support tooling | [`u07-proof-harness.md`](./u07-proof-harness.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md), [`px50-experimental-and-deferred.md`](./px50-experimental-and-deferred.md) | Real and useful, but still bounded to current text-capture semantics rather than a broad visual lab. |
| `experimental_matrix` profile and experimental bridge metadata | Keep experimental | [`../integration/control_profiles.ini`](../integration/control_profiles.ini), [`../integration/projection_surface.ini`](../integration/projection_surface.ini), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md) | These surfaces deliberately opt into experimental content and should remain clearly marked. |

### Implemented But Still Experimental With Caveat

| Area | Experimental Decision | Evidence / Basis | Caveat |
| --- | --- | --- | --- |
| Author/Lab surface | Keep experimental with caveat | [`u03-control-inventory.md`](./u03-control-inventory.md), [`u07-settings-influence-proof.md`](../tests/u07-settings-influence-proof.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | Several controls remain plan-heavy, support-only, or timing-oriented rather than strong visual claims. |
| Glyph output | Keep experimental with caveat | [`u04-output-truth.md`](./u04-output-truth.md), [`pl13-known-limits.md`](./pl13-known-limits.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | The admitted glyph subset is real but still narrow and explicitly bounded. |
| `heightfield` and `ribbon` | Keep experimental with caveat | [`u04-presentation-truth.md`](./u04-presentation-truth.md), [`u07-experimental-coverage.md`](./u07-experimental-coverage.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | Premium and contour-aware presentation remains lane-sensitive and degrade-caveated. |
| Transitions, journeys, and timing controls | Keep experimental with caveat | [`u07-settings-influence-proof.md`](../tests/u07-settings-influence-proof.md), [`u08-surface-evaluation.md`](./u08-surface-evaluation.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | Implemented and useful, but still only partially proved and therefore not stable-worthy. |

## Explicit Deferred, Hidden, Or Non-Claim Cut

| Area | Deferred / Non-Claim Decision | Why |
| --- | --- | --- |
| Diagnostics overlay | Hidden support surface only | Useful for proof and support, but not part of the product claim surface. |
| Hidden `favorites_only` state | Compatibility-only, non-claim | Preserved for continuity, not for current product variety claims. |
| Compatibility-only preset and theme aliases | Preserve, but remove from claim surface | Migration and identity matter, but they are no longer first-class catalogue breadth. |
| Scenic, world-simulated, billboard-style, or free-camera presentation | Deferred and non-claim | Outside the supported Plasma identity and still unsupported. |
| Unsupported output family `surface` and unsupported output modes | Deferred and non-claim | Still taxonomy or enum residue rather than admitted shipped output truth. |
| Broad experimental catalog claims | Deferred and non-claim | The repo still proves only a bounded experimental slice. |
| Exhaustive transition pair coverage | Deferred and non-claim | Proof remains curated and bounded rather than exhaustive. |
| Broad cross-hardware guarantees | Deferred and non-claim | Current proof remains one-machine anchored. |
| Numeric performance SLAs | Deferred and non-claim | No hard numeric envelope exists. |
| Live `suite` or `anthology` behavior | Deferred and non-claim | The repo still proves local metadata/reporting surfaces, not live consumer ingestion. |
| Broad automation, daemon, replay, or community-platform claims | Deferred and non-claim | The current local ecosystem remains bounded, product-local, and metadata-first. |

## Enforceable Stable / Experimental / Deferred Boundaries

These boundaries are materially enforceable in the repo:

- stable defaults still point to `plasma_lava` and `plasma_lava`
- `gdi` remains the floor and `gl11` remains the preferred stable lane
- stable control profiles set `allow_experimental=false`
- stable curated collections carry `visibility=stable` and no longer carry
  `journey_key`
- experimental sampler metadata remains explicitly `visibility=experimental`
- the stable matrix still covers only the `classic_core` preset and theme sets
- `U08` acceptance decisions keep transitions, glyph, and premium presentation
  out of stable

## Opt-In Only Statement

The following remain opt-in only:

- richer lanes beyond `gl11`
- Advanced and Author/Lab control use
- banded, contour, and glyph output studies
- direct treatment-family experimentation
- premium presentation beyond `flat`
- transitions and journey-driven motion
- experimental collections, bridges, and control profiles
- capture-backed compare and capture-backed degrade inspection

## Scope Boundary

`U09` does not hide implemented breadth.
It classifies it.

The final product remains broader than the stable cut, but that broader surface
stays opt-in, bounded, and explicitly non-stable.
