# Plasma U08 Acceptance Decisions

## Purpose

This note freezes the actual `U08` acceptance decisions for the major surviving
Plasma surfaces.

It exists to make the current quality judgment explicit before `U09` performs
the final stable/experimental/deferred recut.

## Decision Vocabulary

`U08` uses these product-facing result classes:

- `pass_stable`
- `pass_experimental`
- `keep_experimental_with_caveat`
- `demote_hide`
- `remove_from_claim_surface`
- `blocked_insufficient_evidence`

## Major Decisions

### Pass Stable

- Basic dialog surface
- default `plasma_lava` plus `plasma_lava` path on `gdi` and `gl11`
- `classic_core` stable preset pool
- `classic_core` stable theme pool
- `classic_core_featured` curated collection
- `dark_room_support` curated collection
- `deterministic_classic` support profile
- `dark_room_support` support profile
- `raster` / `native_raster`
- `flat` presentation

Rationale:

- these surfaces have the strongest combined U06 and U07 evidence
- they preserve lower-band truth
- they keep the default baseline coherent
- they do not require hidden premium-only or journey-only semantics after the
  narrow U08 metadata cleanup

### Pass Experimental

- Advanced dialog surface
- banded and contour output families
- direct filter, emulation, and accent treatment controls
- `wave3_experimental_sampler`

Rationale:

- these surfaces are real and evidence-backed enough to keep
- they are still broader than the stable product promise
- keeping them experimental is more honest than either stable promotion or full
  removal at this stage

### Keep Experimental With Caveat

- Author/Lab dialog surface
- glyph output families
- `heightfield` and `ribbon`
- transitions, journeys, and timing controls

Rationale:

- these surfaces are implemented and useful
- the current proof and support story remains bounded
- they deserve preservation, but not a stronger claim than the evidence allows

### Demote/Hide

- diagnostics overlay

Rationale:

- it remains support-valuable
- it does not belong in the user-facing or ship-language surface

### Remove From Claim Surface

- hidden `favorites_only` compatibility state
- compatibility-only preset aliases
- compatibility-only theme aliases

Rationale:

- these surfaces still matter for continuity and migration
- they no longer deserve first-class product variety language

## Narrow Metadata And Visibility Changes Made In U08

To keep the repo honest after the gate, `U08` makes one narrow status cleanup:

- stable curated collections no longer carry `journey_key`
- stable control profiles no longer carry `journey_key`

That change affects:

- `classic_core_featured`
- `dark_room_support`
- `deterministic_classic`
- `dark_room_support`

Why:

- transition and journey behavior remains implemented
- the current proof is still partial and explicitly bounded
- keeping those keys on stable-visible metadata implied a stronger stable claim
  than the current evidence supports

The journeys themselves are preserved.
Only the stable claim surface stops implying they are part of the stable promise.

## Nontrivial Caveats

- `aurora_cool` and `oceanic_blue` remain the closest stable theme pair and
  stay on the watch list even though they are not being cut here
- glyph output remains bounded to small admitted subsets and stays caveated
- premium presentation remains real but still bounded by lane and degrade truth
- transitions remain real but are explicitly not promoted by stable metadata

## What Remains Deferred To U09

`U08` does not perform the final recut.

`U09` still owns:

- the final stable/experimental/deferred classification pass
- final ship-language and claim-surface wording
- final release-cut documentation updates that consume the `U08` decisions as
  inputs
