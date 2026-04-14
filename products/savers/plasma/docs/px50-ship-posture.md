# PX50 Ship Posture

## Purpose

`PX50` records the current post-Wave-4 ship-posture decision for Plasma.

## Maintainer Executive Summary

Decision: **GO WITH CAVEATS**

Plasma is shippable now as:

- a narrow stable runtime product centered on the preserved Classic-first lower-band path
- a somewhat broader stable support surface through BenchLab reporting, PX40 read-only authoring and ops reports, and the PX41 stable metadata-report slice
- a clearly bounded experimental pool that remains opt-in and non-default

This is a stronger and clearer posture than the original `PL14` cut on supportability and scope clarity.
It is not strong enough for a blanket clean `GO`.

## Explicit Decision

Decision: **GO WITH CAVEATS**

## Concise Rationale

The decision remains `GO WITH CAVEATS` because:

- the stable default product path is still well preserved and directly proved on `gdi` and `gl11`
- the repo now has one unified runtime, truthful settings semantics, and a more coherent stable preset family than it had at `PL14`
- the repo now has a stronger stable support-tooling surface than it had at `PL14`
- the repo now has a clearer, more explicit local ecosystem and curation surface than it had at `PL14`
- the repo now has deterministic influence checks and preset-signature audits that materially reduce fake-control risk
- but richer runtime breadth, transition breadth, and Wave 3 visual-language breadth still remain bounded and experimental
- and the proof envelope is still materially constrained by one-machine hardware breadth and no universal screenshot diff framework

## Strongest Remaining Caveats And Blockers

- proof still relies on one capable validation machine rather than a broad cross-driver sweep
- richer-lane and premium breadth are implemented but still bounded and not broadly re-proved
- transition coverage remains curated rather than exhaustive
- Wave 3 output, treatment, glyph, field, and dimensional widening remains experimentally bounded
- PX40 capture-backed compare remains text-based and bounded rather than universal deterministic replay or rendered diff
- the new deterministic smoke influence harness is still a bounded dead-setting detector rather than a full perceptual screenshot lab
- PX41 integration and curation surfaces remain metadata-first and report-first rather than live-consumer proved
- numeric performance SLAs still do not exist
- the authored registry remains partial because built-in preset and theme descriptors are still partly code-anchored

## What This Decision Means

This decision means:

- Plasma can be released honestly today with a narrow stable runtime claim and an explicit experimental slice
- release messaging can now describe stronger support tooling, truthful settings semantics, and stronger local integration metadata than the original `PL14` docs did
- maintainers have a clearer current answer to what is stable, what is experimental, and what is non-claim

## What This Decision Does Not Mean

This decision does not mean:

- every implemented lane or feature is now stable
- the Wave 3 study surfaces are ready for default exposure
- live `suite` or `anthology` consumer behavior is now claimed
- the repo has broad cross-hardware validation
- the repo has entered a “no caveats remain” posture

## Scope Boundary

`PX50` is a recut and decision phase only.
It does not add features to force a better ship decision.
