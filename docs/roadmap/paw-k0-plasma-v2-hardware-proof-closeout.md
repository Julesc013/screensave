# PAW-K0 Plasma v2 Hardware Visual Proof Closeout

Status: superseded by real-display visual rejection.

PAW-K0 existed to prove the then-publication-ready Plasma v2 artifact on real
displays before PAW-K publication. The real-display product verdict now exists
and rejects publication, so this gate is no longer the active next step.

## Starting Truth

- Plasma v2 is publication-hold.
- Stable is false.
- Historical stable promotion is accepted.
- Current product verdict is visual-rejected.
- Release promotion is withdrawn-for-visual-quality.
- Publication prep is superseded.
- Publication is not published.
- The current local artifact is `out/msvc/vs2017_xp/Release/plasma/plasma.scr`.
- The current local artifact SHA-256 is
  `f52a4e8f66668b88d1005238500c505ac07deb69bda2b1c6c657b7b8e90ce5ec`.

## Purpose

This document remains as the former pre-publication proof shape. It is not the
active next lane while
`validation/captures/plasma-v2/visual-rejection/verdict.toml` records
`decision = "reject-publication"`.

The existing BenchLab/default-demo and publication-prep evidence remain useful.
They do not replace real-display visual proof or the operator's human visual
note.

## Required Evidence

Record the smallest truthful evidence set that answers the publication question:

- hardware and environment matrix using the SY10 vocabulary
- fullscreen `.scr` run notes for the current artifact
- renderer-tier comparison captures or explicit unavailable-tier notes
- contact sheet or screenshot index for the reviewed displays
- frame-time or pacing summary where the available tooling can measure it
- human visual note with a publish, defer, or repair recommendation
- known-limits update if the display proof changes release caveats

Evidence should live under
`validation/captures/plasma-v2/hardware-proof/`.

## Pass Criteria

PAW-K0 could recommend publication only when:

- the artifact hash in the evidence matches the current local package metadata
- the environment matrix names the actual OS, adapter, driver or renderer facts
  that were observed
- fullscreen behavior is reviewed on at least one real display environment
- renderer-tier comparison is either captured or explicitly recorded as blocked
  by local hardware availability
- the human visual note accepts the current artifact for publication
- the note does not broaden Windows compatibility certification

## Hold Criteria

PAW-K0 must recommend hold when:

- no real-display evidence is available
- the current artifact hash diverges from the package or publication metadata
- fullscreen review shows visual, timing, sizing, seam, or control problems
- renderer fallback cannot be explained truthfully
- the human visual note requests changes
- the current visual-rejection verdict remains active

## Work Units

The work-unit packet is
`.aide/work_units/plasma-v2-hardware-proof-closeout.toml`.

- `SS-PLV2-K0-0`: checkpoint the current artifact identity and publication-ready posture
- `SS-PLV2-K0-1`: record the hardware/environment matrix and operator run plan
- `SS-PLV2-K0-2`: collect fullscreen captures, contact sheets, and tier comparison notes
- `SS-PLV2-K0-3`: record frame-time, pacing, and known-limits notes
- `SS-PLV2-K0-4`: record the human visual note and publish/defer recommendation

## Boundary

PAW-K0 does not publish a release, upload artifacts, create a tag, create a
GitHub release, certify broad compatibility, start Workbench MVP, mutate Manager
install/apply state, migrate all savers, expand platforms, freeze the public
SDK, or admit runtime executable plugins.

The current next lane is the Plasma V3 visual-core spike. If a future replacement
path earns an accepted real-display verdict, a new or revised hardware proof
closeout can be opened for that path. Publication by inertia remains forbidden.
