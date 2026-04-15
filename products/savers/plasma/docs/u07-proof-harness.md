# Plasma U07 Proof Harness

## Purpose

This note records the formal proof substrate added in `U07`.

It exists to prove that the surviving Plasma surface after `U01` through `U06`
is not just better structured and better tuned, but measurably real.

## Why U07 Exists Now

The earlier corrective tranches unified the runtime, made settings more
authoritative, narrowed the shipped surface, and retuned the remaining content.

`U07` turns that work into a repeatable proof model:

- settings influence can now be classified as plan-visible, render-visible,
  bounded, or unsupported
- the stable reachable preset and theme surface is now explicitly enumerated
- the broader experimental surface now has an honest bounded coverage strategy
- near-duplicate preset or theme content can now be flagged by generated audits
- lower-band degrade truth can now be backed by capture diffs instead of only
  by prose

## What The Harness Proves

The current U07 harness proves:

- first-class stable preset and theme combinations compile and validate across
  the truthful `gdi` floor and the truthful `gl11` preferred stable lane
- the surviving first-class grammar controls materially change the compiled plan
  and, for the shipped smoke subset, materially change the rendered result
- the experimental surface is not exhaustive, but it is no longer undefined:
  it has a named bounded coverage slice
- near-duplicate preset signatures and near-neighbor theme palettes can now be
  reported from the compiled product truth
- degrade and clamp behavior can now be compared semantically from BenchLab
  captures

## What The Harness Does Not Prove

The current harness still does not prove:

- screenshot-grade visual acceptance across every renderer path
- universal pixel-perfect determinism across every lane and driver stack
- exhaustive proof for the full experimental cross-product
- final stable-versus-experimental promotion decisions
- the later `U08` visual QA bar

`U07` builds the proof substrate. It is not the final quality gate.

## Proof Methods Used

The current harness uses a mixed proof model.

- `render_signature`: deterministic smoke renders compare treated and presented
  buffers and fail if a shipped setting stops moving enough pixels to matter
- `smoke_matrix_compile`: deterministic smoke exhaustively validates the stable
  preset and theme matrix across `gdi` and `gl11`
- `structural_plan`: resolved settings, selection routing, and compiled-plan
  truth are checked without pretending that every control should be judged by a
  single still frame
- `benchlab_capture`: requested, resolved, and degraded text captures prove
  routing, clamp, and fallback truth
- `palette_distance`: compiled theme palette separation is reported directly
- `preset_signature_distance`: compiled preset signatures are audited directly
  for exact duplicates and near-duplicate crowding

## Structural Versus Rendered Proof

The harness makes an explicit distinction:

- render-visible proof is stronger and is used wherever a deterministic smoke
  frame can honestly prove that a setting changes the visual result
- structural proof is used for surfaces whose truth is mostly about routing,
  transitions, selection pools, timing, seed continuity, or other runtime state
  that is not honestly reducible to one still frame

This means some U07 rows are intentionally `partial`.
That is an honest boundary, not a hidden failure.

## Current Evidence Surfaces

The current U07 artifact tree lives under `validation/captures/u07/` and
includes:

- settings influence and result-taxonomy reports
- the stable combination matrix report
- the bounded experimental coverage report
- a thresholded preset-signature and palette audit
- BenchLab default-lane and degrade captures
- capture-diff and degrade-report outputs for lane and presentation-clamp truth

The deterministic smoke harness in `products/savers/plasma/tests/smoke.c`
provides the executable proof path for stable-matrix validation and shipped
render-signature deltas.

## How Later Phases Should Consume The Harness

Later phases should treat U07 output this way:

- `validated`: current evidence-backed proof
- `partial`: real bounded proof exists, but not to the level of a later full QA
  gate
- `documented_only`: recorded source or doctrine truth without direct generated
  evidence in this phase
- `unsupported`: outside the currently admitted surface
- `blocked`: would need an environment or capture surface the repo does not yet
  have

`U08` should consume this harness as a prerequisite substrate, not as a
substitute for human visual review or final recut decisions.
