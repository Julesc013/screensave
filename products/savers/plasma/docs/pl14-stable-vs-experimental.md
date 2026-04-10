# PL14 Stable Vs Experimental

## Purpose

This document defines the original `PL14` stable-versus-experimental split.

Current note:
the current post-`PX41` split now lives in [`px50-stable-recut.md`](./px50-stable-recut.md) and [`px50-experimental-and-deferred.md`](./px50-experimental-and-deferred.md).
`PX50` keeps the stable runtime cut narrow and carries later-wave breadth forward explicitly as experimental or deferred.

## Stable Vs Experimental Criteria

An item belongs in the stable cut only when it is:

- implemented in the current repo
- backed by direct evidence or strong phase-proof continuity
- consistent with the lower-band truthful baseline where relevant
- compatible with the preserved default Plasma identity
- supportable without depending on premium-only or partially validated behavior

An item belongs in the experimental cut when it is:

- genuinely implemented
- useful enough to ship as opt-in material
- bounded enough to describe honestly
- but not broad enough, proven enough, or supportable enough for default stable claims

An item belongs in neither cut when it is:

- unsupported
- only partially conceived
- only documented as future direction
- or too weakly validated to ship honestly

## Stable Content And Features Included In This Release

- preserved Plasma Classic/default content identity
- built-in classic preset and theme inventory
- `ember_lava -> plasma_lava` alias continuity
- current `lava_remix` pack compatibility surface
- default `raster` + `native_raster` + `flat` runtime path
- truthful `gdi` and `gl11` baseline routing posture
- safe default settings posture with stable-only content filtering
- product-local BenchLab reporting for support and proof
- PX40 read-only authoring and ops reports over the current authored substrate and pack shell

## Experimental Content And Features Included In This Release

- optional `gl21` advanced lane
- optional `gl33` modern lane
- optional `gl46` premium lane
- bounded premium dimensional presentation subset
- bounded experimental preset pool for contour, banded, treatment, glyph, field-family-I, and field-family-II studies
- bounded contour output subset
- bounded banded output subset
- bounded treatment-family subset
- bounded glyph output subset
- bounded field-family-I subset
- bounded field-family-II subset
- bounded premium dimensional subset beyond `heightfield`
- capture-backed degrade inspection over existing BenchLab text captures
- semantic BenchLab text-capture diff over existing checked-in report surfaces
- bounded real transition subset:
  - theme morphs
  - preset morphs
  - bridge morphs
  - fallback transitions
  - curated journeys
- Advanced and Author-Lab control surfaces
- bounded BenchLab forcing overrides for the implemented subset

## Content And Features Excluded From Both For Now

- any broad shipped experimental content pool
- field-families II breadth beyond the admitted bounded subset
- surface output family work
- premium-dimensional breadth beyond the admitted bounded subset
- scenic or world-simulated premium presentation
- exhaustive transition pair coverage
- hard numeric performance guarantees
- broad cross-hardware lane guarantees
- full BenchLab forcing or telemetry-console behavior

## Default-Pool Behavior

The default pool remains stable-first and classic-first:

- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- content filter remains stable-only by default
- transitions remain off by default
- richer lanes remain optional and capability-gated, not default-routed

## Stable / Experimental Tagging And Selection Posture

The split is enforceable in the current repo because:

- the default content pool is still stable-only even though a bounded experimental preset slice now exists
- richer lanes are not the default routing center
- premium presentation is opt-in and capability-gated
- transition behavior is opt-in rather than part of the default baseline
- deeper control surfaces sit outside the default simple path
- capture-backed compare surfaces stay outside the default simple path and remain bounded to existing text captures

## User And Support Implications

- End users who do nothing get the stable Plasma Classic-centered experience.
- Users who opt into richer lanes, transitions, deeper lab surfaces, capture-backed compare surfaces, or the bounded PX30-PX32 experimental content slice are stepping into implemented but bounded experimental territory.
- Support and release messaging should describe richer-lane and transition behavior as optional uplift, not as the baseline every system must reproduce.

## Scope Boundary

This split reflects the current implemented repo state.
It does not pretend that stable and experimental material are equally validated.
