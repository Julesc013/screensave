# Plasma U09 Stable Recut

## Purpose

`U09` records the final evidence-backed stable cut for Plasma after the
corrective program.

It exists to replace the earlier `PX50` "current cut" posture with one final
living recut that consumes the unified runtime, settings-authority, UI-truth,
grammar-truth, simplification, retuning, proof, and QA work that landed in
`U01` through `U08`.

## Why U09 Exists Now

The earlier corrective phases did the hard work:

- `U01` removed the split Classic runtime story
- `U02` made settings own the grammar honestly
- `U03` made the dialog tell more truth
- `U04` made the admitted grammar materially real
- `U05` cut fake breadth
- `U06` retuned the surviving catalogue
- `U07` built the proof substrate
- `U08` defined and applied the acceptance bar

`U09` is the curation pass that finally answers what is actually stable now.

## Relationship To PL14, PX50, And The Corrective Program

- `PL14` remains the original release-cut baseline.
- `PX50` remains the earlier post-PL and post-Wave-4 recut kept for historical
  comparison.
- `U09` is the final corrective recut and now supersedes `PX50` as the current
  living stable-cut source for Plasma.

`U09` does not add new features to justify a better answer.
It curates the answer from the repo as it actually exists.

## Stable Cut

The stable cut remains intentionally narrow.

It has two layers:

- a stable runtime core
- a stable support surface that only restates, validates, or inspects already
  stable product truth

### Stable Runtime Core

| Area | Stable Inclusion | Evidence / Basis | Rationale |
| --- | --- | --- | --- |
| Product identity and runtime shape | Ship as `Plasma` on one unified runtime while preserving `Plasma Classic` as content identity and compatibility surface | [`u01-runtime-unification.md`](./u01-runtime-unification.md), [`u01-classic-compatibility.md`](./u01-classic-compatibility.md), [`u08-surface-evaluation.md`](./u08-surface-evaluation.md) | The separate Classic runtime story is gone, but Classic identity and compatibility remain preserved. |
| Default stable center | Keep `default_preset=plasma_lava`, `default_theme=plasma_lava`, `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe` | [`../manifest.ini`](../manifest.ini), [`u06-post-retuning-invariants.md`](./u06-post-retuning-invariants.md), [`u08-surface-evaluation.md`](./u08-surface-evaluation.md) | The repo still points at the same lower-band-first default center and `U08` explicitly kept it. |
| Stable settings surface | Keep the Basic surface as the stable end-user control surface | [`u03-settings-surfaces.md`](./u03-settings-surfaces.md), [`u07-settings-influence-proof.md`](../tests/u07-settings-influence-proof.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | Basic now has stable influence proof and no longer hides preset-owned authority. |
| Stable preset pool | Keep `classic_core`: `plasma_lava`, `aurora_plasma`, `ocean_interference`, `museum_phosphor`, `quiet_darkroom` | [`u05-simplification-decisions.md`](./u05-simplification-decisions.md), [`u06-preset-retuning.md`](./u06-preset-retuning.md), [`u07-stable-combination-matrix.md`](./u07-stable-combination-matrix.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | This pool is small enough to prove, distinct enough to keep, and still preserves Classic-first identity. |
| Stable theme pool | Keep `classic_core`: `plasma_lava`, `aurora_cool`, `oceanic_blue`, `museum_phosphor`, `quiet_darkroom` | [`u05-simplification-decisions.md`](./u05-simplification-decisions.md), [`u06-theme-retuning.md`](./u06-theme-retuning.md), [`u07-diff-and-duplicate-proof.md`](../tests/u07-diff-and-duplicate-proof.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | This pool remains supportable, even though `aurora_cool` and `oceanic_blue` stay on the watch list. |
| Stable curated content slices | Keep `classic_core_featured` and `dark_room_support` | [`../curation/curated_collections.ini`](../curation/curated_collections.ini), [`u06-default-and-preview-behavior.md`](./u06-default-and-preview-behavior.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | These collections now represent stable content without silently implying stable journey coverage. |
| Stable support profiles | Keep `deterministic_classic` and stable `dark_room_support` | [`../integration/control_profiles.ini`](../integration/control_profiles.ini), [`u06-default-and-preview-behavior.md`](./u06-default-and-preview-behavior.md), [`u08-acceptance-decisions.md`](./u08-acceptance-decisions.md) | These remain stable support and review surfaces after `U08` removed the implied stable journey claim. |
| Stable visual grammar | Keep `raster` plus `native_raster`, and `flat` presentation | [`u04-output-truth.md`](./u04-output-truth.md), [`u04-presentation-truth.md`](./u04-presentation-truth.md), [`u08-surface-evaluation.md`](./u08-surface-evaluation.md) | This is still the only lower-band-honest stable grammar claim. |

### Stable Support Surface

| Area | Stable Inclusion | Evidence / Basis | Rationale |
| --- | --- | --- | --- |
| BenchLab reporting | Keep requested, resolved, and degraded reporting as stable support truth | [`pl12-benchlab.md`](./pl12-benchlab.md), [`u02-post-settings-invariants.md`](./u02-post-settings-invariants.md), [`u07-proof-harness.md`](./u07-proof-harness.md) | Requested-versus-resolved truth remains part of the product's honest support posture. |
| Plasma Lab read-only audits | Keep `validate`, `authoring-report`, `compare`, `compat-report`, `migration-report`, `preset-audit`, and pack-shell `degrade-report --pack` as stable support tooling | [`px40-authoring-and-ops-tools.md`](./px40-authoring-and-ops-tools.md), [`../tests/px40-authoring-proof.md`](../tests/px40-authoring-proof.md), [`../tests/px40-ops-tools-proof.md`](../tests/px40-ops-tools-proof.md) | These are read-only support surfaces that inspect existing stable truth instead of widening runtime promises. |
| Stable metadata reports | Keep `integration-report`, `control-report`, and `curation-report` when they only restate stable metadata | [`px41-sdk-and-automation.md`](./px41-sdk-and-automation.md), [`px41-community-and-curation.md`](./px41-community-and-curation.md), [`../tests/px41-integration-proof.md`](../tests/px41-integration-proof.md), [`../tests/px41-curation-proof.md`](../tests/px41-curation-proof.md) | These remain stable only as metadata-first reporting over already-stable collections, profiles, and routing posture. |

## Preserved Default And Classic Invariants

The stable recut preserves all of these:

- product name `Plasma`
- preserved `Plasma Classic`
- alias `ember_lava -> plasma_lava`
- default preset `plasma_lava`
- default theme `plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`
- stable-first content posture by default
- the classic raster-plus-flat default experience
- explicit requested versus resolved versus degraded reporting

## Lower-Band Truth Statement

The stable cut remains lower-band-first.

Stable Plasma must remain honestly available on:

- `gdi`
- `gl11`

Higher lanes remain optional uplifts.
They are not the hidden minimum and they are not part of the stable claim by
implication.

## What Is Intentionally Not In Stable

The following are intentionally not in the stable cut:

- richer lanes beyond `gl11`
- Advanced and Author/Lab as stable user-facing posture
- direct non-raster grammar claims
- direct treatment-family controls
- presentation modes beyond `flat`
- transitions, journeys, and timing controls
- capture-backed text comparison and capture-backed degrade inspection
- experimental bridges, collections, and control profiles
- diagnostics-only and compatibility-only surfaces

## Uncertainty Sources

The strongest remaining uncertainty sources are:

- proof still comes from one capable validation machine
- no universal screenshot or perceptual diff framework exists
- transitions remain bounded by curated rather than exhaustive proof
- richer-lane, glyph, and premium-presentation breadth remain partial
- the authored preset and theme registry remains partly code-anchored
- stable theme and preset crowding is improved but not eliminated entirely

## Scope Boundary

`U09` does not widen stable by optimism.

This stable recut is deliberately narrow because the current repo proves a
narrow stable product more strongly than it proves a broad one.
