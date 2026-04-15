# PX50 Stable Recut

## Purpose

`PX50` records the current evidence-backed stable cut for Plasma after the completed post-`PL14` waves now present in the repo.

## Why PX50 Exists Now

`PL14` froze the original stable, experimental, and deferred cut at the end of the core Plasma program.
`PX10` through `PX41` then added stronger proof, bounded visual-language widening, stronger authoring and ops tooling, and bounded local integration metadata.
The later U00 through U08 salvage pass then unified the runtime, made settings semantics truthful, retuned the shipped presets, and added a real influence-check harness.
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
| Product identity and runtime shape | Ship as `Plasma` with one unified runtime, preserving `Plasma Classic` as content identity and compatibility surface rather than as a separate execution regime | [`pl02-plasma-classic.md`](./pl02-plasma-classic.md), [`u01-runtime-unification.md`](./u01-runtime-unification.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) | Identity continuity remains intact while the shipped runtime story becomes simpler and more truthful. |
| Default content | Ship the stable classic preset/theme inventory, `ember_lava -> plasma_lava`, the retuned stable built-ins, and the current `lava_remix` compatibility surface | [`pl02-classic-migration-map.md`](./pl02-classic-migration-map.md), [`u06-preset-retuning.md`](./u06-preset-retuning.md), [`u06-theme-retuning.md`](./u06-theme-retuning.md), [`../tests/pl06-content-proof.md`](../tests/pl06-content-proof.md) | Default content remains stable-only by default and now reads as a more distinct stable family instead of a cluster of weak palette-only variants. |
| Default runtime path | Ship the preserved classic default path as `raster` + `native_raster` + `flat`, while keeping Classic content on the same unified runtime as the broader product | [`pl04-output-framework.md`](./pl04-output-framework.md), [`u04-output-treatment-presentation-truth.md`](./u04-output-treatment-presentation-truth.md), [`pl13-validation-matrix.md`](./pl13-validation-matrix.md) | The default path remains the most directly exercised and most supportable surface, but the product no longer lies about having a separate Classic runtime. |
| Lower-band routing posture | Ship `gdi` as the truthful floor and `gl11` as the truthful preferred stable lane | [`pl05-universal-compat.md`](./pl05-universal-compat.md), [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt), [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | Lower-band truth is still the strongest direct proof surface in the repo. |
| Default settings posture | Ship the safe default selection posture with stable-only filtering and transitions disabled by default, while keeping the Basic settings surface truthful about what it really controls | [`u02-settings-authority.md`](./u02-settings-authority.md), [`u03-settings-model.md`](./u03-settings-model.md), [`../tests/pl11-settings-proof.md`](../tests/pl11-settings-proof.md) | The stable default still routes users into the preserved classic path, but the settings story is no longer preset-hidden or misleading. |
| Stable runtime proof surface | Ship product-local BenchLab reporting plus the salvage influence harness as part of the stable validation surface | [`pl12-benchlab.md`](./pl12-benchlab.md), [`u07-proof-harness.md`](./u07-proof-harness.md), [`../tests/pl12-benchlab-proof.md`](../tests/pl12-benchlab-proof.md) | Requested-versus-resolved-versus-degraded truth remains central, and the repo now has an executable dead-setting detector for the shipped grammar subset. |

### Stable Support Surfaces

| Area | Stable Decision | Evidence / Basis | Rationale |
| --- | --- | --- | --- |
| PX40 read-only authoring and ops reports | Stable support tooling: `validate`, `authoring-report`, authored-object `compare`, `compat-report`, `migration-report`, `preset-audit`, and pack-shell `degrade-report --pack` | [`px40-authoring-and-ops-tools.md`](./px40-authoring-and-ops-tools.md), [`u07-proof-harness.md`](./u07-proof-harness.md), [`px40-stable-vs-experimental-notes.md`](./px40-stable-vs-experimental-notes.md), [`../tests/px40-authoring-proof.md`](../tests/px40-authoring-proof.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md) | These surfaces are read-only, repo-grounded, and now help catch preset crowding and fake-control drift without widening the runtime promise. |
| PX41 stable local integration and curation reports | Stable support tooling when limited to stable metadata and stable collections: `integration-report`, `control-report`, and `curation-report` | [`px41-suite-and-anthology-bridges.md`](./px41-suite-and-anthology-bridges.md), [`px41-sdk-and-automation.md`](./px41-sdk-and-automation.md), [`px41-community-and-curation.md`](./px41-community-and-curation.md), [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These surfaces only expose or inspect already-stable product meaning. They do not widen stable playback or content behavior. |
| PX41 stable metadata slice | Stable support metadata only for `classic_core_featured`, `dark_room_support`, `anthology_stable_core`, `suite_stable_dark_room`, `deterministic_classic`, the stable `dark_room_support` control profile, and `lava_remix_builtin` provenance indexing | [`px41-stable-vs-experimental-notes.md`](./px41-stable-vs-experimental-notes.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These records restate stable collections, stable routing, or stable pack provenance already present in the shipped product. |

## Stable Recut Outcome

`PX50` keeps the stable runtime narrow, but it no longer leaves the product architecture or settings semantics in a misleading state.
The stable widening in this recut is still narrow and support-surface-first:

- stronger read-only authoring and ops tooling from `PX40`
- stronger read-only local integration, control, and curation reporting over stable metadata from `PX41`
- stronger stable preset distinctness and truthful Basic settings semantics inside the stable runtime itself

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
- Classic preserved as content identity instead of as a separate runtime gate

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
- the broader contour, banded, treatment, glyph, field-family-I, field-family-II, and dimensional studies from `PX30` through `PX32` as stable-by-default claims
- Advanced and Author/Lab use of that broader grammar as stable-default posture
- capture-backed `degrade-report --capture`
- semantic `capture-diff`
- experimental PX41 collections and profiles such as `wave3_experimental_sampler`, `anthology_experimental_studies`, `suite_experimental_sampler`, and `experimental_matrix`
- any live `suite` or `anthology` consumption claim

## Uncertainty Sources

The main uncertainty sources that still constrain the stable cut are:

- proof breadth remains anchored to one capable validation machine
- no universal screenshot diff framework exists even though the smoke now performs deterministic render-signature checks
- richer-lane and premium breadth are still bounded rather than broadly soaked
- transition coverage remains curated rather than exhaustive
- local integration and curation surfaces remain metadata-first and report-first rather than live-consumer proved
- numeric performance SLAs still do not exist

## Scope Boundary

`PX50` is a recut and curation phase only.
It does not add new runtime features, new suite law, new ecosystem behavior, or new stable claims beyond the evidence-backed narrow support-surface widening described above.
