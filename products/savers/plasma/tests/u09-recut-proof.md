# Plasma U09 Recut Proof

## Purpose

This note records the exact evidence surface used for the final `U09` stable,
experimental, deferred, and ship-posture recut.

## Exact Evidence Sources Used

Corrective-program recut sources:

- `products/savers/plasma/docs/u00-diagnostic-baseline.md`
- `products/savers/plasma/docs/u00-corrective-backlog.md`
- `products/savers/plasma/docs/u01-runtime-unification.md`
- `products/savers/plasma/docs/u02-settings-authority.md`
- `products/savers/plasma/docs/u03-settings-surfaces.md`
- `products/savers/plasma/docs/u04-output-truth.md`
- `products/savers/plasma/docs/u05-simplification-decisions.md`
- `products/savers/plasma/docs/u06-preset-retuning.md`
- `products/savers/plasma/docs/u06-theme-retuning.md`
- `products/savers/plasma/docs/u06-default-and-preview-behavior.md`
- `products/savers/plasma/docs/u06-post-retuning-invariants.md`
- `products/savers/plasma/docs/u07-proof-harness.md`
- `products/savers/plasma/docs/u07-stable-combination-matrix.md`
- `products/savers/plasma/docs/u07-experimental-coverage.md`
- `products/savers/plasma/docs/u07-proof-result-taxonomy.md`
- `products/savers/plasma/docs/u08-visual-qa-bar.md`
- `products/savers/plasma/docs/u08-surface-evaluation.md`
- `products/savers/plasma/docs/u08-acceptance-decisions.md`

Historical comparison sources:

- `products/savers/plasma/docs/pl13-validation-matrix.md`
- `products/savers/plasma/docs/pl13-performance-envelopes.md`
- `products/savers/plasma/docs/pl13-known-limits.md`
- `products/savers/plasma/docs/pl14-release-cut.md`
- `products/savers/plasma/docs/pl14-stable-vs-experimental.md`
- `products/savers/plasma/docs/pl14-ship-readiness.md`
- `products/savers/plasma/docs/pl14-release-notes-draft.md`
- `products/savers/plasma/docs/px50-stable-recut.md`
- `products/savers/plasma/docs/px50-experimental-and-deferred.md`
- `products/savers/plasma/docs/px50-ship-posture.md`
- `products/savers/plasma/docs/px50-release-notes-draft.md`
- `products/savers/plasma/tests/px50-recut-proof.md`

Current metadata and product sources:

- `products/savers/plasma/manifest.ini`
- `products/savers/plasma/README.md`
- `products/savers/plasma/integration/projection_surface.ini`
- `products/savers/plasma/integration/sdk_reference.ini`
- `products/savers/plasma/integration/control_profiles.ini`
- `products/savers/plasma/curation/curated_collections.ini`

Proof, test, and capture sources:

- `products/savers/plasma/tests/u06-retuning-proof.md`
- `products/savers/plasma/tests/u07-settings-influence-proof.md`
- `products/savers/plasma/tests/u07-combination-proof.md`
- `products/savers/plasma/tests/u07-diff-and-duplicate-proof.md`
- `products/savers/plasma/tests/u08-qa-proof.md`
- `validation/captures/u07/`
- `validation/captures/u08/`

## Commands And Checks Actually Run In This Checkout

```powershell
python tools\scripts\check_docs_basics.py
python tools\scripts\check_sdk_surface.py products/savers/plasma/packs/lava_remix
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py qa-bar
python tools\scripts\plasma_lab.py surface-evaluation
python tools\scripts\plasma_lab.py acceptance-decisions
python tools\scripts\plasma_lab.py combination-matrix
python tools\scripts\plasma_lab.py experimental-coverage
python tools\scripts\plasma_lab.py integration-report
python tools\scripts\plasma_lab.py control-report --profile deterministic_classic
python tools\scripts\plasma_lab.py control-report --profile dark_room_support
python tools\scripts\plasma_lab.py curation-report
python tools\scripts\plasma_lab.py preset-audit --threshold 3
.\out\msvc\vs2022\Debug\plasma\plasma_smoke_u07.exe
git diff --check
```

## Validated Versus Partial Versus Blocked Versus Documented Only

Validated for recut purposes:

- the stable default center from `manifest.ini`
- the U08 `pass_stable` surface
- the `classic_core` stable matrix across `gdi` and `gl11`
- stable control profiles and curated collections after the U08 cleanup
- the read-only stable support reporting surfaces still present in the repo

Partial for recut purposes:

- richer lanes beyond `gl11`
- transitions, journeys, and timing controls
- glyph output
- `heightfield` and `ribbon`
- capture-backed comparison and capture-backed degrade inspection
- local metadata-first integration and curation surfaces when read as ecosystem
  behavior rather than as local reporting

Blocked for broader promotion:

- broad cross-hardware proof
- universal screenshot-grade or perceptual-diff acceptance
- numeric SLA-style performance claims

Documented only or explicit non-claim:

- scenic or billboard-style premium breadth
- broad community-platform behavior
- broad automation or replay infrastructure
- live `suite` and `anthology` consumer claims

## How Evidence Classes Affected Classification

- `validated` surfaces were eligible for stable retention
- `partial` surfaces could remain experimental or experimental-with-caveat, but
  not stable
- `blocked` surfaces remained outside promotion and stayed caveated or
  non-claim
- `documented_only` surfaces stayed deferred or explicit non-claim

## What Remained Uncertain

- how far the current proof generalizes across broader hardware variance
- whether the bounded glyph and premium-presentation surfaces would survive a
  broader screenshot-grade review unchanged
- whether current stable crowding watch-list pairs would need future cut or
  retune decisions

## What Remained Caveated

- the final ship posture remains `GO WITH CAVEATS`
- the stable cut remains narrow
- transitions remain experimental with caveat
- richer lanes remain experimental
- glyph output and premium presentation remain bounded
- live ecosystem behavior remains non-claim

## Actual Evidence Versus Narrative Summary Only

Actual evidence in this tranche is:

- the cited `U07` and `U08` proof notes and captures
- the current metadata files
- the current U09 rerun reports under `validation/captures/u09/`
- the commands listed above and their successful outputs in this checkout

Narrative summary only:

- any claim of fresh cross-hardware closure
- any claim of universal screenshot-grade acceptance
- any claim of broader live-consumer ecosystem proof
