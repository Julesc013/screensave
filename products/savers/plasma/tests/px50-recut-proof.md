# PX50 Recut Proof

## Purpose

This note records the evidence surface used for the `PX50` stable, experimental, and deferred recut.

## Exact Evidence Sources Used

Core recut and posture sources:

- `products/savers/plasma/docs/pl13-validation-matrix.md`
- `products/savers/plasma/docs/pl13-performance-envelopes.md`
- `products/savers/plasma/docs/pl13-known-limits.md`
- `products/savers/plasma/docs/pl14-release-cut.md`
- `products/savers/plasma/docs/pl14-stable-vs-experimental.md`
- `products/savers/plasma/docs/pl14-ship-readiness.md`
- `products/savers/plasma/docs/pl14-release-notes-draft.md`

Wave evidence and posture sources:

- `products/savers/plasma/docs/px10-support-hardening.md`
- `products/savers/plasma/docs/px10-performance-and-soak.md`
- `products/savers/plasma/docs/px11-transition-coverage.md`
- `products/savers/plasma/docs/px11-promotion-review-1.md`
- `products/savers/plasma/docs/px20-data-and-authoring-substrate.md`
- `products/savers/plasma/docs/px30-stable-vs-experimental-notes.md`
- `products/savers/plasma/docs/px31-stable-vs-experimental-notes.md`
- `products/savers/plasma/docs/px32-stable-vs-experimental-notes.md`
- `products/savers/plasma/docs/px40-stable-vs-experimental-notes.md`
- `products/savers/plasma/docs/px41-stable-vs-experimental-notes.md`

Proof and capture sources:

- `products/savers/plasma/tests/px10-hardware-proof.md`
- `products/savers/plasma/tests/px10-soak-proof.md`
- `products/savers/plasma/tests/px11-transition-proof.md`
- `products/savers/plasma/tests/px11-promotion-review-proof.md`
- `products/savers/plasma/tests/px30-output-proof.md`
- `products/savers/plasma/tests/px30-treatment-proof.md`
- `products/savers/plasma/tests/px31-glyph-proof.md`
- `products/savers/plasma/tests/px31-field-families-i-proof.md`
- `products/savers/plasma/tests/px32-field-families-ii-proof.md`
- `products/savers/plasma/tests/px32-dimensional-proof.md`
- `products/savers/plasma/tests/px40-authoring-proof.md`
- `products/savers/plasma/tests/px40-ops-tools-proof.md`
- `products/savers/plasma/tests/px41-integration-proof.md`
- `products/savers/plasma/tests/px41-curation-proof.md`
- `validation/captures/pl13/benchlab-plasma-gdi.txt`
- `validation/captures/pl13/benchlab-plasma-gl11.txt`
- `validation/captures/pl13/benchlab-plasma-gl21.txt`
- `validation/captures/pl13/benchlab-plasma-gl33.txt`
- `validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`
- `validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`
- `validation/captures/pl13/benchlab-plasma-journey-gl11.txt`
- `validation/captures/px40/README.md`
- `validation/captures/px41/README.md`

Current metadata and contract sources:

- `docs/roadmap/sy40-suite-and-anthology-contract.md`
- `docs/roadmap/sy40-sdk-reference-contract.md`
- `products/savers/plasma/integration/projection_surface.ini`
- `products/savers/plasma/integration/sdk_reference.ini`
- `products/savers/plasma/integration/control_profiles.ini`
- `products/savers/plasma/curation/curated_collections.ini`
- `products/savers/plasma/curation/provenance_index.ini`

## Commands And Checks Actually Run In This Checkout

Commands run while preparing the recut:

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py authoring-report`
- `python tools/scripts/plasma_lab.py compat-report --kind pack --target lava_remix`
- `python tools/scripts/plasma_lab.py compat-report --kind journey --target classic_cycle`
- `python tools/scripts/plasma_lab.py migration-report`
- `python tools/scripts/plasma_lab.py integration-report`
- `python tools/scripts/plasma_lab.py control-report --profile deterministic_classic`
- `python tools/scripts/plasma_lab.py curation-report`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`
- `git diff --check`

## Evidence Classes Used

Validated for current recut decisions:

- lower-band stable runtime proof on `gdi` and `gl11`
- current stable default routing and classic/default invariants
- current BenchLab reporting surface
- current stable PX40 read-only tooling surface
- current stable PX41 metadata and report surface when restricted to stable collections and stable pack truth

Partial for current recut decisions:

- richer lanes beyond `gl11`
- transition breadth
- Wave 3 contour, treatment, glyph, field, and dimensional studies
- capture-backed compare and degrade inspection over text captures
- local integration, automation, and curation surfaces where the current proof is report-first rather than live-consumer-first

Blocked for broader promotion:

- broad cross-hardware claims
- fresh compiled reruns beyond the currently checked-in evidence
- numeric SLA-style performance claims

Documented only or intentionally non-claim:

- scenic or billboard-style premium breadth
- broad community or index behavior
- live remote automation or replay framework behavior

## How Evidence Class Affected Classification

- validated stable default runtime and lower-band evidence remained in stable
- validated read-only support surfaces that only restate stable truth were allowed into the stable support cut
- partial but real runtime and visual-language breadth stayed experimental
- partial text-capture tooling and partial report-first integration surfaces stayed experimental when they touched experimental material
- blocked or documented-only areas stayed deferred or explicit non-claim

## What Remained Uncertain

- how richer lanes behave across broader hardware variance
- whether live `suite` or `anthology` consumers would behave identically on fresh reruns
- how far current bounded transition proof generalizes beyond the curated subset
- whether current qualitative performance posture would hold under a broader machine sweep

## What Remained Caveated

- the ship decision remains `GO WITH CAVEATS`
- stable runtime remains narrow
- stable support widened only narrowly
- Wave 3 and richer-lane breadth remain opt-in experimental
- local ecosystem surfaces remain product-local and metadata-first

## Actual Evidence Versus Narrative Summary Only

Actual evidence in this tranche is:

- the cited proof notes
- the cited BenchLab text captures
- the current checked-in metadata files
- the commands listed above and their successful outputs in this checkout

Narrative summary only:

- any implication of fresh compiled reruns beyond the cited checked-in evidence
- any implication of broad live-consumer ecosystem proof
- any implication of cross-hardware closure beyond the cited one-machine evidence

## Scope Boundary

There is no dedicated PX50 recut harness.
`PX50` is a decision pass over existing evidence plus current read-only report commands, not a new runtime validation phase.
