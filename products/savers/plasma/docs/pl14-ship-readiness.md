# PL14 Ship Readiness

## Purpose

This document records Plasma's final PL14 ship-readiness decision for the current release cut.

## Executive Summary

Decision: **GO WITH CAVEATS**

Plasma is ready to ship as a narrow stable Core saver centered on the preserved Classic/default experience and the truthful `gdi` / `gl11` baseline.
It is not ready for a blanket "everything implemented is stable" claim.
Richer lanes, bounded dimensional presentation, deeper lab surfaces, and the transition engine should ship only as explicit experimental material.

## Decision Criteria Used

The decision was made against these dimensions:

1. identity integrity
2. classic preservation
3. lower-band truth
4. higher-lane optionality
5. content curation
6. transition readiness
7. settings readiness
8. BenchLab readiness
9. known-limit severity
10. support burden

## Dimension Assessment

| Dimension | Assessment | Result |
| --- | --- | --- |
| Identity integrity | Plasma still reads as Plasma and remains field-derived first | Pass |
| Classic preservation | Preserved Classic/default path remains intact and default | Pass |
| Lower-band truth | `gdi` and `gl11` are directly backed by PL13 evidence | Pass |
| Higher-lane optionality | `gl21`, `gl33`, and `gl46` remain optional uplifts rather than hidden minimums | Pass with caveat |
| Content curation | Stable-only shipped content pool is narrow but clear | Pass |
| Transition readiness | Real and meaningful, but only bounded and partially soaked | Experimental only |
| Settings readiness | Layered surfaces are coherent; deeper surfaces should not be marketed as stable defaults | Pass with caveat |
| BenchLab readiness | Reporting surface is strong enough for support and validation | Pass |
| Known-limit severity | Does not block stable Core, but blocks a clean everything-ready claim | Caveat |
| Support burden | Narrow stable cut is supportable; broad richer-lane guarantees are not yet supportable | Caveat |

## Evidence References Used

- [`pl13-validation-matrix.md`](./pl13-validation-matrix.md)
- [`pl13-performance-envelopes.md`](./pl13-performance-envelopes.md)
- [`pl13-known-limits.md`](./pl13-known-limits.md)
- [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`](../../../../validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt)
- [`../tests/pl13-soak-and-proof.md`](../tests/pl13-soak-and-proof.md)
- [`../tests/pl13-transition-validation.md`](../tests/pl13-transition-validation.md)

## Main Caveats

- PL13 proof came from one capable validation machine rather than a wider cross-hardware sweep.
- Transition coverage is meaningful but bounded to the implemented curated subset.
- Premium-dimensional presentation is currently limited to `heightfield`.
- BenchLab forcing is bounded and support-facing rather than a broad product debugger.
- PL13 does not establish hard numeric performance SLAs.

## Required Post-Ship Watchpoints

- Watch for lane-selection issues outside the current validation machine.
- Watch for transition regressions on supported curated journeys and morph pairs.
- Watch for support confusion between stable Core behavior and opt-in experimental richer-lane behavior.
- Keep release messaging tightly aligned with the narrow stable cut.

## Uncertainty Sources

- hardware and driver variance beyond the current proof environment
- broader long-run richer-lane soak behavior
- transition behavior outside the curated supported subset
- richer-lane support burden once exposed to more varied environments

## Ship Checklist Appendix

- Stable default remains `plasma_lava` / `plasma_lava`: yes
- Stable default remains `raster` + `native_raster` + `flat`: yes
- `gdi` lower-band floor is directly proved: yes
- `gl11` preferred stable lane is directly proved: yes
- richer lanes remain optional: yes
- experimental content pool is not being overclaimed: yes
- unsupported premium-dimensional modes are excluded: yes
- release messaging can stay within the evidence: yes

## Scope Boundary

This is a release-cut decision, not a new implementation phase.
The result is **GO WITH CAVEATS**, not a clean everything-ready GO.
