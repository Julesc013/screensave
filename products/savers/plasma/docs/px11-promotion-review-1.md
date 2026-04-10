# PX11 Promotion Review 1

## Purpose

This document records the first evidence-backed stable-promotion review for post-`PL14` Plasma.
`PX11` is allowed to promote nothing if the evidence is not strong enough.

## Promotion Criteria Used

A surface is promotable here only if all of the following are true:

- it is already implemented
- its support boundary is explicit
- its fallback or degrade truth is explicit
- its proof is stronger than one vague smoke assertion
- promoting it would not change the default Classic-first lower-band product identity
- the remaining caveats do not make the stable claim misleading

## Evidence Considered

- checked-in transition captures from `validation/captures/pl13/`
- `PL10`, `PL13`, and `PL14` transition and ship-posture docs
- `PX10` support-hardening docs and risk posture
- the updated `PX11` smoke coverage in source
- the updated validation catalog and known-limit notes
- the current checkout blocker that no local compiler or prebuilt proof binaries were available for a fresh rerun

## Candidates Reviewed

### Candidate: named lower-band direct preset and theme morph subset

Scope considered:

- direct preset morphs inside `fire`, `plasma`, and `interference`
- direct theme morphs inside `warm` and `cool`
- curated lower-band fallback behavior for bridge-capable crossings

Decision: `defer`

Rationale:

- the implemented subset is clearer after `PX11`
- the support boundary is now explicit
- but the current checkout still does not add a fresh compiled rerun, broader host proof, or broader hardware proof
- promoting this subset now would still lean too heavily on inherited captures and bounded smoke coverage

### Candidate: richer-lane bridge morph subset

Scope considered:

- warm-bridge crossings
- cool-field crossings
- `warm_bridge_cycle`
- `cool_bridge_cycle`

Decision: `keep experimental`

Rationale:

- richer-lane bridge support is real and clearer after `PX11`
- it still depends on optional richer lanes
- it still lacks broader rerun breadth and broader cross-hardware evidence
- it is not part of the stable lower-band center of gravity

### Candidate: curated journeys as a stable-facing capability

Scope considered:

- `classic_cycle`
- `dark_room_cycle`
- `warm_bridge_cycle`
- `cool_bridge_cycle`

Decision: `keep experimental`

Rationale:

- journeys are real and now more supportable
- they remain opt-in transition behavior
- transitions remain off by default
- the support promise is still better expressed as bounded experimental posture than as stable default behavior

## Review Outcome

`PX11` promotes nothing.

This is an acceptable outcome and the truthful one in the current repo state.

## Why Promote Nothing

- the stable default path remains intentionally transition-off and lower-band-first
- the current proof base is stronger than before, but still bounded
- the current checkout did not produce a fresh compiled rerun
- broader cross-hardware proof is still missing
- broader release-facing stable widening belongs later and must remain evidence-gated

## Downstream Repo Changes Required By This Review

Because the review promotes nothing:

- `pl14-stable-vs-experimental.md` does not change
- `pl14-release-cut.md` does not change
- `pl14-ship-readiness.md` does not change

The only downstream changes required here are:

- clearer transition-support docs
- clearer proof notes
- clearer known-limit wording
- ledger and risk-register updates that record the no-promotion result honestly
