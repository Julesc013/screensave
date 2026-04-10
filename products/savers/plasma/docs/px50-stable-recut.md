# PX50 Stable Recut

## Purpose

`PX50` records the current evidence-backed stable cut for Plasma after the completed post-`PL14` waves now present in the repo.

## Why PX50 Exists Now

`PL14` froze the original stable, experimental, and deferred cut at the end of the core Plasma program.
`PX10` through `PX41` then added stronger proof, bounded visual-language widening, stronger authoring and ops tooling, and bounded local integration metadata.
`PX50` exists to turn that accumulated repo reality into one current recut without adding new feature scope.

## Relationship To PL14 And The Post-PL/PX Program

- `PL14` remains the original release-cut baseline.
- `PX10` and `PX11` hardened support and transition proof without widening stable scope.
- `PX20` through `PX32` widened the product model and visual language, but kept those wider surfaces bounded and experimental.
- `PX40` and `PX41` added stronger stable support surfaces without widening the stable runtime default.
- `PX50` is the current recut point.
  It curates what is stably shippable now, what remains experimental, and what remains deferred or non-claim.

## Stable Cut

The stable cut stays intentionally narrow.
It now has two layers:

- a preserved stable runtime core
- a narrow stable support surface that only restates or inspects already-stable product truth

### Stable Runtime Core

| Area | Stable Decision | Evidence / Basis | Rationale |
| --- | --- | --- | --- |
| Product identity | Ship as `Plasma`, preserving `Plasma Classic` inside it | [`pl02-plasma-classic.md`](./pl02-plasma-classic.md), [`pl14-release-cut.md`](./pl14-release-cut.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) | Identity continuity remains intact and directly supported by the current repo and proof notes. |
| Default content | Ship the stable classic preset/theme inventory, `ember_lava -> plasma_lava`, and the current `lava_remix` compatibility surface | [`pl02-classic-migration-map.md`](./pl02-classic-migration-map.md), [`pl06-content-system.md`](./pl06-content-system.md), [`../tests/pl06-content-proof.md`](../tests/pl06-content-proof.md) | Default content remains stable-only by default and still anchors the shipped product honestly. |
| Default runtime path | Ship the preserved classic raster path as `raster` + `native_raster` + `flat` with no wider output or dimensional default implied | [`pl04-output-framework.md`](./pl04-output-framework.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) | The preserved classic path remains the most directly exercised and most supportable runtime surface. |
| Lower-band routing posture | Ship `gdi` as the truthful floor and `gl11` as the truthful preferred stable lane | [`pl05-universal-compat.md`](./pl05-universal-compat.md), [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt), [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | Lower-band truth is still the strongest direct proof surface in the repo. |
| Default settings posture | Ship the safe default selection posture with stable-only filtering and transitions disabled by default | [`pl11-settings-surfaces.md`](./pl11-settings-surfaces.md), [`../tests/pl11-settings-proof.md`](../tests/pl11-settings-proof.md) | The stable default still routes users into the preserved classic path rather than richer optional behavior. |
| Stable runtime proof surface | Ship product-local BenchLab reporting as part of the stable support and release-validation surface | [`pl12-benchlab.md`](./pl12-benchlab.md), [`pl12-benchlab-fields-and-forcing.md`](./pl12-benchlab-fields-and-forcing.md), [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md) | Requested-versus-resolved-versus-degraded truth remains central to honest support posture. |

### Stable Support Surfaces

| Area | Stable Decision | Evidence / Basis | Rationale |
| --- | --- | --- | --- |
| PX40 read-only authoring and ops reports | Stable support tooling: `validate`, `authoring-report`, authored-object `compare`, `compat-report`, `migration-report`, and pack-shell `degrade-report --pack` | [`px40-authoring-and-ops-tools.md`](./px40-authoring-and-ops-tools.md), [`px40-stable-vs-experimental-notes.md`](./px40-stable-vs-experimental-notes.md), [`../tests/px40-authoring-proof.md`](../tests/px40-authoring-proof.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md) | These surfaces are read-only, repo-grounded, and supportable without widening the runtime promise. |
| PX41 stable local integration and curation reports | Stable support tooling when limited to stable metadata and stable collections: `integration-report`, `control-report`, and `curation-report` | [`px41-suite-and-anthology-bridges.md`](./px41-suite-and-anthology-bridges.md), [`px41-sdk-and-automation.md`](./px41-sdk-and-automation.md), [`px41-community-and-curation.md`](./px41-community-and-curation.md), [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These surfaces only expose or inspect already-stable product meaning. They do not widen stable playback or content behavior. |
| PX41 stable metadata slice | Stable support metadata only for `classic_core_featured`, `dark_room_support`, `anthology_stable_core`, `suite_stable_dark_room`, `deterministic_classic`, the stable `dark_room_support` control profile, and `lava_remix_builtin` provenance indexing | [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These records restate stable collections, stable routing, or stable pack provenance already present in the shipped product. |

## Stable Recut Outcome

`PX50` does not widen the stable runtime or stable visual-language promise.
The stable widening in this recut is narrow and support-surface-only:

- stronger read-only authoring and ops tooling from `PX40`
- stronger read-only local integration, control, and curation reporting over stable metadata from `PX41`

## Preserved Default And Classic Invariants

The current stable cut preserves:

- product name `Plasma`
- preserved `Plasma Classic`
- default preset `plasma_lava`
- default theme `plasma_lava`
- alias `ember_lava -> plasma_lava`
- routing posture `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- stable-only default content filtering
- transitions off by default
- classic raster-plus-flat runtime posture as the default product path

## Lower-Band Truth Statement

The stable cut remains lower-band-first.
Users must still be able to get the honest stable Plasma experience on `gdi` and `gl11` without depending on:

- `gl21`, `gl33`, or `gl46`
- premium presentation
- transition-heavy behavior
- the experimental Wave 3 preset slice
- the local `PX41` integration or curation metadata surfaces

## What Is Intentionally Not In Stable

The following are intentionally not part of the stable cut:

- `gl21`, `gl33`, and `gl46` as stable runtime support targets
- the bounded transition subset as stable runtime behavior
- the bounded contour, banded, treatment, glyph, field-family-I, field-family-II, and dimensional studies from `PX30` through `PX32`
- capture-backed `degrade-report --capture`
- semantic `capture-diff`
- experimental PX41 collections and profiles such as `wave3_experimental_sampler`, `anthology_experimental_studies`, `suite_experimental_sampler`, and `experimental_matrix`
- any live `suite` or `anthology` consumption claim

## Uncertainty Sources

The main uncertainty sources that still constrain the stable cut are:

- proof breadth remains anchored to one capable validation machine
- fresh compiled reruns remain locally blocked in the later proof notes
- richer-lane and premium breadth are still bounded rather than broadly soaked
- transition coverage remains curated rather than exhaustive
- local integration and curation surfaces remain metadata-first and report-first rather than live-consumer proved
- numeric performance SLAs still do not exist

## Scope Boundary

`PX50` is a recut and curation phase only.
It does not add new runtime features, new suite law, new ecosystem behavior, or new stable claims beyond the evidence-backed narrow support-surface widening described above.
